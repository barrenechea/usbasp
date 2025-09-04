/*
 * LUFA Library Configuration Header File for USBasp
 *
 * This header file is used to configure LUFA's compile time options,
 * as an alternative to the compile time constants supplied through
 * a makefile.
 *
 * License: GNU GPL v2
 */

#ifndef _LUFA_CONFIG_H_
#define _LUFA_CONFIG_H_

    #if (ARCH == ARCH_AVR8)

        /* Non-USB Related Configuration Tokens: */
        #define DISABLE_TERMINAL_CODES

        /* USB Class Driver Related Tokens: */
        #define NO_CLASS_DRIVER_AUTOFLUSH

        /* General USB Driver Related Tokens: */
        #define ORDERED_EP_CONFIG
        #define USE_STATIC_OPTIONS               (USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)
        #define USB_DEVICE_ONLY
        #define USB_STREAM_TIMEOUT_MS            100
        #define NO_LIMITED_CONTROLLER_CONNECT
        #define NO_SOF_EVENTS

        /* USB Device Mode Driver Related Tokens: */
        #define USE_FLASH_DESCRIPTORS
        #define NO_INTERNAL_SERIAL
        #define FIXED_CONTROL_ENDPOINT_SIZE      16
        #define DEVICE_STATE_AS_GPIOR            1
        #define FIXED_NUM_CONFIGURATIONS         1
        #define INTERRUPT_CONTROL_ENDPOINT
        #define NO_DEVICE_REMOTE_WAKEUP
        #define NO_DEVICE_SELF_POWER

    #else

        #error Unsupported architecture for this LUFA configuration file.

    #endif
#endif