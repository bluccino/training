//==============================================================================
// main.c for 09-critical
//==============================================================================

  #include <string.h>
  #include "bluccino.h"                // access Bluccino stuff

  char buf[1000];

  void main(void)
  {
    BL_txt title = PROJECT " (disable/enable interrupts)";
    bl_hello(1,title);
    bl_init(bluccino,NULL);            // init Bluccino, no interest in output


    bl_log(1,BL_M "disable IRQs");
    bl_irq(0);                         // disable interrupts

    strcpy(buf,title);

    bl_irq(1);                         // enable interrupts
    bl_log(1,BL_G "enable IRQs");

    bl_log(1,"done: %s",buf);
  }
