//==============================================================================
// main.c for 02-blink (tiny LED toggle control by buttons)
//==============================================================================

  #include "bluccino.h"

  void main(void)
  {
    bl_init(bluccino,NULL);            // init Bluccino, no interest in output

    for(;;bl_sleep(1000))              // forever (sleep 1000ms in between)
      bl_led(1,-1);                    // toggle LED @1 status
  }
