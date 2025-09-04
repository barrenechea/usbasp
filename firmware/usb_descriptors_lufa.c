/*
 * USB Descriptors for USBasp with LUFA
 *
 * This file contains the USB descriptors for the USBasp device
 * when compiled with LUFA for ATmega32U4, including HID support.
 *
 * License: GNU GPL v2 (see Readme.txt)
 */

#ifdef USE_LUFA

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <lufa/LUFA/Drivers/USB/USB.h>
#include "usbasp.h"
#include "serialnumber.h"

/* LUFA endpoint definitions */
#define HID_TX_EPADDR           (ENDPOINT_DIR_IN  | 1)
#define HID_RX_EPADDR           (ENDPOINT_DIR_OUT | 2)
#define HID_TX3_EPADDR          (ENDPOINT_DIR_IN  | 3)
#define HID_EPSIZE              8

/* Interface and string descriptor IDs */
enum InterfaceDescriptors_t
{
    INTERFACE_ID_VENDOR = 0,
    INTERFACE_ID_HID1   = 1,
    INTERFACE_ID_HID2   = 2,
};

enum StringDescriptors_t
{
    STRING_ID_Language     = 0,
    STRING_ID_Manufacturer = 1,
    STRING_ID_Product      = 2,
    STRING_ID_SerialNumber = 3,
};

/* USB descriptor configuration type */
typedef struct
{
    USB_Descriptor_Configuration_Header_t Config;
    USB_Descriptor_Interface_t            Vendor_Interface;
    USB_Descriptor_Interface_t            HID_Interface1;
    USB_HID_Descriptor_HID_t              HID_HIDData1;
    USB_Descriptor_Endpoint_t             HID_ReportINEndpoint1;
    USB_Descriptor_Endpoint_t             HID_ReportOUTEndpoint1;
    USB_Descriptor_Interface_t            HID_Interface2;
    USB_HID_Descriptor_HID_t              HID_HIDData2;
    USB_Descriptor_Endpoint_t             HID_ReportINEndpoint2;
} USB_Descriptor_Configuration_t;

/** EEPROM storage for serial number - shared with V-USB implementation */
#ifndef USE_VUSB_DESCRIPTORS
const int EEMEM usbDescriptorStringSerialNumber[] = {
    ((2*4+2) | (3<<8)),  /* USB_STRING_DESCRIPTOR_HEADER(4) */
    '0', '0', '0', '0'   /* Default serial number */
};
#else
extern const int EEMEM usbDescriptorStringSerialNumber[];
#endif

/** HID report descriptor for UART functionality */  
const uint8_t PROGMEM HIDReport[] = {
    HID_RI_USAGE_PAGE(16, 0xFF00), /* Vendor Defined */
    HID_RI_USAGE(8, 0x01),
    HID_RI_COLLECTION(8, 0x01), /* Application */
        HID_RI_USAGE(8, 0x02),
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(16, 0x00FF),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_REPORT_COUNT(8, 0x08),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_USAGE(8, 0x03),
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(16, 0x00FF),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_REPORT_COUNT(8, 0x08),
        HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),
    HID_RI_END_COLLECTION(0),
};

