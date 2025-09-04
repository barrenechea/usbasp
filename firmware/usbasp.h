/*
 * usbasp.c - part of USBasp
 *
 * 2023 Add USBASP_CAP_SNHIDUPDATE by Dimitrios Chr. Ioannidis ( d.ioannidis@nephelae.eu )
 *
 * Autor..........: Thomas Fischl <tfischl@gmx.de>
 * Description....: Definitions and macros for usbasp
 * Licence........: GNU GPL v2 (see Readme.txt)
 * Creation Date..: 2009-02-28
 * Last change....: 2023-03-22
 */

#ifndef USBASP_H_
#define USBASP_H_

/* Ensure uchar is defined for both V-USB and LUFA builds */
#ifndef uchar
#define uchar unsigned char
#endif

/* USB function call identifiers */
// ISP:
#define USBASP_FUNC_CONNECT             1
#define USBASP_FUNC_DISCONNECT          2
#define USBASP_FUNC_TRANSMIT            3
#define USBASP_FUNC_READFLASH           4
#define USBASP_FUNC_ENABLEPROG          5
#define USBASP_FUNC_WRITEFLASH          6
#define USBASP_FUNC_READEEPROM          7
#define USBASP_FUNC_WRITEEEPROM         8
#define USBASP_FUNC_SETLONGADDRESS      9
#define USBASP_FUNC_SETISPSCK           10
// TPI:
#define USBASP_FUNC_TPI_CONNECT         11
#define USBASP_FUNC_TPI_DISCONNECT      12
#define USBASP_FUNC_TPI_RAWREAD         13
#define USBASP_FUNC_TPI_RAWWRITE        14
#define USBASP_FUNC_TPI_READBLOCK       15
#define USBASP_FUNC_TPI_WRITEBLOCK      16
#define USBASP_FUNC_GETCAPABILITIES     127
// UART:
#define USBASP_FUNC_UART_CONFIG         60
#define USBASP_FUNC_UART_DISABLE        63

/* USBASP capabilities */
#define USBASP_CAP_0_TPI                0x01
#define USBASP_CAP_6_UART               0x40
#define USBASP_CAP_HIDUART              0x80
#define USBASP_CAP_SNHIDUPDATE          0x20

#define USBASP_CAP_12MHZ_CLOCK          0x00
#define USBASP_CAP_16MHZ_CLOCK          0x01
#define USBASP_CAP_18MHZ_CLOCK          0x02
#define USBASP_CAP_20MHZ_CLOCK          0x03

/* programming state */
#define PROG_STATE_IDLE                 0
#define PROG_STATE_WRITEFLASH           1
#define PROG_STATE_READFLASH            2
#define PROG_STATE_READEEPROM           3
#define PROG_STATE_WRITEEEPROM          4
#define PROG_STATE_TPI_READ             5
#define PROG_STATE_TPI_WRITE            6
#define PROG_STATE_SET_REPORT           7

/* uart state */
#define UART_STATE_ENABLED              16
#define UART_STATE_DISABLED             0

/* Only prog_sck needs to be global for ISP functions */
extern uchar prog_sck;

/* USB function declarations - shared by V-USB and LUFA */
#ifdef USE_LUFA
/* LUFA needs access to these functions */
typedef unsigned int usbMsgLen_t;
struct usbRequest {
    uchar bmRequestType;
    uchar bRequest;
    union {
        unsigned int word;
        uchar bytes[2];
    } wValue, wIndex, wLength;
};

/* V-USB constants needed by main.c functions */
#define USBRQ_TYPE_MASK         0x60
#define USBRQ_TYPE_VENDOR       0x40
#define USBRQ_TYPE_CLASS        0x20
#define USBRQ_RCPT_MASK         0x1f
#define USBRQ_RCPT_DEVICE       0x00
#define USBRQ_RCPT_INTERFACE    0x01
#define USBRQ_HID_GET_REPORT    0x01
#define USBRQ_HID_SET_REPORT    0x09
#define USB_NO_MSG              0xff
#define USBDESCR_BOS            0x0f

