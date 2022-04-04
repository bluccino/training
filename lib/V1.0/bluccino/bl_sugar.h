//==============================================================================
//  bl_sugar.h
//  syntactic sugars
//
//  Created by Hugo Pristauz on 2022-04-03
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_SUGAR_H__
#define __BL_SUGAR_H__

//==============================================================================
// syntactic sugar: set LED @id on/off or togggle LED @id (@id: 0..4)
// - usage: bl_led(id,val)   // val 0:off, 1:on, -1:toggle
//==============================================================================

  static inline int bl_led(int id, int val)
  {
    return bl_msg(bl_in,_LED,val<0?TOGGLE_:SET_, id,NULL, val<0?0:val);
  }

//==============================================================================
// syntactic sugar: check if message is a button press message ([BUTTON:PRESS])
// - usage: pressed = bl_pressed(o)
//==============================================================================

  static inline bool bl_pressed(BL_ob *o)
  {
    return  bl_is(o,_BUTTON,PRESS_);
  }

//==============================================================================
// syntactic sugar: check if message is a button release msg ([BUTTON:RELEASE])
// - usage: released = bl_released(o)
//==============================================================================

  static inline bool bl_released(BL_ob *o)
  {
    return  bl_is(o,_BUTTON,RELEASE_);
  }

//==============================================================================
// syntactic sugar: check if message is a switch status update ([SWITCH:STS])
// - usage: switched = bl_switched(o)
//==============================================================================

  static inline bool bl_switched(BL_ob *o)
  {
    return  bl_is(o,_SWITCH,STS_);
  }

//==============================================================================
// syntactic sugar: store value to non volatile memory (NVM)
// - usage: bl_store(id,val)           // store value at NVM location @id
//==============================================================================

  static inline int bl_store(int id, int val)
  {
    return bl_msg(bl_in,_NVM,STORE_, id,NULL,val);
  }

//==============================================================================
// syntactic sugar: recall value from non volatile memory (NVM)
// - usage: val = bl_recall(id)        // recall value from NMV location @id
//==============================================================================

  static inline int bl_recall(int id)
  {
    return bl_msg(bl_in,_NVM,RECALL_, id,NULL,0);
  }

//==============================================================================
// syntactic sugar: send generic on/off SET message via mesh (using GOOCLI @id)
// - usage: val = bl_gooset(id,onoff)  // (BL_IN)<-[GOOCLI:SET @id,<NULL>,onoff]
//==============================================================================

  static inline int bl_gooset(int id, int onoff)
  {
    return bl_msg(bl_in,_GOOCLI,SET_, id,NULL,onoff);
  }

//==============================================================================
// syntactic sugar: send generic on/off LET message via mesh (using GOOCLI @id)
// - usage: val = bl_goolet(id,onoff)  // (BL_IN)<-[GOOCLI:LET @id,onoff]
//==============================================================================

  static inline int bl_goolet(int id, int onoff)
  {
    return bl_msg(bl_in,_GOOCLI,LET_, id,NULL,onoff);
  }

//==============================================================================
// syntactic sugar: send generic on/off GET message via mesh (using GOOCLI @id)
// - usage: val = bl_googet(id)       // (BL_IN)<-[GOOCLI:GET @id]
//==============================================================================

  static inline int bl_googet(int id)
  {
    return bl_msg(bl_in,_GOOCLI,GET_, id,NULL,0);
  }

#endif // __BL_SUGAR_H__
