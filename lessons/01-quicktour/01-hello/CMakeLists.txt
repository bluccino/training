# SPDX-License-Identifier: Apache-2.0

  cmake_minimum_required(VERSION 3.20.0)
  find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#===============================================================================
# project definition and path setup
#===============================================================================

  project(01-hello)

  set (LIB ../../../lib/V1.0.7)
  set (BLU ${LIB}/bluccino)
  set (SRC src)

  include_directories(${SRC} ${BLU})

#===============================================================================
# source files
#===============================================================================

  target_sources(app PRIVATE ${SRC}/main.c ${BLU}/bluccino.c)
