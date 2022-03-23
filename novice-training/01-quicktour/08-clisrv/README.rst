#===============================================================================
# 07-mcore (Bluccino mesh core for 1 button and 4 LEDs, app accesses mesh)
# - 4 LEDs, 1 BUTTON and mesh GOOSRV/GOOCLIare are controllable from app level
# - any button @id press selects next LED blinking process
# - after selection of 4 different LEDs all LEDs are turned off
# - main program involves an attention-, provision- and startup-module
# - remark: program is suitable for nRF52832/840 DK and nRF52840 dongle
#===============================================================================

- derived from 06-mcore, and thus
  derived from zephyr/samples/boards/nrf/mesh/onoff_level_lighting_vnd_app
- replace app_gpio.c/.h by bc_hw.c/.h (new hardware core supporting also switch)
- move hwcore stuff to Bluccino/lib/hwcore
- change CMakeLists.txt to deal with hwcore

#===============================================================================
# 06-mcore (Bluccino mesh core for 1 button and 4 LEDs, app cannot accesses mesh)
# - 4 LEDs, 1 BUTTON, but mesh GOOSRV/GOOCLI are are controllable from app level
# - any button @id press selects next LED blinking process
# - after selection of 4 different LEDs all LEDs are turned off
# - main program involves an attention-, provision- and startup-module
# - remark: program is suitable for nRF52832/840 DK and nRF52840 dongle
#===============================================================================

- derived from 05-mcore, and thus
  derived from zephyr/samples/boards/nrf/mesh/onoff_level_lighting_vnd_app
- #define CFG_NUMBER_OF_BUTTONS 1 in config.h

#===============================================================================
# 05-mcore (Bluccino mesh core for 1 button and 4 LEDs, app cannot access mesh)
# - 4 LEDs, 1 BUTTON and mesh GOOSRV/GOOCLIare are controllable from app level
# - any button @id press selects next LED blinking process
# - after selection of 4 different LEDs all LEDs are turned off
# - main program involves an attention-, provision- and startup-module
# - remark: program is suitable for nRF52832/840 DK and nRF52840 dongle
#===============================================================================

- derived from 04-mcore, and thus
  derived from zephyr/samples/boards/nrf/mesh/onoff_level_lighting_vnd_app
- #define CFG_NUMBER_OF_BUTTONS 1 in config.h
- change app (main) to achieve specified behavior
- excellent, runs on nRF52840 dongle
- performing MIGRATION_STEP5 (define in config.h)
- add public module interface for ble_mesh.c/.h
- call BLEMESH[SYS:INIT] in mcore init
- introduced status LED @0, where @0 is re-mapped to @1 at the lowest level
- bt_mesh_reset() not directly but notification of app with [RESET:CNT] message
- app can control bt_mesh_reset() with [RESET:PRV] message
- startup module flashes one of RGB LEDs at 90% duty
- startup module to support mesh reset by button press

#===============================================================================
# 04-mcore (Bluccino mesh core for 4 LEDs and 4 buttons, app can't access mesh)
# - 4 LEDs and 4 BUTTONS are now controllable from app level (mesh not)
# - any button @id press turns on blinking process of LED @id
# - repeated button press turns off blinking process
# - press of different button @id turns on LED @id blinking (stops current)
# - remark 1: suitable program for nRF52832/840 DK, but not for nRF52840 dongle
# - remark 2: mesh is running in the background, but not accessible by app
#===============================================================================

- derived from 03-mcore, and thus
  derived from zephyr/samples/boards/nrf/mesh/onoff_level_lighting_vnd_app
- performing MIGRATION_STEP4
- creation of mcore.h with #include "bluccino.h" and setting migration defaults
- app_gpio adopted to allow independent LED control and receive of button events
- all publishing commands deactivated
- main adopted to toogle active LED in tock callback
- main adopted with when callback to change active LED selection by button press
- led() helper function added to main - now test program looking fine
- nice 03-mcore with lED and button functionality (and deactivated publisher)

#===============================================================================
# 03-mcore (Bluccino mesh core)
#===============================================================================

- derived from zephyr/samples/boards/nrf/mesh/onoff_level_lighting_vnd_app
- we call it 'ollv-app by short name'
-
- performing MIGRATION_STEP1
- add MIGRATION define defaults to mcore.c
- added main.c
- moved all *.c stuff into folder src/core or src/main
- increased main stack size from 1024 to 2048 bytes
-
- performing MIGRATION_STEP2
- add MIGRATION define defaults to ble_mesh.c
- in ble_mesh.c:
  - notify provisioning
  - notify provision complete
  - notify attention
  - notify attention complete
