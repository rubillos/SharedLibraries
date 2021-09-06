#include "SimpleTasks.h"

#include "Adafruit_SleepyDog.h"

#define MAX_SIMPLE_TASKS 8
#define MIN_SLEEP_CYCLES 8

typedef struct {
    void (*task)();
    uint32_t period;

    uint32_t lastRunTicks;
} SimpleTaskRec;

struct {
    uint16_t numTasks = 0;
    SimpleSleepType sleepType = SimpleSleepNone;
    int8_t indicatorPin = -1;
    uint32_t millisAdjust = 0;
} simpleTasksInfo;

SimpleTaskRec tasks[MAX_SIMPLE_TASKS];

void SetSimpleSleepType(SimpleSleepType type) {
    simpleTasksInfo.sleepType = type;
}

void SetSimpleSleepIndicatorPin(int8_t pinNumber) {
    simpleTasksInfo.indicatorPin = pinNumber;
    digitalWrite(pinNumber, LOW);
    pinMode(pinNumber, OUTPUT);
}

uint32_t adjustedMills() {
    return millis() + simpleTasksInfo.millisAdjust;
}

uint8_t AddSimpleTask(void (*task)(), uint32_t period, bool runImmediately) {
    int8_t taskIndex = -1;

    if (simpleTasksInfo.numTasks < MAX_SIMPLE_TASKS) {
        taskIndex = simpleTasksInfo.numTasks++;
        tasks[taskIndex].task = task;
        tasks[taskIndex].period = period;
        tasks[taskIndex].lastRunTicks = adjustedMills();

        if (runImmediately) {
            tasks[taskIndex].lastRunTicks -= period+1;
        }
    }

    return taskIndex;
}

void setSimpleTaskPeriod(uint8_t taskIndex, uint32_t period) {
    tasks[taskIndex].period = period;
}

uint32_t SimpleTasksIdle() {
    uint32_t ticksToNextTask = 0xFFFFFFFF;
    uint8_t numTasksRun = 0;
    uint32_t curTime = adjustedMills();
    uint32_t timeSlept = 0;

    for (uint8_t i=0; i<simpleTasksInfo.numTasks; i++) {
        uint32_t ticksSinceLastRun = curTime - tasks[i].lastRunTicks;

        if (ticksSinceLastRun >= tasks[i].period) {
            tasks[i].task();
            numTasksRun++;

            tasks[i].lastRunTicks = curTime;
            ticksSinceLastRun = 0;
        }

        uint32_t ticksToTask = tasks[i].period - ticksSinceLastRun;

        if (ticksToTask < ticksToNextTask) {
            ticksToNextTask = ticksToTask;
        }
    }

    if (simpleTasksInfo.sleepType!=SimpleSleepNone && numTasksRun>0 && ticksToNextTask!=0xFFFFFFFF && ticksToNextTask>=MIN_SLEEP_CYCLES) {
        if (simpleTasksInfo.indicatorPin != -1) {
            digitalWrite(simpleTasksInfo.indicatorPin, HIGH);
        }

        uint32_t allowedSleepTicks = Watchdog.enable(ticksToNextTask, true);

        SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;  // Disable SysTick interrupts

        PM->SLEEPCFG.bit.SLEEPMODE = simpleTasksInfo.sleepType;
        while (PM->SLEEPCFG.bit.SLEEPMODE != simpleTasksInfo.sleepType) {}; // Wait for it to take

        __DSB(); // Data sync to ensure outgoing memory accesses complete
        __WFI(); // Wait for interrupt (places device in sleep mode)

        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  // Enable SysTick interrupts

        // adjust millis
        simpleTasksInfo.millisAdjust += allowedSleepTicks;
        timeSlept = allowedSleepTicks;

        if (simpleTasksInfo.indicatorPin != -1) {
            digitalWrite(simpleTasksInfo.indicatorPin, LOW);
        }
    }

    return timeSlept;
}
