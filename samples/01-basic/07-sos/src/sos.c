//==============================================================================
// sos.c - an SOS sequencer block
//==============================================================================

  #include "bluccino.h"
  #include "sos.h"

  #define PATTERN "* * *  *** *** ***  * * *          ";

  static BL_fct notify = NULL;         // callback for subscriber(s)

//==============================================================================
// timer tick: output sequenced [SOS:ONOFF] messages
//==============================================================================

  static int tick(BL_ob *o, int val)   // ticking the SOS module
  {
    static BL_txt p = NULL;            // work pointer to pattern
    static bool last = 0;              // remember last output level

    bl_logo(2,BL_M "sos",o,val);       // log what's going on

    if (!p || *p == 0)                 // first time, or end of pattern?
      p = PATTERN;                     // reset work pointer to pattern begin

    bool level = (*p++ != ' ');        // next output level to display

      // output event generation is only in case of level change

    if (level != last)                 // only in case of level change
    {
      BL_ob oo = {CL_SOS,OP_LEVEL,0,NULL};
      bl_out(&oo,level,notify);        // emit output message [SOS:LEVEL level]
      last = level;                    // remember last output level
    }

    return 0;                          // well done!
  }

//==============================================================================
// public module interface (message dispatcher)
//==============================================================================

  int sos(BL_ob *o, int val)           // public module interface
  {
    switch (BL_PAIR(o->cl,o->op))      // dispatch message ID
    {
      case BL_PAIR(CL_SOS,OP_INIT):    // [SOS:INIT <cb>]
        notify = o->data;              // store notify callback
        return 0;

      case BL_PAIR(CL_SOS,OP_TICK):    // [SOS:TICK]
        return tick(o,val);            // delegate to tick()

      default:                         // anything else
        return -1;                     // bad input
    }
  }
