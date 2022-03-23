# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
set(QEMU_EXTRA_FLAGS -s)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#===============================================================================
# project definition and path setup
#===============================================================================

  project(04-clisrv)

  set (LIB ../../../lib/V0.6)
  set (BLU ${LIB}/bluccino)
  set (HWC ${LIB}/hwcore)
  set (SRC src)

  include_directories(${SRC} ${BLU} ${HWC})

  target_sources(app PRIVATE
               ${SRC}/main.c
               ${SRC}/ocore5.c
               ${HWC}/bl_hw.c
               ${BLU}/bluccino.c
              )
