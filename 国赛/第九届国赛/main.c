#include "STC15F2K60S2.H"
#include "INTRINS.H"
#include "iic.h"
#include "onewire.h"

#define uint    unsigned int
#define uchar   unsigned char

uchar LED_Bits=0xff;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
                0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
                                                        0xbf,0xff,
                                            0xc1,0x8e,0xc6,0x89,0x8c};//22--U,23--F,24--C,25--H,26--P
uchar Digbuf[]={21,21,21,21,21,21,21,21,};
uchar Digcom=0;



void allinit();
void Timer2Init(void);		//1??@12.000MHz
void Delay_ms(uchar ms);		//@12.000MHz
void keyscan();
void Timer0Init(void);		//1毫秒@11.0592MHz
void Timer1Init(void);		//1毫秒@12.000MHz


void SHUJU_DISPLAY_Dianya(void);
void SHUJU_DISPLAY_Pinlv(void);
void SHUJU_DISPLAY_Wendu(void);

void SHUJU_HUIXIAN_DISPLAY(void);
void DIANYA_YUZHI_SET_DISPLAY(void);

void DISPLAY_ALL();

void LED_SHOW(void);

uchar Xianshi_Status=0;//0--温度，1--电压，2--频率
uchar Xianshi_Huixian_Status=0;//0--温度，1--电压，2--频率

bit _If_Canshu_Set=0;
bit _If_Huixian_On=0;

uint Temperature=0;
uchar AD=0;
uint Pinlv=0;

uint Pinlv_Save=0;
uchar AD_Save=0;
uint Temperature_Save=0;

uchar Dianya_Yuzhi=0;

uchar _Longpress_Flag=0;
uint Key_Count=0;

bit fla=0;
uint Led_tt=0;

void main()
{
	allinit();
	Timer2Init();
	// AD_1=EEPROM_Read(0x01);
	Delay_ms(5);
	Timer1Init();
	Timer0Init();

	while(1)
	{
		AD=(uchar)(AD_Read(0x03)*1.96);
		
		// EEPROM_Write(0x01,AD);
		Temperature=Temper_Get();
		// Digbuf[0]=Temperature/1000;
		// Digbuf[1]=Temperature%1000/100+10;
		// Digbuf[2]=Temperature%100%10;
		// Digbuf[3]=Temperature%10;

		// Digbuf[5]=AD_1/100;
		// Digbuf[6]=AD_1%100/10;
		// Digbuf[7]=AD_1%10;
        keyscan();
		DISPLAY_ALL();
		LED_SHOW();
	}
}
void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x05;		//设置为计数模式
	TL0 = 0;		//设置定时初始值
	TH0 = 0;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}
