//==============================================================================
// bl_hwled.c
// core LED functions
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================

  #include <drivers/gpio.h>

  #include "bluccino.h"
  #include "bl_hw.h"
  #include "bl_gpio.h"
  #include "bl_hwled.h"

  #define PMI bl_hwled            // public module interface

//==============================================================================
// logging shorthands
//==============================================================================

  #define WHO "bl_hwled:"          // who is logging

  #define LOG                     LOG_LED
  #define LOGO(lvl,col,o,val)     LOGO_LED(lvl,col WHO,o,val)

//==============================================================================
// defines
//==============================================================================

  #ifdef ONE_LED_ONE_BUTTON_BOARD      // overrules everything !!!
    #undef  CFG_NUMBER_OF_LEDS
    #define CFG_NUMBER_OF_LEDS     1
  #endif

  #ifndef CFG_NUMBER_OF_LEDS
    #define CFG_NUMBER_OF_LEDS     4
  #endif

  #define NLEDS  CFG_NUMBER_OF_LEDS

  #define LED0_NODE          DT_ALIAS(led0)
  #define LED1_NODE          DT_ALIAS(led1)
  #define LED2_NODE          DT_ALIAS(led2)
  #define LED3_NODE          DT_ALIAS(led3)

  #define N                  4                   // max number of LEDs

//==============================================================================
// locals
//==============================================================================

  static bool led_onoff[4] = {0,0,0,0};
  static const struct device *led_device[4];

//==============================================================================
// LED set  [SET:LED @id onoff]  // @id = 1..4
//==============================================================================

  static int led_set(BL_ob *o, int onoff)
  {
    if (o->id < 1 || o->id > NLEDS)
      return -1;                       // bad input

    switch (o->id)     // id 1..4 maps to led_device[0..3]
    {
      case 1:
        led_onoff[0] = onoff;
        gpio_pin_set(led_device[0], DT_GPIO_PIN(DT_ALIAS(led0), gpios),onoff);
        return 0;

      case 2:
        led_onoff[1] = onoff;
        gpio_pin_set(led_device[1], DT_GPIO_PIN(DT_ALIAS(led1), gpios),onoff);
        return 0;

      case 3:
        led_onoff[2] = onoff;
        gpio_pin_set(led_device[2], DT_GPIO_PIN(DT_ALIAS(led2), gpios),onoff);
        return 0;

      case 4:
        led_onoff[3] = onoff;
        gpio_pin_set(led_device[3], DT_GPIO_PIN(DT_ALIAS(led3), gpios),onoff);
        return 0;

      default:
        return -1;                     // bad input
    }
  }

//==============================================================================
// LED toggle
//==============================================================================

  static int led_toggle(BL_ob *o,int val)
  {
    if (o->id < 1 || o->id > NLEDS)
      return -1;                       // bad args

    val = (led_onoff[o->id-1] == 0);   // new LED value
    int ok = led_set(o,val);           // toggle LED state

    LOGO(4,BL_Y,o,val);                // log changed LED level
    return ok;
  }

//==============================================================================
// LED init
//==============================================================================

  static int sys_init(BL_ob *o, int val)
  {
  	  // LEDs configuration & setting

    LOG(4,BL_B "init %d LED%s",NLEDS, NLEDS==1?"":"s");

    #if (NLEDS >= 1)
    	led_device[0] = device_get_binding(DT_GPIO_LABEL(LED0_NODE, gpios));
    	gpio_pin_configure(led_device[0], DT_GPIO_PIN(LED0_NODE, gpios),
    			   DT_GPIO_FLAGS(LED0_NODE, gpios) |
    			   GPIO_OUTPUT_INACTIVE);
    #endif

    #if (NLEDS >= 2)
    	led_device[1] = device_get_binding(DT_GPIO_LABEL(LED1_NODE, gpios));
    	gpio_pin_configure(led_device[1], DT_GPIO_PIN(LED1_NODE, gpios),
    			   DT_GPIO_FLAGS(LED1_NODE, gpios) |
    			   GPIO_OUTPUT_INACTIVE);
    #endif

    #if (NLEDS >= 3)
    	led_device[2] = device_get_binding(DT_GPIO_LABEL(LED2_NODE, gpios));
    	gpio_pin_configure(led_device[2], DT_GPIO_PIN(LED2_NODE, gpios),
    			   DT_GPIO_FLAGS(LED2_NODE, gpios) |
    			   GPIO_OUTPUT_INACTIVE);
    #endif

    #if (NLEDS >= 4)
    	led_device[3] = device_get_binding(DT_GPIO_LABEL(LED3_NODE, gpios));
    	gpio_pin_configure(led_device[3], DT_GPIO_PIN(LED3_NODE, gpios),
    			   DT_GPIO_FLAGS(LED3_NODE, gpios) |
    			   GPIO_OUTPUT_INACTIVE);
    #endif

    return 0;                          // OK
  }

//==============================================================================
// public module interface
//==============================================================================
//
// (!) := (<parent>);
//                  +--------------------+
//                  |        LED         |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (!)->     INIT ->|       <out>        | init module, ignore <out> callback
//                  +--------------------+
//                  |        LED:        | LED interface
// (!)->      SET ->|      @id,onoff     | set LED's onoff state
// (!)->   TOGGLE ->|        @id         | toggle LED's onoff state
//                  +--------------------+
//
//==============================================================================

  int bl_hwled(BL_ob *o, int val)      // public module interface
  {
    switch (bl_id(o))
    {
      case SYS_INIT_0_cb_0:
      	return sys_init(o,val);        // delegate to sys_init() worker

      case LED_SET_id_0_onoff:
      {
        BL_ob oo ={o->cl,o->op,1,NULL};// change @id=0 -> @id=1
        if (o->id)
          LOGO(4,"@",o,val);

        o = o->id ? o : &oo;           // if (o->id==0) re-map o to &oo
	      return led_set(o,val != 0);    // delegate to led_set();
      }

      case LED_TOGGLE_id_0_0:
      {
        BL_ob oo = {o->cl,o->op,1,NULL};  // change @id=0 -> @id=1
        o = o->id ? o : &oo;           // if (o->id==0) re-map o to &oo
	      return led_toggle(o,val);      // delegate to led_toggle();
      }

      default:
	      return -1;                     // bad input
    }
  }

//==============================================================================
// cleanup (needed for *.c file merge of the bluccino core)
//==============================================================================

  #include "bl_cleanup.h"
