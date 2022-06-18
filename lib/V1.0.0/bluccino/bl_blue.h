//==============================================================================
//  bl_blue.h
//  auxillary header to include Bluetooth/Mesh Zephyr-stuff
//
//  Created by Hugo Pristauz on 19.02.2022
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_BLUE_H__
#define __BL_BLUE_H__

#include "bl_type.h"

    #if defined(__ZEPHYR__)
        #include <settings/settings.h>
        #include <bluetooth/bluetooth.h>
        #include <bluetooth/conn.h>
        #include <bluetooth/l2cap.h>
        #include <bluetooth/hci.h>
        #include <bluetooth/mesh.h>
    #endif // defined(__ZEPHYR__)


//==============================================================================
// get own Bluetooth address
//==============================================================================

    void bl_get_bdaddr(uint8_t *paddr);

#endif // __BL_BLUE_H__
