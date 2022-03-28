//==============================================================================
// bl_hwbut.c
// Bluccino HW core supporting basic functions for button & LED
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_hwbut.h"
  #include "bl_gpio.h"

  #define _PRESS_       BL_HASH(PRESS_)      // #PRESS opcode
  #define _RELEASE_     BL_HASH(RELEASE_)    // #RELEASE opcode
  #define _CLICK_       BL_HASH(CLICK_)      // #CLICK opcode
  #define _HOLD_        BL_HASH(HOLD_)       // #HOLD opcode
  #define _STS_         BL_HASH(STS_)        // #STS opcode

//==============================================================================
// BUTTON level logging shorthands
//==============================================================================

  #define LOG                     LOG_BUTTON
  #define LOGO(lvl,col,o,val)     LOGO_BUTTON(lvl,col"button:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_BUTTON(lvl,col,o,val)
  #define ERR 1,BL_R

//==============================================================================
// defines
//==============================================================================

//#define NBUTS  CFG_NUMBER_OF_BUTS

  #define N             4              // number of buttons

  #define SW0_NODE      DT_ALIAS(sw0)
  #define SW1_NODE      DT_ALIAS(sw1)
  #define SW2_NODE      DT_ALIAS(sw2)
  #define SW3_NODE      DT_ALIAS(sw3)

//==============================================================================
// Get button configuration from the devicetree sw0 alias. This is mandatory.
//==============================================================================

  static int id = 0;                   // button ID
  static GP_ctx context[N];            // button context

  static const GP_io button[N] =
               {
                 GP_IO(SW0_NODE, gpios,{0}),
                 GP_IO(SW1_NODE, gpios,{0}),
                 GP_IO(SW2_NODE, gpios,{0}),
                 GP_IO(SW3_NODE, gpios,{0}),
               };

  static int ms = 350;                   // click grace time (ms)
  static BL_ms time[N+1]  = {0,0,0,0,0}; // button press time for click detection
  static bool hold[N+1]   = {0,0,0,0,0}; // button is in a HOLD phase
  static bool toggle[N+1] = {0,0,0,0,0}; // toggle switch states

//==============================================================================
// click modul (click/hold detection)
// - usage: val = click(o,val)   // val = 0:click, 1:hold
// - supports 4 buttons (@id:1..4) and the default button (@id:0)
//==============================================================================
//
// CLICK Interfaces
//    [] = SYS(INIT,TICK)
//    [] = SET(MS)
//    [CLICK,HOLD] = BUTTON(#PRESS,#RELEASE)
//
//                             +-----------------+
//                             |      CLICK      |
//                             +-----------------+
//                      INIT ->|      SYS:       |
//                      TICK ->|                 |
//                             +-----------------+
//                        MS ->|      SET:       |
//                             +-----------------+
//                   #PRESS -> |     BUTTON:     |-> CLICK
//                 #RELEASE -> |                 |-> HOLD
//                             +-----------------+
//
// Input messages:
//   [SYS:INIT]                   init BUTTON module
//   [SYS:TICK @id,cnt]           tick BUTTON module
//   [SET:MS ms]                  set ms time for recognition as HOLD
//   [BUTTON:PRESS @id,0]         button press event received from driver
//   [BUTTON:RELEASE @id,time]    button release event received from driver
//                                note: time is PRESS->RELEASE elapsed time
// Output messages:
//   [BUTTON:CLICK @id,n]         button click events (click time < ms time)
//                                note: n is the number of button clicks
//   [BUTTON:HOLD @id,time]       button hold (hold time >= ms time)
//                                note: after grace time [BUTTON:HOLD 0]
//                                note: at button release [BUTTON:HOLD ms] with
//                                      ms as total hold time
//
//==============================================================================

  static int click(BL_ob *o, int val)
  {
    int id = o->id;                    // short hand for @id
    BL_ms now = bl_ms();

    if (id < 0 || id > N)
      return -1;                       // bad args

    switch (bl_id(o))
    {
      case BL_ID(_BUTTON,_PRESS_):     // [BUTTON:#PRESS]
      {
        time[id] = now;                // store button press time stamp
        BL_ob oo = {_BUTTON,_CLICK_,id,NULL};
        LOGO(4,BL_B,&oo,0);
        bl_hwbut(&oo,0);               // forward to post [BUTTON:#CLICK]
        return 0;
      }

      case BL_ID(_BUTTON,_RELEASE_):   // [BUTTON:#RELEASE]
        val = (now >= time[id] + ms);  // grace time exceeded? (return value)

        if (val)                       // grace time exceeded?
        {
          val = 0;                     // return value = 0
          int dt = (int)(now-time[id]);// hold time
          BL_ob oo = {_BUTTON,_HOLD_,id,NULL};
          LOGO(4,BL_B,&oo,dt);
          bl_hwbut(&oo,dt);            // forward to output [BUTTON:#HOLD @id,t]
        }
        else                           // within grace time!
        {
          val = 0;                     // return value = 0
          int count = 1;               // 1 click
          BL_ob oo = {_BUTTON,_CLICK_,id,NULL};
          LOGO(4,BL_B,&oo,count);
          bl_hwbut(&oo,count);         // forward to post [BUTTON:#CLICK @id,1]
        }

        time[id] = 0;                  // inactivate click tracking for @id
        hold[id] = 0;                  // clear button HOLD state
        return val;

      default:
        return -1;                     // bad arg
    }
  }

