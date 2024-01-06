#pragma once
#ifndef __SERVO_H__
#define __SERVO_H__

#include "singleton.h"
#include "dispatch.h"


class CServo : public CDispatch, public Singleton<CServo>
{

    int     m_pulsesLeft;
    int     m_pulsewidth;
    bool    m_highPhase;
    uint8_t m_tics;
public:
    CServo() throw()
        : CDispatch()
    {
    }

    void Init();
    virtual void Update();

    void setAngle(int angle);

    bool Ready()
    {
        return (m_pulsesLeft == 0) ? true : false;
    }

    void waitForSet();
};

#endif // __SERVO_H__