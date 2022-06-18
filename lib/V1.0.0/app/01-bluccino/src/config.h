//==============================================================================
//  config.h
//  config settings for application
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
//             #include "config.h"       // app specific
//                => #include "msg.h"    // app specific
//          #endif
//          #ifdef BL_LOGGING
//             #include "logging.h"      // app specific
//          #endif
//
//==============================================================================

#ifndef __CONFIG_H__
#define __CONFIG_H__

//#include "symbol.h"
//#include "blmsg.h"

//==============================================================================
// app personality
//==============================================================================

  #define CFG_APP_VERSION        "07-mcore"
  #define CFG_APP_VERBOSE         4    // verbose level for application

//==============================================================================
// Migration Steps
//==============================================================================

  #define MIGRATION_STEP1         1
  #define MIGRATION_STEP2         1
  #define MIGRATION_STEP3         1
  #define MIGRATION_STEP4         1
  #define MIGRATION_STEP5         1
  #define MIGRATION_STEP6         1

#endif // __CONFIG_H__
