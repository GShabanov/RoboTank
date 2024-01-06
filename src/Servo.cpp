#include <arduino.h>

//#include <avr/pgmspace.h>
//#include <avr/interrupt.h>
#include "inc\defines.h"
#include "inc\Servo.h"
#include "inc\Timer.h"

#define servoPin    10   // servo Pin

void
CServo::Init()
{
    pinMode(servoPin, OUTPUT);
    CTimer::Get().SetCounter(ServoTimer, -1);

    m_tics = 0;
    m_pulsesLeft = 0;
    m_pulsewidth = 0;
    m_highPhase = false;

}

#define MIN_PULSE_WIDTH       400     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH      2000     // the longest pulse sent to a servo 


// the function to control the servo
void
CServo::setAngle(int angle)
{
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    m_pulsewidth = _map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);  // Calculate the pulse width value, which should be the mapping value from 500 to 2500. Considering the influence of the infrared library, 500~2000 is used here.
    m_pulsesLeft = 100;
    //m_angle = angle;
    m_highPhase = true;
    m_tics = m_pulsewidth / 100;
    digitalWrite(servoPin, HIGH);
    //pulsewidth = map(myangle, 0, 180, 400, 2000);  // Calculate the pulse width value, which should be the mapping value from 500 to 2500. Considering the influence of the infrared library, 500~2000 is used here.

    /*for (int i = 0; i < 100; i++)
    {
        digitalWrite(servoPin, HIGH);
        delayMicroseconds(m_pulsewidth);    // The duration of the high level is the pulse width
        digitalWrite(servoPin, LOW);
        delayMicroseconds(MAX_PULSE_WIDTH - m_pulsewidth);
        //delay((20 - m_pulsewidth / 1000));  // The period is 20ms, so the low level lasts the rest of the time
    }*/

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

            if (m_highPhase)
            {

                digitalWrite(servoPin, LOW);
                m_highPhase = false;
                m_tics = (MAX_PULSE_WIDTH - m_pulsewidth) / 100;
            }
            else
            {
                if (m_pulsesLeft > 0)
                {
                    digitalWrite(servoPin, HIGH);
                    m_highPhase = true;
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