/** Device descriptor structure for USBasp device */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
    .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

    .USBSpecification       = VERSION_BCD(2,0,0),
    .Class                  = USB_CSCP_NoDeviceClass,
    .SubClass               = USB_CSCP_NoDeviceSubclass,
    .Protocol               = USB_CSCP_NoDeviceProtocol,

    .Endpoint0Size          = 16,

    .VendorID               = 0x16c0,  // USBasp vendor ID
    .ProductID              = 0x05dc,  // USBasp product ID
    .ReleaseNumber          = VERSION_BCD(1,5,0),

    .ManufacturerStrIndex   = STRING_ID_Manufacturer,
    .ProductStrIndex        = STRING_ID_Product,
    .SerialNumStrIndex      = STRING_ID_SerialNumber,

    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure - vendor + HID interface */

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
    .Config = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

        .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
        .TotalInterfaces        = 3,  // Vendor + 2 HID interfaces (like V-USB)

        .ConfigurationNumber    = 1,
        .ConfigurationStrIndex  = NO_DESCRIPTOR,

        .ConfigAttributes       = USB_CONFIG_ATTR_RESERVED,

        .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
    },

    .Vendor_Interface = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

        .InterfaceNumber        = 0,
        .AlternateSetting       = 0x00,

        .TotalEndpoints         = 0,  // No endpoints for vendor interface

        .Class                  = USB_CSCP_VendorSpecificClass,
        .SubClass               = USB_CSCP_NoDeviceSubclass,
        .Protocol               = USB_CSCP_NoDeviceProtocol,

        .InterfaceStrIndex      = STRING_ID_Product
    },

    .HID_Interface1 = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

        .InterfaceNumber        = 1,
        .AlternateSetting       = 0x00,

        .TotalEndpoints         = 2,  // IN + OUT endpoints (like V-USB HID Interface 1)

        .Class                  = HID_CSCP_HIDClass,
        .SubClass               = HID_CSCP_NonBootSubclass,
        .Protocol               = HID_CSCP_NonBootProtocol,

        .InterfaceStrIndex      = STRING_ID_Product
    },

    .HID_HIDData1 = {
        .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},

        .HIDSpec                = VERSION_BCD(1,1,1),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = HID_DTYPE_Report,
        .HIDReportLength        = sizeof(HIDReport)
    },

    .HID_ReportINEndpoint1 = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

        .EndpointAddress        = HID_TX_EPADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = HID_EPSIZE,
        .PollingIntervalMS      = 0x05
    },

    .HID_ReportOUTEndpoint1 = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

        .EndpointAddress        = HID_RX_EPADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = HID_EPSIZE,
        .PollingIntervalMS      = 0x05
    },

    .HID_Interface2 = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

        .InterfaceNumber        = 2,
        .AlternateSetting       = 0x00,

        .TotalEndpoints         = 1,  // Only IN endpoint (like V-USB HID Interface 2)

        .Class                  = HID_CSCP_HIDClass,
        .SubClass               = HID_CSCP_NonBootSubclass,
        .Protocol               = HID_CSCP_NonBootProtocol,

        .InterfaceStrIndex      = STRING_ID_Product
    },

    .HID_HIDData2 = {
        .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},

        .HIDSpec                = VERSION_BCD(1,1,1),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = HID_DTYPE_Report,
        .HIDReportLength        = sizeof(HIDReport)
    },

    .HID_ReportINEndpoint2 = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

        .EndpointAddress        = HID_TX3_EPADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = HID_EPSIZE,
        .PollingIntervalMS      = 0x19  // 25ms like V-USB (5 * 0x05)
    }

};

/** Language descriptor structure */
const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

/** Manufacturer descriptor string */
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"www.fischl.de");

/** Product descriptor string */
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"USBasp");


/* BOS and Microsoft OS 2.0 Descriptors for WCID support */
#define VENDOR_CODE 0x5D
#define MS_OS_2_0_DESCRIPTOR_INDEX 0x07

/** BOS Descriptor for Microsoft OS 2.0 support */
const uint8_t PROGMEM BOS_Descriptor[] = {
    0x05, 0x0F, 0x21, 0x00, 0x01,
    0x1C, 0x10, 0x05, 0x00,
    0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C,
    0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F,
    0x00, 0x00, 0x03, 0x06, 0xBE, 0x01, VENDOR_CODE, 0x00
};

/** Microsoft OS 2.0 Descriptor Set */
const uint8_t PROGMEM MS_OS_2_0_Descriptor[] = {
    0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x06, 0xBE, 0x01,
    0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0xB4, 0x01,
    0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x9C, 0x00,
    0x14, 0x00, 0x03, 0x00, 
    'W','I','N','U','S','B', 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x04, 0x00, 0x01, 0x00, 0x28, 0x00,
    'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,
    'e',0x00,'I',0x00,'n',0x00,'t',0x00,'e',0x00,
    'r',0x00,'f',0x00,'a',0x00,'c',0x00,'e',0x00,
    'G',0x00,'U',0x00,'I',0x00,'D',0x00,0x00,0x00,
    0x4e, 0x00,
    '{',0x00,'A',0x00,'D',0x00,'5',0x00,'7',0x00,
    'D',0x00,'3',0x00,'B',0x00,'9',0x00,'-',0x00,
    '1',0x00,'1',0x00,'6',0x00,'6',0x00,'-',0x00,
    '4',0x00,'3',0x00,'F',0x00,'8',0x00,'-',0x00,
    '8',0x00,'7',0x00,'9',0x00,'0',0x00,'-',0x00,
    '0',0x00,'B',0x00,'E',0x00,'1',0x00,'4',0x00,
    'D',0x00,'D',0x00,'C',0x00,'7',0x00,'5',0x00,
    '0',0x00,'4',0x00,'}',0x00,0x00,0x00
};

