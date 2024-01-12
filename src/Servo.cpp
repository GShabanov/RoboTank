/***************************************************************************************\
*   File:                                                                               *
*       Servo.cpp                                                                       *
*                                                                                       *
*   Abstract:                                                                           *
*       TowerPro SG90 - Micro Servo code                                                *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov  29-Dec-2023                                                         *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $

#include <arduino.h>

#include "inc\defines.h"
#include "inc\Servo.h"
#include "inc\Timer.h"

//*******************************************
// Additional Specifications
// Rotational Range : 180°
// Pulse Cycle : ca. 20 ms
// Pulse Width : 500 - 2400 us
//*******************************************

//
// equations:
// ServoTimer period:  100 us
// Pulse circle:    20'000 us
//

#define servoPin    10   // servo Pin

void
CServo::Init()
{
    pinMode(servoPin, OUTPUT);
    digitalWrite(servoPin, LOW);
    CTimer::Get().SetCounter(ServoTimer, -1);

    m_tics = 0;
    m_pulsesLeft = 0;
    m_pulsewidth = 0;
    m_lowPhase = true;

}

#define MIN_PULSE_WIDTH       500     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH      2400     // the longest pulse sent to a servo 
#define PULSE_CIRCLE_WIDTH   20000


// the function to control the servo
void
CServo::setAngle(int angle)
{
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    //
    // Calculate the pulse width value, which should be the mapping value from 500 to 2500. Considering the influence of the infrared library, 500~2000 is used here.
    //
    m_pulsewidth = _map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
    m_pulsesLeft = 50;
    m_lowPhase = false;
    m_tics = m_pulsewidth / 100;
    digitalWrite(servoPin, HIGH);
}

void
CServo::waitForSet()
{
    while (m_pulsesLeft != 0)
    {
        Update();
        _NOP();
    }
}

void
CServo::Update()
{
    if (CTimer::Get().IsTimerFinish(ServoTimer))
    {
        if (m_tics > 0)
        {
            m_tics--;
        }
        else
        {
            if (!m_pulsesLeft)
                return;

            if (m_lowPhase)
            {

                digitalWrite(servoPin, LOW);
                m_lowPhase = false;
                m_tics = (PULSE_CIRCLE_WIDTH - m_pulsewidth) / 100;
            }
            else
            {
                if (m_pulsesLeft > 0)
                {
                    digitalWrite(servoPin, HIGH);
                    m_lowPhase = true;
                    m_tics = m_pulsewidth / 100;

                    //
                    // one period done
                    //
                    m_pulsesLeft--;
                }
            }
        }
    }
}
