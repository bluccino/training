//==============================================================================
// led.c - controlling a single LED (LED @1)
//==============================================================================

  #include "bluccino.h"
	#include "bl_gpio.h"

  #define LED0_NODE          DT_ALIAS(led0)

  static const GP_device *led_dev;

//==============================================================================
// worker: set - [SET:LED onoff]
//==============================================================================

  static int set(BL_ob *o, int val)
  {
    gpio_pin_set(led_dev, DT_GPIO_PIN(LED0_NODE, gpios),val!=0);
    return 0;
  }

//==============================================================================
// woker: init - [SYS:INIT <out>]
//==============================================================================

  static int init(BL_ob *o, int val)
  {
    bl_log(3,BL_B "init LED @1");

    led_dev = device_get_binding(DT_GPIO_LABEL(LED0_NODE, gpios));

    gpio_pin_configure(led_dev, DT_GPIO_PIN(LED0_NODE, gpios),
    		       DT_GPIO_FLAGS(LED0_NODE, gpios) | GPIO_OUTPUT_INACTIVE);
    return 0;
  }

//==============================================================================
// public module interface
//==============================================================================
//
// (A) := (APP);
//
//                  +--------------------+
//                  |        LED         |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (A)->     INIT ->|       <out>        | init module, ignore <out> callback
//                  +--------------------+
//                  |        LED:        | LED interface
// (A)->      SET ->|      @1,onoff      | set LED's onoff state
//                  +--------------------+
//
//==============================================================================

  int led(BL_ob *o, int val)           // public module interface
  {
    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>]
      	return init(o,val);            // delegate to init() worker

      case BL_ID(_LED,SET_):           // [LED:set onoff]
	return set(o,val);             // delegate to set() worker

      default:
	return -1;                     // bad input
    }
  }
