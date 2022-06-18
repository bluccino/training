# SPDX-License-Identifier: Apache-2.0

	cmake_minimum_required(VERSION 3.20.0)
	set(QEMU_EXTRA_FLAGS -s)

	find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#===============================================================================
# project definition and path setup
#===============================================================================

  project(01-bluccino)

  set (LIB ../../../../lib/V1.0.6)
  set (BLU ${LIB}/bluccino)
  set (HWC ${LIB}/core/hwcore/hwtiny)
  set (WLC ${LIB}/core/wlcore/wlstd)
  set (LMO ${LIB}/module)
  set (SRC src)

  include_directories(${SRC} ${BLU} ${LMO} ${HWC} ${WLC})

  add_definitions(-DBL_CONFIG="config.h")   # push define down to C level
  add_definitions(-DBL_LOGGING="logging.h") # push define down to C level

#===============================================================================
# source files
#===============================================================================

target_sources(app PRIVATE
	       ${SRC}/main.c
				 ${BLU}/bluccino.c
	       ${HWC}/bl_hw.c
	       ${WLC}/bl_wl.c
	       ${LMO}/bl_house.c
	       )

target_sources_ifdef(CONFIG_MCUMGR app PRIVATE ${CORE}/smp_svr.c)

zephyr_include_directories(
	src/
	src/mesh
)
