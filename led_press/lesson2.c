#include <ioCC2530.h>

typedef unsigned char uchar;
typedef unsigned int  uint;

#define led1 P1_0
#define led2 P1_1
#define led3 P1_4
#define led4 P0_1
#define key P0_5

void delayms(uint ms)
{ 
  uint i,j;
  for(i=0;i<ms;i++)
  {
    for(j=0;j<535;j++);
  }
}

void ctlall(uchar ctl)
{
  led1=ctl;
  led2=ctl;
  led3=ctl;
  led4=ctl;
}

void intled()
{
  P1DIR=0x13;
  P0DIR=0x02;
  ctlall(0);
}

uchar key_press()
{
  P0DIR&=0xdf;
  if (key==0)
  {
    delayms(10);
    if(key==0)
    {
      while(!key);
      return 1;
    }
  }
  return 0;
}

main()
{
  intled();
  
  while(1)
  {
    delayms(2);
    if(key_press())
    {
      led1=1;
      delayms(200);
      led1=0;
      led2=1;
      delayms(200);
      led2=0;
      led3=1;
      delayms(200);
      led3=0;
      led4=1;
      delayms(200);
      led4=0;
    }
  }
}
