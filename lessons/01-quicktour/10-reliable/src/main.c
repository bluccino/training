//==============================================================================
// main.c for 10-clisrv (Bluetooth mesh client/server app)
//==============================================================================

  #include "bluccino.h"                     // access bluccino stuff
  #include "bl_gonoff.h"                    // generic on/off mesh models
  #include "bl_mpub.h"                      // mesh publisher
  #include "bl_trans.h"

  #define PMI  app                          // public module interface

  int app(BL_ob *o, int val);               // forward declaration of PMI

//==============================================================================
// defining our transition object
//==============================================================================

  static BL_trans trans = {0,0,0,0};

//==============================================================================
// worker: generic on/off status update
//==============================================================================

  static int gonoff_sts(BL_ob *o, int val)
  {
    BL_goo *g = bl_data(o);
    bl_log(1,BL_R "receive [GOOSRV:STS @%d,<#%d,/%dms,&%dms>,%d]",
                  o->id,  g->tid,g->tt,g->delay,  val);

    bl_trans(&trans,&g->trans);        // update transition in trans. object
    return 0;                          // OK
  }

//==============================================================================
// worker: switch status update
// - with any switch status change (@id does not matter) forward switch status
//   value with 1s delay to mesh generic on/off client
// - use a BL_goo data structure to define delay
//==============================================================================

  static int switch_sts(BL_ob *o, int val)
  {
    BL_goo goo = {delay:200, tt:100};

    bl_log(1,BL_R "post [#GOOCLI:SET @%d,</%dms,&%dms>,%d] -> (app)",
                  o->id, goo.tt,goo.delay, val);
    return _bl_gooset(1,&goo,val,(PMI));    // [GONOFF:SET @1,&goo,val] -> (PMI)
  }

//==============================================================================
// worker: system tick (manages transition)
//==============================================================================

  static int sys_tick(BL_ob *o, int val)
  {
    if (bl_period(o,50))                    // every 50 ms
    {
      if (bl_fin(&trans))
        _bl_led(1,trans.target,(PMI));      // turn LED @1 on/off
    }
    return 0;                               // OK
  }

//==============================================================================
//
// (U) := (bl_up);  (D) := (bl_down)
//
//                  +--------------------+
//                  |        APP         |
//                  +--------------------+
//                  |      SWITCH:       |  SWITCH interface
// (U)->      STS ->|      @id,val       |  receive switch @id status
//                  +--------------------+
//                  |      GOOSRV:       |  GOOSRV input interface
// (U)->      STS ->|  @id,<BL_goo>,val  |  recieve generic on/off status update
//                  |....................|
//                  |     #GOOCLI:       |  GOOCLI output interface
// (D)<-      SET <-|  @id,<BL_goo>,val  |  publish generic on/off SET message
//                  +--------------------+
//                  |        LED:        |  LED interface
// (D)<-      SET <-|      @id,val       |  set LED @id on/off
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)                // public APP module interface
  {
    static BL_oval D = bl_down;             // down gear
    static BL_oval P = bl_mpub;             // mesh publisher

    switch (bl_id(o))
    {
      case SYS_INIT_0_cb_0:
        return bl_fwd(o,val,(P));           // init bl_mpub module

      case SYS_TICK_id_BL_pace_cnt:
        bl_fwd(o,val,(P));                  // tick bl_mpub module
        return sys_tick(o,val);             // delegate to sys_tick() worker

      case SWITCH_STS_id_0_sts:
        return switch_sts(o,val);           // delegate to switch_sts() worker

      case GOOSRV_STS_id_BL_goo_sts:
        return gonoff_sts(o,val);           // delegate to gonoff_sts() worker

      case _GOOCLI_SET_id_BL_goo_onoff:
        return bl_out(o,val,(P));           // output to mesh publisher

      case _LED_SET_id_0_onoff:
      case _LED_TOGGLE_id_0_0:
        return bl_out(o,val,(D));           // output to down gear

      default:
        return 0;                           // OK
    }
  }

//==============================================================================
// main function
// - set verbose level and print hello message
// - run init/tick/tock engine
//==============================================================================

  void main(void)
  {
    bl_hello(5,PROJECT);               // set verbose level, show project title
    bl_cfg(bl_down,_BUTTON,BL_SWITCH); // mask [BUTTON:SWITCH] events only
    bl_engine(app,2,1000);             // run 2/1000ms tick/tock engine
  }
