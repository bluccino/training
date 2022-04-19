//==============================================================================
// bl_hwbut.c
// Bluccino HW core supporting basic functions for button & LED
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================

  #include "bluccino.h"
	#include "bl_hw.h"
  #include "bl_hwbut.h"
  #include "bl_gpio.h"

//==============================================================================
// BUTTON level logging shorthands
//==============================================================================

  #define LOG                     LOG_BUTTON
  #define LOGO(lvl,col,o,val)     LOGO_BUTTON(lvl,col"button:",o,val)

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

  static BL_word mask = 0xFFFF;        // all button events enabled
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
// (B) := (bl_hwbut);
//
//                  +--------------------+
//                  |        click       | module click
//                  +--------------------+
//                  |       BUTTON:      | BUTTON interface
// (B)<-    CLICK <-|     @id,clicks     | button click (click time < ms)
// (B)<-     HOLD <-|       @id,ms       | button hold (hold time >= ms)
//                  |....................|
//                  |      #BUTTON:      | internal BUTTON interface
// (#)->    PRESS ->|       @id,0        | receive button press event
// (#)->  RELEASE ->|       @id,ms       | receive button release event
//                  +--------------------+
//
//==============================================================================

  static int click(BL_ob *o, int val)
  {
		static BL_oval B = bl_hwbut;

    int id = o->id;                    // short hand for @id
    BL_ms now = bl_ms();

    if (id < 0 || id > N)
      return -1;                       // bad args

    switch (bl_id(o))
    {
      case _BL_ID(_BUTTON,PRESS_):     // [#BUTTON:PRESS]
      {
        time[id] = now;                // store button press time stamp
				if (mask & BL_CLICK)
				{
          LOG(4,BL_B "button click (begin)");
				  _bl_msg((B), _BUTTON,CLICK_, id,NULL,0);
				}
        return 0;
      }

      case _BL_ID(_BUTTON,RELEASE_):   // [#BUTTON:RELEASE]
        val = (now >= time[id] + ms);  // grace time exceeded? (return value)

        if (val)                       // grace time exceeded?
        {
          val = 0;                     // return value = 0
          if (mask & BL_HOLD)
					{
            BL_ms dt = now - time[id]; // hold time
            LOG(4,BL_B "button hold event");
  				  _bl_msg((B), _BUTTON,HOLD_, id,NULL,(int)dt);
					}
        }
        else                           // within grace time!
        {
          val = 0;                     // return value = 0
          if (mask & BL_CLICK)
          {
            int count = 1;             // 1 click
            LOG(4,BL_B "button clicked %d times",count);
            _bl_msg((B), _BUTTON,CLICK_, id,NULL,count);
          }
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

    //LOG(4,BL_Y"button @%d %s",id,val?"press":"release");

    if (val)                           // [BUTTON:PRESS 0] event
		{
      if (mask & BL_PRESS)
        _bl_msg(bl_hwbut,_BUTTON,PRESS_, id,NULL,0);

      _bl_msg(click,_BUTTON,PRESS_, id,NULL,0);
      toggle[idx] = !toggle[idx];
      if (mask & BL_SWITCH)
        _bl_msg(bl_hwbut,_SWITCH,STS_, id,NULL,toggle[idx]);
    }
    else                               // [BUTTON:RELEASE ms] event
    {
      int dt = (int)(bl_ms() - time[id]);
      if (mask & BL_RELEASE)
        _bl_msg(bl_hwbut,_BUTTON,RELEASE_, id,NULL,dt);

      _bl_msg(click,_BUTTON,RELEASE_, id,NULL,dt);
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
					if (mask & BL_HOLD)
					{
            LOG(4,BL_Y "button hold (begin)");
            _bl_msg(bl_hwbut,_BUTTON,HOLD_, id,NULL,0);
					}
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
// (!) := (<parent>);  (#) := (bl_hwbut);  (U) := (bl_up);  (D) := (bl_down);
//
//                  +--------------------+
//                  |      bl_hwbut      | module bl_hwbut
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (!)->     INIT ->|       <out>        | init module, ignore <out> callback
// (!)->     TICK ->|       @id,cnt      | tick module
//                  +--------------------+
//                  |       BUTTON:      | BUTTON interface
// (U)<-    PRESS <-|        @id,0       | button press at time 0
// (U)<-  RELEASE <-|        @id,ms      | button release after elapsed ms-time
// (U)<-    CLICK <-|        @id,n       | number of button clicks
// (U)<-     HOLD <-|       @id,ms       | button hold event at ms-time
// (!)->      CFG ->|        mask        | config button event mask
//                  +--------------------+
//                  |       SWITCH:      | SWITCH interface
// (U)<-      STS <-|       @id,sts      | emit status of toggle switch event
//                  +--------------------+
//                  |        SET:        | SET interface
// (D)->       MS ->|         ms         | set grace time for click/hold events
//                  +====================+
//                  |      #BUTTON:      | private #BUTTON interface
// (#)->    PRESS ->|        @id,0       | button press at time 0
// (#)->  RELEASE ->|        @id,ms      | button release after elapsed ms-time
// (#)->    CLICK ->|        @id,n       | number of button clicks
// (#)->     HOLD ->|        @id,ms      | button hold event at ms-time
//                  +--------------------+
//                  |      #SWITCH:      | private #SWITCH interface
// (#)->      STS ->|       @id,sts      | emit status of toggle switch event
//                  +--------------------+
//
//==============================================================================

  int bl_hwbut(BL_ob *o, int val)         // BUTTON core module interface
  {
    static BL_oval U = bl_up;             // to store output callback

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):             // [SYS:INIT <cb>]
        U = bl_cb(o,(U),"bl_hwbut:(U)");  // store output callback
      	return init(o,val);               // delegate to init() worker

      case BL_ID(_SYS,TICK_):             // [SYS:TICK @id,cnt]
      	return tick(o,val);               // delegate to tick() worker

      case BL_ID(_BUTTON,CFG_):           // [BUTTON:CFG mask] config event mask
			  mask = (BL_word)val;              // store event mask
      	return 0;                         // OK

        // internal interfaces

      case _BL_ID(_BUTTON,PRESS_):        // [#BUTTON:PRESS @id,0]
      case _BL_ID(_BUTTON,RELEASE_):      // [#BUTTON:RELEASE @id,time]
        return bl_out(o,val,(U));         // post to output subscriber

      case _BL_ID(_BUTTON,CLICK_):        // [#BUTTON:CLICK @id,n]
      case _BL_ID(_BUTTON,HOLD_):         // [#BUTTON:HOLD @id,ms]
        return bl_out(o,val,(U));         // post to output subscriber

      case _BL_ID(_SWITCH,STS_):          // [#SWITCH:STS @id,sts]
        return bl_out(o,val,(U));         // post to output subscriber

      default:
	      return -1;                        // bad input
    }
  }
