#include "SimpleTasks.h"

#include "elapsedMillis.h"
#include "Adafruit_SleepyDog.h"

#define MAX_SIMPLE_TASKS 8
#define MIN_SLEEP_CYCLES 8

// extern volatile uint32_t _ulTickCount;

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
    SimpleTasksStats stats;
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

SimpleTasksStats GetSimpleTasksStats() {
    SimpleTasksStats curStats = simpleTasksInfo.stats;
    simpleTasksInfo.stats.totalMicros = 0;
    simpleTasksInfo.stats.taskMicros = 0;
    simpleTasksInfo.stats.tasksRun = 0;
    return curStats;
}

uint32_t adjustedInterval(uint32_t interval) {
    return interval + simpleTasksInfo.millisAdjust;
}

uint32_t adjustedMillis() {
    return adjustedInterval(millis());
}

uint8_t AddSimpleTask(void (*task)(), uint32_t period, bool runImmediately) {
    int8_t taskIndex = -1;

    if (simpleTasksInfo.numTasks < MAX_SIMPLE_TASKS) {
        taskIndex = simpleTasksInfo.numTasks++;
        tasks[taskIndex].task = task;
        tasks[taskIndex].period = period;
        tasks[taskIndex].lastRunTicks = adjustedMillis();

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
    static elapsedMicros externalMicros;
    uint32_t taskMicros = 0;
    uint32_t ticksToNextTask = 0xFFFFFFFF;
    uint8_t numTasksRun = 0;
    uint32_t curTime = adjustedMillis();
    uint32_t timeSlept = 0;

    for (uint8_t i=0; i<simpleTasksInfo.numTasks; i++) {
        uint32_t ticksSinceLastRun = curTime - tasks[i].lastRunTicks;

        if (ticksSinceLastRun >= tasks[i].period) {
            elapsedMicros taskTime;
            tasks[i].task();
            numTasksRun++;

            taskMicros += taskTime;
            simpleTasksInfo.stats.taskMicros += taskTime;
            simpleTasksInfo.stats.tasksRun++;

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

#if (SAMD20 || SAMD21)
        // Don't fully power down flash when in sleep
        NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;
        if (simpleTasksInfo.sleepType == SimpleSleepIdle) {
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            PM->SLEEP.reg = 0;
        }
        else if (simpleTasksInfo.sleepType == SimpleSleepSleep) {
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
        }
#endif
#if defined(__SAMD51__)
        PM->SLEEPCFG.bit.SLEEPMODE = simpleTasksInfo.sleepType;
        while (PM->SLEEPCFG.bit.SLEEPMODE != simpleTasksInfo.sleepType) {}; // Wait for it to take
#endif

        __DSB(); // Data sync to ensure outgoing memory accesses complete
        __WFI(); // Wait for interrupt (places device in sleep mode)

        // adjust millis
        // _ulTickCount += allowedSleepTicks;
        simpleTasksInfo.millisAdjust += allowedSleepTicks;
        timeSlept = allowedSleepTicks;

        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  // Enable SysTick interrupts

        if (simpleTasksInfo.indicatorPin != -1) {
            digitalWrite(simpleTasksInfo.indicatorPin, LOW);
        }
    }

    simpleTasksInfo.stats.totalMicros += externalMicros + timeSlept * 1000 - taskMicros;
    externalMicros = 0;
    return timeSlept;
}