void Timer1Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x18;		//设置定时初始值
	TH1 = 0xFC;		//设置定时初始值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1=1;
	EA=1;
}
void timer_1() interrupt 3
{
	static unsigned int pinlv_tt=0;
	if (++pinlv_tt==1000)
	{
		TR0=0;
		pinlv_tt=0;
		Pinlv=TH0;
		Pinlv=Pinlv<<8;
		Pinlv=Pinlv|TL0;
		TH0=0;
		TL0=0;
		TR0=1;
	}
	
}
void LED_SHOW(void)
{
	
	if (_If_Canshu_Set==0)
	{
		if (_If_Huixian_On==0)
		{
			if (Xianshi_Status==0)			//温度测量状态下
			{		
				LED_Bits&=~0x01;
				P2|=0x80;
				P2&=0x9f;
				P0=LED_Bits;
			}
			else
			{
				LED_Bits|=0x01;
				P2|=0x80;
				P2&=0x9f;
				P0=LED_Bits;
			}
			
			if (Xianshi_Status==2)		//频率测量状态下
			{
				LED_Bits&=~0x02;
				P2|=0x80;
				P2&=0x9f;
				P0=LED_Bits;
			}
			else
			{
				LED_Bits|=0x02;
				P2|=0x80;
				P2&=0x9f;
				P0=LED_Bits;
			}
			
			if (Xianshi_Status==1)		//电压测量状态下
			{
				LED_Bits&=~0x04;
				P2|=0x80;
				P2&=0x9f;
				P0=LED_Bits;
			}
			else
			{
				LED_Bits|=0x04;
				P2|=0x80;
				P2&=0x9f;
				P0=LED_Bits;
			}
			
		}
		else
		{
			LED_Bits|=0x07;
			P2|=0x80;
			P2&=0x9f;
			P0=LED_Bits;
		}
		
	}
	else
	{
		LED_Bits|=0x07;
		P2|=0x80;
		P2&=0x9f;
		P0=LED_Bits;
	}
	
}
void DISPLAY_ALL()
{
	if (_If_Canshu_Set==0)
	{
		if (_If_Huixian_On==0)
		{
			if (Xianshi_Status==0)
			{
				SHUJU_DISPLAY_Wendu();
			}
			else if (Xianshi_Status==1)
			{
				SHUJU_DISPLAY_Dianya();
			}
			else if (Xianshi_Status==2)
			{
				SHUJU_DISPLAY_Pinlv();
			}
		}
		else if (_If_Huixian_On==1)
		{
			
			SHUJU_HUIXIAN_DISPLAY();
		
		}
		
	}
	else if (_If_Canshu_Set==1)
	{
		DIANYA_YUZHI_SET_DISPLAY();
	}
	
	
}

