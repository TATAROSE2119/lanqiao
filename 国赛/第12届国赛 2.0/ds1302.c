/*
  程序说明: DS1302驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台 8051，12MHz
  日    期: 2011-8-9
*/

#include <STC15F2K60S2.H>
#include <INTRINS.H>

sbit SCK=P1^7;		
sbit SDA=P2^3;		
sbit RST = P1^3;   // DS1302复位

unsigned char shijian[]={0,20,20,0,0,0,0};

void Write_Ds1302_Byte(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK=0;
		SDA=temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

void Write_Ds1302( unsigned char address,unsigned char dat )     
{
 	RST=0;
	_nop_();
 	SCK=0;
	_nop_();
 	RST=1;	
   	_nop_();  
 	Write_Ds1302_Byte(address);	
    dat=((dat/10)<<4)|(dat%10);
 	Write_Ds1302_Byte(dat);		
 	RST=0; 
}

unsigned char Read_Ds1302 ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;
	_nop_();
 	SCK=0;
	_nop_();
 	RST=1;
	_nop_();
 	Write_Ds1302_Byte(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;
	_nop_();
 	RST=0;
	SCK=0;
	_nop_();
	SCK=1;
	_nop_();
	SDA=0;
	_nop_();
	SDA=1;
	_nop_();
    temp=((temp/16)*10)+(temp%16);
	return (temp);			
}
void DS1302_Init()
{
    unsigned char i=0;
    unsigned char add=0x80;
    Write_Ds1302(0x8e,0x00);
    for ( i = 0; i < 8; i++)
    {
        Write_Ds1302(add,shijian[i]);
        add+=2;
    }
    Write_Ds1302(0x8e,0x80);

}
void DS1302_Get()
{
    unsigned char i=0;
    unsigned char add=0x81;
    Write_Ds1302(0x8e,0x00);
    for ( i = 0; i < 8; i++)
    {
        shijian[i]=Read_Ds1302(add);
        add+=2;
    }
    Write_Ds1302(0x8e,0x80);

}