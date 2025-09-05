/*
 * USB Interface Abstraction Layer Implementation
 *
 * This file provides the common interface implementation and
 * the global USB interface pointer that both VUSB and LUFA
 * implementations can register with.
 *
 * License: GNU GPL v2 (see Readme.txt)
 */

#include <stddef.h>
#include "usb_interface.h"

/* Global USB interface pointer - initialized by the implementation modules */
const usb_interface_t *usb_interface = NULL;