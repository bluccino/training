//==============================================================================
// symbol.h
//==============================================================================
//==============================================================================

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

//==============================================================================
// class tag definitions
//==============================================================================

  #define BL_CL_TEXT {"VOID","BUTTON","LED"}

  typedef enum BL_cl                   // class tag
          {
            _VOID = 0x7FFF,            // invalid message class (a big number!)
            _BUTTON = 1,               // button interface
            _LED,                      // LED interface
          } BL_cl;

//==============================================================================
// opcode definitions
//==============================================================================

  #define BL_OP_TEXT {"VOID","STS","SET"}

  typedef enum BL_op
          {
            VOID_ = 0x7FFF,            // invalid opcode (a big number)
            STS_ = 1,                  // status
            SET_,                      // set property
          };

#endif __SYMBOL_H__
