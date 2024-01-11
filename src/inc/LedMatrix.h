#pragma once
#ifndef __LEDMATRIX_H__
#define __LEDMATRIX_H__
#include "singleton.h"
#include "dispatch.h"

extern const uint8_t start01[];
extern const uint8_t Smile[];
extern const uint8_t Disgust[];
extern const uint8_t Happy[];

extern const uint8_t Squint[];
extern const uint8_t Despise[];
extern const uint8_t Heart[];

extern const uint8_t STOP01[];
extern const uint8_t front[];
extern const uint8_t back[];
extern const uint8_t left[];
extern const uint8_t right[];
extern const uint8_t Test[];

typedef struct _BANNER_INFO {
    int16_t   offset;
    int8_t    inc;
    int16_t   minScroll;
    int16_t   maxScroll;
    char*     string;
} BANNER_INFO, * PBANNER_INFO;

extern BANNER_INFO    HelloBanner;


class CLedMatrix : public CDispatch, public Singleton<CLedMatrix>
{

    PBANNER_INFO   p_banner;

public:
    CLedMatrix() throw()
        : CDispatch()
    {

        p_banner = 0;
    }

    void Init();
    virtual void Update();

    void setBrightness(uint8_t value, bool on = true);

    void displayChar(char value);
    void printString(char *value, int16_t shift);

    void display(const uint8_t *matrix_value = NULL);
    void displayBanner(PBANNER_INFO bannerInfo)
    {
        p_banner = bannerInfo;
    }

private:
    void displayPrivate(const uint8_t* matrix_value = NULL);

    //the condition that data starts transmitting
    static void IIC_start();
    //the sign that transmission of data ends
    static void IIC_end();

    static void IIC_send(unsigned char send_data);
};

#endif // __LEDMATRIX_H__