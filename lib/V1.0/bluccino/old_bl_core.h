// bl_core.h - mesh/HW core interface

#ifndef __BL_CORE_H__
#define __BL_CORE_H__

#include "sdk.h"

  void bl_core_set(BL_ob *o, int value);
  void bl_core_led(BL_ob *o, int value);         // set LED on/off

  void bl_core_init(void);
  void bl_core_loop(void);

#endif // __BL_CORE_H__
