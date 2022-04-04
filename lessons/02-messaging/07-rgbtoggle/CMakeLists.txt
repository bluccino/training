# SPDX-License-Identifier: Apache-2.0

  cmake_minimum_required(VERSION 3.20.0)
  find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
# set(QEMU_EXTRA_FLAGS -s)

#===============================================================================
# project definition and path setup
#===============================================================================

  project(05-toggle)

  set (LIB ../../../lib/V1.0)
  set (BLU ${LIB}/bluccino)
  set (SRC src)

  include_directories(${SRC} ${BLU})

  add_definitions(-DBL_SYMBOL="symbol.h")   # push define down to C level

#===============================================================================
# source files
#===============================================================================

  target_sources(app PRIVATE
                 ${SRC}/main.c
                 ${SRC}/button.c
                 ${SRC}/led.c
                 ${BLU}/bluccino.c
                )
