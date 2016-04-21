#include <ioCC2530.h>

#define unit unsigned int
#define uchar unsigned char

#define led1 P1_0
#define led2 P1_1
#define led3 P1_4
#define led4 P0_1

void delayms(uchar ms)
{
  uchar i,j;
  for(i=0;i<ms;i++)
  {
    for(j=0;j<535;j++);
  }
}

void ctlall(uchar flag)
{
  led1=flag;
  led2=flag;
  led3=flag;
  led4=flag;
}

main()
{
  ctlall(0);
  P1DIR=0x13;
  P0DIR=0x01;
  while(1)
  {
    led1=!led1;
    delayms(200);
    led2=!led2;
    delayms(200);
    led3=!led3;
    delayms(200);
    led4=!led4;
    delayms(200);    
  }
}