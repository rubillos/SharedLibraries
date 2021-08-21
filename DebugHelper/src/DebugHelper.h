#ifndef __DEBUG_HELPER__
#define __DEBUG_HELPER__

#include <stdint.h>

#ifdef DO_SERIAL
#define Debug_print(...) Serial.print(__VA_ARGS__)
#define Debug_println(...) Serial.println(__VA_ARGS__)
#define Debug_begin(...) Serial.begin(__VA_ARGS__)
#define Debug_begin_wait(...) { Serial.begin(__VA_ARGS__); while (!Serial); Serial.println(); delay(250); }
#define Debug_delay(...) delay(__VA_ARGS__)
#define Debug_defined
#endif

// Add to platformio.ini
//  monitor_port = socket://localhost:19021

#ifdef DO_RTT
#include "SEGGER_RTT.h"
#include "Print.h"

class RTT_Helper : public Print {
    virtual size_t write(uint8_t c) {
        SEGGER_RTT_PutChar(0, c);
        return 1;
    };
    virtual size_t write(const uint8_t *buffer, size_t size) {
        SEGGER_RTT_Write(0, buffer, size);
        return size;
    };
};

extern RTT_Helper rtt;

#define Debug_print(...) rtt.print(__VA_ARGS__)
#define Debug_println(...) rtt.println(__VA_ARGS__)
#define Debug_begin(...) SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL)
#define Debug_begin_wait(...) Debug_begin(__VA_ARGS__)
#define Debug_delay(...) delay(__VA_ARGS__)
#define Debug_defined
#endif

#ifndef Debug_defined
#define Debug_print(...)
#define Debug_println(...)
#define Debug_begin(...)
#define Debug_begin_wait(...)
#define Debug_delay(...)
#endif

extern int32_t FreeRam();

// Add this after Debug setup in setup()
// initial_stop: NOT_USED;

#ifdef __GNUC__
#define NOT_USED __attribute__ ((unused))
#else
#define NOT_USED
#endif

#endif
