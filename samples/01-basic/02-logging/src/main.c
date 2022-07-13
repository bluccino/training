//==============================================================================
// main.c for 02-logging demo
//==============================================================================

#include "bluccino.h"

#define LOG  LOG_TEST  // use LOG as a shorthand for predefined macro LOG_TEST

//==============================================================================
// log() function - does 4 loggings at different log levels and log colors
//==============================================================================

  static void log(void)
  {
    char *msg = "is a prime number";
    int n = 17;

    LOG(1,BL_M "number %d %s",n,msg);  // no comma between BL_M and "number ..."
    LOG(2,BL_G "number %d %s",n,msg);  // no comma between BL_G and "number ..."
    LOG(3,BL_B "number %d %s",n,msg);  // no comma between BL_B and "number ..."
    LOG(4,BL_Y "number %d %s",n,msg);  // no comma between BL_Y and "number ..."
  }

//==============================================================================
// our main program
//==============================================================================

  void main(void)
  {
    bl_hello(4,"02-logging (verbose level 4)");
    log();

    bl_hello(3,"02-logging (verbose level 3)");
    log();

    bl_hello(2,"02-logging (verbose level 2)");
    log();

    bl_hello(1,"02-logging (verbose level 1)");
    log();

    bl_hello(0,"02-logging (verbose level 0)");
    log();
  }
