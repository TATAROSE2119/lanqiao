//*********³¬Éù²¨*********//

#include <stc15f2k60s2.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int
	
#define Somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
sbit TX=P1^0;
sbit RX=P1^1;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
				0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
														0xbf,0xff};
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

void allinit();
void Timer0Init(void);		//1??@11.0592MHz
void Timer2Init(void);		//1??@11.0592MHz

void Bo_Function();
void Send_Wave();
uint Dis_Get();
uint dis=0;
uint distance=0;
bit bo_flag=0;

void main()
{
	allinit();
	Timer0Init();
	Timer2Init();
	while(1)
	{
		Bo_Function();
		Digbuf[0]=dis/100;
		Digbuf[1]=dis%100/10;
		Digbuf[2]=dis%10;
		
	}
}
void Bo_Function()
{
	if(bo_flag==1)
	{
		bo_flag=0;
		dis=Dis_Get();
	}
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
uint Dis_Get()
{
	TH0=0;
	TL0=0;
	Send_Wave();
	TR0=1;
	while((RX==1)&&(TF0==0));
	TR0=0;
	if(RX==0)
	{
		RX=1;
		distance=((TH0<<8)|(TL0))*0.017;
	}
	if(TF0==1)
	{
		TF0=0;
		distance=999;
	}
	return distance;
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
	P0=0xff;//¶ÎÑ¡
	
	P2|=0xc0;
	P2&=0xdf;
	P0=0x01<<Digcom;
	
	P2|=0xe0;
	P2&=0xff;
	P0=tab[Digbuf[Digcom]];
	
	if(++Digcom==8)
	{
		Digcom=0;
	}
	
	if(++bo_tt==200)
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
