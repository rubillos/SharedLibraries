#include <Arduino.h>

#include "PushButton.h"

PushButton::PushButton(int16_t pinNumber, bool doublePress, uint32_t longTimeSeconds) {
  _pinNumber = pinNumber;
  _longMillis = longTimeSeconds * 1000;
  _doublePress = doublePress;
  _lastShortPressTime = 10000;
  if (_pinNumber != -1) {
    pinMode(_pinNumber, INPUT_PULLUP);
  }
}

int16_t PushButton::pinNumber() {
  return _pinNumber;
}

void PushButton::swapPinsWith(PushButton &otherButton) {
  uint8_t otherPin = otherButton._pinNumber;
  otherButton._pinNumber = _pinNumber;
  _pinNumber = otherPin;
  _lastShortPressTime = 10000;
  _buttonIsPressed = false;
  _returnedLongPress = false;
}

int32_t mapAndConstrainLong(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
  value = (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

  int32_t minV = min(out_min, out_max);
  int32_t maxV = max(out_min, out_max);

  if (value <= minV) { return minV; }
  else if (value >= maxV) { return maxV; }
  else { return value; }
}

buttonPressType PushButton::checkPress() {
  buttonPressType pressed = notPressed;
  bool currentButtonState = isCurrentlyPressed();

  if (currentButtonState != _lastButtonState) {
    _stateChangeTime = 0;
    _lastButtonState = currentButtonState;
    _repeatDelay = 0;
    _repeatRate = 0;
  }

  if (_stateChangeTime >= DEBOUNCE_TIME) {
    if (currentButtonState) {       // currently pressed
      if ((_longMillis > 0) && (_stateChangeTime >= _longMillis)) {
        if (!_returnedLongPress) {
          pressed = longPress;
          _returnedLongPress = true;
        }
      }
      if ((_longMillis == 0) && !_buttonIsPressed) {
        pressed = shortPress;
      }

      if ((_longMillis == 0) && (_repeatDelay > REPEAT_DELAY)) {
        uint32_t curRepeatRate;

        if (_repeatRampTime > 0) {
          curRepeatRate = mapAndConstrainLong(_stateChangeTime-REPEAT_DELAY, 0, _repeatRampTime, REPEAT_RATE, _repeatRampMinRate);
        }
        else {
          curRepeatRate = REPEAT_RATE;
        }

        if (_repeatRate > curRepeatRate) {
          pressed = shortPress;
          _repeatRate = 0;
        }
      }
    }
    else {
      if (_buttonIsPressed) {
        if (_returnedLongPress) {
          // this gets returned while still down
        }
        else if (_doublePress && _lastShortPressTime < DOUBLE_PRESS_TIME) {
          pressed = doublePress;
        }
        else if (_longMillis > 0) {
          _lastShortPressTime = 0;
          pressed = shortPress;
        }
      }
      _returnedLongPress = false;
    }

    _buttonIsPressed = currentButtonState;
  }

  return(pressed);
}

bool PushButton::isCurrentlyPressed() {
  return (_pinNumber == -1) ? false : (digitalRead(_pinNumber) == LOW);
}
