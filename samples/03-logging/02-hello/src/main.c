//==============================================================================
// main.c for 01-hello demo (hello world)
//==============================================================================

  #include "bluccino.h"                // access Bluccino stuff

  void main(void)                      // main() for 01-hello demo (hello world)
  {
    bl_rtl_init();
//    bl_prt("hello, world!\n");
    BL_LOG(1,"hello, world!");
  }
