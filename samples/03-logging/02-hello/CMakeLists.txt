# SPDX-License-Identifier: Apache-2.0

  cmake_minimum_required(VERSION 3.20.0)
  find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#===============================================================================
# project personality (definitions to be pushed down to C-level)
#===============================================================================

  project(02-hello)

    # use Bluccino Real Time Logging (RTL) with 1k log buffer

  add_definitions(-DCFG_BLUCCINO_RTL=1024)   # Bluccino RTL, 1k log buffer

#===============================================================================
# path setup
#===============================================================================

  set (LIB ../../../lib/V1.0.8)
  set (BLU ${LIB}/bluccino)
  set (SRC src)

  include_directories(${BLU})

#===============================================================================
# source files
#===============================================================================

  target_sources(app PRIVATE ${SRC}/main.c
                 ${BLU}/bl_time.c
                 ${BLU}/bl_log.c
                 ${BLU}/bl_deco.c
                 ${BLU}/bl_gear.c
                 ${BLU}/bl_run.c
                 ${BLU}/bl_core.c
                )
