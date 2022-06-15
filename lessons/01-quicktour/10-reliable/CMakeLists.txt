# SPDX-License-Identifier: Apache-2.0

	cmake_minimum_required(VERSION 3.20.0)
	set(QEMU_EXTRA_FLAGS -s)

	find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#===============================================================================
# project personality (definitions to be pushed down to C-level)
#===============================================================================

  project(10-reliable)

  add_definitions(-DPROJECT="${CMAKE_PROJECT_NAME}")

  add_definitions(-DPROJECT="${CMAKE_PROJECT_NAME}")
  add_definitions(-DCFG_MPUB_INTERVAL=200)

#===============================================================================
# project definition and path setup
#===============================================================================

  set (LIB ../../../lib/V1.0.6)        # library path
  set (BLU ${LIB}/bluccino)            # Bluccino library modules
  set (BLM ${LIB}/module)              # Bluccino app modules
  set (HWC ${LIB}/core/hwcore/hwtiny)  # tiny hardware core
  set (WLC ${LIB}/core/wlcore/wlstd)   # standard wireless core
  set (SRC src)

  set (MAIN ${SRC})

  include_directories(${SRC} ${BLU} ${BLM} ${HWC} ${MAIN})

#===============================================================================
# source files
#===============================================================================

  target_sources(app PRIVATE
	               ${MAIN}/main.c         # main module
				         ${BLU}/bluccino.c      # Bluccino standard library
				         ${HWC}/bl_hw.c         # hardware core
				         ${WLC}/bl_wl.c         # wireless core
                 ${BLM}/bl_mpub.c       # mesh publishing module
	              )
