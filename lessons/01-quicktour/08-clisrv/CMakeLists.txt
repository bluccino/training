# SPDX-License-Identifier: Apache-2.0

	cmake_minimum_required(VERSION 3.20.0)
	set(QEMU_EXTRA_FLAGS -s)

	find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#===============================================================================
# project definition and path setup
#===============================================================================

  project(08-clisrv)

  set (LIB ../../../lib/V1.0)          # library path
  set (BLU ${LIB}/bluccino)            # Bluccino library modules
  set (HWC ${LIB}/hwcore)              # hardware core
  set (WLC ${LIB}/mcore)               # wireless core
  set (SRC src)

  set (MAIN ${SRC})
  set (CORE ${SRC}/core)

  include_directories(${SRC} ${BLU} ${HWC} ${MAIN} ${CORE})

#===============================================================================
# source files
#===============================================================================

  target_sources(app PRIVATE
	               ${MAIN}/main.c         # main module
				         ${BLU}/bluccino.c      # Bluccino standard library
				         ${HWC}/bl_hw.c         # hardware core
				         ${WLC}/mcore.c         # wireless core
	              )
