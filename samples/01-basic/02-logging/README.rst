//==============================================================================
// 02-logging demo
//==============================================================================

- simple program to demonstrate Bluccino log functions
- we stick with two Bluccino functions: bl_hello() which can be used to define
  the verbose level, and use LOG() as shorthand for predefined LOG_TEST() macro
- CMakeLists.txt is the same as with 01-hello, except the project name
  => project(02-logging)

- Description:
- function log() logs 4 messages on different log levels (1..4) and different
  log colors
- depending on the verbose level defined in the bl_hello(verbose,...) calls only
  log messages with log level <= verbose are actually logged
- note that bl_log() adds an implicite newline character (\n), no explicite
  newline character must be provided at the end of a log message

- #include "bluccino.h" defines a couple of ANSI control sequences to change
  the current output color of an ANSI terminal. These are:
  BL_R: red, BL_G: green, BL_B: blue, BL_M: magenta, BL_C: cyan, BL_Y: yellow,
  and BL_0: default (either black or white)
- note that each of these symbols define a string which can be concatenated with
  any text string, e.g. we write bl_log(1,BL_R "text") without any comma between
  BL_R and "text" to tell the compiler that we want to have string concatenation
