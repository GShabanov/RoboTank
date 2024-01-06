/*
  Keyestudio Mini Tank Robot V3 (Popular Edition)
  lesson 22
  multiple functions
  http://www.keyestudio.com
*/
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <arduino.h>

#include "inc\defines.h"
#include "inc\Program.h"

CProgram   program(NULL);

void setup()
{

    program.Init();

}

void loop()
{
    program.Run();
    program.stopFlag(false);
}

