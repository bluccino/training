//==============================================================================
// main.c for 02-funny (basic Bluccino messaging - stage 1)
//==============================================================================
//
//        +-----------------+                       +-----------------+
//        |                 |    [SINGLE:SET]       |                 |
//        |      FUNNY      |---------------------->|       LED       |
//        |                 |                       |                 |
//        +-----------------+                       +-----------------+
//
//==============================================================================
//
// Event Flow Chart
//
//                 FUNNY                                 LED
//                   |                                    |
//      +------------v------------+                       |
//      | switch LED on  at t==3  |                       |
//      | switch LED off at t==5  |                       |
//      +------------v------------+                       |
//                   |                                    |
//              t==3 -      [SINGLE:SET @1,true]          |
//                   o----------------------------------->|
//                   |                                    |
//              t==5 -      [SINGLE:SET @1,false]         |
//                   o----------------------------------->|
//                   |                                    |
//      +------------v------------+                       |
//      | after t=10s toggle LED  |                       |
//      | forever with 1s period  |                       |
//      +------------v------------+                       |
//                   |                                    |
//             t==10 -       [SINGLE:TOGGLE @1]           |
//                   o----------------------------------->|
//                   |                                    |
//             t==11 -       [SINGLE:TOGGLE @1]           |
//                   o----------------------------------->|
//                   |                                    |
//                   :                                    :
//
//==============================================================================

  #include "bluccino.h"                // access Bluccino stuff

  #define SINGLE      1                // message class SINGLE
  #define MULTI       2                // message class MULTI

  #define TOGGLE      1                // opcode for SINGLE:TOGGLE message ID
  #define SET         2                // opcode for SINGLE:SET message ID

  #define ID(cl,op)   ((cl<<16)|op)    // compound message ID

  typedef struct ST_multi
          {
            bool mask[4];              // access array
          } ST_multi;

//==============================================================================
// LED "module", understands two "messages"
// - [LED:SET @id,onoff]   // set LED @id on or off (depending on onoff value)
// - [LED:TOGGLE @id]      // toggle LED @id
//==============================================================================

  void led(int cl, int op, int id, void *data, int val)
  {
    switch (ID(cl,op))                 // dispatch message ID
    {
      case ID(SINGLE,SET):
        bl_led(id,val);
        return;

      case ID(SINGLE,TOGGLE):
        bl_led(id,-1);
        return;
    }
  }

//==============================================================================
// Funny module
// - funny module turns LED @1 on after 3s and off after 5 seconds
// - after 10s funny module toggles LED @1 with 1s period
//==============================================================================

  void funny(void)                     // a FUNNY module
  {
    for (int i=0;;i++)                 // forever with 1s period
    {
      if (i == 3)
        led(SINGLE,SET, 1,NULL,true);  // turn single LED @1 on
      if (i == 5)
        led(SINGLE,SET, 1,NULL,false); // turn single LED @1 off
      if (i >= 10)
        led(SINGLE,TOGGLE, 1,NULL,0);  // toggle single LED @1

      bl_sleep(1000);                  // sleep 1s
    }
  }

//==============================================================================
// main function
//==============================================================================

  void main(void)
  {
    bl_hello(4,"02-ledmsg - (basic Bluccino messaging - stage 1)");
    bl_init(bluccino,NULL);
    funny();                           // run our funny module
  }
