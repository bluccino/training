//==============================================================================
//  bl_sugar.h
//  syntactic sugars
//
//  Created by Hugo Pristauz on 2022-04-03
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_SUGAR_H__
#define __BL_SUGAR_H__

  #include "bl_hw.h"

//==============================================================================
// syntactic sugar: event message forwading
// - usage: bl_fwd(o,val,(to))  // forward to given module
// - similar function to bl_out() without un-augmenting feature and NULL check
//==============================================================================

  static inline int bl_fwd(BL_ob *o, int val, BL_oval to)
  {
    return to(o,val);     // it's PURE syntactic sugar
  }

//==============================================================================
// syntactic sugar: set LED @id on/off or togggle LED @id (@id: 0..4)
// - usage: bl_led(id,val)   // val 0:off, 1:on, -1:toggle
//==============================================================================

  static inline int bl_led(int id, int val)  // (BL_DOWN) <- [LED:op onoff]
  {
    BL_id mid = (val >= 0 ? LED_SET_id_0_onoff : LED_TOGGLE_id_0_0);
    return bl_post((bl_down),mid, id,NULL, val<0?0:val);
  }

//==============================================================================
// syntactic sugar: augmented set LED @id on/off or togggle LED @id (@id: 0..4)
// - usage: bl_led(id,val,(to))   // val 0:off, 1:on, -1:toggle
//==============================================================================

  static inline int _bl_led(int id, int val, BL_oval to) // (to)<-[LED:op onoff]
  {
    BL_id mid = (val >= 0 ? LED_SET_id_0_onoff : LED_TOGGLE_id_0_0);
    return bl_post((to),mid, id,NULL, val<0?0:val);
  }

//==============================================================================
// syntactic sugar: check if message is a button press message ([BUTTON:PRESS])
// - usage: pressed = bl_pressed(o)
//==============================================================================

  static inline bool bl_pressed(BL_ob *o)
  {
    return  (bl_id(o) == BUTTON_PRESS_id_0_0);
  }

//==============================================================================
// syntactic sugar: check if message is a button release msg ([BUTTON:RELEASE])
// - usage: released = bl_released(o)
//==============================================================================

  static inline bool bl_released(BL_ob *o)
  {
    return  (bl_id(o) == BUTTON_RELEASE_id_0_ms);
  }

//==============================================================================
// syntactic sugar: check if message is a switch status update ([SWITCH:STS])
// - usage: switched = bl_switched(o)
//==============================================================================

  static inline bool bl_switched(BL_ob *o)
  {
    return  (bl_id(o) == SWITCH_STS_id_0_sts);
  }

//==============================================================================
// syntactic sugar: store value to non volatile memory (NVM)
// - usage: bl_store(id,val)           // store value at NVM location @id
//==============================================================================

  static inline int bl_store(int id, int val)
  {
    return bl_msg(bl_down,_NVM,STORE_, id,NULL,val);
  }

//==============================================================================
// syntactic sugar: recall value from non volatile memory (NVM)
// - usage: val = bl_recall(id)        // recall value from NMV location @id
//==============================================================================

  static inline int bl_recall(int id)
  {
    return bl_msg(bl_down,_NVM,RECALL_, id,NULL,0);
  }

//==============================================================================
// syntactic sugar: send generic on/off SET message via mesh (using GOOCLI @id)
// - usage: val = bl_gooset(id,onoff)  // (BL_DOWN)<-[GOOCLI:SET @id,<NULL>,onoff]
//==============================================================================

  static inline int bl_gooset(int id, int onoff)
  {
    return bl_msg(bl_down,_GOOCLI,SET_, id,NULL,onoff);
  }

//==============================================================================
// syntactic sugar: send generic on/off LET message via mesh (using GOOCLI @id)
// - usage: val = bl_goolet(id,onoff)  // (BL_DOWN)<-[GOOCLI:LET @id,onoff]
//==============================================================================

  static inline int bl_goolet(int id, int onoff)
  {
    return bl_msg(bl_down,_GOOCLI,LET_, id,NULL,onoff);
  }

//==============================================================================
// syntactic sugar: send generic on/off GET message via mesh (using GOOCLI @id)
// - usage: val = bl_googet(id)       // (BL_DOWN)<-[GOOCLI:GET @id]
//==============================================================================

  static inline int bl_googet(int id)
  {
    return bl_msg(bl_down,_GOOCLI,GET_, id,NULL,0);
  }

#endif // __BL_SUGAR_H__
