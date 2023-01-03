#include <stc15f2k60s2.h>
#include <intrins.h>
#include "iic.h"
#include "ds1302.h"
#include "onewire.h"
#include <stdio.h>

#define Somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

#define uchar unsigned char
#define uint unsigned int

sbit TX=P1^0;
sbit RX=P1^1;

extern uchar shijian[7];

code uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
					0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
															0xbf,0xff};
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;
void allinit();
void Delay_ms(uint ms);		//@11.0592MHz
void Timer2Init(void);		//1??@11.0592MHz

uchar AD=0;
uint temper=0;

//*************³¬Éù²¨****************//
void Bo_function();
void Timer0Init(void);		//1??@11.0592MHz
void Send_Wave();
uint Dis_Get();
uint dis=0;
uint distance=0;
bit bo_flag=0;

//*************´®¿Ú*******************//
void UartInit(void);		//115200bps@11.0592MHz
void Uart_function();
void Send_Buffer(uchar *p,uchar length);
uchar rec_num=0;
uchar rec_flag=0;
bit rec_over=0;
uchar rec_buffer[]="123\r\n";
uchar send_buffer[15];


void main()
{
	allinit();
	Timer2Init();
	Timer0Init();
	ds1302_init();
	UartInit();
	while(1)
	{	
		DA_Write(255);
		temper=rd_temperature();
//		AD=AD_Read(0x03);
//		Digbuf[0]=AD/100;
//		Digbuf[1]=AD%100/10;
//		Digbuf[2]=AD%10;
		ds1302_get();
//		Digbuf[0]=shijian[0]/10;
//		Digbuf[1]=shijian[0]%10;
//		Digbuf[2]=20;
//		Digbuf[3]=shijian[1]/10;
//		Digbuf[4]=shijian[1]%10;
//		Digbuf[5]=20;
//		Digbuf[6]=shijian[2]/10;
//		Digbuf[7]=shijian[2]%10;
//		Digbuf[4]=temper/1000;
//		Digbuf[5]=temper%1000/100+10;
//		Digbuf[6]=temper%100/10;
//		Digbuf[7]=temper%10;
		Uart_function();
		Bo_function();
		Digbuf[5]=dis/100;
		Digbuf[6]=dis%100/10;
		Digbuf[7]=dis%10;
	}
}
void Bo_function()
{
	if(bo_flag==1)
	{
		bo_flag=0;
		dis=Dis_Get();
	}
}
uint Dis_Get()
{
	TH0=0;
	TL0=0;
	Send_Wave();
	TR0=1;
	while((TF0==0)&&(RX==1));
	TR0=0;
	if(TF0==1)
	{
		distance=999;
		TF0=0;
	}
	if(RX==0)
	{
		RX=1;
		distance=(((TH0<<8)|(TL0))*0.017);
	}
	return distance;
}
void Send_Wave()
{
	uchar i=8;
	do
	{
		TX=1;
		Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;
		TX=0;
		Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;
	}while(i--);
}

void Uart_function()
{
	if(rec_over==1)
	{
		rec_over=0;
		if((rec_buffer[0]=='t')&(rec_buffer[1]=='t')&(rec_buffer[2]=='t')&(rec_buffer[3]=='\r')&(rec_buffer[4]=='\n')&&(rec_num==5))
		{
			sprintf((uchar*)send_buffer,"123213\r\n");
			Send_Buffer(send_buffer,8);
		}
		else
		{
			sprintf((uchar*)send_buffer,"error\r\n");
			Send_Buffer(send_buffer,7);
		}
		rec_num=0;
	}
}

void UartInit(void)		//115200bps@11.0592MHz
{
	SCON = 0x50;		//8???,?????
	AUXR |= 0x40;		//?????1T??
	AUXR &= 0xFE;		//??1?????1???????
	TMOD &= 0x0F;		//???????
	TL1 = 0xE8;		//???????
	TH1 = 0xFF;		//???????
	ET1 = 0;		//?????%d??
	TR1 = 1;		//???1????
	
	ES=1;
	EA=1;
}
void red_ser() interrupt 4
{
	if(RI)
	{
		rec_buffer[rec_num]=SBUF;
		rec_num++;
		RI=0;
		rec_flag=1;
	}
}
void Send_Buffer(uchar *p,uchar length)
{
	uchar i=0;
	for(i=0;i<length;i++)
	{
		SBUF=*(p+i);
		while(TI==0);
		TI=0;
	}
}

void Timer2Init(void)		//1??@11.0592MHz
{
	AUXR |= 0x04;		//?????1T??
	T2L = 0xCD;		//???????
	T2H = 0xD4;		//???????
	AUXR |= 0x10;		//???2????
	IE2|=0x04;
	EA=1;
}
void t2_ser() interrupt 12
{
	
	static uint bo_tt=0;
	
	P2|=0xe0;
	P2&=0xff;
	P0=0xff;
	
	P2|=0xc0;
	P2&=0xdf;
	P0=0x01<<Digcom;
	
	P2|=0xe0;
	P2&=0xff;
	P0=tab[Digbuf[Digcom]];
	if(++Digcom==8){Digcom=0;}
	
	if(rec_flag>=1)
	{
		rec_flag++;
		if(rec_flag>=20)
		{
			rec_over=1;
			rec_flag=0;
		}
	}
	
	if(++bo_tt==500)
	{
		bo_tt=0;
		bo_flag=1;
	}
}
void Timer0Init(void)		//1??@11.0592MHz
{
	AUXR &= 0x7F;		//?????12T??
	TMOD &= 0xF0;		//???????
	TL0 = 0;		//???????
	TH0 = 0;		//???????
	TF0 = 0;		//??TF0??
	TR0 = 0;		//???0????
}

void allinit()
{
	P2|=0xa0;
	P2&=0xbf;
	P0=0x00;
	
	P2|=0x80;
	P2&=0x9f;
	P0=0xff;
	
	P2|=0xc0;
	P2&=0xdf;
	P0=0xff;
	
	P2|=0xe0;
	P2&=0xff;
	P0=0xff;
	
}
void Delay_ms(uint ms)		//@11.0592MHz
{
	unsigned char i, j,k;
	for(k=0;k<ms;k++)
	{
		_nop_();
		_nop_();
		_nop_();
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);
	}
}
