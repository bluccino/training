================================================================================
01-bluccino (app for testing and docu of Bluccino library)
================================================================================

- used to document development progress
- also used for representative regression test of Bluccino library

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
- test all training lecture sample programs
================================================================================

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
- Bluccino v1.0.1
