/***************************************************************************************\
*   File:                                                                               *
*       Timer.cpp                                                                       *
*                                                                                       *
*   Abstract:                                                                           *
*       General interrupt based timer code                                              *
*                                                                                       *
*   Author:                                                                             *
*       Gennady Shabanov () 21-Sep-2020                                                 *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $

#include <avr/io.h>
//#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "inc\Timer.h"
#include "inc\defines.h"

CTimer::CTimer()
{

#if TIMER_WITH_CALLBACKS
    m_nCallbacks = 0;
#endif // TIMER_WITH_CALLBACKS

    for (uint8_t i = 0; i < ARRAYSIZE(m_TimedValues); i++) {
        m_TimedValues[i].counter = 0;
        m_TimedValues[i].millis = 0;
    }

    m_Delay = 0;

    uint8_t oldSREG = SREG;
    cli();

    TCCR1A = 0;                  // normal counting mode
    TCCR1B = 0;
    //
    // http://embeddedsystemengineering.blogspot.com/2015/07/make-delay-with-timer-0-ctc-mode-pooling.html
    // 
    // To calculate compare match value for delay 10ms, we can use this formula OCR0A = (0.01s * 16000000Hz) / 1024 = 156 = 0x9C.
    //
    // OCR1A = 0x9C;
    // Recalc to  0.001s with 256 prescaller OCR0A = (0.001s * 16000000Hz) / 256 = 62.5 => 63 => 0x3F.
    // Recalc to  0.0001s with 256 prescaller OCR0A = (0.0001s * 16000000Hz) / 256 = 6.25 => 0x6.
    //                                        OCR0A = (1600Hz) / 64 = 25 => 0x19.

    // OCR1A = 15624;           // = (target time / timer resolution) - 1 or 1 / 6.4e-5 - 1 = 15624
    // 
    // Load compare match value (for 0.001s)
    // 
    //OCR1A = 0x3E;        // for 256 prescaller
    //OCR1A = 0xFA;        // for 64 prescaller

    // 
    // Load compare match value (for 0.0001s)
    // 
    OCR1A = 0x19;        // for 64 prescaller

    // Init timer mode and prescaler
    sbi(TCCR1B, WGM12);     // CTC mode on
 
    // 
    //sbi(TCCR1B, CS12);        // Set CS12 bits for 256 prescaler:
    //
    sbi(TCCR1B, CS10);          // Set CS10 and CS11 bits for 64 prescaler:
    sbi(TCCR1B, CS11);

    // timer compare interrupt
    sbi(TIMSK1, OCIE1A);

    SREG = oldSREG;
}

#if TIMER_WITH_CALLBACKS
void
CTimer::AddTimerCallback(uint8_t millis, isrCallback callback, void *context)
{
    if (m_nCallbacks >= ARRAYSIZE(m_callbacks))
        return;

    uint8_t status = SREG;

    cli();

    m_callbacks[m_nCallbacks]._function = callback;
    m_callbacks[m_nCallbacks].context   = context;
    m_callbacks[m_nCallbacks].millis = millis;
    m_callbacks[m_nCallbacks].counter = millis;

    m_nCallbacks++;
    
    //sei();
    SREG = status;
}
#endif // TIMER_WITH_CALLBACKS

void
CTimer::SetCounter(uint8_t index, int32_t halfMcs)
{
    if (index > ARRAYSIZE(m_TimedValues))
        return;

    uint8_t status = SREG;
    cli();

    m_TimedValues[index].millis = halfMcs;
    m_TimedValues[index].counter = halfMcs >= 0 ? halfMcs : -halfMcs;

    SREG = status;
    //sei();
}

bool
CTimer::IsTimerFinish(uint8_t index)
{
    if (index > ARRAYSIZE(m_TimedValues))
        return false;

    volatile TIMED_EVENT* values = &m_TimedValues[index];

    if (values->counter == 0)
    {
        if (values->millis < 0)
        {
            uint8_t oldSREG = SREG;
            cli();

            values->counter = -values->millis;

            SREG = oldSREG;
        }

        return true;
    }

    return false;
}

void
CTimer::Delay(uint16_t millis)
{
    uint8_t oldSREG = SREG;
    cli();

    m_Delay = millis;

    SREG = oldSREG;
    //sei();

    while (m_Delay > 0) {

        _NOP();
    }
}

void CTimer::Timer1COMPA(void)
{

    register CTimer *lpThis = &CTimer::Get();

    volatile TIMED_EVENT *values = lpThis->m_TimedValues;

#if TIMER_WITH_CALLBACKS
    uint8_t n = lpThis->m_nCallbacks;
    PISR_CALLBACK callbacks = lpThis->m_callbacks;
#endif // TIMER_WITH_CALLBACKS

    if (lpThis->m_Delay > 0) {

        lpThis->m_Delay--;
    }

    for (uint8_t i = 0, j = ARRAYSIZE(lpThis->m_TimedValues); i < j; i++) {

        if (values[i].counter > 0)
        {
            values[i].counter--;
        }
    }

#if TIMER_WITH_CALLBACKS

    //
    // walk through all installed callbacks
    //
    for (uint8_t i = 0; i < n; i++)
    {
        if (callbacks[i].counter > 0)
        {
            callbacks[i].counter--;
        }
        else
        {
            callbacks[i]._function(callbacks[i].context);
            callbacks[i].counter = callbacks[i].millis;
        }
    }
#endif // TIMER_WITH_CALLBACKS


    sei();
}
