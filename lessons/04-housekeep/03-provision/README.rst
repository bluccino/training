================================================================================
03-provision (using WLSTD wireless core, HWTINY hardware core)
================================================================================

- skeleton program for use with WLSTD wireless core and HWTINY hardware core
- attention module which runs a blink sequence whenever attentioning is being
  activated
- provision module which takes care of provision status changes
- short period blinking if node is not provosioned and attention mode not active
- long period blinking if node is provosioned and attention mode not active
- activation of attention mode prohibits provision blinking
