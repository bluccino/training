--------------------------------------------------------------------------------
# Bluccino V1.0.7
--------------------------------------------------------------------------------

* more precise timing for bl_run()/bl_engine()
* introduce bl_after() function
* new hwstd hardware core with NVM support
* mesh publisher (bl_mpub) for repeated publishing of mesh messages
* support of transitions (bl_trans)
* logging.h and config.h in many cases replaced by CMakeLists.txt definitions
* bl_node module for mesh node house keeping (replaces bl_house)
* new (simplified) top gear
* add bl_duty(o,duty,period) inline function
* add bl_run monitoring
* add bl_util.h to support bl_rand() (random function)
* enable/disable interrupts (bl_irq())

## Roadmap:

- extend wlstd mesh core with 3 additional gonoff client/server models
- extend wlstd mesh core with 3 additional glevel client/server models

## Changelog History

* more precise timing for bl_run()/bl_engine()
* new hwstd hardware core with NVM support
* mesh publisher (bl_mpub) for repeated publishing of mesh messages
* support of transitions (bl_trans)
- test all 01-quicktour/* samples
- [SYS:TICK] and [SYS:TOCK] -> SYS_TICK_id_BL_pace_cnt, SYS_TOCK_id_BL_pace_cnt
* logging.h and config.h in many cases replaced by CMakeLists.txt definitions
* bl_node module for mesh node house keeping (replaces bl_house)
* new (simplified) top gear
* add bl_duty(o,duty,period) inline function
* add bl_after() inline function (bl_time.h)
* add bl_reset module (to implement reset logic in wlcore/wlstd/bl_wl)
* add [GET:PRV] and [GET:ADD] input interfaces to bl_wl of wlstd core
* add bl_run monitoring
- rename Bluccino V1.0.6 to V1.0.7
* Bluccino V1.0.7 release
- event spin out of bl_run monitoring (see 01-bluccino app)
* Bluccino V1.0.7 patch 1
* add bl_util.h to support bl_rand() (random function)
* Bluccino V1.0.7 patch 2
- enhanced bl_rand() function to support unsigned modulo n random numbers
* Bluccino V1.0.7 patch 3
* enable/disable interrupts
* Bluccino V1.0.7 patch 4

--------------------------------------------------------------------------------
# Bluccino V1.0.6
--------------------------------------------------------------------------------

* use GEAR logging instead of API logging
* introduce _bl_out() for augmented output
* add augmented _bl_led() function
* bl_cb() must not warn for defaults which are NULL
* introduce syntactic sugar bl_fwd() as simplified bl_out() function
* introduce bl_post() function and SYS: message definition symbols
* define message IDs for SYS:, LED:, BUTTON: and SWITCH: classes

## Roadmap:

## Changelog History
* use GEAR logging instead of API logging
* introduce _bl_out() for augmented output
* add augmented _bl_led() function
* bl_cb() must not warn for defaults which are NULL
- slight modification of standard log macro (intro of WHO define)
* introduce syntactic sugar bl_fwd() as simpliefied bl_out() function
* introduce bl_post() function and SYS: message definition symbols
* define message IDs for SYS:, LED:, BUTTON: and SWITCH: classes
- replace BL_ID() macro calls in bl_hwled.c case statements by message ID defs
- replace BL_ID() macro calls in bl_hwbut.c case statements by message ID defs
- Bluccino V1.0.6a Beta
* more precise timing for bl_run()/bl_engine()
* new hwstd hardware core with NVM support
* mesh publisher (bl_mpub) for repeated publishing of mesh messages
* support of transitions (bl_trans)
- test all 01-quicktour/* samples
- [SYS:TICK] and [SYS:TOCK] -> SYS_TICK_id_BL_pace_cnt, SYS_TOCK_id_BL_pace_cnt
* logging.h and config.h in many cases replaced by CMakeLists.txt definitions
* bl_node module for mesh node house keeping (replaces bl_house)
* new (simplified) top gear
* add bl_duty(o,duty,period) inline function
* add bl_after() inline function (bl_time.h)
* add bl_reset module (to implement reset logic in wlcore/wlstd/bl_wl)
* add [GET:PRV] and [GET:ADD] input interfaces to bl_wl of wlstd core

--------------------------------------------------------------------------------
# Bluccino V1.0.5
--------------------------------------------------------------------------------

## Goals:
+ new organisation (bl_run.c/.h, bl_gear.c/.h, ...)

## Changelog History
+ new organisation (bl_run.c/.h, bl_gear.c/.h, ...)
- Bluccino v1.0.5

--------------------------------------------------------------------------------
# Bluccino V1.0.4
--------------------------------------------------------------------------------

## Goals:

## Changelog History
- Bluccino v1.0.4

--------------------------------------------------------------------------------
# Bluccino V1.0.3
--------------------------------------------------------------------------------

## Goals:
- bug: housekeep starts flashing the blue LED in the startup sequence, but it
       should start the status LED
- bug: wltiny has an issue wit GOOLET/GOOSET (seems they fire at the same time)
+ test all training lecture sample programs
- bug: crash in 07-attention

## Changelog History
- some bug fixes in interface docs
- test all training lecture sample programs
- Bluccino v1.0.3

--------------------------------------------------------------------------------
# Bluccino V1.0.2
--------------------------------------------------------------------------------

## Goals:
+ bug fix: already defined LOG/LOGO macros in case of #include "symbol.h"
- test all training lecture sample programs
- bug: crash in 07-attention

## Changelog History
+ bug fix: already defined LOG/LOGO macros in case of #include "symbol.h"
- test 01-quicktour and 03-meshfun samples
- Bluccino v1.0.2

--------------------------------------------------------------------------------
# Bluccino V1.0.1
--------------------------------------------------------------------------------

## Goals:
+ provide a bl_core.c/.h module as a default template for HW core and WL core
  integration
+ separation of bluccino() and bl_in() function => bluccino.c
+ reorganization of file structure for cores
+ seperation of bl_run() and bl_engine() into bl_run.c/.h module
+ separation of bl_up() and bl_down() into bl_gear.c/.h module
+ renaming bl_api.c to bl_time.c, split bl_api.h into bl_time.h and bl_msg.h
+ test all 03-meshfun training lecture sample programs

## Changelog History
+ provide a bl_core.c/.h module as a default template for HW core and WL core
  integration
+ separation of bluccino() and bl_in() function => bluccino.c
+ reorganization of file structure for cores
+ seperation of bl_run() and bl_engine() into bl_run.c/.h module
+ separation of bl_up() and bl_down() into bl_gear.c/.h module
+ renaming bl_api.c to bl_time.c, split bl_api.h into bl_time.h and bl_msg.h
- test 03-meshfun/03-ledsrv in combi with 03-meshfun/02-swcli - works well :-)))
- test 03-meshfun/04-clisrv - works well :-)))
- modify and test 03-meshfun/05-nvm with different LEDs blinking after restart
+ test all 03-meshfun training lecture sample programs
- Bluccino v1.0.1
