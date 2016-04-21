#include <ioCC2530.h>
#include <string.h>

#define uchar unsigned char
#define uint unsigned int


uchar datacache[]=" >_< ";
uchar length;

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
  P2DIR&=~0XC0; 
  
  U0CSR|=0x80;
  
  U0GCR=11;
  U0BAUD=216;
  UTX0IF=0;
}

void trans_data()
{
  uint i;
  for(i=0;i<length;i++)
  {
    U0DBUF=datacache[i];
    while(UTX0IF==0);
    UTX0IF=0;
  }
}

main()
{
  CLKCONCMD&=~0x40;
  while(CLKCONSTA&0x40);
  CLKCONCMD&=~0x47;
  length=strlen(datacache);
  
  init_uart();
    
  while(1)
  {
    trans_data();
    delayms(1000);
  }
}