#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include "onewire.h"

#define uchar unsigned char
#define uint unsigned int

uchar LED_Bits=0xff;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
				0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
														0xbf,0xff};
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

uchar temperature=0;
uchar qujian=0;
uchar T_MAX=30;
uchar T_MIN=20;
bit SET_MODE=0;
bit DOWN_flag=0;
uchar NUM=0;
bit REMOVE=0;
uint tt=0;
bit fla=0;

void allinit();
void Timer2Init(void);		//1??@12.000MHz
void Delay_ms(uint ms);		//@12.000MHz
void keyscan16_2();
void Timer0Init(void);		//1毫秒@12.000MHz

void main()
{
	allinit();
	Timer2Init();
	Timer0Init();
	ET0=1;
	while(1)
	{   
        temperature=Temper_Get();

        if (qujian<T_MIN)
		{
			qujian=0;
			P2|=0xa0;
			P2&=0xbf;
			P0=0x00;
		}
		else if ((qujian>=T_MIN)&&(qujian<=T_MAX))
		{
			qujian=1;
			P2|=0xa0;
			P2&=0xbf;
			P0=0x00;
		}
		else if (qujian>T_MAX)
		{
			qujian=2;
			P2|=0xa0;
			P2&=0xbf;
			P0=0x10;
		}
		if (SET_MODE==0)
		{
			Digbuf[0]=20;
			Digbuf[1]=qujian;
			Digbuf[2]=20;
			Digbuf[3]=21;
			Digbuf[4]=21;
			Digbuf[5]=21;
			Digbuf[6]=temperature/10;
			Digbuf[7]=temperature%10;
		}
		else if (SET_MODE==1)
		{
			if (REMOVE==1)
			{
				Digbuf[0]=20;
				Digbuf[1]=21;
				Digbuf[2]=21;
				Digbuf[3]=21;
				Digbuf[4]=21;
				Digbuf[5]=20;
				Digbuf[6]=21;
				Digbuf[7]=21;

				REMOVE=0;
			}
			
			
			if ((Digbuf[1]==21)&&(DOWN_flag==1))
			{
				DOWN_flag=0;
				Digbuf[1]=NUM;
			}
			else if ((Digbuf[2]==21)&&(DOWN_flag==1))
			{
				DOWN_flag=0;
				Digbuf[2]=NUM;
			}
			else if ((Digbuf[6]==21)&&(DOWN_flag==1))
			{
				DOWN_flag=0;
				Digbuf[6]=NUM;
			}
			else if ((Digbuf[7]==21)&&(DOWN_flag==1))
			{
				DOWN_flag=0;
				Digbuf[7]=NUM;

				if ((Digbuf[6]*10+Digbuf[7])>(Digbuf[1]*10+Digbuf[2]))
				{
					Digbuf[0]=20;
					Digbuf[1]=21;
					Digbuf[2]=21;
					Digbuf[3]=21;
					Digbuf[4]=21;
					Digbuf[5]=20;
					Digbuf[6]=21;
					Digbuf[7]=21;

					LED_Bits=~0x02;
					P2=0x80;
					P0=LED_Bits;
				}
				
			}
			

		}
		

		keyscan16_2();
	}
}
void allinit()
{
	P2|=0xa0;
	P2&=0xbf;
	P0=0x00;//�������̵���
	
	P2|=0x80;
	P2&=0x9f;
	P0=0xff;//LED
	
	P2|=0xc0;
	P2&=0xdf;
	P0=0xff;//λѡ
	
	P2|=0xe0;
	P2&=0xff;
	P0=0xff;//��ѡ
	
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
void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x20;		//设置定时初始值
	TH0 = 0xD1;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void t2int() interrupt 12           //????
{
	P2|=0xe0;
	P2&=0xff;
	P0=0xff;//��ѡ
	
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
	
}
void tm0_isr() interrupt 1
{
	tt++;
	if ((qujian==0)&&(tt==800))
	{
		if (fla==0)
		{
			fla=1;P2=0x80;P0=~0x01;
		}
		else if (fla==1)
		{
			fla=0;P2=0x80;P0=0xff;
		}
		tt=0;
	}
	else if ((qujian==1)&&(tt==400))
	{
		if (fla==0)
		{
			fla=1;P2=0x80;P0=~0x01;
		}
		else if (fla==1)
		{
			fla=0;P2=0x80;P0=0xff;
		}
		tt=0;

	}
	else if ((qujian==2)&&(tt==200))
	{
		if (fla==0)
		{
			fla=1;P2=0x80;P0=~0x01;
		}
		else if (fla==1)
		{
			fla=0;P2=0x80;P0=0xff;
		}
		tt=0;

	}
	
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
void keyscan16_2()
{
	uchar temp;
	
	P3=0x7f;P44=0;P42=1;
	temp=P3&0x0f;
	if(temp!=0x0f)
	{
		Delay_ms(5);
		temp=P3&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case(0x7e):
					DOWN_flag=1;NUM=0;break;
				case(0x7d):
					DOWN_flag=1;NUM=3;break;
				case(0x7b):
					DOWN_flag=1;NUM=6;break;
				case(0x77):
					DOWN_flag=1;NUM=9;break;
				default:
					break;
			}
		}
		while(temp!=0x0f)
		{
			temp=P3&0x0f;
		}
	}
	P3=0xbf;P44=1;P42=0;
	temp=P3&0x0f;
	if(temp!=0x0f)
	{
		Delay_ms(5);
		temp=P3&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case(0xbe):
					DOWN_flag=1;NUM=1;break;
				case(0xbd):
					DOWN_flag=1;NUM=4;break;
				case(0xbb):
					DOWN_flag=1;NUM=7;break;
				case(0xb7):
					if (SET_MODE==0)
					{
						SET_MODE=1;

						Digbuf[0]=20;
						Digbuf[1]=21;
						Digbuf[2]=21;
						Digbuf[3]=21;
						Digbuf[4]=21;
						Digbuf[5]=20;
						Digbuf[6]=21;
						Digbuf[7]=21;
						
						ET0=0;
					}
					else if (SET_MODE==1)
					{
						SET_MODE=0;

						ET0=1;
					}
					break;
				default:
					break;

			}
		}
		while(temp!=0x0f)
		{
			temp=P3&0x0f;
		}
	}
	P3=0xdf;P44=1;P42=1;
	temp=P3&0x0f;
	if(temp!=0x0f)
	{
		Delay_ms(5);
		temp=P3&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case(0xde):
					DOWN_flag=1;NUM=2;break;
				case(0xdd):
					DOWN_flag=1;NUM=5;break;
				case(0xdb):
					DOWN_flag=1;NUM=8;break;
				case(0xd7):
					if (REMOVE==0)
					{
						REMOVE=1;
					}
					else if (REMOVE==1)
					{
						REMOVE=0;

						T_MAX=Digbuf[1]*10+Digbuf[2];
						T_MIN=Digbuf[6]*10+Digbuf[7];
					}
					break;
				default:
					break;
			}
		}
		while(temp!=0x0f)
		{
			temp=P3&0x0f;
		}
	}
}
//����LED1
//LED_Bits&=0xfe;
//	P2|=0x80;
//	P2&=0x9f;
//	P0=LED_Bits;
//Ϩ��LED1
//LED_Bits|=0x01;
//	P2|=0x80;
//	P2&=0x9f;
//	P0=LED_Bits;
