/***************************************************************************************\
*   File:                                                                               *
*       Cart.cpp                                                                        *
*                                                                                       *
*   Abstract:                                                                           *
*       Motorized cart control code                                                     *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov  22-Dec-2023                                                         *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#include <avr/eeprom.h>
#include <arduino.h>

//#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "inc\defines.h"
#include "inc\Cart.h"
#include "inc\Timer.h"
#include "inc\LedMatrix.h"

#define ML_Ctrl 4   // define the direction control pin of the left motor as 4
#define ML_PWM  6   // define the PWM control pin of the left motor 
#define MR_Ctrl 2   // define the direction control pin of the right sensor
#define MR_PWM  5   // define the PWM control pin of the right motor 

void
CCart::Init()
{
    pinMode(ML_Ctrl, OUTPUT);
    pinMode(ML_PWM, OUTPUT);
    pinMode(MR_Ctrl, OUTPUT);
    pinMode(MR_PWM, OUTPUT);

    CTimer::Get().SetCounter(CartTimer, -500);

}

void
CCart::IncrementValue(uint8_t& val, int8_t &inc, uint8_t max)
{
    int8_t  local;
    local = inc;

    if (local < 0)
    {
        local = -local;
    }

    if (val + local <= max)
    {
        val += local;
    }
    else
    {
        val = max;
        inc = 0;
    }

}

void
CCart::Update()
{

    if (CTimer::Get().IsTimerFinish(CartTimer))
    {
        if (m_state == state::eStop)
            return;

        if (m_left_inc != 0)
        {
            if (m_left_inc < 0)
            {
                digitalWrite(ML_Ctrl, LOW);

                IncrementValue(m_speeds_L, m_left_inc, m_l_maxSpeed);

                analogWrite(ML_PWM, m_speeds_L);
            }
            else
            {
                digitalWrite(ML_Ctrl, HIGH);

                IncrementValue(m_speeds_L, m_left_inc, m_l_maxSpeed);

                analogWrite(ML_PWM, 255 - m_speeds_L);
            }
        }

        if (m_right_inc != 0)
        {
            if (m_right_inc < 0)
            {
                digitalWrite(MR_Ctrl, LOW);

                IncrementValue(m_speeds_R, m_right_inc, m_r_maxSpeed);

                analogWrite(MR_PWM, m_speeds_R);
            }
            else
            {
                digitalWrite(MR_Ctrl, HIGH);

                IncrementValue(m_speeds_R, m_right_inc, m_r_maxSpeed);

                analogWrite(MR_PWM, 255 - m_speeds_R);
            }
        }

    }
}

void
CCart::Back()
{
    if (m_state == state::eBack)
        return;

    m_state = state::eBack;
    m_right_inc = -m_r_speed_inc;
    m_left_inc = -m_l_speed_inc;
    m_speeds_L = 30;
    m_speeds_R = 20;

    CLedMatrix::Get().display(back);
}

void
CCart::Front()
{
    if (m_state == state::eFront)
        return;

    m_state = state::eFront;
    m_right_inc = m_r_speed_inc;
    m_left_inc = m_l_speed_inc;
    m_speeds_L = 30;
    m_speeds_R = 20;

    CLedMatrix::Get().display(front);  //show the image of going front

}

void
CCart::Right()
{
    if (m_state == state::eRight)
        return;

    m_state = state::eRight;
    m_right_inc = -m_r_speed_inc;
    m_left_inc = m_l_speed_inc;
    m_speeds_L = 30;
    m_speeds_R = 20;

    CLedMatrix::Get().display(right);  //show the image of turning right
}

void
CCart::Left()
{
    if (m_state == state::eLeft)
        return;

    m_state = state::eLeft;
    m_right_inc = m_r_speed_inc;
    m_left_inc = -m_l_speed_inc;
    m_speeds_L = 30;
    m_speeds_R = 20;

    CLedMatrix::Get().display(left);  //show the image of turning left

}

void
CCart::Stop()
{
    if (m_state == state::eStop)
        return;

    m_state = state::eStop;
    m_right_inc = 0;
    m_left_inc = 0;
    m_speeds_R = 0;
    m_speeds_L = 0;
    digitalWrite(MR_Ctrl, LOW);
    digitalWrite(ML_Ctrl, LOW);
    analogWrite(MR_PWM, 0);
    analogWrite(ML_PWM, 0);

    CLedMatrix::Get().display(STOP01);  //show the stop image

}

