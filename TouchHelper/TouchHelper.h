#ifndef _TOUCH_HELPER_
#define _TOUCH_HELPER_

#include "Adafruit_STMPE610.h"
#include "elapsedMillis.h"

typedef enum {
    touchNone = 0,
    touchPressed,
    touchStillPressed,
    touchReleased
} TouchType;

class TouchHelper {
    public:
        TouchHelper(Adafruit_STMPE610* touchDriver, bool flip = false);
        TouchType checkTouch();
        uint16_t touchX() { return _lastX; };
        uint16_t touchY() { return _lastY; };

    private:
        Adafruit_STMPE610* _touchDriver;

        elapsedMillis _releaseTime = 500;
        bool _wasPressed = false;
        uint16_t _lastX;
        uint16_t _lastY;
        
        uint8_t _rangeMin;
        uint8_t _rangeMax;

        int32_t mapAndConstrain(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
};

#endif