/* Function declarations */
usbMsgLen_t usbFunctionDescriptor(struct usbRequest *rq);
usbMsgLen_t usbFunctionSetup(uchar data[8]);
uchar usbFunctionRead(uchar *data, uchar len);
uchar usbFunctionWrite(uchar *data, uchar len);
void usbFunctionWriteOut(uchar *data, uchar len);

/* Variables that main.c functions need - LUFA will provide these */
extern uchar featureReport[8];
extern uchar replyBuffer[8];

/* Vendor constants for Windows compatibility */
#define VENDOR_CODE 0x5D
#define MS_OS_2_0_DESCRIPTOR_INDEX 0x07
extern const uchar MS_2_0_OS_DESCRIPTOR_SET[];
extern const uchar BOS_DESCRIPTOR[];

/* Descriptor sizes - LUFA needs these */
#define BOS_DESCRIPTOR_SIZE 33
#define MS_2_0_OS_DESCRIPTOR_SET_SIZE 173

/* V-USB interrupt functions - LUFA provides stubs */
void usbSetInterrupt(uchar *data, uchar len);
void usbSetInterrupt3(uchar *data, uchar len);

/* HID endpoint functions from main.c */
void HID_EP_1_IN(void);
void HID_EP_3_IN(void);
#endif

/* Block mode flags */
#define PROG_BLOCKFLAG_FIRST            1
#define PROG_BLOCKFLAG_LAST             2

/* ISP SCK speed identifiers */
#define USBASP_ISP_SCK_AUTO             0
#define USBASP_ISP_SCK_0_5              1   /* 500 Hz */
#define USBASP_ISP_SCK_1                2   /*   1 kHz */
#define USBASP_ISP_SCK_2                3   /*   2 kHz */
#define USBASP_ISP_SCK_4                4   /*   4 kHz */
#define USBASP_ISP_SCK_8                5   /*   8 kHz */
#define USBASP_ISP_SCK_16               6   /*  16 kHz */
#define USBASP_ISP_SCK_32               7   /*  32 kHz */
#define USBASP_ISP_SCK_93_75            8   /*  93.75 kHz */
#define USBASP_ISP_SCK_187_5            9   /* 187.5  kHz */
#define USBASP_ISP_SCK_375              10  /* 375 kHz   */
#define USBASP_ISP_SCK_750              11  /* 750 kHz   */
#define USBASP_ISP_SCK_1500             12  /* 1.5 MHz   */
#define USBASP_ISP_SCK_3000             13  /* 3 MHz   */

// UART flags.
#define USBASP_UART_PARITY_MASK         0b11
#define USBASP_UART_PARITY_NONE         0b00
#define USBASP_UART_PARITY_EVEN         0b01
#define USBASP_UART_PARITY_ODD          0b10

#define USBASP_UART_STOP_MASK           0b100
#define USBASP_UART_STOP_1BIT           0b000
#define USBASP_UART_STOP_2BIT           0b100

#define USBASP_UART_BYTES_MASK          0b111000
#define USBASP_UART_BYTES_5B            0b000000
#define USBASP_UART_BYTES_6B            0b001000
#define USBASP_UART_BYTES_7B            0b010000
#define USBASP_UART_BYTES_8B            0b011000
#define USBASP_UART_BYTES_9B            0b100000

/* macros for gpio functions */
/* LEDs are active low */
#ifdef USE_LUFA
/* Arduino Micro LED definitions - LEDs are ACTIVE LOW */
/* RX LED (red) on PB0, TX LED (red) on PD5 */
#define ledRedOff()                     PORTB |= (1 << PB0); DDRB |= (1 << PB0)   // HIGH = OFF
#define ledRedOn()                      PORTB &= ~(1 << PB0); DDRB |= (1 << PB0)  // LOW = ON
#define ledGreenOff()                   PORTD |= (1 << PD5); DDRD |= (1 << PD5)   // HIGH = OFF  
#define ledGreenOn()                    PORTD &= ~(1 << PD5); DDRD |= (1 << PD5)  // LOW = ON
#else
/* Legacy targets - original pins */
#define ledRedOff()                     DDRC &= ~(1 << PC1)
#define ledRedOn()                      DDRC |= (1 << PC1)
#define ledGreenOff()                   DDRC &= ~(1 << PC0)
#define ledGreenOn()                    DDRC |= (1 << PC0)
#endif

#endif /* USBASP_H_ */