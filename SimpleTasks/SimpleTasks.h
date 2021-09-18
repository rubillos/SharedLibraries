#ifndef __SIMPLE_TASKS__
#define __SIMPLE_TASKS__

#include "stdint.h"

typedef enum {
    SimpleSleepNone = -1,
    SimpleSleepIdle = 0x2,
    SimpleSleepSleep = 0x4
} SimpleSleepType;

typedef struct {
    uint32_t totalMicros;
    uint32_t taskMicros;
    uint32_t tasksRun;
} SimpleTasksStats;

extern uint32_t adjustedMillis();
extern uint32_t adjustedInterval(uint32_t interval);
extern void SetSimpleSleepType(SimpleSleepType type);
extern void SetSimpleSleepIndicatorPin(int8_t pinNumber);
extern uint8_t AddSimpleTask(void (*task)(), uint32_t period, bool runImmediately);
extern void setSimpleTaskPeriod(uint8_t taskIndex, uint32_t period);
extern uint32_t SimpleTasksIdle();
extern SimpleTasksStats GetSimpleTasksStats();

//------------------------------------------------------
class adjustedElapsedMillis
{
private:
	unsigned long ms;
public:
	adjustedElapsedMillis(void) { ms = adjustedMillis(); }
	adjustedElapsedMillis(unsigned long val) { ms = adjustedMillis() - val; }
	adjustedElapsedMillis(const adjustedElapsedMillis &orig) { ms = orig.ms; }
	operator unsigned long () const { return adjustedMillis() - ms; }
	adjustedElapsedMillis & operator = (const adjustedElapsedMillis &rhs) { ms = rhs.ms; return *this; }
	adjustedElapsedMillis & operator = (unsigned long val) { ms = adjustedMillis() - val; return *this; }
	adjustedElapsedMillis & operator -= (unsigned long val)      { ms += val ; return *this; }
	adjustedElapsedMillis & operator += (unsigned long val)      { ms -= val ; return *this; }
	adjustedElapsedMillis operator - (int val) const           { adjustedElapsedMillis r(*this); r.ms += val; return r; }
	adjustedElapsedMillis operator - (unsigned int val) const  { adjustedElapsedMillis r(*this); r.ms += val; return r; }
	adjustedElapsedMillis operator - (long val) const          { adjustedElapsedMillis r(*this); r.ms += val; return r; }
	adjustedElapsedMillis operator - (unsigned long val) const { adjustedElapsedMillis r(*this); r.ms += val; return r; }
	adjustedElapsedMillis operator + (int val) const           { adjustedElapsedMillis r(*this); r.ms -= val; return r; }
	adjustedElapsedMillis operator + (unsigned int val) const  { adjustedElapsedMillis r(*this); r.ms -= val; return r; }
	adjustedElapsedMillis operator + (long val) const          { adjustedElapsedMillis r(*this); r.ms -= val; return r; }
	adjustedElapsedMillis operator + (unsigned long val) const { adjustedElapsedMillis r(*this); r.ms -= val; return r; }
};

class adjustedElapsedSeconds
{
private:
	unsigned long s;
public:
	adjustedElapsedSeconds(void) { s = adjustedMillis()/1000; }
	adjustedElapsedSeconds(unsigned long val) { s = adjustedMillis()/1000 - val; }
	adjustedElapsedSeconds(const adjustedElapsedSeconds &orig) { s = orig.s; }
	operator unsigned long () const { return adjustedMillis()/1000 - s; }
	adjustedElapsedSeconds & operator = (const adjustedElapsedSeconds &rhs) { s = rhs.s; return *this; }
	adjustedElapsedSeconds & operator = (unsigned long val) { s = adjustedMillis()/1000 - val; return *this; }
	adjustedElapsedSeconds & operator -= (unsigned long val)      { s += val ; return *this; }
	adjustedElapsedSeconds & operator += (unsigned long val)      { s -= val ; return *this; }
	adjustedElapsedSeconds operator - (int val) const           { adjustedElapsedSeconds r(*this); r.s += val; return r; }
	adjustedElapsedSeconds operator - (unsigned int val) const  { adjustedElapsedSeconds r(*this); r.s += val; return r; }
	adjustedElapsedSeconds operator - (long val) const          { adjustedElapsedSeconds r(*this); r.s += val; return r; }
	adjustedElapsedSeconds operator - (unsigned long val) const { adjustedElapsedSeconds r(*this); r.s += val; return r; }
	adjustedElapsedSeconds operator + (int val) const           { adjustedElapsedSeconds r(*this); r.s -= val; return r; }
	adjustedElapsedSeconds operator + (unsigned int val) const  { adjustedElapsedSeconds r(*this); r.s -= val; return r; }
	adjustedElapsedSeconds operator + (long val) const          { adjustedElapsedSeconds r(*this); r.s -= val; return r; }
	adjustedElapsedSeconds operator + (unsigned long val) const { adjustedElapsedSeconds r(*this); r.s -= val; return r; }
};

#endif
