# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
set(QEMU_EXTRA_FLAGS -s)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#===============================================================================
# project definition and path setup
#===============================================================================

  project(02-swicli)

  set (LIB ../../../lib/V1.0)
  set (BLU ${LIB}/bluccino)
  set (HWC ${LIB}/hwcore)
  set (WLC ${LIB}/wlcore/ocore)
  set (SRC src)

  include_directories(${SRC} ${BLU} ${HWC} ${WLC})

  target_sources(app PRIVATE
               ${SRC}/main.c
               ${WLC}/bl_wl.c
               ${HWC}/bl_hw.c
               ${BLU}/bluccino.c
              )
