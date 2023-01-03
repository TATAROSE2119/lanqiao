#include <stc15f2k60s2.h>
#include <intrins.h>
#include <stdio.h>

#define uchar unsigned char
#define uint unsigned int
	
void allinit();
void UartInit(void);		//115200bps@11.0592MHz
void Timer2Init(void);		//1??@11.0592MHz

void Uart_Function();
void Send_Buffer(uchar *p,uchar length);
uchar rec_flag=0;
uchar rec_buffer[]="123\r\n";
uchar send_buffer[15];
bit rec_over=0;
uchar rec_num=0;

void main()
{
	allinit();
	UartInit();
	Timer2Init();
	while(1)
	{
		Uart_Function();
	}
}
void Uart_Function()
{
	if(rec_over==1)
	{
		rec_over=0;
		rec_flag=0;
		if(((rec_buffer[0]=='s')&(rec_buffer[1]=='s')&(rec_buffer[2]=='s'))&&(rec_num=3))
		{
			sprintf((char *)send_buffer,"909090\r\n");
			Send_Buffer(send_buffer,8);
		}
		else
		{
			sprintf((char *)send_buffer,"error\r\n");
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
void rec_ser() interrupt 4
{
	if(RI)
	{
		
		rec_buffer[rec_num]=SBUF;
		rec_num++;
		rec_flag=1;
		RI=0;
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
	if(rec_flag>=1)
	{
		rec_flag++;
		if(rec_flag>=20)
		{
			rec_over=1;
		}
	}
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
