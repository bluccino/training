//==============================================================================
// bl_gpio.h
// shorthannd typedefs and helpers for GPIO setup
//
// Created by Hugo Pristauz on 2022-Jan-09
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================
//
// GP_io structure
//
//   typedef struct gpio_dt_spec
//           {
//          	 const GP_device *port;        // const struct device *port;
//           	 GP_pin pin;                   // gpio_pin_t pin;
//             gpio_dt_flags_t dt_flags;
//           } GP_io;
//
//==============================================================================

#ifndef __BL_GPIO_H__
#define __BL_GPIO_H__

  #include <zephyr.h>
  #include <device.h>
  #include <drivers/gpio.h>
  #include <sys/util.h>
  #include <sys/printk.h>
  #include <inttypes.h>

//==============================================================================
// some shorthands
//==============================================================================

  #define GP_IO                GPIO_DT_SPEC_GET_OR

  typedef uint32_t             GP_pins;          // PINs qualifier in irs
  typedef gpio_flags_t         GP_flags;         // GPIO flags
  typedef gpio_pin_t           GP_pin;           // GPIO pin

  typedef struct device        GP_device;        // GPIO device
  typedef struct gpio_dt_spec  GP_io;            // GPIO device tree spec
  typedef struct gpio_callback GP_ctx;           // GPIO callback context

  typedef void (*GP_irs)(const GP_device *dev, GP_ctx *ctx, GP_pins pins);

//==============================================================================
// GPIO pin structure
//==============================================================================

  typedef struct BL_pin
  {
    GP_io io;                          // input I/O pin
    GP_ctx ctx;                        // context
  } BL_pin;

  #define BL_PIN(alias)      {.io = GP_IO(alias, gpios,{0}) }

//==============================================================================
// GPIO pin configuration (helper)
// - usage: gp_pin_cfg(io,flags)
// -        gp_pin_cfg(io,GPIO_INPUT)
// -        gp_pin_cfg(io,GPIO_INPUT | GPIO_INT_DEBOUNCE)
// -        gp_pin_cfg(io,GPIO_OUTPUT)
//==============================================================================

  static inline int gp_pin_cfg(const GP_io *io, GP_flags flags)
  {
    int err = gpio_pin_configure_dt(io,flags);
    if (err)
      printk("Error %d: failed to configure %s @ pin %d\n",
  		       err, io->port->name, io->pin);

    return err;
  }

//==============================================================================
// GPIO pin interrupt configuration (helper)
// - usage: gp_int_cfg(io,flags)
// -        gp_int_cfg(io,GPIO_INT_EDGE_TO_ACTIVE)
// -        gp_int_cfg(io,GPIO_INT_EDGE_BOTH)
//==============================================================================

  static inline int gp_int_cfg(const GP_io *io, GP_flags flags)
  {
    int err = gpio_pin_interrupt_configure_dt(io,flags);
    if (err)
      printk("Error %d: failed to config %s pin %d interrupt\n",
  			     err, io->port->name, io->pin);

    return err;
  }

//==============================================================================
// GPIO pin add interrupt callback (helper)
// - usage: gp_add_cb(io,ctx,irs)
// -        gp_add_cb(&but,&but_ctx,but_irs)
//==============================================================================

  static inline void gp_add_cb(const GP_io *io, GP_ctx *ctx, GP_irs cb)
  {
    gpio_init_callback(ctx, cb, BIT(io->pin));
    gpio_add_callback(io->port, ctx);
  }

//==============================================================================
// get I/O input value
// - usage: val = bl_pin_get(&io)
//==============================================================================

  static inline int gp_pin_get(const GP_io *io)
  {
    return gpio_pin_get_dt(io);
  }

//==============================================================================
// set I/O output value
// - usage: gp_pin_set(&io,val)
//==============================================================================

  static inline int gp_pin_set(const GP_io *io, int value)
  {
    return gpio_pin_set_dt(io,value);
  }

//==============================================================================
// config I/O pin
// - usage: bl_pin_cfg(pin,flags)
// -        bl_ipin(&button, GPIO_INPUT)
//==============================================================================

  static inline void bl_pin_cfg(BL_pin *pin, GP_flags flags)
  {
    if (!device_is_ready(pin->io.port))
    {
      LOG_GPIO(1,BL_R"error: pin %s not ready", pin->io.port->name);
      return;
    }

    gp_pin_cfg(&pin->io, flags);
  }

//==============================================================================
// attach interrupt handler to I/O pin
// - usage: bl_pin_attach(pin,flags,cb)
// -        bl_pin_attach(&button, GPIO_INT_EDGE_TO_ACTIVE, but_cb)
// -        bl_pin_attach(&button, GPIO_INT_EDGE_BOTH, but_cb)
//==============================================================================

  static inline void bl_pin_attach(BL_pin *pin, GP_flags flags, GP_irs cb)
  {
    if (!device_is_ready(pin->io.port))
    {
      LOG_GPIO(1,BL_R"error: pin %s not ready", pin->io.port->name);
      return;
    }

    if (flags)
      gp_int_cfg(&pin->io, flags);

    if (cb)
      gp_add_cb(&pin->io, &pin->ctx, cb);
  }

//==============================================================================
// get input pin value
// - usage: val = bl_pin_get(&pin)
//==============================================================================

  static inline int bl_pin_get(BL_pin *pin)
  {
    return gpio_pin_get_dt(&pin->io);
  }

//==============================================================================
// set output pin value
// - usage: bl_pin_set(&pin,val)
//==============================================================================

  static inline int bl_pin_set(BL_pin *pin, int value)
  {
    return gpio_pin_set_dt(&pin->io,value);
  }

#endif // __BL_GPIO_H__
