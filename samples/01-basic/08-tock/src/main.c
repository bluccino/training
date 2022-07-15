//==============================================================================
// main.c for 08-tock demo (hello Bluccino)
//==============================================================================

#include "bluccino.h"

//==============================================================================
// TEST level logging shorthands
//==============================================================================

  #define LOG                     LOG_TEST
  #define LOGO(lvl,col,o,val)     LOGO_TEST(lvl,col"main:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_TEST(lvl,col,o,val)

//==============================================================================
// tock
//==============================================================================

  static int tock(BL_ob *o, int val)
  {
    BL_ob oo = {_LED,SET_,1,NULL};
    LOGO(4,BL_B,o,val);

    if (val % 2 == 0)
    {
      LOG(1,BL_G "LED on");
      bl_down(&oo,1);
    }
    else
    {
      LOG(1,BL_M "LED off");
      bl_down(&oo,0);
    }

    if (val % 10 == 0)
      LOG(1,BL_R "I am alive :-)");

    return 0;                          // OK
  }

//==============================================================================
// init
//==============================================================================

  static int init(BL_ob *o, int val)
  {
    LOGO(2,BL_C,o,val);
    return 0;                          // OK
  }

//==============================================================================
// main program
//==============================================================================

  void main(void)
  {
    bl_hello(2,"08-tock");

    LOG(1,BL_R "Hallo Julian");
    LOG(2,BL_G "Hallo Julian");
    LOG(3,BL_B "Hallo Julian");
    LOG(4,BL_Y "Hallo Julian");

      // init bluccino

    bl_init(bluccino,NULL);
    bl_init(init,NULL);

    for (int tocks=0;;tocks++)
    {
      bl_tock(tock,0,tocks);
      bl_sleep(1000);
    }
  }
