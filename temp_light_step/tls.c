#include <ioCC2530.h>

#define uchar unsigned char
#define uint unsigned int

#define TEMPER P1_1
#define SWITCH P0_5
#define JDC P1_4

uchar temh,teml;
uchar timer;
uint temp;
uchar ana;
uchar position;
uchar datacache[8];

uchar flag;
uchar on_off;
float light_vol;

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

//为18B20初始化时钟晶振
void init_clock()
{
  CLKCONCMD &= 0x00;
  while(CLKCONSTA != 0x00);
  CLKCONCMD &= 0x00;
}

//初始化18B20
void init_18B20() 
{
  P1DIR |= 0x02;
  TEMPER = 1;
  delay_ms(1);
  TEMPER = 0;
  delay_us(720);
  TEMPER = 1;
  
  P1DIR &= ~0x02;
  delay_us(40);
  
  if (TEMPER == 0)
  {
    flag = 1;
  }
  else
  {
    flag = 0;
  }
  delay_us(200);
  
  P1DIR |= 0x02;
  TEMPER = 1;
}

//向18B20写数据
void write_18B20(uint data)
{  
  uint i;
  P1DIR |= 0x20;
  TEMPER = 1;
  
  for(i=0; i<8; i++)  
  {  
    TEMPER = 0;
    delay_us(1);
    
    if(data & (0x01 << i))
    {
      TEMPER = 1;
    }
    else
    {
      TEMPER = 0;
    }
    delay_us(40);
    TEMPER = 1;
  }
} 

//从18B20读数据
uint read_18B20()  
{   
  uint data,i;
  data = 0;
  
  for(i=0; i<8; i++)  
  {  
    P1DIR |= 0x02;
    TEMPER = 0;
    delay_us(1);
    TEMPER = 1;
    
    P1DIR &=  ~0x02;
    if(TEMPER)
    {
      data |= (1 << i);
    }
    else
    {
      data &= ~(1 << i);
    }
    delay_us(80);
  }  
  return data;
}

//将从18B20读取的数据转化为十进制
void read_data(void)
{
  init_18B20();
  write_18B20(0xcc);    //空指令
  write_18B20(0x44);    //开始转换指令
  delay_ms(2);
 
  init_18B20(); 
  write_18B20(0xcc);    
  write_18B20(0xbe);    //读RAM指令

  teml = read_18B20();   //18B20芯片特性，连续两次读取分别得低高8位
  temh = read_18B20();
  
  if (temh & 0xf0)    //11-15位为符号位
  {
    ana = 0;

    temp = ((temh << 8) | teml);
    temp = (~temp) + 1;
    temp *= 0.625;    //精度0.0625，uint格式限制，取0.625
  }
  else
  {
    ana = 1;
    
    temp = ((temh << 8) | teml);
    temp *= 0.625;
  }
}

//数据存入缓存数组
void transf_temper()
{
  init_clock();
  read_data();

  if(ana == 1)
  {
    datacache[0] = '+';
  }
  else
  {
    datacache[0] = '-';
  }
  if (temp/1000 == 0)
  {
    datacache[1] = ' ';
  }
  else
  {
    datacache[1] = temp/1000 + 0x30;
  }
  if ((temp/1000 == 0) && (temp/100 == 0))
  {
    datacache[2] = ' ';
  }
  else
  {
    datacache[2] = temp/100 + 0x30;
  }
  datacache[3] = (temp%100)/10 + 0x30;
  datacache[4] = '.';
  datacache[5] = temp%10 + 0x30;
  datacache[6] = 'C';
  datacache[7] = '\r';
}

//控制继电器开关
void jdc_switch(uint run)
{
  P1DIR |= 0x10;
  P1SEL &= 0x10;
  if ((run == 1) && (position == 0))
  {
    JDC = 1;
    position = 1;
  }
  if ((run == 0) && (position == 1))
  {
    JDC = 0;
    position = 0;
  }
}

//判断室温及光照
void temp_ctl()
{
  if (temp > 280)    //28度以上
  {
    if (light_vol > 3)    //光线强时为高阻
    {
      jdc_switch(1);
    }
  }
  if (temp < 200)    //20度以下
  {
    if (light_vol < 1)    //光线弱时为低阻
    {
      jdc_switch(0);
    }
  }
}

//初始化ad
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

//ad采集键盘
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

//读取键盘
uint scan_key()
{
  uint key_flag;
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
      if ((vol<1.4) && (vol>1.1))
      {
        key_flag = 1;
      }
      if ((vol<1.8) && (vol>1.6))
      {
        key_flag = 2;
      }
      if ((vol<2.1) && (vol>1.9))
      {
        key_flag = 3;
      }
    }
  }
  return key_flag;
}

//ad采集光敏电阻
void light_adc()
{
  uint value;
  
  init_adc();
  
  P0DIR &= ~0x01;
  APCFG |= 0x01;
  
  ADCCON3 = 0xb1;
  ADCCON1 |= 0x30;
  ADCCON1 |= 0x40;
  while ((ADCCON1 & 0x80) != 0x80);

  value =  ADCL >> 4;
  value |= (((uint)ADCH) << 4);
  
  light_vol = (float)value/(float)2048*3.3;
}

//初始化串口
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

//串口发送数据
void trans_data()
{
  uint i;
  init_uart();

  for (i=0; i<10; i++)
  {
    U0DBUF = datacache[i];
    while(UTX0IF == 0);
    UTX0IF = 0;
  }
  delay_ms(1000);
}

main()
{
  uint key_flag;
  on_off = 1;
  position = 0;
  
  while (1)
  {
    
    key_flag = scan_key();
    if (key_flag == 3)
    {
      on_off = !on_off;
    }
    
    if ((key_flag == 1) | (key_flag == 0))
    {
      jdc_switch(key_flag);
    }
    
    transf_temper();

    if (on_off == 1)
    {
      temp_ctl();
    }
    trans_data();
  }
}