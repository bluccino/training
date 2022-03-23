//==============================================================================
//  logging.h
//  log macros for application (this file is app specific)
//
//  Created by Hugo Pristauz on 2021-Dec-15
//  Copyright © 2021 fuseAware. All rights reserved.
//==============================================================================
//
//  CmakeFile.lst or emProject file provides the following definitions
//
//    #define BL_CONFIG   1  // causes bluccino.h to #include "config.h"
//    #define BL_LOGGING  1  // causes bluccino.h to #include "logging.h"
//
//  note the calling sequence:
//    #include bluccino.h
//       => // bluccino.h
//          #ifdef BL_CONFIG
//             #include "config.h"         // app specific
//                => #include "msg.h"      // app specific
//          #endif
//          #ifdef BL_LOGGING
//             #include "logging.h"        // app specific
//          #endif
//
//==============================================================================

#ifndef __LOGGING_H__
#define __LOGGING_H__

#if 0
//==============================================================================
// XYZ level generic logging shorthands
//==============================================================================

  #define LOG                     LOG_XYZ
  #define LOGO(lvl,col,o,val)     LOGO_XYZ(lvl,col"xyz:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_XYZ(lvl,col,o,val)

#endif
//==============================================================================
// general log control
//==============================================================================

  #define CFG_PRETTY_LOGGING      1    // pretty text for class tag & opcode
  #define CFG_SHUT_OFF_VERBOSE    5    // shut off verbose talking after n calls

//==============================================================================
// Logging
//==============================================================================

  #define CFG_LOG_API             1    // Bluccino API logging
  #define CFG_LOG_APP             1    // APP logging
  #define CFG_LOG_CORE            1    // core logging
  #define CFG_LOG_LLL             1    // LLL logging
  #define CFG_LOG_MAIN            1    // main logging

//==============================================================================
// MAIN Logging
//==============================================================================

#ifndef CFG_LOG_LLL
    #define CFG_LOG_LLL    0           // no LLL logging by default
#endif

#if (CFG_LOG_LLL)
    #define LOG_LLL(l,f,...)    BL_LOG(CFG_LOG_LLL-1+l,f,##__VA_ARGS__)
    #define LOGO_LLL(l,f,o,v)   bl_logo(CFG_LOG_LLL-1+l,f,o,v)
#else
    #define LOG_LLL(l,f,...)    {}     // empty
    #define LOGO_LLL(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// MAIN Logging
//==============================================================================

#ifndef CFG_LOG_MAIN
    #define CFG_LOG_MAIN    0           // no MAIN logging by default
#endif

#if (CFG_LOG_MAIN)
    #define LOG_MAIN(l,f,...)    BL_LOG(CFG_LOG_MAIN-1+l,f,##__VA_ARGS__)
    #define LOGO_MAIN(l,f,o,v)   bl_logo(CFG_LOG_MAIN-1+l,f,o,v)
#else
    #define LOG_MAIN(l,f,...)    {}     // empty
    #define LOGO_MAIN(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// end of logging.h
//==============================================================================
#endif // __LOGGING_H__
