#pragma once
#ifndef __LEDMATRIX_H__
#define __LEDMATRIX_H__
#include "singleton.h"
#include "dispatch.h"

extern unsigned char start01[];
extern unsigned char Smile[];
extern unsigned char Disgust[];
extern unsigned char Happy[];

extern unsigned char Squint[];
extern unsigned char Despise[];
extern unsigned char Heart[];

extern unsigned char STOP01[];
extern unsigned char front[];
extern unsigned char back[];
extern unsigned char left[];
extern unsigned char right[];
extern unsigned char Test[];


class CLedMatrix : public CDispatch, public Singleton<CLedMatrix>
{

public:
    CLedMatrix() throw()
        : CDispatch()
    {

    }

    void Init();
    virtual void Update();

    void display(unsigned char *matrix_value = NULL);

private:

    //the condition that data starts transmitting
    static void IIC_start();
    //the sign that transmission of data ends
    static void IIC_end();

    static void IIC_send(unsigned char send_data);
};

#endif // __LEDMATRIX_H__