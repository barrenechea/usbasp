/*
 * serialnumber.c - part of USBasp
 *
 * Autor..........: Dimitrios Chr. Ioannidis ( d.ioannidis@nephelae.eu )
 * Description....: Provides functions for updating the Serial Number
                    using HID
 * Licence........: GNU GPL v2 (see Readme.txt)
 * Creation Date..: 2023-04-01
 * Last change....: 2023-04-11
 */

#ifndef USE_LUFA
#include "usbdrv.h"
#endif
#include "serialnumber.h"
#include <avr/eeprom.h>

/* Declare the V-USB EEPROM serial number - shared by both V-USB and LUFA */
extern const int EEMEM usbDescriptorStringSerialNumber[];

void serialNumberWrite(uchar *reportData) {
    
    uchar i;
    
    unsigned tmp = (reportData[1] << 8) | reportData[0];
    
    /* Write 4 digits to EEPROM as 16-bit words (compatible with V-USB format) */
    for (i=4; i > 0; i--)
        {
            eeprom_update_word((uint16_t*)&usbDescriptorStringSerialNumber[i], 48 + tmp%10);
            tmp /= 10;
        }     
}