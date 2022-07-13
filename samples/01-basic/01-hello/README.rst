//==============================================================================
// 01-hello demo (hello world)
//==============================================================================

- the simplest program to get running with Bluccino
- uses the Bluccino bl_prt() function
- bl_prt() is an exact equivalent for the Zephyr printk() function, supporting
  variable argument lists
- note that bl_prt() requires a newline character (\n) at the end of the line 

- important to understand is CMakeLists.txt file with the following details
- 1) setting up the project name
- 2) setting up a symbol LIB for the library path
- 3) setting up a symbol BLU for the Bluccino library path
- 4) setting up a symbol SRC for the folder containing the sources
- 5) defining include folders ${SRC} and ${BLU}
- 6) linking both ${SRC}/main.c file and ${BLU}/bluccino.c library source
