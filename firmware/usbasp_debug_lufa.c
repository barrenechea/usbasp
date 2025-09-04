/*
 * USBasp Debug Implementation for LUFA
 *
 * This file provides the LUFA implementation of the unified debug interface.
 * It uses LUFA's Serial driver to provide identical debug output format
 * and behavior as V-USB's oddebug system.
 *
 * License: GNU GPL v2 (see Readme.txt)
 */

#ifdef USE_LUFA

#include "usbasp_debug.h"

#if DEBUG_LEVEL > 0

/* Include LUFA serial driver */
#include <LUFA/Drivers/Peripheral/Serial.h>

/* Warning message to match V-USB behavior */
#warning "Never compile production devices with debugging enabled"

/* Helper function to convert nibble to hex ASCII - matches V-USB oddebug */
static unsigned char hexAscii(unsigned char h)
{
    h &= 0xf;
    if(h >= 10)
        h += 'a' - (unsigned char)10 - '0';
    h += '0';
    return h;
}

/* Helper function to send hex byte - matches V-USB oddebug format */
static void printHex(unsigned char c)
{
    Serial_SendByte(hexAscii(c >> 4));
    Serial_SendByte(hexAscii(c));
}

void usbasp_debug_init(void) {
    /* Initialize LUFA Serial at 19200 baud to match V-USB oddebug */
    Serial_Init(19200, false);
}

void usbasp_debug(unsigned char prefix, unsigned char *data, unsigned char len) {
    /* Output format matches V-USB oddebug exactly:
     * Format: "F4: 01 02 03 04 05 06 07 08\r\n"
     * Where F4 is the prefix, followed by colon, space-separated hex data
     */
    
    /* Print prefix in hex */
    printHex(prefix);
    
    /* Print separator */
    Serial_SendByte(':');
    
    /* Print data bytes in hex with spaces */
    while(len--) {
        Serial_SendByte(' ');
        printHex(*data++);
    }
    
    /* Print line ending (matches V-USB) */
    Serial_SendByte('\r');
    Serial_SendByte('\n');
}

#endif /* DEBUG_LEVEL > 0 */

#endif /* USE_LUFA */