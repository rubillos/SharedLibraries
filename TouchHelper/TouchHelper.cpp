#include "TouchHelper.h"

TouchHelper::TouchHelper(Adafruit_STMPE610* touchDriver, bool flip) {
    _touchDriver = touchDriver;
    _rangeMin = (flip) ? 100 : 0;
    _rangeMax = (flip) ? 0 : 100;
}

int32_t TouchHelper::mapAndConstrain(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
  value = (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

  int32_t minV = min(out_min, out_max);
  int32_t maxV = max(out_min, out_max);

  if (value <= minV) {
    return minV;
  }
  else if (value >= maxV) {
    return maxV;
  }
  else {
    return value;
  }
}

TouchType TouchHelper::checkTouch() {
    TouchType type = touchNone;
    uint16_t x, y;
    uint8_t z = 255;
    uint16_t touchCount = 0;
    bool isPressed = false;

  if (_touchDriver->touched()) {
    while (!_touchDriver->bufferEmpty()) {
      _touchDriver->readData(&x, &y, &z);
      touchCount++;
    }

    isPressed = (touchCount>0) && (z < 50) && (_releaseTime > 200);

    if (isPressed) {
        _lastX = mapAndConstrain(y, 400, 3600, _rangeMin, _rangeMax);
        _lastY = mapAndConstrain(x, 400, 3600, _rangeMin, _rangeMax);
        type = (_wasPressed) ? touchStillPressed : touchPressed;
    }
  }
  else {
    while (!_touchDriver->bufferEmpty()) {
      _touchDriver->readData(&x, &y, &z);
      touchCount++;
    }
  }

  if (!isPressed && _wasPressed) {
    type = touchReleased;
    _releaseTime = 0;
  }

  _wasPressed = isPressed;

  return type;
}

