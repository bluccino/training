//==============================================================================
// main.c for 01-random
//==============================================================================

  #include "bluccino.h"                // access Bluccino stuff
  #include "bl_util.h"                 // access random function

  void main(void)
  {
    bl_hello(1,PROJECT " (print random numbers)");
    bl_init(bluccino,NULL);            // init Bluccino, no interest in output

    bl_prt("random integers: bl_rand(0)\n");

    for(int i=1;i<=20;i++)             // print 20 random integers
    {
      int n = bl_rand(0);              // get random integer
      int x = n % 10;
      bl_prt("   #%2d: %d (modulo 10: %d)\n", i, n, x);
    }

    bl_prt("\nrandom values 0..9: bl_rand(10)\n");

    for(int i=1;i<=50;i++)             // print 50 random numbers in range 0..9
    {
      int n = bl_rand(10);             // get random number (0..9)
      bl_prt("   #%2d: %d\n", i, n);
    }
  }
