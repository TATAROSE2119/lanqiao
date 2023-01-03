#include "ds1302.h"  									
#include <stc15f2k60s2.h>
#include <intrins.h>

sbit SCK = P1^7;		
sbit SDA = P2^3;		
sbit RST = P1^3; 


unsigned char shijian[7]={55,59,23,0,0,0,0};
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
	dat=(((dat/10)<<4)|(dat%10));
 	Write_Ds1302(dat);		
 	RST=0; 
}

//
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	temp=(temp/16)*10+(temp%16);
	return (temp);			
}
void ds1302_init()
{
	unsigned char i=0;
	unsigned char add=0x80;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<8;i++)
	{
		Write_Ds1302_Byte(add,shijian[i]);
		add+=2;
	}
	Write_Ds1302_Byte(0x8e,0x80);
}
void ds1302_get()
{
	unsigned char i=0;
	unsigned char add=0x81;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<8;i++)
	{
		shijian[i]=Read_Ds1302_Byte(add);
		add+=2;
	}
	Write_Ds1302_Byte(0x8e,0x80);
}