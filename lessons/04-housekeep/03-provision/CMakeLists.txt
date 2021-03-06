# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
set(QEMU_EXTRA_FLAGS -s)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#===============================================================================
# project definition and path setup
#===============================================================================

  project(03-provision)

  set (LIB ../../../lib/V1.0.5)
  set (BLU ${LIB}/bluccino)            # Bluccino stuff
  set (HWC ${LIB}/core/hwcore/hwtiny)  # tiny hardware core
  set (WLC ${LIB}/core/wlcore/wlstd)   # standard wireless core
  set (SRC src)

  include_directories(${SRC} ${BLU} ${HWC})

#===============================================================================
# source files
#===============================================================================

  target_sources(app PRIVATE
               ${SRC}/main.c           # main program
							 ${SRC}/attention.c      # attention module
							 ${SRC}/provision.c      # provision module
               ${WLC}/bl_wl.c          # wireless core
               ${HWC}/bl_hw.c          # hardware core
               ${BLU}/bluccino.c       # work bench
              )
