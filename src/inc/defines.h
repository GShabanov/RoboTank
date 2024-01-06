#pragma once

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <stdint.h>
//#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))

inline void *operator new(unsigned int, void *_Where) throw()
{   // construct array with placement at _Where
    return (_Where);
}

#ifndef _NOP
#define _NOP()  __asm__ __volatile__ ("nop" ::)
//#define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

typedef void(*_SENSORUPDATE)();

extern void setup();
extern void loop();

extern  long _map(long x, long in_min, long in_max, long out_min, long out_max);

#endif // __DEFINES_H__

