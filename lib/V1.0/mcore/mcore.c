//==============================================================================
// mcore.c (version 7 - copied files from 07-mcore)
// multi model mesh demo based mesh core (combining all stuff)
//
// Created by Hugo Pristauz on 2022-Jan-02
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================

  #include "bl_mesh.c"
  #undef LOG
  #undef LOGO
  #undef LOG0

  #include "ble_mesh.c"
  #undef LOG
  #undef LOGO

  #include "device_composition.c"
  #undef LOG
  #undef LOGO

  #include "bl_core.c"

  #include "no_transition_work_handler.c"
  #undef LOG
  #undef LOGO

  #include "publisher.c"
  #undef ONOFF

  #include "state_binding.c"
  #undef LOG
  #undef LOGO

  #include "storage.c"

  #include "transition.c"
