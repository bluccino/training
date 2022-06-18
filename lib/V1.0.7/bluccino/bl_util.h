//==============================================================================
//  bl_util.h
//  utilities
//
//  Created by Hugo Pristauz on 2022-06-18
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_UTIL_H__
#define __BL_UTIL_H__

//==============================================================================
// get a random int value (passes general randomness tests)
// - usage: val = bl_rand()  // get random number (not cryptocally secure)
//==============================================================================

  static inline int bl_rand(void)
  {
    return sys_rand32_get();
  }

#endif // __BL_UTIL_H__
