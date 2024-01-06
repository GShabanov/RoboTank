#pragma once
#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include "singleton.h"
#include "dispatch.h"


class CUltrasonic : public CDispatch, public Singleton<CUltrasonic>
{

public:
    CUltrasonic() throw()
        : CDispatch()
    {

    }

    void Init();
    virtual void Update();

    float distance();
};

#endif // __ULTRASONIC_H__