void SHUJU_DISPLAY_Dianya(void)
{
	Digbuf[0]=22;
	Digbuf[1]=21;
	Digbuf[2]=21;
	Digbuf[3]=21;
	Digbuf[4]=21;
	Digbuf[5]=21;
	Digbuf[6]=AD/100+10;
	Digbuf[7]=AD%100/10;
}
void SHUJU_DISPLAY_Pinlv(void)
{
	Digbuf[0]=23;
	Digbuf[1]=21;
	if (Pinlv>99999)
	{
		Digbuf[2]=Pinlv/100000;
		Digbuf[3]=Pinlv%100000/10000;
		Digbuf[4]=Pinlv%10000/1000;
		Digbuf[5]=Pinlv%1000/100;
		Digbuf[6]=Pinlv%100/10;
		Digbuf[7]=Pinlv%10/1;
	}
	else if ((Pinlv<99999)&&(Pinlv>9999))
	{
		Digbuf[2]=21;
		Digbuf[3]=Pinlv/10000;
		Digbuf[4]=Pinlv%10000/1000;
		Digbuf[5]=Pinlv%1000/100;
		Digbuf[6]=Pinlv%100/10;
		Digbuf[7]=Pinlv%10/1;
	}
	else if ((Pinlv<9999)&&(Pinlv>999))
	{
		Digbuf[2]=21;
		Digbuf[3]=21;
		Digbuf[4]=Pinlv/1000;
		Digbuf[5]=Pinlv%1000/100;
		Digbuf[6]=Pinlv%100/10;
		Digbuf[7]=Pinlv%10/1;
	}
	else if ((Pinlv<999)&&(Pinlv>99))
	{
		Digbuf[2]=21;
		Digbuf[3]=21;
		Digbuf[4]=21;
		Digbuf[5]=Pinlv/100;
		Digbuf[6]=Pinlv%100/10;
		Digbuf[7]=Pinlv%10/1;
	}
	else if ((Pinlv<99)&&(Pinlv>9))
	{
		Digbuf[2]=21;
		Digbuf[3]=21;
		Digbuf[4]=21;
		Digbuf[5]=21;
		Digbuf[6]=Pinlv/10;
		Digbuf[7]=Pinlv%10;
	}
	else if (Pinlv<9)
	{
		Digbuf[2]=21;
		Digbuf[3]=21;
		Digbuf[4]=21;
		Digbuf[5]=21;
		Digbuf[6]=21;
		Digbuf[7]=Pinlv;
	}
	
}
void SHUJU_DISPLAY_Wendu(void)
{
	Digbuf[0]=24;
	Digbuf[1]=21;
	Digbuf[2]=21;
	Digbuf[3]=21;
	Digbuf[4]=Temperature/1000;
	Digbuf[5]=Temperature%1000/100+10;
	Digbuf[6]=Temperature%100/10;
	Digbuf[7]=Temperature%10;


}
void SHUJU_HUIXIAN_DISPLAY(void)
{
	Digbuf[0]=25;
	//chose//
	if (Xianshi_Huixian_Status==0)
	{
		Digbuf[1]=24;		//回显数据选择
		Digbuf[2]=21;
		Digbuf[3]=21;
		Digbuf[4]=Temperature_Save/1000;
		Digbuf[5]=Temperature_Save%1000/100+10;
		Digbuf[6]=Temperature_Save%100/10;
		Digbuf[7]=Temperature_Save%10;
	}
	
	//chose//
	else if (Xianshi_Huixian_Status==1)
	{
		Digbuf[1]=22;		//回显数据选择
		Digbuf[2]=21;
		Digbuf[3]=21;
		Digbuf[4]=21;
		Digbuf[5]=21;
		Digbuf[6]=AD_Save/10;
		Digbuf[7]=AD_Save%10;
	}
	
	//chose//
	else if (Xianshi_Huixian_Status==2)
	{
		Digbuf[1]=23;		//回显数据选择
		if (Pinlv_Save>99999)
		{
			Digbuf[2]=Pinlv_Save/100000;
			Digbuf[3]=Pinlv_Save%100000/10000;
			Digbuf[4]=Pinlv_Save%10000/1000;
			Digbuf[5]=Pinlv_Save%1000/100;
			Digbuf[6]=Pinlv_Save%100/10;
			Digbuf[7]=Pinlv_Save%10/1;
		}
		else if ((Pinlv_Save<99999)&&(Pinlv_Save>9999))
		{
			Digbuf[2]=21;
			Digbuf[3]=Pinlv_Save/10000;
			Digbuf[4]=Pinlv_Save%10000/1000;
			Digbuf[5]=Pinlv_Save%1000/100;
			Digbuf[6]=Pinlv_Save%100/10;
			Digbuf[7]=Pinlv_Save%10/1;
		}
		else if ((Pinlv_Save<9999)&&(Pinlv_Save>999))
		{
			Digbuf[2]=21;
			Digbuf[3]=21;
			Digbuf[4]=Pinlv_Save/1000;
			Digbuf[5]=Pinlv_Save%1000/100;
			Digbuf[6]=Pinlv_Save%100/10;
			Digbuf[7]=Pinlv_Save%10/1;
		}
		else if ((Pinlv_Save<999)&&(Pinlv_Save>99))
		{
			Digbuf[2]=21;
			Digbuf[3]=21;
			Digbuf[4]=21;
			Digbuf[5]=Pinlv_Save/100;
			Digbuf[6]=Pinlv_Save%100/10;
			Digbuf[7]=Pinlv_Save%10/1;
		}
		else if ((Pinlv_Save<99)&&(Pinlv_Save>9))
		{
			Digbuf[2]=21;
			Digbuf[3]=21;
			Digbuf[4]=21;
			Digbuf[5]=21;
			Digbuf[6]=Pinlv_Save/10;
			Digbuf[7]=Pinlv_Save%10;
		}
		else if (Pinlv_Save<9)
		{
			Digbuf[2]=21;
			Digbuf[3]=21;
			Digbuf[4]=21;
			Digbuf[5]=21;
			Digbuf[6]=21;
			Digbuf[7]=Pinlv_Save;
		}
	}
	


}
void DIANYA_YUZHI_SET_DISPLAY(void)
{
	Digbuf[0]=26;
	Digbuf[1]=21;
	Digbuf[2]=21;
	Digbuf[3]=21;
	Digbuf[4]=21;
	Digbuf[5]=21;
	Digbuf[6]=Dianya_Yuzhi/10+10;
	Digbuf[7]=Dianya_Yuzhi%10;
}

