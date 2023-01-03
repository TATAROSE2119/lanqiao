#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include "onewire.h"
#include "DS1302.h"

#define uchar unsigned char
#define uint unsigned int
	
uchar LED_Bits=0xff;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90
				,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10
															,0xbf,0xff,
																0xc6};
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

extern unsigned char shijian[];
uchar Alarm_Num[3]={0,0,0};
uchar Temperature=0;
bit Display_Mode=0;//0为时钟显示，1为温度显示
bit Any_Button_Down=0;
uchar Shijian_Set=0;
uchar Alarm_Set=0;
bit Alarm_ON=0;
uint LED_tt=0;
bit fla=0;
bit s4=0;

void allinit();
void keyscan();
void Timer2Init(void);		//1??@12.000MHz
void Delay_ms(uint ms);		//@12.000MHz

void Digbuf_Display();


void main()
{
	allinit();
	Timer2Init();
	DS1302_Init();
	while (1)
	{
		Temperature=Temper_Get();
		DS1302_Get();
		
		keyscan();
		if(Display_Mode==0)
		{
			if(Shijian_Set==0)
			{
				Digbuf[0]=shijian[2]/10;
				Digbuf[1]=shijian[2]%10;
				Digbuf[2]=20;
				Digbuf[3]=shijian[1]/10;
				Digbuf[4]=shijian[1]%10;
				Digbuf[5]=20;
				Digbuf[6]=shijian[0]/10;
				Digbuf[7]=shijian[0]%10;
			}
			else if(Shijian_Set==1)//设置时
			{
				if(shijian[0]%2==0)
				{
					Digbuf[0]=shijian[2]/10;
					Digbuf[1]=shijian[2]%10;
				}
				else
				{
					Digbuf[0]=21;
					Digbuf[1]=21;
				}
				Digbuf[2]=20;
				Digbuf[3]=shijian[1]/10;
				Digbuf[4]=shijian[1]%10;
				Digbuf[5]=20;
				Digbuf[6]=shijian[0]/10;
				Digbuf[7]=shijian[0]%10;
				
			}
			else if(Shijian_Set==2)//设置分
			{
				if(shijian[0]%2==0)
				{
					Digbuf[3]=shijian[1]/10;
					Digbuf[4]=shijian[1]%10;
				}
				else
				{
					Digbuf[3]=21;
					Digbuf[4]=21;
				}
				Digbuf[2]=20;
				Digbuf[1]=shijian[2]/10;
				Digbuf[2]=shijian[2]%10;
				Digbuf[5]=20;
				Digbuf[6]=shijian[0]/10;
				Digbuf[7]=shijian[0]%10;
				
			}
			else if(Shijian_Set==3)//设置秒
			{
				if(shijian[0]%2==0)
				{
					Digbuf[6]=shijian[0]/10;
					Digbuf[7]=shijian[0]%10;
				}
				else
				{
					Digbuf[6]=21;
					Digbuf[7]=21;
				}
				Digbuf[2]=20;
				Digbuf[3]=shijian[1]/10;
				Digbuf[4]=shijian[1]%10;
				Digbuf[5]=20;
				Digbuf[0]=shijian[2]/10;
				Digbuf[1]=shijian[2]%10;
				
			}
			if (Alarm_Set==0)
			{
				Digbuf[0]=Alarm_Num[2]/10;
				Digbuf[1]=Alarm_Num[2]%10;
				Digbuf[2]=20;
				Digbuf[3]=Alarm_Num[1]/10;
				Digbuf[4]=Alarm_Num[1]%10;
				Digbuf[5]=20;
				Digbuf[6]=Alarm_Num[0]/10;
				Digbuf[7]=Alarm_Num[0]%10;
			}
			
			else if(Alarm_Set==1)//设置时
			{
				if(shijian[0]%2==0)
				{
					Digbuf[0]=Alarm_Num[2]/10;
					Digbuf[1]=Alarm_Num[2]%10;
				}
				else
				{
					Digbuf[0]=21;
					Digbuf[1]=21;
				}
				Digbuf[2]=20;
				Digbuf[3]=Alarm_Num[1]/10;
				Digbuf[4]=Alarm_Num[1]%10;
				Digbuf[5]=20;
				Digbuf[6]=Alarm_Num[0]/10;
				Digbuf[7]=Alarm_Num[0]%10;
				
			}
			else if(Alarm_Set==2)//设置分
			{
				if(shijian[0]%2==0)
				{
					Digbuf[3]=Alarm_Num[1]/10;
					Digbuf[4]=Alarm_Num[1]%10;
				}
				else
				{
					Digbuf[3]=21;
					Digbuf[4]=21;
				}
				Digbuf[2]=20;
				Digbuf[1]=Alarm_Num[2]/10;
				Digbuf[2]=Alarm_Num[2]%10;
				Digbuf[5]=20;
				Digbuf[6]=Alarm_Num[0]/10;
				Digbuf[7]=Alarm_Num[0]%10;
				
			}
			else if(Alarm_Set==3)//设置秒
			{
				if(shijian[0]%2==0)
				{
					Digbuf[6]=Alarm_Num[0]/10;
					Digbuf[7]=Alarm_Num[0]%10;
				}
				else
				{
					Digbuf[6]=21;
					Digbuf[7]=21;
				}
				Digbuf[2]=20;
				Digbuf[3]=Alarm_Num[1]/10;
				Digbuf[4]=Alarm_Num[1]%10;
				Digbuf[5]=20;
				Digbuf[0]=Alarm_Num[2]/10;
				Digbuf[1]=Alarm_Num[2]%10;
				
			}
		}
		else if (Display_Mode==1)
		{
			Digbuf[0]=21;
			Digbuf[1]=21;
			Digbuf[2]=21;
			Digbuf[3]=21;
			Digbuf[4]=21;
			Digbuf[5]=Temperature/10;
			Digbuf[6]=Temperature%10;
			Digbuf[7]=22;
		}
		
		if ((shijian[0]==Alarm_Num[0])&&(shijian[1]==Alarm_Num[1])&&(shijian[2]==Alarm_Num[2]))
		{
			Alarm_ON=1;
			LED_tt=0;
		}
		else
		{
			Alarm_ON=0;
		}
		if (Any_Button_Down==1)
		{
			Alarm_ON=0;
		}
		
	}
}
void keyscan()
{
	if(P30==0)//s7
	{
		Delay_ms(5);
		if(P30==0)
		{
			Any_Button_Down=1;
			if(Shijian_Set==0)
			{
				Shijian_Set=1;//小时
			}
			else if(Shijian_Set==1)
			{
				Shijian_Set=2;//分钟
			}
			else if(Shijian_Set==2)
			{
				Shijian_Set=3;//秒
			}
			else if(Shijian_Set==3)
			{
				Shijian_Set=0;
			}

		}
		while(!P30);
	}
	else if(P31==0)//s6
	{
		Delay_ms(5);
		if(P31==0)
		{
			Any_Button_Down=1;
			if(Alarm_Set==0)
			{
				Alarm_Set=1;//小时
			}
			else if(Alarm_Set==1)
			{
				Alarm_Set=2;//分钟
			}
			else if(Alarm_Set==2)
			{
				Alarm_Set=3;//秒
			}
			else if(Alarm_Set==3)
			{
				Alarm_Set=0;
			}
			
		}
		while(!P31);
	}
	else if(P32==0)//s5
	{
		Delay_ms(5);
		if(P32==0)
		{
			Any_Button_Down=1;
			
			if(Display_Mode==0)
			{
				if(Shijian_Set==1)
				{
					if(shijian[2]>=24){shijian[2]=0;}
					shijian[2]++;
					DS1302_Init();
				}
				
				else if(Shijian_Set==2)
				{
					if(shijian[1]>=60){shijian[1]=0;}
					shijian[1]++;
					DS1302_Init();
				}
				
				else if(Shijian_Set==3)
				{
					if(shijian[0]>=60){shijian[0]=0;}
					shijian[0]++;
					DS1302_Init();
				}
				
				else if(Alarm_Set==1)
				{
					if(Alarm_Num[2]>=24){Alarm_Num[2]=0;}
					Alarm_Num[2]++;
				}
				else if(Alarm_Set==2)
				{
					if(Alarm_Num[1]>=60){Alarm_Num[1]=0;}
					Alarm_Num[1]++;
				}
				else if(Alarm_Set==3)
				{
					if(Alarm_Num[0]>=60){Alarm_Num[0]=0;}
					Alarm_Num[0]++;
				}
			}
		}
		while(!P32);
	}
	else if(P33==0)//s4
	{
		Delay_ms(5);
		if(P33==0)
		{
			s4=1;
			if (shijian==0)
			{
				Display_Mode=1;
			}
		}
	}	
	if ((s4==1)&&(P33=1))
	{
		s4=0;
		Any_Button_Down=1;
		if(Display_Mode==0)
		{
			if(Shijian_Set==1)
			{
				if(shijian[2]<=0){shijian[2]=24;}
				shijian[2]--;
				DS1302_Init();
			}
			
			else if(Shijian_Set==2)
			{
				if(shijian[1]<=0){shijian[1]=60;}
				shijian[1]--;
				DS1302_Init();
			}
			
			else if(Shijian_Set==3)
			{
				if(shijian[0]<=0){shijian[0]=60;}
				shijian[0]--;
				DS1302_Init();
			}
			
			else if(Alarm_Set==1)
			{
				if(Alarm_Num[2]<=0){Alarm_Num[2]=24;}
				Alarm_Num[2]--;
			}
			else if(Alarm_Set==2)
			{
				if(Alarm_Num[1]<=0){Alarm_Num[1]=60;}
				Alarm_Num[1]--;
			}
			else if(Alarm_Set==3)
			{
				if(Alarm_Num[0]<=0){Alarm_Num[0]=60;}
				Alarm_Num[0]--;
			}
		

			if(Display_Mode==0){Display_Mode=1;}
		}
		while(!P33)
		{
			Display_Mode=0;
		}

	}
}
void Digbuf_Display()
{
	if(Display_Mode==0)
	{
		Digbuf[0]=shijian[2]/10;
		Digbuf[1]=shijian[2]%10;
		Digbuf[2]=20;
		Digbuf[3]=shijian[1]/10;
		Digbuf[4]=shijian[1]%10;
		Digbuf[5]=20;
		Digbuf[6]=shijian[0]/10;
		Digbuf[7]=shijian[0]%10;
	}
	else if(Display_Mode==1)
	{
		Digbuf[0]=21;
		Digbuf[1]=21;
		Digbuf[2]=21;
		Digbuf[3]=21;
		Digbuf[4]=21;
		Digbuf[5]=Temperature/10;
		Digbuf[6]=Temperature%10;
		Digbuf[7]=22;
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
	P0=0xff;//位选
	
	P2|=0xe0;
	P2&=0xff;
	P0=0xff;//段选
	
	
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
	P0=0xff;
	
	P2|=0xc0;
	P2&=0xdf;
	P0=0x01<<Digcom;
	
	P2|=0xe0;
	P2&=0xff;
	P0=tab[Digbuf[Digcom]];
	
	if(++Digcom==8){Digcom=0;}
	
	LED_tt++;
	if (Alarm_ON==1)
	{
		if (LED_tt%200==0)
		{
			if (fla==0)
			{
				fla=1;
				P2=0x80;
				P0=0xfe;
			}
			else if (fla==1)
			{
				fla=0;
				P2=0x80;
				P0=0xff;
			}
			
		}
		if (LED_tt==5000)
		{
			LED_tt=0;
		}
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
