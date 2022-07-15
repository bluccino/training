# SPDX-License-Identifier: Apache-2.0

  cmake_minimum_required(VERSION 3.20.0)
  find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
# set(QEMU_EXTRA_FLAGS -s)

#===============================================================================
# project definition and path setup
#===============================================================================

  project(01-hello)

  set (LIB ../../lib/V0.5)
  set (BLU ${LIB}/bluccino)
  set (MCORE ${LIB}/mcore)
  set (SRC src)

  include_directories(${SRC} ${BLU} ${MCORE})

# add_definitions(-DCFG_PRETTY_LOGGING=1)   # push define down to C level
# add_definitions(-DBL_CONFIG="config.h")   # push define down to C level
# add_definitions(-DBL_LOGGING="logging.h") # push define down to C level

#===============================================================================
# source files
#===============================================================================

  target_sources(app PRIVATE
                 ${SRC}/main.c
                 ${BLU}/bluccino.c
                 ${MCORE}/mcore.c
                )
