//==============================================================================
// main.c for 06-rgbcycle (tiny RGB LED cycle control by buttons)
//==============================================================================

  #include "bluccino.h"

  static int when(BL_ob *o, int val)
  {
    static int id = 5;                 // RGB color index (R:@2, G:@3, B:@4)
                                       // id=5 => no LED (invalid, dark)
    if ( bl_press(o) )                 // [SWITCH:STS @id]
    {
      bl_led(id,0);                    // set LED(@id) off
      id = (id >= 5) ? 2 : id+1;       // next RGB color (R:@2, G:@3, B:@4)
      bl_led(id,1);                    // set LED(@id) on
    }
    return 0;                          // OK
  }

  void main(void)
  {
    bl_hello(4,"06-rgbcycle (click any button to activate another LED");
    bl_run(NULL,10,100,when);          // run - output goes to when()
  }
