#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include "iic.h"
#include "onewire.h"


#define uchar unsigned char
#define uint unsigned int
	
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
				0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
															0xbf,0xff,
															0xc6,0x8c};//C  P//
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

bit Interface_Change=0;
bit CHANGE_MAX_MIN=1;//0--MAX   1--MIN
uchar Temperature=0;
uchar T_MAX=30;
uchar T_MIN=20;
uchar T_MAX_1=30;
uchar T_MIN_1=20;


void allinit();
void keyscan();
void Timer2Init(void);		//1??@12.000MHz
void Delay_ms(uint ms);		//@12.000MHz


void LED_SHOW();

void main()
{
	allinit();
	Timer2Init();
	while(1)
	{
		Temperature=Temper_Get();
		keyscan();
		
		if(Interface_Change==0)
		{
			Digbuf[0]=22;
			Digbuf[1]=21;
			Digbuf[2]=21;
			Digbuf[3]=21;
			Digbuf[4]=21;
			Digbuf[5]=21;
			Digbuf[6]=Temperature/10;
			Digbuf[7]=Temperature%10;
			
		}
		else if(Interface_Change==1)
		{
			Digbuf[0]=23;
			Digbuf[1]=21;
			Digbuf[2]=21;
			Digbuf[3]=T_MAX/10;
			Digbuf[4]=T_MAX%10;
			Digbuf[5]=21;
			Digbuf[6]=T_MIN/10;
			Digbuf[7]=T_MIN%10;
		}
		LED_SHOW();
	}
}

void keyscan()
{
	if(P30==0)// -
	{
		Delay_ms(5);
		if(P30==0)
		{
			if(Interface_Change==1)
			{
				if(CHANGE_MAX_MIN==0)
				{
					if(T_MAX<=0){T_MAX=99;}
					T_MAX--;
				}
				else if(CHANGE_MAX_MIN==1)
				{
					if(T_MIN<=0){T_MIN=99;}
					T_MIN--;
				}
			}
		}
		while(!P30);
	}
	else if(P31==0)// +
	{
		Delay_ms(5);
		if(P31==0)
		{
			if(Interface_Change==1)
			{
				if(CHANGE_MAX_MIN==0)
				{
					if(T_MAX>=100){T_MAX=0;}
					T_MAX++;
				}
				else if(CHANGE_MAX_MIN==1)
				{
					if(T_MIN>=100){T_MIN=0;}
					T_MIN++;
				}
			}
		}
		while(!P31);
	}
	else if(P32==0)
	{
		Delay_ms(5);
		if(P32==0)
		{
			if(Interface_Change==1)
			{
				if(CHANGE_MAX_MIN==0)
				{
					CHANGE_MAX_MIN=1;
				}
				else if(CHANGE_MAX_MIN==1)
				{
					CHANGE_MAX_MIN=0;
				}
			}
		}
		while(!P32);
	}
	else if(P33==0)
	{
		Delay_ms(5);
		if(P33==0)
		{
			if(Interface_Change==0)
			{
				Interface_Change=1;
			}
			else if(Interface_Change==1)
			{
				if(T_MAX>=T_MIN)
				{
					T_MAX_1=T_MAX;   T_MIN_1=T_MIN;
				}
				else
				{
					T_MAX=T_MAX_1;T_MIN=T_MIN_1;
					
				}
				Interface_Change=0;

			}
		}
		while(!P33);
	}
}
//   400/1.96=204   300/1.96=153  200/1.96=102  
void LED_SHOW()
{
	if(Temperature>T_MAX)
	{
		P2=0x80;
		P0=~0x01;
		
		DA_Write(204);
	}
	else if((Temperature>=T_MIN)&&(Temperature<=T_MAX))
	{
		P2=0x80;
		P0=~0x02;
		
		DA_Write(153);

	}
	else if(Temperature<T_MIN)
	{
		P2=0x80;
		P0=~0x04;
		
	    DA_Write(102);

	}
	
	if(T_MAX<T_MIN){P2=0x80;P0=~0x08;}
}

void Timer2Init(void)		//1??@12.000MHz
{
	AUXR |= 0x04;		//?????1T??
	T2L = 0x20;		//???????
	T2H = 0xD1;		//???????
	AUXR |= 0x10;		//???2????
	
	IE2 |= 0x04;                    //????2??
    EA = 1;
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
void t2int() interrupt 12           //????
{
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

}
void Delay_ms(uint ms)		//@12.000MHz
{
	unsigned char i, j,k;
	for(k=0;k<ms;k++)
	{
		i = 12;
		j = 169;
		do
		{
			while (--j);
		} while (--i);
	}
}

	
