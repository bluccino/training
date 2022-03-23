//==============================================================================
// bl_basis.h
// basis functions of a mesh node (startup, provision, attention)
//
// Created by Hugo Pristauz on 2022-Feb-21
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

#ifndef __BL_BASIS_H__
#define __BL_BASIS_H__

//==============================================================================
// public module interface
//==============================================================================
//
// BL_BASE Interfaces:
//   SYS Interface:      [] = SYS(INIT,TICK)
//   GET Interface:      [] = GET(PRV,ATT,BUSY)
//   SET Interface:      [] = SET(PRV,ATT)
//   HDL Interface:      [] = HDL(INC)
//   BUTTON Interface:   [] = BUTTON(PRESS)
//   LED Interface:      [LED] = HDL()
//   RESET Interface:    [INC,PRV] = RESET(INC,DUE)
//
//                          +-----------------+
//                          |     BL_BASE     |
//                          +-----------------+
//                   INIT ->|      SYS:       |
//                   TICK ->|                 |
//                          +-----------------+
//                    PRV ->|      GET:       |
//                    ATT ->|                 |
//                   BUSY ->|                 |
//                          +-----------------+
//                    PRV ->|      SET:       |
//                    ATT ->|                 |
//                          +-----------------+
//                    INC ->#      HDL:       |
//                          +-----------------+
//                  PRESS ->|     BUTTON:     |
//                          +-----------------+
//                          |       LED:      |-> LED
//                          +-----------------+
//                    INC ->|     RESET:      |-> INC
//                    DUE ->|                 |-> PRV
//                          +-----------------+
//
//==============================================================================

  int bl_basis(BL_ob *o, int val);

#endif // __BL_BASIS_H__
