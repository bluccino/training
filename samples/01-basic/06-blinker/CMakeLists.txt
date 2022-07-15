# SPDX-License-Identifier: Apache-2.0

  cmake_minimum_required(VERSION 3.20.0)
  set(QEMU_EXTRA_FLAGS -s)
  find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#===============================================================================
# project definition and path setup
#===============================================================================

  project(05-blinker)

  set (LIB ../../lib/V0.1)
  set (BLU ${LIB}/bluccino)
  set (SRC src)

  include_directories(${SRC} ${BLU})

  add_definitions(-DCFG_PRETTY_LOGGING=1)   # push define down to C level
  add_definitions(-DBL_CONFIG="config.h")   # push define down to C level
# add_definitions(-DBL_LOGGING="logging.h") # push define down to C level

#===============================================================================
# source files
#===============================================================================

  target_sources(app PRIVATE
                 ${SRC}/main.c
                 ${SRC}/ocore3.c
                 ${BLU}/bluccino.c
                )
