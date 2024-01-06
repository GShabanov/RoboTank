
//#include <avr/eeprom.h>
#include <arduino.h>

//#include <avr/pgmspace.h>
//#include <avr/interrupt.h>
#include "inc\defines.h"
#include "inc\ultrasonic.h"

#define Trig 12
#define Echo 13

void
CUltrasonic::Init()
{
    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);
}

//Control the ultrasonic sensor
float CUltrasonic::distance()
{
    unsigned long duration;
    float distance;
    digitalWrite(Trig, LOW);
    //delayMicroseconds(2);
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);
    duration = pulseIn(Echo, HIGH, 100000L);
    //distance = pulseIn(Echo, HIGH) / 58.20;  //
    distance = (float)duration / 58.30;

    return distance;
}

void
CUltrasonic::Update()
{
}
