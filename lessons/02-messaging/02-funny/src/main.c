//==============================================================================
// main.c for 02-funny (basic Bluccino messaging - stage 1)
//==============================================================================
//
//        +-----------------+                       +-----------------+
//        |                 |     [MULTI:TOGGLE]    |                 |
//        |      FUNNY      |---------------------->|       LED       |
//        |                 |       <MT_multi>      |                 |
//        +-----------------+                       +-----------------+
//
//==============================================================================
//
// Event Flow Chart
//
//                 FUNNY                                 LED
//                   |                                    |
//                   |      [MULTI:TOGGLE <{1,0,0,1}>]    |
//                   o----------------------------------->|
//                   |                                    |
//                   |                                    |
//              t==1 -      [MULTI:TOGGLE <{1,1,1,1}>]    |
//                   o----------------------------------->|
//                   |                                    |
//              t==2 -      [MULTI:TOGGLE <{1,1,1,1}>]    |
//                   o----------------------------------->|
//                   :                                    :
//
//==============================================================================

  #include "bluccino.h"                // access Bluccino stuff

  #define MULTI       2                // message class MULTI
  #define TOGGLE      2                // opcode for MULTI:TOGGLE message ID

  #define ID(cl,op)   ((cl<<16)|op)    // compound message ID

  typedef struct MT_mask
	{
	  bool mask[4];
	} MT_mask;

//==============================================================================
// LED module understands:
// - [MULTI:TOGGLE @id,onoff]  // set LED @id on or off (depending on onoff value)
//==============================================================================
//
// (F) := (FUNNY)
//                  +--------------------+
//                  |        LED         |
//                  +--------------------+
//                  |       MULTI:       | MULTI input interface
// (F)->   TOGGLE ->|     <MT_mask>      | multi toggle of selected LEDs
//                  +--------------------+
//
//==============================================================================

  void led(int cl, int op, MT_mask *data)
  {
    if ( ID(cl,op) == ID(MULTI,TOGGLE) )
    {
      for (int id=1; id <= BL_LEN(data->mask); id++)
	if (data->mask[id-1])
	  bl_led(id,-1);                   // toggle LED @id
    }
  }

//==============================================================================
// Funny module
// - funny module turns LED @1 on after 3s and off after 5 seconds
// - after 10s funny module toggles LED @1 with 1s period
//==============================================================================
//
// (L) := (LED)
//                  +--------------------+
//                  |       FUNNY        |
//                  +--------------------+
//                  |       MULTI:       | MULTI output interface
// (L)<-   TOGGLE <-|     <MT_mask>      | toggle multiple LEDs
//                  +--------------------+
//
//==============================================================================

  void funny(void)                     // a FUNNY module
  {
    MT_mask ini = {mask:{1,0,0,1}};    // initializing mask
    MT_mask run = {mask:{1,1,1,1}};    // mask during permanent run

    led(MULTI,TOGGLE, &ini);           // toggle multiple LEDs
    for (int i=0; ;i++)                // forever with 1s period
    {
      bl_sleep(1000);                  // sleep 1s
      led(MULTI,TOGGLE, &run);         // toggle multiple LEDs
    }
  }

//==============================================================================
// main function
//==============================================================================

  void main(void)
  {
    bl_hello(4,"02-funny - (basic Bluccino messaging - stage 1)");
    bl_init(bluccino,NULL);
    funny();                           // run our funny module
  }
