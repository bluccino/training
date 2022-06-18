//==============================================================================
// main.c for 01-ledmsg (basic Bluccino messaging - stage 1)
//==============================================================================
//
// Event Flow Chart
//
//           CLOCK                   SOS                    LED
//             |   [LED_TOGGLE,1]     |                      |
//             o-------------------------------------------->|
//             |                      |                      |
//             o-------------------------------------------->|
//             |                      |                      |
//             :                      :                      :
//             |                      | [LED_SET,1,true]     |
//             |                      o--------------------->|
//             |                      |                      |
//             |                      | [LED_SET,1,false]    |
//             |                      o--------------------->|
//             |                      |                      |
//             :                      :                      :
//
//==============================================================================

  #include "bluccino.h"                // access Bluccino stuff

  #define LED_SET     0x0001           // some fantasy message ID
  #define LED_TOGGLE  0x0002           // some other fantasy message ID

//==============================================================================
// LED "module", understands two "messages"
// - [LED_SET,id,onoff]    // set LED @id on or off (depending on onoff value)
// - [LED_TOGGLE,id]       // toggle LED @id
//==============================================================================

  void led_set(int id, bool onoff)
  {
    bl_led(id, onoff);                 // turn LED @id on/off
  }

  void led_toggle(int id)
  {
    bl_led(id, -1);                    // toggle LED @id
  }

//==============================================================================
// CLOCK "modue"
//==============================================================================

  void clock(void)                     // run clock module
  {
    for (int i=0; i < 10; i++)
    {
      led_toggle(1);                   // toggle LED @led_id
      bl_sleep(500);                   // sleep 500ms
    }
  }

//==============================================================================
// SOS "module"
//==============================================================================

  void sos(void)                       // run clock module
  {
    BL_txt pattern = "*-*-*--***--***--***--*-*-*------";
    BL_txt p = pattern;

    for (; *p; p++)
    {
      p = *p ? p : pattern;            // re-start pattern, if end of pattern
      led_set(1, (*p=='*'));           // toggle LED @led_id
      bl_sleep(500);                   // sleep 500ms
    }
  }

//==============================================================================
// main function
//==============================================================================

  void main(void)
  {
    bl_hello(4,"01-ledmsg - (basic Bluccino messaging - stage 1)");
    bl_init(bluccino,NULL);
    clock();                           // run 10 toggle cycles
    sos();                             // after this run SOS pattern forever
  }
