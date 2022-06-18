//==============================================================================
//  bl_class.h
//  bluccino C++ class definitions
//
//  Created by Hugo Pristauz on 2021-12-04
//  Copyright © 2021 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_CLASS_H__
#define __BL_CLASS_H__

  #include <stdint.h>
  #include <stdbool.h>
  #include <zephyr.h>

  extern "C"
  {
    #include "blapi.h"
    #include "bllog.h"
  }

//==============================================================================
// Type definitions
//==============================================================================

  class Blob;                          // bluccino object (base class)
  class Button;                        // Button access object
  class Led;                           // LED access object
  class GooCli;                        // Generic OnOff Client
  class GooSrv;                        // Generic OnOff Server

      // event notification callbacks

  typedef void (*WhenButton)(Button &o, int value);
  typedef void (*WhenLed)(Led &o, int value);
  typedef void (*WhenGooSrv)(GooSrv &o, int value);
  typedef void (*WhenGooCli)(GooCli &o, int value);

//==============================================================================
// Class Blob (bluccino Object)
//==============================================================================

  class Blob                                // bluccino object class
  {
    public:
      int tag;
      int op;                               // op code
      int id;

    public:
      int debug;                            // verbose level for logging

    public:                                 // constructors
      Blob(int Tag)         { tag = Tag; id = 0;  op = 0; debug = 1; }
      Blob(int Tag, int id_=0, int op_=0) { tag = Tag; id = id_; op = op_; }

    public:                                 // operations
      void set(int value);
      void let(int value);
      void sleep(int ms);                   // sleep some miliseconds

    public:                                 // logging methods
      void verbose(int value) { debug = value; }
      void log(const char *msg, int id, int value);
      void log(const char *msg, int value);
      void log(const char *msg);
  };

//==============================================================================
// Class System (bluccino System Object)
//==============================================================================

  class System : public Blob
  {
    public:
      System(int verbose) : Blob(BL_SYS,0) { debug = verbose; }
      void init(int debug);                 // init verbose (debug) level

  };

//==============================================================================
// Class Button (bluccino Button Object)
//==============================================================================

  class Button : public Blob
  {
    public:
      Button(int id, WhenButton cb=0);
      Button(int id, int op): Blob(BL_BUTTON,id,op) {}
//    void when_set(int value);             // 'button set' event handler
  };

//==============================================================================
// Class Led (bluccino Led Object)
//==============================================================================

  class Led : public Blob
  {
    public:
      Led(int id, WhenLed cb=0);
      Led(int id, int op): Blob(BL_LED,id,op) {}
      void set(int value);
//    void when_set(int value);             // 'button set' event handler
  };

//==============================================================================
// Class GooCli (Generic OnOff Client Object)
//==============================================================================

  class GooCli : public Blob
  {
    public:
      GooCli(int Id) : Blob(BL_GOO_CLI,Id) {}
  };

//==============================================================================
// Class GooSrv (Generic OnOff Server Object)
//==============================================================================

  class GooSrv : public Blob
  {
    public:
      GooSrv(int id, WhenGooSrv cb=0);
      GooSrv(int id, int op): Blob(BL_GOO_SRV,id,op) {}
//    void when_set(int value);        // 'button set' event handler
  };

//==============================================================================
// bluccino Init() and Loop functions
//==============================================================================

  void Init(int verbose=1);                 // Blucion init function
  void Loop(int ms=0,const char *msg=0);    // bluccino loop function

#endif // __BL_CLASS_H__
