#ifndef __SIMPLE_TASKS__
#define __SIMPLE_TASKS__

#include "stdint.h"

typedef enum {
    SimpleSleepNone = 0,
    SimpleSleepIdle = 0x2,
    SimpleSleepSleep = 0x4
} SimpleSleepType;

extern uint32_t adjustedMills();
extern void SetSimpleSleepType(SimpleSleepType type);
extern void SetSimpleSleepIndicatorPin(int8_t pinNumber);
extern uint8_t AddSimpleTask(void (*task)(), uint32_t period, bool runImmediately);
extern void setSimpleTaskPeriod(uint8_t taskIndex, uint32_t period);
extern uint32_t SimpleTasksIdle();

#endif
