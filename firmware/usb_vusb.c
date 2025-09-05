/*
 * VUSB Implementation Module for USBasp
 *
 * This module implements the USB interface abstraction using V-USB
 * for legacy AVR targets (ATmega8, ATmega48, ATmega88).
 *
 * License: GNU GPL v2 (see Readme.txt)
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usb_interface.h"
#include "usbasp.h"

/* V-USB specific includes */
#include "usbdrv.h"

/* External functions from main.c that handle the actual USB protocol */
extern usbMsgLen_t usbFunctionSetup(uchar data[8]);
extern uchar usbFunctionRead(uchar *data, uchar len);
extern uchar usbFunctionWrite(uchar *data, uchar len);
extern usbMsgLen_t usbFunctionDescriptor(struct usbRequest *rq);

#ifdef __HIDUART__
extern void usbFunctionWriteOut(uchar *data, uchar len);
#endif

/* VUSB Implementation Functions */
static void vusb_init(void) {
    /* Initialize V-USB */
    usbInit();
}

static void vusb_task(void) {
    /* Poll V-USB */
    usbPoll();
}


#ifdef __HIDUART__
static bool vusb_interrupt_ready(void) {
    return usbInterruptIsReady();
}

static bool vusb_interrupt3_ready(void) {
    return usbInterruptIsReady3();
}

static void vusb_send_interrupt(uchar *data, uchar len) {
    usbSetInterrupt(data, len);
}

static void vusb_send_interrupt3(uchar *data, uchar len) {
    usbSetInterrupt3(data, len);
}
#endif

/* VUSB Interface Implementation */
static const usb_interface_t vusb_interface = {
    .init = vusb_init,
    .task = vusb_task,
#ifdef __HIDUART__
    .interrupt_ready = vusb_interrupt_ready,
    .interrupt3_ready = vusb_interrupt3_ready,
    .send_interrupt = vusb_send_interrupt,
    .send_interrupt3 = vusb_send_interrupt3,
#endif
};

/* Registration function */
void usb_register_vusb_interface(void) {
    usb_interface = &vusb_interface;
}