//==============================================================================
// bl_node.h
// mesh node house keeping (startup, provision, attention)
//
// Created by Hugo Pristauz on 2022-Feb-21
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

#ifndef __BL_NODE_H__
#define __BL_NODE_H__

//==============================================================================
// NODE Logging
//==============================================================================

#ifndef CFG_LOG_NODE
    #define CFG_LOG_NODE    1           // NODE logging is by default on
#endif

#if (CFG_LOG_NODE)
    #define LOG_NODE(l,f,...)    BL_LOG(CFG_LOG_NODE-1+l,f,##__VA_ARGS__)
    #define LOGO_NODE(l,f,o,v)   bl_logo(CFG_LOG_NODE-1+l,f,o,v)
#else
    #define LOG_NODE(l,f,...)    {}     // empty
    #define LOGO_NODE(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// public module interface
//==============================================================================

  int bl_node(BL_ob *o, int val);

#endif // __BL_NODE_H__
