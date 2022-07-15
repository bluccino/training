#===============================================================================
# 04-attention demo
#===============================================================================

With this demo we migrate from ocore1.c mesh core to ocore2.c, supporting
provisioning and attention messages being forwarded from the core to Bluccino
API layer. The high level app behavior is changed as follows:

  * periodic loggings with log text
       - "I'm a (unprovisoned) mesh device" (in unprovisioned case)
       - "I'm a (provisoned) mesh node" (in provisioned case)
  * All log messages show color switch of the log header (time stamps)
    when attention is in process or node state changes from device state
    (unprovisioned) to node state (provisioned)
  * App is also logging an addtional log message "attention" during active
    attention mode

# Lessons to Learn

  * how additional 'migration defines' are added to config.h (remember: config.h
    file is included automatically for each #include "bluccino.h" statement
    (this is caused by proper settings in CMakeLists.txt)
  * how to transform ocore.1 to ocore.2 by further adding of migration code
  * how to post attention and provisioning messages from ocore2.c up to
    Bluccino API where global state variables bl_provisioned and bl_attention
    are updated accordingly
  * how to use the bl_log() function

# Project Configuration

  * Stack size needs to be configured as 1024 bytes
    (see prj.conf: CONFIG_MAIN_STACK_SIZE=1024)

# Application Architecture

                    +----------------+
                    |      MAIN      |
                    +----------------+
    ====================o|o===o|o===========================================
    ||                   |     |  Bluccino  API     ^     ^               ||
    ====================o|o===o|o==================o|o===o|o================
        ||               |     |                    |     |           ||
        ||       [:INIT] v     v [:LOOP]     [:PROV]|     |[:ADV]     ||
        ||      +----------------------------------------------|      ||
        oo      |                                              |      oo
  ) ) ) ) ) ) ) )                    ocore2                    ) ) ) ) ) ) ) )
        oo      |                                              |      oo
        ||      +----------------------------------------------+      ||
        ||============================================================||
        ||                                                            ||
