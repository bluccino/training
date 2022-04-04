================================================================================
02-swcli (using WLTINY wireless core and HWTINY hardware core)
================================================================================

- switch-client mesh application: switch status updates are posted down to the
  generic on/off client with corresponding instance @id
- This is shown in the following event flow chart

			    BL_UP                APP                BL_DOWN
					  |                   |                   |
					  |   [SWITCH:STS]    |                   |
					  o------------------>|                   |
					  |      @id,sts      |                   |
					  |                   |                   |
					  |            +-------------+            |
					  |            | onoff = sts |            |
					  |            +-------------+            |
					  |                   |                   |
					  |                   |     [LED:SET]     |
					  |                   o------------------>|
					  |                   |     @id,onoff     |
					  |                   |                   |

- from the event flow chart we can easily derive the app's module interface

                 +--------------------+
                 |        APP         |
                 +--------------------+
                 |      SWITCH:       |  SWITCH interface
     (^)-> STS ->|      @id,sts       |  receive switch @id status
                 +--------------------+
                 |      GOOCLI:       |  GOOCLI ifc. (generic on/off client)
     (v)<- SET <-| @id,<BL_goo>,onoff |  publish generic on/off SET message
                 +--------------------+

- in the wireless core dephts button press events are toggling an internal
  switch state, while status updates (sts) of the toggle switch are posted to
	the app as [SWITCH:STS @id,sts] events
- the app dispatches the SWITCH:STS messages and posts in to the down gear in
  terms of [LED:SET @id,onoff] event messages, where the onoff value corresponds
	to the status value (sts) of the switch
- Note the path definitions to HWTINY and WLTINY cores:

	  set (LIB ../../../lib/V1.0)
	  set (HWC ${LIB}/core/hwcore/hwtiny)  # tiny hardware core
	  set (WLC ${LIB}/core/wlcore/wltiny)  # tiny wireless core

- Also note that the core (BL_CORE, HWTINY, WLTINY) needs to be initialized
  and to be ticked/tocked. This is done by the bl_engine() (Bluccino engine)
	call in main()
