//==============================================================================
// bl_type.h - bluccino typedefs
//==============================================================================

#ifndef __BL_TYPE_H__
#define __BL_TYPE_H__

  #include <stdint.h>
  #include <stdbool.h>
  #include <stdlib.h>

  #define BL_LENGTH(a)      (sizeof(a)/sizeof(a[0]))         // array length
  #define BL_LEN(a)         (sizeof(a)/sizeof(a[0]))         // array length

    // we use the concept of "hashed opcodes", a kind of internal opcodes
    // which have the second most significant bit ("hash bit") of the lower
    // 2 byte nibble set. Functions like bl_out will always clear the hash bit
    // before posting

  #define BL_HASHBIT        0x00008000      // or mask to set hash bit
  #define BL_HASHCLR        0x00007FFF      // and mask to clear hash bit

     // macro BL_HASH() sets opcode's hashbit, macro BL_CLEAR() clears opcode's
     // hashbit, BL_HASHED() checks if opcode's hash bit is set

  #define BL_HASH(op)       ((BL_op)((uint32_t)(op)|BL_HASHBIT))
  #define BL_HASHED(op)     (((op) & BL_HASHBIT) != 0)
  #define BL_CLEAR(op)      ((BL_op)((uint32_t)(op)&BL_HASHCLR))

    // macros for mesh message identification

  #define BL_ID_(cl,op)     (((uint32_t)(cl)<<16)|BL_HASH(op))  // hashed msg ID

    // similarily we use the concept of "augmented class tags", denoting a kind
    // of internal interface which have the second most significant bit ("aug
    // bit") of the lower 2 byte nibble set. Functions like bl_out will always
    // clear the aug bit before posting

  #define BL_AUGBIT        0x00008000      // or mask to set hash bit
  #define BL_AUGCLR        0x00007FFF      // and mask to clear hash bit

     // macro BL_AUG() sets opcode's hashbit, macro BL_CLEAR() clears opcode's
     // hashbit, BL_AUGED() checks if opcode's hash bit is set

  #define BL_AUG(op)        ((BL_op)((uint32_t)(op)|BL_AUGBIT))
  #define BL_ISAUG(op)      (((op) & BL_AUGBIT) != 0)

    // macros for mesh message identification

  #define BL_ID(cl,op)      (((uint32_t)(cl)<<16)|(op))         // message ID
  #define _BL_ID(cl,op)     (((uint32_t)BL_AUG(cl)<<16)|(op))   // aug'ed msg ID
  #define BL_CLR(cl)        ((BL_cl)((uint32_t)(cl)&BL_AUGCLR)) // clear AUG bit

    // useful macros for min(), max() and abs()

  #define BL_MAX(x,y) (((x) > (y)) ? (x) : (y))
  #define BL_MIN(x,y) (((x) < (y)) ? (x) : (y))
  #define BL_ABS(x)   ((x) < 0    ? -(x) : (x))

//==============================================================================
// typedefs
//==============================================================================

  typedef uint8_t  BL_byte;             // unsigned 8-bit byte
  typedef uint16_t BL_word;             // unsigned 16-bit word

  typedef int8_t   BL_s8;               // we like short type identifiers :-)
  typedef int16_t  BL_s16;              // we like short type identifiers :-)
  typedef int32_t  BL_s32;              // we like short type identifiers :-)
  typedef int64_t  BL_s64;              // we like short type identifiers :-)

  typedef uint8_t  BL_u8;               // we like short type identifiers :-)
  typedef uint16_t BL_u16;              // we like short type identifiers :-)
  typedef uint32_t BL_u32;              // we like short type identifiers :-)
  typedef uint64_t BL_u64;              // we like short type identifiers :-)

  typedef const char *BL_txt;

    // we define ZL_ms to represent miliseconds since system start or clock
    // restart in 64-bit signed integer representation. This allows
    //
    //     a) negative time stamps to indicate invalid time stamps
    //     b) sufficient long time () before overrun
    //        (1 year .= 2^35 ms, 2^28 years .= 268 million years = 2^63 ms)

  typedef int64_t BL_ms;               // mili seconds
  typedef int64_t BL_us;               // micro seconds

  typedef enum
  {
      BL_ERR_ANY     = 1100,           // any error
      BL_ERR_BADARG  = 1200,           // bad input arg
      BL_ERR_FAILED  = 1300,           // operation failed
      BL_ERR_MEMORY  = 1400,           // out of memory
  } BL_err;
/*
  typedef struct BL_pace               // tick/tock pace control
          {
            BL_ms start;               // start time frame for pace maker
            BL_ms clock;               // pace clock
            BL_ms period;              // tick period
            int divider;               // tock divider
          } BL_pace;                   // tick/tock pace control
*/
  typedef struct BL_pace               // tick/tock pace control
          {
            BL_ms period;              // tick/tock period
            BL_ms time;                // tick/tock time
          } BL_pace;                   // tick/tock pace control

  #define BL_LO(x)           ((BL_byte)  ((x) & 0xff))
  #define BL_HI(x)           ((BL_byte)  (((x) >> 8) & 0xff))
  #define BL_HILO(hi,lo)     ((uint16_t) ((((uint16_t)(hi)) << 8) | (lo)))

  #define BL_LB(x)           ((BL_byte)  ((x) & 0xff))
  #define BL_HB(x)           ((BL_byte)  (((x) >> 8) & 0xff))
  #define BL_HBLB(hi,lo)     ((uint16_t) ((((uint16_t)(hi)) << 8) | (lo)))

  #define BL_LW(x)           ((BL_word)  ((x) & 0xffff))
  #define BL_HW(x)           ((BL_word)  (((x) >> 16) & 0xffff))
  #define BL_HWLW(hi,lo)     ((uint32_t) ((((uint32_t)(hi)) << 16) | (lo)))

#endif // __BL_TYPE_H__
