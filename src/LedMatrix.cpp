/***************************************************************************************\
*   File:                                                                               *
*       LedMatrix.cpp                                                                   *
*                                                                                       *
*   Abstract:                                                                           *
*       Led display code                                                                *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov  29-Dec-2023                                                         *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#include <avr/eeprom.h>
#include <arduino.h>

//#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "inc\defines.h"
#include "inc\LedMatrix.h"
#include "inc\Timer.h"

#define SCL_Pin  A5  // set the pin of clock to A5
#define SDA_Pin  A4  // set the data pin to A4

//Array, used to save data of images, can be calculated by yourself or gotten from modulus tool
unsigned char start01[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

/*unsigned char start01_2[] = { 0b00001110,
                              0b00010001,
                              0b00010101,
                              0b00010001,
                              0b00001110,
                              0b00000000,
                              0b01000000,
                              0b10000000,
                              0b10000000,
                              0b01000000,
                              0b00000000,
                              0b00001110,
                              0b00010001,
                              0b00010101,
                              0b00010001,
                              0b00001110
                              };*/


unsigned char Smile[]   = {0x00, 0x00, 0x1c, 0x02, 0x02, 0x02, 0x5c, 0x40, 0x40, 0x5c, 0x02, 0x02, 0x02, 0x1c, 0x00, 0x00};
/*unsigned char Smile[]     = { 0b00001110,
                              0b00010001,
                              0b00010101,
                              0b00010001,
                              0b00001110,
                              0b00000000,
                              0b01000000,
                              0b10000000,
                              0b10000000,
                              0b01000000,
                              0b00000000,
                              0b00001110,
                              0b00010001,
                              0b00010101,
                              0b00010001,
                              0b00001110
};*/

unsigned char Disgust[] = { 0x00, 0x00, 0x02, 0x02, 0x02, 0x12, 0x08, 0x04, 0x08, 0x12, 0x22, 0x02, 0x02, 0x00, 0x00, 0x00 };
unsigned char Happy[] = { 0x02, 0x02, 0x02, 0x02, 0x08, 0x18, 0x28, 0x48, 0x28, 0x18, 0x08, 0x02, 0x02, 0x02, 0x02, 0x00 };
/*unsigned char Happy[] = { 0b00000000,
                          0b00000000,
                          0b00111100,
                          0b01000010,
                          0b01001010,
                          0b10000001,
                          0b10001111,
                          0b11110001,
                          0b10000001,
                          0b01001010,
                          0b01000010,
                          0b00100100,
                          0b00010100,
                          0b00001100,
                          0b00000000,
                          0b00000000 };*/


unsigned char Squint[] = { 0x00, 0x00, 0x00, 0x41, 0x22, 0x14, 0x48, 0x40, 0x40, 0x48, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00 };
unsigned char Despise[] = { 0x00, 0x00, 0x06, 0x04, 0x04, 0x04, 0x24, 0x20, 0x20, 0x26, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00 };
unsigned char Heart[] = { 0x00, 0x00, 0x0C, 0x1E, 0x3F, 0x7F, 0xFE, 0xFC, 0xFE, 0x7F, 0x3F, 0x1E, 0x0C, 0x00, 0x00, 0x00 };

unsigned char STOP01[] = { 0x2E, 0x2A, 0x3A, 0x00, 0x02, 0x3E, 0x02, 0x00, 0x3E, 0x22, 0x3E, 0x00, 0x3E, 0x0A, 0x0E, 0x00 };
unsigned char front[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x12, 0x09, 0x12, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char back[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x48, 0x90, 0x48, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char left[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x28, 0x10, 0x44, 0x28, 0x10, 0x44, 0x28, 0x10, 0x00 };
unsigned char right[] = { 0x00, 0x10, 0x28, 0x44, 0x10, 0x28, 0x44, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

unsigned char Test[] = { 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF };

void
CLedMatrix::Init()
{

    pinMode(SCL_Pin, OUTPUT);
    pinMode(SDA_Pin, OUTPUT);

    display();    //clear screen

}


void
CLedMatrix::Update()
{

}

/***************dot matrix******************/
//this function is used for the display of dot matrix 
void CLedMatrix::display(unsigned char *matrix_value)
{
    IIC_start();  //use the function to start transmitting data
    IIC_send(0xc0);  //select an address

    for (int i = 0; i < 16; i++) //image data have 16 characters
    {
        if (matrix_value != NULL)
        {
            IIC_send(matrix_value[i]); //data to transmit pictures
        }
        else
        {
            IIC_send(0x00); //data to transmit pictures
        }
    }

    IIC_end();   //end the data transmission of pictures
    IIC_start();
    IIC_send(0x8A);  //show control and select pulse width 4/16
    IIC_end();
}

//the condition that data starts transmitting
void CLedMatrix::IIC_start()
{
    digitalWrite(SDA_Pin, HIGH);
    digitalWrite(SCL_Pin, HIGH);
    delayMicroseconds(3);
    digitalWrite(SDA_Pin, LOW);
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, LOW);
}

//transmit data
void CLedMatrix::IIC_send(unsigned char send_data)
{
    for (byte mask = 0x01; mask != 0; mask <<= 1) //each character has 8 digits, which is detected one by one
    {
        if (send_data & mask) { //set high or low levels in light of each bit(0 or 1)
            digitalWrite(SDA_Pin, HIGH);
        }
        else {
            digitalWrite(SDA_Pin, LOW);
        }

        delayMicroseconds(3);
        digitalWrite(SCL_Pin, HIGH); //pull up the clock pin SCL_Pin to end the transmission of data
        delayMicroseconds(3);
        digitalWrite(SCL_Pin, LOW); //pull down the clock pin SCL_Pin to change signals of SDA 
    }
}

//the sign that transmission of data ends
void CLedMatrix::IIC_end()
{
    digitalWrite(SCL_Pin, LOW);
    digitalWrite(SDA_Pin, LOW);
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, HIGH);
    delayMicroseconds(3);
    digitalWrite(SDA_Pin, HIGH);
    delayMicroseconds(3);
}

