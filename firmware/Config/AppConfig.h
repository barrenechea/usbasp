/*
 * Application Configuration Header File for USBasp with LUFA
 *
 * This file configures the USBasp application when using LUFA
 * with the ATmega32U4 microcontroller.
 *
 * License: GNU GPL v2 (see Readme.txt)
 */

#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

    /* ISP Pin Configuration for ATmega32U4 */
    #define AUX_LINE_PORT              PORTB    // Reset line port
    #define AUX_LINE_PIN               PINB     // Reset line pin register
    #define AUX_LINE_DDR               DDRB     // Reset line direction register  
    #define AUX_LINE_MASK              (1 << 0) // Reset line on PB0

    /* ISP Protocol Configuration */
    #define ENABLE_ISP_PROTOCOL                  // Enable ISP programming
    #define ENABLE_XPROG_PROTOCOL               // Enable PDI/TPI programming

    /* Pin Mapping for ATmega32U4 ISP */
    // These match the standard Arduino Leonardo ISP pinout
    // MISO: PB3 (pin 14)
    // MOSI: PB2 (pin 16) 
    // SCK:  PB1 (pin 15)
    // RESET: PB0 (pin 8)

    /* VTARGET Configuration - Optional voltage monitoring */
    #define VTARGET_ADC_CHANNEL        0        // ADC channel for target voltage
    #define VTARGET_REF_VOLTS          5        // Reference voltage (5V from USB)
    #define VTARGET_SCALE_FACTOR       1        // No voltage divider
    #define NO_VTARGET_DETECT                   // Disable voltage detection for simplicity

    /* Clock Configuration */
    // #define XCK_RESCUE_CLOCK_ENABLE           // Enable rescue clock on XCK pin
    // #define INVERTED_ISP_MISO                 // Invert MISO if needed

    /* Firmware Version */
    #define FIRMWARE_VERSION_MINOR     0x11     // Compatible with Atmel Studio

    /* USBasp Specific Configuration */
    #define USBASP_VENDOR_ID           0x16c0   // USBasp vendor ID
    #define USBASP_PRODUCT_ID          0x05dc   // USBasp product ID

#endif