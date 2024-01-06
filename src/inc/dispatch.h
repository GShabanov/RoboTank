#pragma once
#ifndef __DISPATCH_H__
#define __DISPATCH_H__

class CDispatch
{

protected:

public:
    CDispatch() throw()
    {
    }

    virtual void Update() = 0;

};


#endif // __DISPATCH_H__

