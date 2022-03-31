//==============================================================================
//  bl_symb.h
//  Bluccino message symbol definitions
//
//  Created by Hugo Pristauz on 2022-02-22
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_SYMB_H__
#define __BL_SYMB_H__

//==============================================================================
// message class definitions
//==============================================================================

  #ifndef BL_CL_TEXT

    #define BL_CL_TEXT {"VOID","SYS","OUT","RESET","TIMER","TEST","TIC",\
                        "SET","GET","MESH","BLE","NGMN","CFGSRV","CFGCLI",\
                        "HEASRV","HEACLI","GOOCLI","GOOSRV","GLVCLI","GLVSRV",\
                        "BUTTON", "SWITCH", "LED",           \
                        "CTRL","SCAN","ADVT","SOS","NVM"}

    typedef enum BL_cl                 // class tag
            {
              _VOID = 0x7FFF,          // invalid message class (a big number!)
              _SYS = 1,                // system
              _OUT,                    // output a message
              _RESET,                  // reset
              _TIMER,                  // timer module
              _TEST,                   // test class
              _TIC,                    // tic/toc object

              _SET,                    // set property
              _GET,                    // get property
              _MESH,                   // mesh
              _BLE,                    // BLE
              _NGMN,                   // 3G/4G/5G (next gen mobile networks)

              _CFGSRV,                 // Config Server
              _CFGCLI,                 // Config Client
              _HEASRV,                 // Health Server
              _HEACLI,                 // Health Client

              _GOOCLI,                 // Generic OnOff Client
              _GOOSRV,                 // Generic OnOff Server
              _GLVCLI,                 // Generic Level Client
              _GLVSRV,                 // Generic Level Server

              _BUTTON,                 // button
              _SWITCH,                 // switch
              _LED,                    // LED

              _CTRL,                   // Generic controller
              _SCAN,                   // scanning
              _ADVT,                   // advertizer
              _SOS,                    // SOS module
              _NVM,                    // non volatile memory
            } BL_cl;                   // class tag

  #endif // BL_CL_TEXT

//==============================================================================
// message opcodes
//==============================================================================

  #ifndef BL_OP_TEXT

    #define BL_OP_TEXT {"VOID","INIT","OUT","WHEN","USE","TOC","READY","BUSY", \
                        "PING","PONG","PRV","ATT","DUE","SET","LET", \
                        "GET","STS","TRIG","TICK","TOCK","CMD","VAL","LEVEL", \
                        "ONOFF","COUNT","TOGGLE","INC","DEC","PAY", "ADV", \
                        "BEACON","SEND","PRESS","RELEASE","CLICK","HOLD","MS", \
                        "STORE","RECALL","SAVE"}

    typedef enum BL_op
            {
              VOID_ = 0x7FFF,          // invalid opcode (a big number)
              INIT_ = 1,               // init function
              OUT_,                    // set <out> callback
              WHEN_,                   // provide module handling callback
              USE_,                    // can module be used
              TOC_,                    // tic/toc object

              READY_,                  // ready state
              BUSY_,                   // busy state
              PING_,                   // ping a module
              PONG_,                   // pong response
              PRV_,                    // provisioning state changed
              ATT_,                    // attention state change
              DUE_,                    // timer is due

              SET_,                    // acknowledged set operation
              LET_,                    // unacknowledged
              GET_,                    // ask for status
              STS_,                    // send status

              TRIG_,                   // generic trigger signal
              TICK_,                   // timer ticks
              TOCK_,                   // timer tocks
              CMD_,                    // generic command
              VAL_,                    // generic value
              LEVEL_,                  // generic level
              ONOFF_,                  // generic onoff command
              COUNT_,                  // counter
              TOGGLE_,                 // toggle state

              INC_,                    // increment
              DEC_,                    // decrement

              PAY_,                    // generic payload
              ADV_,                    // LLL advertising reports
              BEACON_,                 // LLL beacon packet reports

              SEND_,                   // send a message

              PRESS_,                  // button press
              RELEASE_,                // button release
              CLICK_,                  // button click
              HOLD_,                   // button hold
              MS_,                     // millisecond (grace time)

              STORE_,                  // store data
              RECALL_,                 // recall data
              SAVE_,                   // save data
            } BL_op;

  #endif // BL_OP_TEXT

//==============================================================================
// message object & message callback definition
//==============================================================================

  typedef struct BL_ob
          {
            BL_cl cl;                  // class tag
            BL_op op;                  // opcode
            int id;                    // object ID
            const void *data;          // pointer to data
          } BL_ob;

    // define "OVAL" function interface and callback type

  typedef int (*BL_oval)(BL_ob *o, int value);
  typedef BL_oval BL_fct;              // alias to be compatible to legacy stuff

#endif // __BL_SYMB_H__
