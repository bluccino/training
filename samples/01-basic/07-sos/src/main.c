//==============================================================================
// main.c for 06-sos (SOS - rapid prototyping demo)
//==============================================================================
//
// SOS app controls the blinking pattern of one selected LED of a multi LED
// board (like Nordic NRF52DK_NRF52832 development kit) according to the
// following blinking pattern:
//
// LEVEL
//  ^
//  |== +== +==   +======     +======     +======   +== +== +==        +==
//  | | | | | |   |     |     |     |     |     |   | | | | | |        | |
// -+-+==-+==-+====-----+======-----+======-----+====-+==-+==-+===...===-+=...>
//
// The tasks of the application are assigned to two modules:
//
// - module SOS: deals with timing (derived from a timed TICK input) and gene-
//   rates a sequence of binary levels according to the SOS pattern. Input INIT
//   is used to select one of the multiple LEDs at initial time for performing
//   the blinbk task
//
// - module LED: receives LEVEL events and executes it as on/off switch actions
//   of the configured LED.
//
//                    +-------------+         +-------------+
//             INIT -->             |  LEVEL  |             |
//                    |     SOS     |--------->     LED     |
//    (500 ms) TICK -->             |         |             |
//                    +-------------+         +-------------+
//
// main taks:
// - 1) setup a main engine with 500 ms period tick generatiom
// - 2) main engine to call Bluccino and app initializers
// - 3) main engine to call Bluccino ticker and app tick() function
// - 4) app init() to init SOS and LED module
// - 5) app init() to setup message forwarding from SOS module to LED module
// - 6) app tick() to tick SOS module
//
//==============================================================================

  #include "bluccino.h"
  #include "sos.h"
  #include "led.h"

//==============================================================================
// tick function (sends tick messages to all modules which have to be ticked)
//==============================================================================

  static int tick(BL_ob *o, int val)   // system ticker: ticks all subsystems
  {
     bl_logo(1,"main",o,val);          // log to see we are alife
     return sos_tick(val);             // SOS module needs ticking
  }

//==============================================================================
// app init
// - 1) init all modules of app (note: Bluccino init is done in main engine!)
// - 2) setup connections: all outputs of SOS module have to go to LED module
//==============================================================================

  static void init(void)               // init all modules
  {
    led_init(1);                       // init LED module, act on LED @1
    sos_init(led);                     // init SOS module, output to LED module
  }

//==============================================================================
// main engine
// - calling Bluccino init and app init() function
// - periodic (500ms) calls of app tick() function
//==============================================================================

  void main(void)
  {
    bl_hello(3,"06-sos demo");         // set verbose level 3 & print hello msg

    bl_init(bluccino,NULL);            // Bluccino init
    init();                            // app init

    for(int count=0;;count++)          // loop generating (approx) 500ms ticks
    {
      bl_tick(tick,0,count);           // app tick
      bl_sleep(500);                   // sleep 500 ms
    }
  }
