#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include "iic.h"

#define uchar unsigned char
#define uint unsigned int

uchar LED_Bits=0xff;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
				0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
														0xbf,0xff};
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

uchar Work_Moded=1;
uint Led_Light_T=0;
uint i=0;
uint Liuzhuan_Interval=400;
uint Liuzhuan_Count_Tt=0;
bit Led_Liuzhuan_On=0;


uchar S4_FLAG=0;
uchar FLOW_INTERVAL=1200;
uchar LED_FLOW=1;
uchar S6_SETTING;
uint LED_TT=0,LED_T=0,SET_TT=0,AD_TT=0;
uchar LED_TIME=0;
uchar LED_NUM=0;
uchar WORK_MODE=1;
uchar LIGHT=0;
uchar FLOW_INTERVAL_SAVE_VALUE[5];
uchar AD=0;
uchar KEY_TEMP=0xff,KEY=0xff;


void allinit();
void Timer2Init(void);		//1??@12.000MHz
void Timer0Init(void);		//1姣?绉扏12.000MHz

void Delay_ms(uchar ms);		//@12.000MHz
void keyscan();


void main(void)
{
	allinit();
	Timer2Init();
	//Timer0Init();
	while (1)
	{
		// AD=AD_Read(0x03);
		// Digbuf[5]=AD/100;
		// Digbuf[6]=AD%100/10;
		// Digbuf[7]=AD%10;
		Digbuf[0]=WORK_MODE;
		keyscan();
	}
}
void keyscan()
{
	if (P30==0)//7
	{
		Delay_ms(5);
		if (P30==0)
		{
			if (WORK_MODE>3)
			{
				WORK_MODE=0;
			}
			
			WORK_MODE++;
		}
		while(!P30);
	}
	
}
void Timer0Init(void)		//1微秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xF4;		//设置定时初始值
	TH0 = 0xFF;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时

	ET0 = 1;                        //浣胯兘瀹氭椂鍣?0涓?鏂?
    EA = 1;
}
void tm0_isr() interrupt 1
{		

		
		
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
void t2int() interrupt 12           //????
{

	P2|=0xe0;
	P2&=0xff;
	P0=0xff;//露脦脩隆
	
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

	LED_TT++;
	SET_TT++;
	AD_TT++;
	if ((LED_FLOW==1)&&(LED_TT==1))
	{
		if (WORK_MODE==1)
		{
			P2=0x00;
			P0=0xff;
			P2=0x80;
			P0=~(0x01<<LED_NUM);
			P2=0x00;
		}
		else if (WORK_MODE==2)
		{
			P2=0x00;
			P0=0xff;
			P2=0x80;
			P0=~(0x80>>LED_NUM);
			P2=0x00;
		}
		else if (WORK_MODE==3)
		{
			P2=0X00;
			P0=0XFF;
			P2=0X80;
			if(LED_NUM==0)P0=0X7E;
			else if(LED_NUM==1)P0=0XBD;
			else if(LED_NUM==2)P0=0XDB;
			else if(LED_NUM==3)P0=0XE7;
			P2=0X00;
		}
		else if (WORK_MODE==4)
		{
			P2=0X00;P0=0XFF;P2=0X80;
			if(LED_NUM==0)P0=0XE7;
			else if(LED_NUM==1)P0=0XDB;
			else if(LED_NUM==2)P0=0XBD;
			else if(LED_NUM==3)P0=0X7E;
			P2=0X00;
		}
	}
	else if (LED_TT==LIGHT)
	{
		P2=0X00;P0=0XFF;P2=0X80;P0=0xff;P20=0x00;
	}
	else if (LED_TT==20)
	{
		LED_TT=0;
		LED_T++;
		if ((LED_T*20)>=LED_TIME)
		{
			LED_T=0;
			if (WORK_MODE==1)
			{
				LED_NUM++;if (LED_NUM>=8){LED_NUM=0;}
			}
			else if (WORK_MODE==2)
			{
				LED_NUM++;if (LED_NUM>=8){LED_NUM=0;}
			}
			else if (WORK_MODE==3)
			{
				LED_NUM++;if (LED_NUM>=4){LED_NUM=0;}
			}
			else if (WORK_MODE==4)
			{
				LED_NUM++;if (LED_NUM>=4){LED_NUM=0;}
			}
			
		}
		
	}
	if (AD_TT==200)
	{
		AD_TT=0;
		AD=AD_Read(0x03);
		if (AD<64)
		{
			LIGHT=2;
		}
		else if ((AD>=64)&&(AD<128))
		{
			LIGHT=5;
		}
		else if ((AD>=128)&&(AD<192))
		{
			LIGHT=10;
		}
		else if ((AD>=196)&&(AD<256))
		{
			LIGHT=19;
		}
		
	}

}
void Delay_ms(uchar ms)		//@12.000MHz
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
void allinit()
{
	P2|=0xa0;
	P2&=0xbf;
	P0=0x00;//路盲脙霉脝梅录脤碌莽脝梅
	
	P2|=0x80;
	P2&=0x9f;
	P0=0xff;//LED
	
	P2|=0xc0;
	P2&=0xdf;
	P0=0xff;//脦禄脩隆
	
	P2|=0xe0;
	P2&=0xff;
	P0=0xff;//露脦脩隆
	
}
