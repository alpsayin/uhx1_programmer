

//
//  This file is for an mbed prototyping board.
//  Don't try to compile this file with any kind of toolchain.
//  It can only be compiled with the mbed library and therefore,
//  it can only be compiled with online mbed compiler.
//

#include "mbed.h"
#include "string.h"
#include "stdint.h"


BusOut myleds(LED4, LED3, LED2, LED1);
AnalogIn adc(p20);
Serial pc(USBTX,USBRX);
int adcVal;

char input;
int transistorVal;
void sendOK()
{
    putchar('O');
    putchar('K');
    putchar(0x0D);
}
void sendFakeDump()
{
    int i, j;
    for(i=0; i<4; i++)
    {
        for(j=0; j<16; j++)
        {
//            putchar(0x30 | (((i*16+j)&0xF0)>>4));
            putchar(0x30 | (i+1));
//            putchar(0x30 | ((i*16+j)&0x0F));
            if(j<10)
                putchar(0x30 | j);
            else
                putchar('A' + j - 10);
            putchar(' ');
        }
        putchar(0x0D);
    }
}
int main() 
{
    char inputBuffer[128];
    int index=0;
    uint16_t fullRangeAdcVal;
    uint8_t byteVal;
    
    pc.baud(115200);
    while(1) 
    {
        inputBuffer[index]=getchar();
        if(inputBuffer[index] == ':')
        {
            inputBuffer[0]=':';
            index=0;
        }
        if(inputBuffer[index] != 0x0D)
        {
            index=(index+1)%sizeof(inputBuffer);
            continue;
        }
        inputBuffer[index++]=0;
        index=0;
        if(!strcmp(":RSSI", inputBuffer))
        {
            fullRangeAdcVal=adc.read_u16();
            byteVal=(fullRangeAdcVal>>8)&0xFF;
            myleds = byteVal>>4;
            putchar(byteVal);
        }
        else if(!strcmp(":IDENT", inputBuffer))
        {
            putchar('S');
        }
        else if(!strcmp(":LOCKSERIALMODE", inputBuffer))
        {
            sendOK();
        }
        else if(!strncmp(":DEF", inputBuffer, 4))
        {
            sendOK();
        }
        else if(!strcmp(":DUMP", inputBuffer))
        {
            sendFakeDump();
        }
        else
        {
            //do nothing
        }
    }
}



    