//==============================================================================
// button work horse - posts [BUTTON:PRESS @id 1] or [BUTTON:RELEASE @id 0]
// - IRS routine sets id (button ID) and submits (button) work, which invokes
// - workhorse to process [BUTTON:PRESS @id 1] or [BUTTON:RELEASE @id,0]
//==============================================================================

  static void workhorse(struct k_work *work)
  {
    if (id < 1 || id > N)
      return;                          // ignore out of range ID values

    int idx = id-1;

    int val = gp_pin_get(button+idx);  // read I/O pin input value

      // post button state to module interface for output

    if (1)
    {
      int dt = val ? 0 : (int)(bl_ms() - time[id]);
      BL_ob oo = {_BUTTON, val?_PRESS_:_RELEASE_, id,NULL};
      LOGO(4,BL_Y,&oo,dt);
      bl_hwbut(&oo,dt);                // post to module interface for output
    }

      // post switch status update to module interface for output

    if (val)                           // if button pressed
    {
      BL_ob oo = {_SWITCH,_STS_, id,NULL};
      val = toggle[idx] = !toggle[idx];

      LOGO(4,BL_Y,&oo,val);
       bl_hwbut(&oo,val);              // post to module interface for output
    }
  }

  K_WORK_DEFINE(work, workhorse);      // assign work with workhorse

//==============================================================================
// submit button work for button @id
//==============================================================================

  static void submit(int bid)
  {
    id = bid;                          // store global button ID
    k_work_submit(&work);              // invoke workhorse(), which picks id
  }

//==============================================================================
// provide button IRS callback (button pressed/released)
//==============================================================================

  static void irs1(const GP_device *dev, GP_ctx *ctx, GP_pins pins)
  {
    submit(1);
  }

  static void irs2(const GP_device *dev, GP_ctx *ctx, GP_pins pins)
  {
    submit(2);
  }

  static void irs3(const GP_device *dev, GP_ctx *ctx, GP_pins pins)
  {
    submit(3);
  }

  static void irs4(const GP_device *dev, GP_ctx *ctx, GP_pins pins)
  {
    submit(4);
  }

  static GP_irs irs[N] = {irs1,irs2,irs3,irs4};

//==============================================================================
// configure button
// - check whether device is ready
// - configure GPIO
//==============================================================================

  static void config(int id)
  {
    int idx = id - 1;                  // button index (0..3)
    if (idx < 0 || idx > 3)
       return;                         // bad arg

    if (!device_is_ready(button[idx].port))
    {
      LOG(1,BL_R"error: button device %s not ready\n", button[idx].port->name);
      return;
    }

    gp_pin_cfg(&button[idx], GPIO_INPUT | GPIO_INT_DEBOUNCE);
    gp_int_cfg(&button[idx], GPIO_INT_EDGE_BOTH);
    gp_add_cb(&button[idx], &context[idx], irs[idx]);

    //gpio_init_callback(&context, irs_button, BIT(button.pin));
    //gpio_add_callback(button.port, &context);

    LOG(5,"set up button[@%d] @ %s pin %d",
           id, button[idx].port->name, button[idx].pin);
  }

