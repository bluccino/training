//==============================================================================
//  bluccino.cpp
//  bluccino class implementation
//
//  Created by Hugo Pristauz on 2021-11-21
//  Copyright © 2021 Bluenetics GmbH. All rights reserved.
//==============================================================================

  #include <assert.h>
  #include "bluccino.h"

//==============================================================================
//
//                                Bl_ob Class
//
//==============================================================================

//==============================================================================
// set and let operation
//==============================================================================

  void Bl_ob::set(int value)
  {
    //bl_log2(2,"@blu.send",id,value);

    //BL_ob o = {tag,BL_SET,id,NULL};
    //bl_set(&o,value);
  }

  void Bl_ob::let(int value)
  {
    //bl_log2(2,"@blu.send",id,value);

    //BL_ob o = {tag,BL_LET,id,NULL};
    //bl_set(&o,value);
  }

//==============================================================================
// deep sleeping
//==============================================================================

  void Bl_ob::sleep(int ms)             // sleep some miliseconds
  {
    bl_sleep(ms);                      // good night ...
  }

//==============================================================================
// logging
//==============================================================================

  void Bl_ob::log(const char *msg, int Id, int value)
  {
    //bl_log2(debug,msg,Id,value);
  }

  void Bl_ob::log(const char *msg, int value)
  {
    //bl_log1(debug,msg,value);
  }

  void Bl_ob::log(const char *msg)
  {
    //bl_log(debug,msg);
  }

//==============================================================================
//
//                    bluccino System Class
//
//==============================================================================

  void System::init(int verbose)            // init verbose (debug) level
  {
    id = verbose;                           // store verbose level in id
    log("verbose",verbose);
  }

//==============================================================================
//
//                    bluccino Button Class
//
//==============================================================================

  static WhenButton when_button = 0;

  But_ton::But_ton(int id, WhenButton cb) : Bl_ob(_BUTTON,id)
  {
    when_button = cb;
  }

//==============================================================================
//
//                    bluccino Led Class
//
//==============================================================================

  static WhenLed when_led = 0;

  L_ed::L_ed(int id, WhenLed cb) : Bl_ob(_LED,id)
  {
    when_led = cb;
  }

  void L_ed::set(int value)
  {
    //bl_log2(2,"@blu.led",id,value);

    //BL_ob o = {tag,BL_SET,id,NULL};
    //bl_set(&o,value);
  }

//==============================================================================
//
//                    bluccino Goo_Srv Class
//
//==============================================================================

  static WhenGooSrv when_goosrv = 0;

  Goo_Srv::Goo_Srv(int id, WhenGooSrv cb) : Bl_ob(_GOOSRV,id)
  {
    when_goosrv = cb;
  }

//==============================================================================
//
//                      bluccino Global Functionality
//
//==============================================================================

//==============================================================================
// event notification
//==============================================================================
/*
  static void notify(BL_ob *o, int value)
  {
    //bl_log2(4,"@notify",o->id,value);

    switch (o->cl)
    {
      case _BUTTON:
        if (when_button)
        {
          Button obj(o->id,o->op);
          when_button(obj,value);
        }
        return;

      case _GOOSRV:
        if (when_goosrv)
        {
          Goo_Srv obj(o->id,o->op);
          when_goosrv(obj,value);
        }
        return;

      default:
        return;
    }
  }
*/
//==============================================================================
// bluccino Init() and Loop functions
//==============================================================================

  void I_nit(int verbose)               // Blucion init function
  {
    //bl_init(notify,verbose);           // init bluccino kernel
  }

  void L_oop(int ms,const char *msg)    // bluccino loop function
  {
    //bl_loop();                         // further forward
    //if (msg)
    //  bl_log(1,msg);
    //if (ms)
    //  bl_sleep(ms);
  }
