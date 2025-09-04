/*
 * LUFA Implementation Module for USBasp (Clean Version)
 *
 * This module implements the USB interface abstraction using LUFA
 * for modern AVR targets (ATmega32U4) with native USB support.
 *
 * License: GNU GPL v2 (see Readme.txt)
 */

#ifdef USE_LUFA

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <string.h>

#include <lufa/LUFA/Drivers/USB/USB.h>
#include <lufa/LUFA/Drivers/USB/Class/Common/HIDClassCommon.h>

#include "usb_interface.h"
#include "usbasp.h"

/* External functions from main.c that handle the actual USB protocol */
extern usbMsgLen_t usbFunctionSetup(uchar data[8]);
extern uchar usbFunctionRead(uchar *data, uchar len);
extern uchar usbFunctionWrite(uchar *data, uchar len);
extern usbMsgLen_t usbFunctionDescriptor(struct usbRequest *rq);
extern uchar replyBuffer[8];

#ifdef __HIDUART__
extern void usbFunctionWriteOut(uchar *data, uchar len);
#include "cbuf.h"
extern uchar uart_state;
extern uchar prog_state;
static uchar interruptBuffer[8];
static uchar monitorBuffer[8];
#endif

/* LUFA endpoint definitions */
#define HID_TX_EPADDR           (ENDPOINT_DIR_IN  | 1)
#define HID_RX_EPADDR           (ENDPOINT_DIR_OUT | 2)
#define HID_TX3_EPADDR          (ENDPOINT_DIR_IN  | 3)
#define HID_EPSIZE              8

/* LUFA Implementation Functions */
static void lufa_init(void) {
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
    MCUCR |= _BV(JTD);
    MCUCR |= _BV(JTD);
    clock_prescale_set(clock_div_1);
    USB_Init();
    
#ifdef __HIDUART__
    memset(interruptBuffer, 0, sizeof(interruptBuffer));
    memset(monitorBuffer, 0, sizeof(monitorBuffer));
#endif
}

static void lufa_task(void) {
    USB_USBTask();
}

#ifdef __HIDUART__
static bool lufa_interrupt_ready(void) {
    Endpoint_SelectEndpoint(HID_TX_EPADDR);
    return Endpoint_IsINReady();
}

static bool lufa_interrupt3_ready(void) {
    Endpoint_SelectEndpoint(HID_TX3_EPADDR);
    return Endpoint_IsINReady();
}

static void lufa_send_interrupt(uchar *data, uchar len) {
    Endpoint_SelectEndpoint(HID_TX_EPADDR);
    if (Endpoint_IsINReady()) {
        Endpoint_Write_Stream_LE(data, len, NULL);
        Endpoint_ClearIN();
    }
}

static void lufa_send_interrupt3(uchar *data, uchar len) {
    Endpoint_SelectEndpoint(HID_TX3_EPADDR);
    if (Endpoint_IsINReady()) {
        Endpoint_Write_Stream_LE(data, len, NULL);
        Endpoint_ClearIN();
    }
}
#endif

/* LUFA Interface Implementation */
static const usb_interface_t lufa_interface = {
    .init = lufa_init,
    .task = lufa_task,
#ifdef __HIDUART__
    .interrupt_ready = lufa_interrupt_ready,
    .interrupt3_ready = lufa_interrupt3_ready,
    .send_interrupt = lufa_send_interrupt,
    .send_interrupt3 = lufa_send_interrupt3,
#endif
};

/* Registration function */
void usb_register_lufa_interface(void) {
    usb_interface = &lufa_interface;
}

/* LUFA Event Handlers */
void EVENT_USB_Device_Connect(void) {
}

void EVENT_USB_Device_Disconnect(void) {
}

void EVENT_USB_Device_ConfigurationChanged(void) {
    bool ConfigSuccess = true;
    ConfigSuccess &= Endpoint_ConfigureEndpoint(HID_TX_EPADDR, EP_TYPE_INTERRUPT, HID_EPSIZE, 1);
    ConfigSuccess &= Endpoint_ConfigureEndpoint(HID_RX_EPADDR, EP_TYPE_INTERRUPT, HID_EPSIZE, 1);  
    ConfigSuccess &= Endpoint_ConfigureEndpoint(HID_TX3_EPADDR, EP_TYPE_INTERRUPT, HID_EPSIZE, 1);
    (void)ConfigSuccess;
}

