#ifndef INCLUDED_PushButton_h
#define INCLUDED_PushButton_h

#include <elapsedMillis.h>

#define DEBOUNCE_TIME 50
#define MEDIUM_PRESS_TIME 1000
#define LONG_PRESS_TIME 5000
#define DOUBLE_PRESS_TIME 300
#define REPEAT_DELAY 500
#define REPEAT_RATE 300

typedef enum buttonPressType {
  notPressed = 0,
  shortPress = 1,
  longPress,
  doublePress,
} buttonPressType;

class PushButton
{
  public:
    PushButton(int16_t pinNumber, bool doublePress = true, uint32_t longTimeSeconds = LONG_PRESS_TIME/1000);
    buttonPressType checkPress();
    virtual bool isCurrentlyPressed();
    uint32_t currentStateTime() { return _stateChangeTime; };

    int16_t pinNumber();
    void swapPinsWith(PushButton &otherButton);

    void SetRepeatRamp(uint16_t rampMinRate, uint32_t rampTime) {
                                                _repeatRampMinRate = rampMinRate;
                                                _repeatRampTime = rampTime;
                                              };

  protected:
    int16_t _pinNumber;

  private:
    uint32_t _longMillis;
    bool _doublePress;
    elapsedMillis _stateChangeTime;
    bool _lastButtonState;
    elapsedMillis _lastShortPressTime;
    bool _buttonIsPressed = false;
    bool _returnedLongPress = false;
    elapsedMillis _repeatDelay;
    elapsedMillis _repeatRate;

    uint16_t _repeatRampMinRate = 0;
    uint16_t _repeatRampTime = 0;
};

#endif
