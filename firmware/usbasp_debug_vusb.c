/*
 * USBasp Debug Implementation for V-USB
 *
 * This file provides the V-USB implementation of the unified debug interface.
 * It wraps the existing oddebug functionality to provide consistent behavior.
 *
 * License: GNU GPL v2 (see Readme.txt)
 */

#ifdef USE_VUSB

#include "usbasp_debug.h"
#include "oddebug.h"

#if DEBUG_LEVEL > 0

void usbasp_debug_init(void) {
    /* Initialize V-USB debug UART at 19200 baud */
    odDebugInit();
}

void usbasp_debug(unsigned char prefix, unsigned char *data, unsigned char len) {
    /* Use V-USB's existing debug function directly */
    odDebug(prefix, data, len);
}

#endif /* DEBUG_LEVEL > 0 */

#endif /* USE_VUSB */