uint16_t get_bos_descriptor(const void** const DescriptorAddress) {
    *DescriptorAddress = &BOS_Descriptor;
    return sizeof(BOS_Descriptor);
}

uint16_t get_ms_os_descriptor(const void** const DescriptorAddress) {
    *DescriptorAddress = &MS_OS_2_0_Descriptor;
    return sizeof(MS_OS_2_0_Descriptor);
}

/* Custom serial descriptor function - handles dynamic serial like LUFA internal serial */
static void USB_Device_GetEEPROMSerialDescriptor(void)
{
    struct {
        USB_Descriptor_Header_t Header;
        uint16_t UnicodeString[4];  /* 4 characters for serial number */
    } SerialDescriptor;

    SerialDescriptor.Header.Type = DTYPE_String;
    SerialDescriptor.Header.Size = USB_STRING_LEN(4);  /* 2 + 4*2 = 10 bytes */
    
    /* Read serial number from EEPROM */
    SerialDescriptor.UnicodeString[0] = eeprom_read_word((uint16_t*)&usbDescriptorStringSerialNumber[1]);
    SerialDescriptor.UnicodeString[1] = eeprom_read_word((uint16_t*)&usbDescriptorStringSerialNumber[2]);
    SerialDescriptor.UnicodeString[2] = eeprom_read_word((uint16_t*)&usbDescriptorStringSerialNumber[3]);
    SerialDescriptor.UnicodeString[3] = eeprom_read_word((uint16_t*)&usbDescriptorStringSerialNumber[4]);

    Endpoint_ClearSETUP();
    
    /* Use RAM stream function like LUFA internal serial */
    Endpoint_Write_Control_Stream_LE(&SerialDescriptor, sizeof(SerialDescriptor));
    Endpoint_ClearOUT();
}

/** This function is called by the library when in device mode, and must be overridden */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
    const uint8_t  DescriptorType   = (wValue >> 8);
    const uint8_t  DescriptorNumber = (wValue & 0xFF);

    /* Handle serial number request early like LUFA internal serial */
    if ((DescriptorType == DTYPE_String) && (DescriptorNumber == STRING_ID_SerialNumber)) {
        USB_Device_GetEEPROMSerialDescriptor();
        return NO_DESCRIPTOR;  /* Return early, already handled */
    }

    const void* Address = NULL;
    uint16_t    Size    = NO_DESCRIPTOR;

    switch (DescriptorType)
    {
        case DTYPE_Device:
            Address = &DeviceDescriptor;
            Size    = sizeof(USB_Descriptor_Device_t);
            break;
        case DTYPE_Configuration:
            Address = &ConfigurationDescriptor;
            Size    = sizeof(USB_Descriptor_Configuration_t);
            break;
        case DTYPE_String:
            switch (DescriptorNumber)
            {
                case STRING_ID_Language:
                    Address = &LanguageString;
                    Size    = pgm_read_byte(&LanguageString.Header.Size);
                    break;
                case STRING_ID_Manufacturer:
                    Address = &ManufacturerString;
                    Size    = pgm_read_byte(&ManufacturerString.Header.Size);
                    break;
                case STRING_ID_Product:
                    Address = &ProductString;
                    Size    = pgm_read_byte(&ProductString.Header.Size);
                    break;
            }
            break;
        case 0x0F: /* BOS Descriptor */
            Size = NO_DESCRIPTOR;
            break;
        case HID_DTYPE_HID:
            if (wIndex == INTERFACE_ID_HID1) {
                Address = &(((USB_Descriptor_Configuration_t*)&ConfigurationDescriptor)->HID_HIDData1);
                Size = sizeof(USB_HID_Descriptor_HID_t);
            }
            else if (wIndex == INTERFACE_ID_HID2) {
                Address = &(((USB_Descriptor_Configuration_t*)&ConfigurationDescriptor)->HID_HIDData2);
                Size = sizeof(USB_HID_Descriptor_HID_t);
            }
            break;
        case HID_DTYPE_Report:
            if (wIndex == INTERFACE_ID_HID1) {
                Address = &HIDReport;
                Size = sizeof(HIDReport);
            }
            else if (wIndex == INTERFACE_ID_HID2) {
                Address = &HIDReport;
                Size = sizeof(HIDReport);
            }
            break;
    }

    *DescriptorAddress = Address;
    return Size;
}

#endif /* USE_LUFA */