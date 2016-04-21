#include <ioCC2530.h>
#include <string.h>

#define uchar unsigned char
#define uint unsigned int

#define led1 P1_0
#define led2 P1_1
#define led3 P1_4
#define led4 P0_1

char datacache[50];
uchar cont;

void delayms(uint ms)
{  
  uint i,j;
  for(i=0;i<ms;i++)
  for(j=0;j<535;j++);
}

void init_uart()
{ 
  PERCFG=0x00;
  P0SEL=0x0c;
  P2DIR&=~0xC0;
  P0DIR|=0x02;
  P1DIR|=0x13;
  
  U0GCR=11;
  U0BAUD=216;
  UTX0IF=0;
  
  U0CSR|=0xc0;
  
  IEN0=0x84;
}

void del_data()
{
  uint i;
  for(i=0;i<cont;i++)
  {
    datacache[i]=0;
  }
  cont=0;
}

void trans_data()
{
  uint i;
  for(i=0;i<cont;i++)
  {
    U0DBUF=datacache[i];
    while(UTX0IF==0);
    UTX0IF=0;
  }
  del_data();
  cont=0;
}

main()
{
  
  CLKCONCMD&=~0x40;
  while(CLKCONSTA&0x40);
  CLKCONCMD&=~0x47;
 
  init_uart();
  
  cont=0;
  while(1)
  { 
    if(cont<20)
    {
      trans_data();
      delayms(1000);
    }
    if(datacache[0]=='1')
    {
      led1=!led1;
    }
    if(datacache[0]=='2')
    {
      led2=!led2;
    }
    if(datacache[0]=='3')
    {
      led3=!led3;
    }
    if(datacache[0]=='4')
    {
      led4=!led4;
    }
  }
}

#pragma vector = URX0_VECTOR 
__interrupt void UART0_ISR(void) 
{
  URX0IF = 0;
  datacache[cont++] = U0DBUF;
}