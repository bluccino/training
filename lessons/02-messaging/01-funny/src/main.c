//==============================================================================
// main.c for 01-funny (basic Bluccino messaging - stage 1)
//==============================================================================
//
//        +-----------------+                       +-----------------+
//        |                 |     [SINGLE:SET]      |                 |
//        |      funny      |---------------------->|       led       |
//        |                 |      @id,onooff       |                 |
//        +-----------------+                       +-----------------+
//
//==============================================================================
//
// Event Flow Chart
//
//                 funny                                 led
//                  (F)                                  (L)
//                   |                                    |
//              t==3 -      [SINGLE:SET @1,true]          |
//                   o----------------------------------->|
//                   |                                    |
//              t==5 -      [SINGLE:SET @1,false]         |
//                   o----------------------------------->|
//                   |                                    |
//
//==============================================================================

  #include "bluccino.h"                // access Bluccino stuff

  #define SINGLE      1                // message class SINGLE
  #define SET         1                // opcode for SINGLE:SET message ID

  #define ID(cl,op)   ((cl<<16)|op)    // compound message ID

//==============================================================================
// LED module understands:
// - [SINGLE:SET @id,onoff]  // set LED @id on or off (depending on onoff value)
//==============================================================================
//
// (F) := (funny)
//                  +--------------------+
//                  |        led         | led module
//                  +--------------------+
//                  |       SINGLE:      | SINGLE input interface
// (F)->      SET ->|      @id,onoff     | turn single LED @id on or off
//                  +--------------------+
//
//==============================================================================

  void led(int cl, int op, int id, int onoff)
  {
    if ( ID(cl,op) == ID(SINGLE,SET) )
      bl_led(id,onoff);
  }

//==============================================================================
// Funny module
// - funny module turns LED @1 on after 3s and off after 5 seconds
// - after 10s funny module toggles LED @1 with 1s period
//==============================================================================
//
// (L) := (led)
//                  +--------------------+
//                  |       funny        | funny module
//                  +--------------------+
//                  |      #SINGLE:      | SINGLE output interface
// (L)<-      SET <-|      @id,onoff     | turn single LED @id on or off
//                  +--------------------+
//
//==============================================================================

  void funny(void)                     // funny module
  {
    for (int i=0;;bl_sleep(1000),i++)  // forever with 1s period
    {
      if (i == 3)
        led(SINGLE,SET, 1,true);       // turn single LED @1 on
      if (i == 5)
        led(SINGLE,SET, 1,false);      // turn single LED @1 off
    }
  }

//==============================================================================
// main function
//==============================================================================

  void main(void)
  {
    bl_hello(4,PROJECT " - (basic Bluccino messaging - stage 1)");
    bl_init(bluccino,NULL);            // init bluccino cores/drivers
    funny();                           // run our funny module
  }
