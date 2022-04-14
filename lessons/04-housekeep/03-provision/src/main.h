//==============================================================================
// main.h
//==============================================================================
//
// Recap: the wireless core
//
// (N) := (BL_NVM);  (!) := (<parent>); (O) := (<out>); (B) := (BLE_MESH);
//                  +--------------------+
//                  |        BL_WL       | wireless core
//                  +--------------------+
//                  |        SYS:        | SYS: public interface
// (!)->     INIT ->|       @id,cnt      | init module, store <out> callback
// (!)->     TICK ->|       @id,cnt      | tick the module
// (!)->     TOCK ->|       @id,cnt      | tock the module
//                  +--------------------+
//                  |       MESH:        | MESH upper interface
// (O)<-      PRV <-|       onoff        | provision on/off
// (O)<-      ATT <-|       onoff        | attention on/off
//                  |....................|
//                  |       MESH:        | MESH lower interface
// (B)->      PRV ->|       onoff        | provision on/off
// (B)->      ATT ->|       onoff        | attention on/off
//                  +--------------------+
//                  |       RESET:       | RESET: public interface
// (O)<-      DUE <-|                    | reset timer is due
// (!)->      INC ->|         ms         | inc reset counter & set due timer
// (!)->      PRV ->|                    | unprovision node
//                  +--------------------+
//                  |        NVM:        | NVM: upper interface
// (O)<-    READY <-|       ready        | notification that NVM is now ready
// (!)->    STORE ->|      @id,val       | store value in NVM at location @id
// (!)->   RECALL ->|        @id         | recall value in NVM at location @id
// (!)->     SAVE ->|                    | save NVM cache to NVM
//                  |....................|
//                  |        NVM:        | NVM: lower interface
// (N)->    READY ->|       ready        | notification that NVM is now ready
// (N)<-    STORE <-|      @id,val       | store value in NVM at location @id
// (N)<-   RECALL <-|        @id         | recall value in NVM at location @id
// (N)<-     SAVE <-|                    | save NVM cache to NVM
//                  +====================+
//                  |       #SET:        | SET: private interface
// (#)->      PRV ->|       onoff        | provision on/off
// (#)->      ATT ->|       onoff        | attention on/off
//                  +--------------------+
//                  |      #RESET:       | RESET: private interface
// (#)->      DUE ->|                    | reset timer is due
//                  +--------------------+
//
//==============================================================================
//
// Initializing scheme and default output flow
//
// +- MAIN => bl_engine(app,tick_ms,tock_ms)
//     |
//     |   |<======================|
//     |   |                       |
//     +- APP ====================>|    (application)
//     |   |                       |
//     |   +- ATTENTION ==========>|    (attention house keeping)
//     |   |                       |
//     |   +- PROVISION ==========>|    (provision house keeping)
//     |                           |
//   ..|...........................|...................................
//     |                           |
//     +- BLUCCINO                 |    (Bluccino module)
//         |                       |
//         +- BL_TOP =============>|    (top gear)
//         |   |        
//         |   |<==================|
//         |                       |
//         +- BL_UP ==============>|    (up gear)
//         |   |
//         |   |<==================|
//         |                       |
//         +- BL_DOWN ========>|   |    (down gear)
//             |               |   |
//             |   |<==========|   |
//             |   |               |
//             +- BL_CORE ========>|    (core system)
//
//==============================================================================
//
// Event flowchart: Initializing
//
//    APP            UP/DOWN         ATTENTION       PROVISION
//     |               | |               |               |
//     |          [SYS:INIT <app>]       |               |
//     o-------------------------------->|               |
//     |               | |               |               |
//     |          [SYS:INIT <app>]       |               |
//     o------------------------------------------------>|
//     |               | |               |               |
//
// Event flowchart: provision blinking
//
//    APP            UP/DOWN         ATTENTION       PROVISION
//     |               | |               |               |
//     |               | |       prv=[MESH:PRV sts]      |
//     |               o-------------------------------->|
//     |               | |               |               |
//     :               : :               :               :
//     |               | |               |               |
//     |          cnt=[SYS:TICK @id,cnt] |               |
//     o------------------------------------------------>|
//     |               | |               |               |
//     |               | |               | att=[GET:ATT] |
//     |               | |               |<------------->o
//     |               | |               |               |
//     |               | |               |               - prv && !att
//     |               | |               |               |
//     |               | |               |     +-------------------+
//     |               | |               |     |     M = g(prv)    |
//     |               | |               |     | onoff = cnt%M == 0|
//     |               | |               |     +-------------------+
//     |               | |               |               |
//     |               | |      [LED:SET @0,onoff]       |
//     |               | |<------------------------------o
//     |               | |               |               |
//
// Event flowchart: attention blinking
//
//    APP            UP/DOWN         ATTENTION       PROVISION
//     |               | |               |               |
//     |               | | att=[MESH:ATT sts]            |
//     |               o---------------->|               |
//     |               | |               |               |
//     :               : :               :               :
//     |               | |               |               |
//     |    cnt=[SYS:TICK @id,cnt]       |               |
//     o-------------------------------->|               |
//     |               | |               |               |
//     |               | |               - att           |
//     |               | |               - every 800ms   |
//     |               | |               |               |
//     |               | |[LED:TOGGLE @0]|               |
//     |               | |<--------------o               |
//     |               | |               |               |
