//==============================================================================
//  bl_gear.h
//  Bluccino gear
//
//  Created by Hugo Pristauz on 2021-11-06
//  Copyright © 2021 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_GEAR_H__
#define __BL_GEAR_H__

//==============================================================================
// event message output (message emission of a module)
// - usage: bl_out(o,val,(to))  // output to given module
// - important note: class tags are automatically un-augmented before posting
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================

  int bl_out(BL_ob *o, int val, BL_oval to);

//==============================================================================
// augmented event message output (message emission of a module)
// - usage: _bl_out(o,val,(to))   // output to given module
// - important note: class tags are automatically augmented before posting
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================

  int _bl_out(BL_ob *o, int val, BL_oval to);

//==============================================================================
// gear upward/downward and top gear interface
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================

  int bl_top(BL_ob *o, int val);       // top gear
  int bl_up(BL_ob *o, int value);      // upward gear
  int bl_down(BL_ob *o, int value);    // downward gear

#endif // __BL_GEAR_H__