void EVENT_USB_Device_ControlRequest(void) {
    /* Convert LUFA control request to V-USB format and call main.c function */
    uchar data[8];
    data[0] = USB_ControlRequest.bmRequestType;
    data[1] = USB_ControlRequest.bRequest;
    data[2] = USB_ControlRequest.wValue & 0xFF;
    data[3] = (USB_ControlRequest.wValue >> 8) & 0xFF;
    data[4] = USB_ControlRequest.wIndex & 0xFF;
    data[5] = (USB_ControlRequest.wIndex >> 8) & 0xFF;
    data[6] = USB_ControlRequest.wLength & 0xFF;
    data[7] = (USB_ControlRequest.wLength >> 8) & 0xFF;
    
    usbMsgLen_t len = usbFunctionSetup(data);
    
    /* Handle HID Get/Set Report requests */
    if ((USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) &&
        (USB_ControlRequest.bRequest == HID_REQ_GetReport) &&
        ((USB_ControlRequest.wValue >> 8) == 3)) {
        if (len > 0) {
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_Stream_LE(replyBuffer, len);
            Endpoint_ClearOUT();
        }
        return;
    }
    
    if ((USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) &&
        (USB_ControlRequest.bRequest == HID_REQ_SetReport)) {
        if (len == USB_NO_MSG && USB_ControlRequest.wLength > 0) {
            Endpoint_ClearSETUP();
            uint8_t featureReportData[USB_ControlRequest.wLength];
            Endpoint_Read_Control_Stream_LE(featureReportData, USB_ControlRequest.wLength);
            Endpoint_ClearIN();
            
            uint8_t remaining = USB_ControlRequest.wLength;
            uint8_t offset = 0;
            while (remaining > 0) {
                uint8_t chunkSize = (remaining > 8) ? 8 : remaining;
                uint8_t result = usbFunctionWrite(&featureReportData[offset], chunkSize);
                if (result == 1) break;
                offset += chunkSize;
                remaining -= chunkSize;
            }
        }
        return;
    }
    
    /* Handle vendor requests */
    if (((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_TYPE) == REQTYPE_VENDOR) &&
        (((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_RECIPIENT) == REQREC_DEVICE) ||
         ((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_RECIPIENT) == REQREC_INTERFACE))) {
        
        if (len == USB_NO_MSG) {
            if (USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST) {
                Endpoint_ClearSETUP();
                uint8_t buffer[8];
                uint8_t bytesRead;
                do {
                    bytesRead = usbFunctionRead(buffer, sizeof(buffer));
                    if (bytesRead > 0) {
                        Endpoint_Write_Control_Stream_LE(buffer, bytesRead);
                    }
                } while (bytesRead == sizeof(buffer));
                Endpoint_ClearOUT();
            } else {
                Endpoint_ClearSETUP();
                if (USB_ControlRequest.wLength > 0) {
                    uint8_t buffer[USB_ControlRequest.wLength];
                    Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
                    Endpoint_ClearIN();
                    
                    uint8_t remaining = USB_ControlRequest.wLength;
                    uint8_t offset = 0;
                    while (remaining > 0) {
                        uint8_t chunkSize = (remaining > 8) ? 8 : remaining;
                        uint8_t result = usbFunctionWrite(&buffer[offset], chunkSize);
                        if (result == 1) break;
                        offset += chunkSize;
                        remaining -= chunkSize;
                    }
                } else {
                    Endpoint_ClearStatusStage();
                }
            }
        } else if (len > 0) {
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_Stream_LE(replyBuffer, (len <= 8) ? len : 8);
            Endpoint_ClearIN();
            Endpoint_ClearStatusStage();
        } else if (len == 0) {
            Endpoint_ClearSETUP();
            Endpoint_ClearStatusStage();
        }
    }
}

bool CALLBACK_HIDParser_FilterHIDReportItem(HID_ReportItem_t* const CurrentItem) {
    (void)CurrentItem;
    return false;
}

#endif /* USE_LUFA */