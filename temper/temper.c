#include <ioCC2530.h>

#define uchar unsigned char
#define uint unsigned int

#define TEMPER P1_1

uchar temh,teml;
uint temp;
uchar ana;
uchar datacache[8];

uchar flag;


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

//初始化时钟晶振
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

//数据转换并存入缓存数组
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
  datacache[7] = '\n';
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
  
  while(1)
  {
    transf_temper();
    trans_data();
  }
}