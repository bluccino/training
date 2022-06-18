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
//                                Blob Class
//
//==============================================================================

//==============================================================================
// set and let operation
//==============================================================================

  void Blob::set(int value)
  {
    bl_log2(2,"@blu.send",id,value);

    BL_ob o = {tag,BL_SET,id,NULL};
    bl_set(&o,value);
  }

  void Blob::let(int value)
  {
    bl_log2(2,"@blu.send",id,value);

    BL_ob o = {tag,BL_LET,id,NULL};
    bl_set(&o,value);
  }

//==============================================================================
// deep sleeping
//==============================================================================

  void Blob::sleep(int ms)             // sleep some miliseconds
  {
    bl_sleep(ms);                      // good night ...
  }

//==============================================================================
// logging
//==============================================================================

  void Blob::log(const char *msg, int Id, int value)
  {
    bl_log2(debug,msg,Id,value);
  }

  void Blob::log(const char *msg, int value)
  {
    bl_log1(debug,msg,value);
  }

  void Blob::log(const char *msg)
  {
    bl_log(debug,msg);
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

  Button::Button(int id, WhenButton cb) : Blob(BL_BUTTON,id)
  {
    when_button = cb;
  }

//==============================================================================
//
//                    bluccino Led Class
//
//==============================================================================

  static WhenLed when_led = 0;

  Led::Led(int id, WhenLed cb) : Blob(BL_LED,id)
  {
    when_led = cb;
  }

  void Led::set(int value)
  {
    bl_log2(2,"@blu.led",id,value);

    BL_ob o = {tag,BL_SET,id,NULL};
    bl_set(&o,value);
  }

//==============================================================================
//
//                    bluccino GooSrv Class
//
//==============================================================================

  static WhenGooSrv when_goosrv = 0;

  GooSrv::GooSrv(int id, WhenGooSrv cb) : Blob(BL_GOO_SRV,id)
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

  static void notify(BL_ob *o, int value)
  {
    bl_log2(4,"@notify",o->id,value);

    switch (o->tag)
    {
      case BL_BUTTON:
        if (when_button)
        {
          Button obj(o->id,o->op);
          when_button(obj,value);
        }
        return;

      case BL_GOO_SRV:
        if (when_goosrv)
        {
          GooSrv obj(o->id,o->op);
          when_goosrv(obj,value);
        }
        return;

      default:
        return;
    }
  }

//==============================================================================
// bluccino Init() and Loop functions
//==============================================================================

  void Init(int verbose)               // Blucion init function
  {
    bl_init(notify,verbose);           // init bluccino kernel
  }

  void Loop(int ms,const char *msg)    // bluccino loop function
  {
    bl_loop();                         // further forward
    if (msg)
      bl_log(1,msg);
    if (ms)
      bl_sleep(ms);
  }