- now we are able to see color change of logs in case of attention & provision
- in mcore.c
  - add LOG macro definitions
  - use LOG for printk("Initializing ...\n")
  - use LOG for printk("Bluetooth initialized\n")
  - use LOG for printk("Mesh initialized\n")
  - use LOG for reset counter logging
- most system messages replaced by Bluccino LOGs
- [MESH:PRV] and [MESH:ATT] posted upward via bl_core()
- performing MIGRATION_STEP3


#===============================================================================
# Appendix
#===============================================================================

.. _bluetooth-mesh-onoff-level-lighting-vnd-sample:

Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
######################################################################
Overview
********
This is a application demonstrating a Bluetooth mesh node in
which Root element has following models

- Generic OnOff Server
- Generic OnOff Client
- Generic Level Server
- Generic Level Client
- Generic Default Transition Time Server
- Generic Default Transition Time Client
- Generic Power OnOff Server
- Generic Power OnOff Setup Server
- Generic Power OnOff Client
- Light Lightness Server
- Light Lightness Setup Server
- Light Lightness Client
- Light CTL Server
- Light CTL Setup Server
- Light CTL Client
- Vendor Model

And Secondary element has following models

- Generic Level Server
- Generic Level Client
- Light CTL Temperature Server

Prior to provisioning, an unprovisioned beacon is broadcast that contains
a unique UUID. It is obtained from the device address set by Nordic in the
Factory information configuration register (FICR).

Associations of Models with hardware
************************************
For the nRF52840-PDK board, these are the model associations:

* LED1 is associated with generic OnOff Server's state which is part of Root element
* LED2 is associated with Vendor Model which is part of Root element
* LED3 is associated with generic Level (ROOT) / Light Lightness Actual value
* LED4 is associated with generic Level (Secondary) / Light CTL Temperature value
* Button1 and Button2 are associated with gen. OnOff Client or Vendor Model which is part of Root element
* Button3 and Button4 are associated with gen. Level Client / Light Lightness Client / Light CTL Client which is part of Root element

States of Servers are bounded as per Bluetooth SIG Mesh Model Specification v1.0

After provisioning, the button clients must
be configured to publish and the LED servers to subscribe.
If a server is provided with a publish address, it will
also publish its relevant status.

Requirements
************
This sample has been tested on the Nordic nRF52840-PDK board, but would
likely also run on the nrf52dk_nrf52832 board.

Building and Running
********************
This sample can be found under :zephyr_file:`samples/boards/nrf/mesh/onoff_level_lighting_vnd_app` in the
Zephyr tree.

The following commands build the application.

.. zephyr-app-commands::
   :zephyr-app: samples/boards/nrf/mesh/onoff_level_lighting_vnd_app
   :board: nrf52840dk_nrf52840
   :goals: build flash
   :compact:

Provisioning is done using the BlueZ meshctl utility. In this example, we'll use meshctl commands to bind:

- Button1, Button2, and LED1 to application key 1. It then configures Button1 and Button2
  to publish to group 0xC000 and LED1 to subscribe to that group.
- Button3, Button4, and LED3 to application key 1. It then configures Button3 and Button4
  to publish to group 0xC000 and LED3 to subscribe to that group.

.. code-block:: console

   discover-unprovisioned on
   provision <discovered UUID>
   menu config
   target 0100
   appkey-add 1
   bind 0 1 1000
   bind 0 1 1001
   bind 0 1 1002
   bind 0 1 1003
   sub-add 0100 c000 1000
   sub-add 0100 c000 1002
   pub-set 0100 c000 1 0 5 1001
   pub-set 0100 c000 1 0 5 1003

The meshctl utility maintains a persistent JSON database containing
the mesh configuration. As additional nodes (boards) are provisioned, it
assigns sequential unicast addresses based on the number of elements
supported by the node. This example supports 2 elements per node.

The meshctl target for configuration must be the root element's unicast
address as it is the only one that has a configuration server model. If
meshctl is gracefully exited, it can be restarted and reconnected to
network 0x0.

The meshctl utility also supports a onoff model client that can be used to
change the state of any LED that is bound to application key 0x1.
This is done by setting the target to the unicast address of the element
that has that LED's model and issuing the onoff command.
Group addresses are not supported.

This application was derived from the sample mesh skeleton at
:zephyr_file:`samples/bluetooth/mesh`.

See :ref:`bluetooth samples section <bluetooth-samples>` for details.
