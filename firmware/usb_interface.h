/*
 * USB Interface Abstraction Layer for USBasp
 *
 * This header defines a common interface that both VUSB and LUFA implementations
 * can provide, allowing the main application logic to be completely decoupled
 * from the specific USB stack in use.
 *
 * License: GNU GPL v2 (see Readme.txt)
 */

#ifndef USB_INTERFACE_H_
#define USB_INTERFACE_H_

#include <stdint.h>
#include <stdbool.h>

/* Include uchar definition from usbasp.h */
#include "usbasp.h"

/* Define uchar if not already defined (e.g., by usbdrv.h) */
#ifndef uchar
typedef unsigned char uchar;
#endif

/* Common USB types and constants */
typedef uint16_t usbMsgLen_t;

/* USB constants that may not be defined in all implementations */
#ifndef USB_NO_MSG
#define USB_NO_MSG              ((usbMsgLen_t)-1)
#endif

#ifndef USBDESCR_BOS
#define USBDESCR_BOS            0x0F
#endif

/* Windows compatibility constants - use values from usb_descriptors.h if not defined */
#ifndef VENDOR_CODE
#define VENDOR_CODE             0x5D
#endif

#ifndef MS_OS_2_0_DESCRIPTOR_INDEX
#define MS_OS_2_0_DESCRIPTOR_INDEX  0x07
#endif

/* Forward declaration for usbRequest from V-USB */
struct usbRequest;


/* USB Interface Virtual Function Table */
typedef struct {
    /* Core USB functions */
    void (*init)(void);
    void (*task)(void);
    
    /* HID functions (optional - only if HIDUART is enabled) */
#ifdef __HIDUART__
    bool (*interrupt_ready)(void);
    bool (*interrupt3_ready)(void);
    void (*send_interrupt)(uchar *data, uchar len);
    void (*send_interrupt3)(uchar *data, uchar len);
#endif
} usb_interface_t;

/* Global USB interface - set by the implementation module */
extern const usb_interface_t *usb_interface;

/* Interface functions - these call through the function table */
static inline void usb_init(void) {
    usb_interface->init();
}

static inline void usb_task(void) {
    usb_interface->task();
}

#ifdef __HIDUART__
static inline bool usb_interrupt_ready(void) {
    return usb_interface->interrupt_ready ? usb_interface->interrupt_ready() : false;
}

static inline bool usb_interrupt3_ready(void) {
    return usb_interface->interrupt3_ready ? usb_interface->interrupt3_ready() : false;
}

static inline void usb_send_interrupt(uchar *data, uchar len) {
    if (usb_interface->send_interrupt) {
        usb_interface->send_interrupt(data, len);
    }
}

static inline void usb_send_interrupt3(uchar *data, uchar len) {
    if (usb_interface->send_interrupt3) {
        usb_interface->send_interrupt3(data, len);
    }
}
#endif

/* Implementation registration functions - called by VUSB/LUFA modules */
void usb_register_vusb_interface(void);

#endif /* USB_INTERFACE_H_ */