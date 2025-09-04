/*
 * USBasp Unified Debug Interface
 *
 * This header provides a unified debug interface that works with both
 * V-USB and LUFA implementations, ensuring identical debug output format
 * and behavior across both USB stacks.
 *
 * License: GNU GPL v2 (see Readme.txt)
 */

#ifndef USBASP_DEBUG_H_
#define USBASP_DEBUG_H_

#include <stdint.h>

/* Debug level configuration - matches V-USB oddebug behavior */
#ifndef DEBUG_LEVEL
#   define DEBUG_LEVEL 0
#endif

/* Debug macros - identical interface for both V-USB and LUFA */
#if DEBUG_LEVEL > 0
#   define DBG1(prefix, data, len) usbasp_debug(prefix, data, len)
#else
#   define DBG1(prefix, data, len)
#endif

#if DEBUG_LEVEL > 1
#   define DBG2(prefix, data, len) usbasp_debug(prefix, data, len)
#else
#   define DBG2(prefix, data, len)
#endif

/* Unified debug function declaration */
#if DEBUG_LEVEL > 0
void usbasp_debug(unsigned char prefix, unsigned char *data, unsigned char len);
void usbasp_debug_init(void);
#else
#   define usbasp_debug_init()
#endif

#endif /* USBASP_DEBUG_H_ */