/*
  ????: ???????
  ????: Keil uVision 4.10 
  ????: CT107????????? 8051 12MHz
  ?    ?: 2011-8-9
*/
#include <STC15F2K60S2.H>
#include "intrins.h"

sbit DQ = P1^4;  //?????

//???????
void Delay_OneWire(unsigned int t)  
{
	
	t=t*10;
	while(t--);
}


//??????DS18B20?????
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//?DS18B20??????
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}

//DS18B20?????
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
  	Delay_OneWire(10); 
    	initflag = DQ;     
  	Delay_OneWire(5);
  
  	return initflag;
}

unsigned int Temper_Get()
{
	unsigned int temp;
	unsigned char low;
	unsigned char high;
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	Delay_OneWire(20);
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	
	
	low=Read_DS18B20();
	high=Read_DS18B20();
	
	high=high&0x0f;
	temp=high;
	temp=temp<<8;
	temp=temp|low;
	temp=temp*6.25;
	
	return temp;
	
}
	





