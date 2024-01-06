#pragma once
#ifndef __PROGRAM_H__
#define __PROGRAM_H__

class CProgram
{
protected:
    CProgram* m_parent;
    bool      m_stopFlag;

public:
    CProgram(CProgram* parent) throw()
        : m_parent(parent)
    {
        m_stopFlag = false;
    }

    void Init();
    bool Run();

    bool stopFlag() {
        return m_stopFlag;
    }

    void stopFlag(bool value) {
        m_stopFlag = value;
    }

protected:
    virtual bool Process(char &command);

};

//
// obstacles avoid
//

class CAvoidProgram : public CProgram
{
    int  m_angleDelta = -10;
    int  m_angleInc = 4;

    int  m_minDistance = 2000;
    int  m_frontDistanceSum;
    int  m_currentDistance;
    int  m_samples = 0;

    int  m_runForwardCredit = 0;

public:
    CAvoidProgram(CProgram* parent) throw()
        : CProgram(parent)
    {
    }

    void Init();

protected:
    virtual bool Process(char &command);

};

class CFindWayProgram : public CProgram
{
    const static int m_div = 10;
    const static int m_maxAngle = 60;

    int  m_angle;
    int  m_distancesRight[m_maxAngle / m_div];
    int  m_distancesLeft[m_maxAngle / m_div];
    int  m_currentDistance;
    int  m_moveCredit;

    bool (CFindWayProgram::* m_Subprogram)(void);


public:
    CFindWayProgram(CProgram* parent) throw()
        : CProgram(parent)
    {
    }

    void Init();

protected:
    bool ScanRight();
    bool ScanLeft();
    bool MakeDecisionMove();
    bool FinishMove();

    static void getMinMax(int* arr, int size, int& Min, int& Max);
    static void getMid(int* arr, int size, int& Mid);

    virtual bool Process(char &command);

};

//
// follow 
//
class CFollowProgram : public CProgram
{

public:
    CFollowProgram(CProgram* parent) throw()
        : CProgram(parent)
    {
    }

    void Init() { }

protected:
    virtual bool Process(char &command);

};

//
// light following
//

class CLightFollowing : public CProgram
{
public:
    CLightFollowing(CProgram* parent) throw()
        : CProgram(parent)
    {
    }

    void Init() { }

protected:
    virtual bool Process(char &command);
};

//
// tracking
//

class CTrackingProgram : public CProgram
{
public:
    CTrackingProgram(CProgram* parent) throw()
        : CProgram(parent)
    {
    }

    void Init() { }

protected:
    virtual bool Process(char &command);
};

//
// confinement
//

class CConfinementProgram : public CProgram
{
public:
    CConfinementProgram(CProgram* parent) throw()
        : CProgram(parent)
    {
    }

    void Init() { }

protected:
    virtual bool Process(char &command);
};



#endif // __PROGRAM_H__