void allinit()
{
	P2|=0xa0;
	P2&=0xbf;
	P0=0x00;//·??ù?÷?????÷
	
	P2|=0x80;
	P2&=0x9f;
	P0=0xff;//LED
	
	P2|=0xc0;
	P2&=0xdf;
	P0=0xff;//????
	
	P2|=0xe0;
	P2&=0xff;
	P0=0xff;//????
	
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
	P0=0xff;//????
	
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
	
	Key_Count++;
	if (_Longpress_Flag==1)
	{
		if (Key_Count==800)
		{
			_Longpress_Flag=2;
		}
		
	}

	
	if (AD>Dianya_Yuzhi*10)
	{
	Led_tt++;

		if (Led_tt==200)
		{
			if (fla==0)
			{
				fla=1;
			
				LED_Bits&=~0x80;
				P2|=0x80;
				P2&=0x9f;
				P0=LED_Bits;

				
			}
			else if (fla==1)
			{
				fla=0;
				
				LED_Bits|=0x80;
				P2|=0x80;
				P2&=0x9f;
				P0=LED_Bits;

				
			}
			Led_tt=0;
		}
		
	}
	else
	{
		LED_Bits|=0x80;
		P2|=0x80;
		P2&=0x9f;
		P0=LED_Bits;
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
void keyscan()
{
	if(P30==0)//s7
	{
		Delay_ms(5);
		if(P30==0)
		{
			if (_If_Canshu_Set==0)
			{
				_If_Canshu_Set=1;
			}
			else if (_If_Canshu_Set==1)
			{
				_If_Canshu_Set=0;
			}
			
		}
		while(!P30);
	}
	else if(P31==0)//s6
	{
		Delay_ms(5);
		if(P31==0)
		{
			if (_If_Canshu_Set==0)//在非参数设置状态下
			{
				if (_If_Huixian_On==0)
				{
					_If_Huixian_On=1;
				}
				else if (_If_Huixian_On==1)
				{
					_If_Huixian_On=0;
				}
				
			}
			else if (_If_Canshu_Set==1)//在参数设置状态下
			{
				
				Dianya_Yuzhi++;
				if (Dianya_Yuzhi>=51)
				{
					Dianya_Yuzhi=1;
				}
				_Longpress_Flag=1;
				Key_Count=0;
			}
			
		}
		while(!P31)
		{
			if (_If_Canshu_Set==1)
			{
				if (_Longpress_Flag==2)
				{
					Delay_ms(60000);
					Dianya_Yuzhi++;
					if (Dianya_Yuzhi>=51)
					{
						Dianya_Yuzhi=1;
					}
				}
				DIANYA_YUZHI_SET_DISPLAY();
			}
			
		}
		_Longpress_Flag=0;
	}
	else if(P32==0)//s5
	{
		Delay_ms(5);
		if(P32==0)
		{
			EEPROM_Write(0x01,AD);
			EEPROM_Write(0x02,Pinlv);
			EEPROM_Write(0x03,Temperature);
		}
		while(!P32);
	}
	else if(P33==0)//s4
	{
		Delay_ms(5);
		if(P33==0)
		{
			if (_If_Canshu_Set==0)
			{
				if (_If_Huixian_On==0)
				{
					if (Xianshi_Status==0)
					{
						Xianshi_Status=1;
					}
					else if (Xianshi_Status==1)
					{
						Xianshi_Status=2;
					}
					else if (Xianshi_Status==2)
					{
						Xianshi_Status=0;
					}
				}
				else if (_If_Huixian_On==1)
				{
					if (Xianshi_Huixian_Status==0)
					{
						Xianshi_Huixian_Status=1;
					}
					else if (Xianshi_Huixian_Status==1)
					{
						Xianshi_Huixian_Status=2;
					}
					else if (Xianshi_Huixian_Status==2)
					{
						Xianshi_Huixian_Status=0;
					}
				}
			}
			
			
			
		}
		while(!P33);
	}
}