//==============================================================================
// ticking
//==============================================================================

  static int tick(BL_ob *o, int val)
  {
    BL_ms now = bl_ms();

    for (int id=0; id <= N; id++)
    {
      if ( time[id] )
      {
        int held = now - time[id];     // hold time
        if ( held >= ms && !hold[id] )
        {
          hold[id] = true;             // button @id entered HOLD state
          BL_ob oo = {_BUTTON,_HOLD_,id,NULL};
          LOGO(4,BL_B,&oo,0);
          bl_hwbut(&oo,0);            // forward to output [BUTTON:HOLD @id,0]
        }
      }
    }

    return 0;                      // OK
  }

//==============================================================================
// init
//==============================================================================

  static int init(BL_ob *o, int val)
  {
    LOG(4,BL_B "button init");

  	k_work_init(&work, workhorse);

    for (int i=1; i <= N; i++)
      config(i);
    return 0;
  }

//==============================================================================
// public module interface
//==============================================================================
//
// BL_HWBUT Interfaces:
//   SYS Interface:     [] = SYS(INIT)
//   BUTTON Interface:  [PRESS,RELEASE] = BUTTON(PRESS,RELEASE)
//   SWITCH Interface:  [STS] = SWITCH(STS)
//   SET Interface:     [] = SET(MS)
//
//                             +-------------+
//                             |  BL_HWBUT   |
//                             +-------------+
//                      INIT ->|    SYS:     |
//                      TICK ->|             |
//                             +-------------+
//                    #PRESS ->|   BUTTON:   |-> PRESS
//                  #RELEASE ->|             |-> RELEASE
//                    #CLICK ->|             |-> CLICK
//                     #HOLD ->|             |-> HOLD
//                             +-------------+
//                       STS ->|   SWITCH:   |-> STS
//                             +-------------+
//                        MS ->|    SET:     |
//                             +-------------+
//  Input Messages:
//    - [SYS:INIT <cb>]                // init module, provide output callback
//    - [SYS:TICK @id,cnt]             // tick module (for click/long detection)
//    - [BUTTON:PRESS @id,active]      // forward button press event to output
//    - [BUTTON:RELEASE @id,active]    // forward button release event to output
//    - [SWITCH:STS @id,onoff]         // forward switch status update to output
//    - [SET:MS ms]                    // set click grace time (in ms)
//
//  Output Messages:
//    - [BUTTON:PRESS @id 1]           // output a button press event
//    - [BUTTON:RELEASE @id 0]         // output a button release event
//    - [SWITCH:STS @id,onoff]         // output switch status update
//
//==============================================================================

  int bl_hwbut(BL_ob *o, int val)     // BUTTON core module interface
  {
    static BL_fct output = NULL;       // to store output callback

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):             // [SYS:INIT <cb>]
        output = o->data;                 // store output callback
      	return init(o,val);               // delegate to init() worker

      case BL_ID(_SYS,TICK_):             // [SYS:TICK @id,cnt]
      	return tick(o,val);               // delegate to tick() worker

      case BL_ID(_BUTTON,_PRESS_):        // [BUTTON:#PRESS @id]
      case BL_ID(_BUTTON,_RELEASE_):      // [BUTTON:#RELEASE @id]
        click(o,val);                     // forward to CLICK module
        return bl_out(o,val,output);      // post to output subscriber

      case BL_ID(_BUTTON,_CLICK_):        // [BUTTON:#CLICK @id,n]
      case BL_ID(_BUTTON,_HOLD_):         // [BUTTON:#HOLD @id,ms]
      case BL_ID(_SWITCH,_STS_):          // [SWITCH:#STS @id,onoff]
        return bl_out(o,val,output);      // post to output subscriber

      default:
	      return -1;                        // bad input
    }
  }
