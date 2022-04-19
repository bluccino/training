================================================================================
01-bluccino (app for testing and docu of Bluccino library)
================================================================================

- used to document development progress
- also used for representative regression test of Bluccino library

================================================================================

Bluccino V1.0.6
Goals:
+ use GEAR logging instead of API logging
+ introduce _bl_out() for augmented output
+ add augmented _bl_led() function
- rewrite top gear to use augmented messages for upstream and unaugmented
  messages for downstream messages

Changelog History
+ use GEAR logging instead of API logging
+ introduce _bl_out() for augmented output
+ add augmented _bl_led() function

################################################################################

================================================================================

Bluccino V1.0.5
Goals:
+ new organisation (bl_run.c/.h, bl_gear.c/.h, ...)

Changelog History
+ new organisation (bl_run.c/.h, bl_gear.c/.h, ...)
- Bluccino v1.0.5

================================================================================

Bluccino V1.0.4
Goals:

Changelog History
- Bluccino v1.0.4

================================================================================

Bluccino V1.0.3
Goals:
- bug: housekeep starts flashing the blue LED in the startup sequence, but it
       should start the status LED
- bug: wltiny has an issue wit GOOLET/GOOSET (seems they fire at the same time)
+ test all training lecture sample programs
- bug: crash in 07-attention

Changelog History
- some bug fixes in interface docs
- test all training lecture sample programs
- Bluccino v1.0.3

================================================================================

Bluccino V1.0.2
Goals:
+ bug fix: already defined LOG/LOGO macros in case of #include "symbol.h"
- test all training lecture sample programs
- bug: crash in 07-attention

Changelog History
+ bug fix: already defined LOG/LOGO macros in case of #include "symbol.h"
- test 01-quicktour and 03-meshfun samples
- Bluccino v1.0.2

================================================================================

Bluccino V1.0.1
Goals:
+ provide a bl_core.c/.h module as a default template for HW core and WL core
  integration
+ separation of bluccino() and bl_in() function => bluccino.c
+ reorganization of file structure for cores
+ seperation of bl_run() and bl_engine() into bl_run.c/.h module
+ separation of bl_up() and bl_down() into bl_gear.c/.h module
+ renaming bl_api.c to bl_time.c, split bl_api.h into bl_time.h and bl_msg.h
+ test all 03-meshfun training lecture sample programs

Changelog History
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
