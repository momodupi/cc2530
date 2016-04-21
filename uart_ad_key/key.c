#include <ioCC2530.h>
#include <string.h>

#define uchar unsigned char
#define uint unsigned int

#define led1 P1_1
#define led2 P1_2
#define led3 P1_3
#define led4 P0_1
/*
#define Q P1_4
#define W P1_5
#define E P1_6
#define R P1_7
*/
//uchar step1[]={0xcf,0xef,0x6f,0x7f,0x3f,0xbf,0x9f,0xdf};
//uchar step2[]={0xdf,0x9f,0xbf,0x3f,0x7f,0x6f,0xef,0xcf};

uchar datacache[10];

//延时微秒
void delay_us(uint us)
{
  uint i;
  for (i=0; i<us; i++)
  {
    asm("nop");
    asm("nop");
    asm("nop");
  }
}

//延时毫秒
void delay_ms(uint ms)
{
  uint i,j;
  for (i=0; i<ms; i++)
  {
    for(j=0;j<1000;j++)
    {
      asm("nop");
      asm("nop");
      asm("nop");
    }
  }
}

//ad初始化
void init_adc()
{
  IEN0 = 0;
  IEN1 = 0;
  IEN2 = 0;
  CLKCONCMD &= ~0x40;
  while (CLKCONSTA & 0x40);
  CLKCONCMD &= ~0x47;
  TR0 = 0x01;
}

//ad采集
uint get_adc()
{
  uint value;
  init_adc();

  P0DIR &= ~0x40;
  APCFG |= 0x40;
  
  ADCCON3 = 0xb6;
  ADCCON1 |= 0x30;
  ADCCON1 |= 0x40;
  while ((ADCCON1 & 0x80) != 0x80);

  value =  ADCL >> 4;
  value |= (((uint)ADCH) << 4);
  
  return value;
}


//串口初始化
void init_uart()
{ 
  PERCFG = 0x00;
  P0SEL |= 0x0c;
  P2DIR &= ~0xc0;
  
  U0CSR |= 0x80;
  
  U0GCR = 11;
  U0BAUD = 216;
  UTX0IF = 0;
}

//发送函数
void trans_data(float vol)
{
  init_uart();
  uint real;
  uchar i;
  vol = vol*1000;
  real = (uint)vol;
  datacache[0] = real/1000 + 0x30;
  datacache[1] = '.';
  datacache[2] = real/100%10 + 0x30;
  datacache[3] = real/100/10 + 0x30;
  datacache[4] = real%10 + 0x30;
  datacache[5] = 'V';
  datacache[6] = '\n';
  
  for (i=0; i<10; i++)
  {
    U0DBUF = datacache[i];
    while (UTX0IF == 0);
    UTX0IF = 0;
  }
  delay_ms(1000);
}


uchar scan_key()
{
  uchar key_flag;
  uint value = 0;
  float vol;
  P2DIR &= ~0x01;
  if (P2_0 == 1)
  {
    delay_ms(3);
    if (P2_0 == 1)
    {
      value = get_adc();
      vol = (float)value/(float)2048*3.3;
      if ((vol<0.4) && (vol>0.2))
      {
        key_flag = 0;
      }
      if ((vol<1.5) && (vol>1.0))
      {
        key_flag = 1;
      }
      if ((vol<1.9) && (vol>1.5))
      {
        key_flag = 2;
      }
      if ((vol<2.1) && (vol>1.9))
      {
        key_flag = 3;
      }
    }
  }
  trans_data(vol);

  return key_flag;
}

void led_show(uchar key_flag)
{
  P0DIR |= 0x02;
  P1DIR |= 0x13;
  switch(key_flag)
  {
    case 0: led1 = !led1;
    break;
    case 1: led2 = !led2;
    break;
    case 2: led3 = !led3;
    break;
    case 3: led4 = !led4;
    break;
  }
  key_flag = 4;
}
/*
void step_start(uchar run)
{
  uchar i;
  uint cont;
  P1DIR |= 0xf0;
  P1SEL &= 0x0f;
  if (run == '1')
  {
    for(cont=0; cont<400; cont++)
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
  else
  {
    for(cont=0; cont<400; cont++)
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
}
*/

main()
{
  //uchar key_flag;
  while (1)
  {
    led_show(scan_key());
    //key_flag = scan_key();

  }
}