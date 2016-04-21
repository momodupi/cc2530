#include <ioCC2530.h>

typedef unsigned char uchar;
typedef unsigned int  uint;

#define LED1 P1_0

uint count;

void InitLed(void)
{
  P1DIR |= 0x01;
  LED1 = 1;
}

void InitT3()
{
  T3CTL = 0x02;
  T3CTL |= 0x08;
  T3IE = 1;
  T3CTL |= 0xe0;
  //T3CTL &= 0xfe;
  T3CTL |= 0x10;
  T3CC0 = 0xff;
  EA = 1;
}

void main(void)
{
  InitLed();
  InitT3();
  while(1)
  {
    if (count > 245)
    {
      LED1 = !LED1;
      count = 0;
    }
  }
}

#pragma vector = T3_VECTOR 
__interrupt void T3_ISR(void)
{
  IRCON = 0x00;
  count++;
}