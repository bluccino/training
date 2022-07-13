#===============================================================================
# 03-alive demo
#===============================================================================

This demo separates the original main.c file of onoff_app sample into
  * a tiny new main.c file with a simple main loop demonstrating periodic
    "I'm alive!" log messages
  * and a simple Bluccino mesh core which allows to switch each of the
    4 LEDs of a nRF52 DK board on/off by single (ON) or double (OFF) button
    press
  * additionally with a provisioning app the board and similar other boards
    can be onboarded to a Bluetooth mesh network, such that by proper confi-
    guration any button can control any LED in the mesh network, no matter
    whether button and LED are on the same board or different boards.

# Lessons to Learn

  * how to provide a config.h file which is included automatically for each
    #include "bluccino.h" statement
  * how to add migration code to an existing program which can be activated
    or deactivated
  * how to use the functions bl_dbg() and bl_prt() for log and module level
    based customized logging
  * how to use the Nordic nRF Mesh app to initialize a new Bluetooth mesh
    network, to provision and configure mesh nodes for proper communication

# Project Configuration

  * Stack size needs to be configured as 1024 bytes
    (see prj.conf: CONFIG_MAIN_STACK_SIZE=1024)

# Application Architecture

                    +----------------+
                    |      MAIN      |
                    +----------------+
    ====================o|o===o|o===========================================
    ||                   |     |  Bluccino  API                           ||
    ====================o|o===o|o===========================================
        ||               |     |                                      ||
        ||       [:INIT] v     v [:LOOP]                              ||
        ||      +----------------------------------------------|      ||
        oo      |                                              |      oo
  ) ) ) ) ) ) ) )                    ocore1                    ) ) ) ) ) ) ) )
        oo      |                                              |      oo
        ||      +----------------------------------------------+      ||
        ||============================================================||
        ||                                                            ||
