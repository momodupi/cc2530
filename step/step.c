#include <iocc2530.h>

#define uint unsigned int
#define uchar unsigned char

#define Q P1_0
#define W P1_1
#define E P1_2
#define R P1_3


//char excite[]={0xf7,0xf3,0xfb,0xf9,0xfd,0xfc,0xfe,0xf6};

void delay_ms(uint ms)
{
  uint i,j;
  for(i=0;i<100;i++)
  {
    for (j=0;j<ms;j++)
    {
      asm("nop");
      asm("nop");
      asm("nop");
    }
  }
}

void main()
{
  uchar i;
  P1SEL=0x00;
  P1DIR=0xff;

  while(1)
  {

    for(i=0;i<8;i++)
    {
      switch(i)
      {
        case 0:
        {
          Q=1;
          W=1;
          E=1;
          R=0;
        }
        break;
        case 1:
        {
          Q=1;
          W=1;
          E=0;
          R=0;
        }
        break;
        case 2:
        {
          Q=1;
          W=1;
          E=0;
          R=1;
        }
        break;
        case 3:
        {
          Q=1;
          W=0;
          E=0;
          R=1;
        }
        break;
        case 4:
        {
          Q=1;
          W=0;
          E=1;
          R=1;
        }
        break;
        case 5:
        {
          Q=0;
          W=0;
          E=1;
          R=1;
        }
        break;
        case 6:
        {
          Q=0;
          W=1;
          E=1;
          R=1;
        }
        break;
        case 7:
        {
          Q=0;
          W=1;
          E=1;
          R=0;
        }
        break;

      }
      delay_ms(1);

    }

  }
}