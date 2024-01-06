#pragma once

#ifndef __TIMER_H__
#define __TIMER_H__
#include "singleton.h"

//#define TIMER_WITH_CALLBACKS  1

typedef enum timers : uint8_t {

    ServoTimer,
    CartTimer,
    ScanTimer,
    LargeTimer,

    //--------------
    TimerMax
};

class CTimer : public Singleton<CTimer>
{

#if TIMER_WITH_CALLBACKS
public:
    typedef void (* isrCallback)(void *context);

private:
    typedef struct _ISR_CALLBACK {
        int8_t       millis;
        uint8_t      counter;
        isrCallback  _function;
        void *       context;
    } ISR_CALLBACK, *PISR_CALLBACK;

#endif // TIMER_WITH_CALLBACKS

public:
    typedef struct _TIMED_EVENT {
        int32_t        millis;
        uint32_t       counter;
    } TIMED_EVENT, * PTIMED_EVENT;

    CTimer() throw();

#if TIMER_WITH_CALLBACKS
    void AddTimerCallback(uint8_t millis, isrCallback callback, void *context);
#endif // TIMER_WITH_CALLBACKS
    void SetCounter(uint8_t index, int32_t halfMcs);
    bool IsTimerFinish(uint8_t index);

    void Delay(uint16_t millis);

private:

#if TIMER_WITH_CALLBACKS
    volatile unsigned char m_nCallbacks;
    ISR_CALLBACK           m_callbacks[3];
#endif // TIMER_WITH_CALLBACKS

    volatile TIMED_EVENT    m_TimedValues[TimerMax];
    volatile uint16_t       m_Delay;


    // 0x0007   TIMER2_COMPATimer / Counter2     Compare Match A

#if defined(__AVR_ATmega328P__)
    static void Timer1COMPA() __asm__("__vector_11") __attribute__((signal, __used__, __externally_visible__));

#elif defined(__AVR_ATmega168__)
    static void Timer1COMPA() __asm__("__vector_11") __attribute__((signal, __used__, __externally_visible__));

#endif

};

#endif // __TIMER_H__