/*
串口波特率发生器——定时器1
超声波定时器——定时器0
*/

#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include "iic.h"
#include "onewire.h"
#include "STDIO.H"

#define uchar unsigned char
#define uint unsigned int

#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

sbit TX=P1^0;
sbit RX=P1^1;

uchar LED_Bits=0xff;
uchar code tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
				0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
														0xbf,0xff,
											0xc6,0xc7,0xc8,0x8c};//22--C,23--L,24--N,25--P
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

void allinit();
void Timer2Init(void);		//1??@12.000MHz
void Timer0Init(void);		//1毫秒@12.000MHz
void Delay_ms(uchar ms);		//@12.000MHz
void keyscan16_2();
//串口函数
void UartInit();	//4800bps@12.000MHz
void send_buffer(unsigned char *p,unsigned char Length);
//超声波函数
uint Get_Distance();
void send_wave();
//显示函数
void SHUJU_DISPLAY_Wendu();
void SHUJU_DISPLAY_Juli();
void SHUJU_DISPLAY_Biangencishu();
void CANSHU_DISPLAY_Wendu();
void CANSHU_DISPLAY_Juli();
void DISPLAY_ALL();

void CHUANKO_FUNTION();
void CHAOSHENBO_FUNTION();

void DAC_FUNCTION();

void LED_SHOW();

bit INTERFACE=0;					//0--数据界面，1--参数界面
uchar Xianshi_Shuju_Qiehuan=0;				//0--温度，1--距离，2--变更次数
uchar Canshu_Shuju_Qiehuan=0;				//0--温度参数，1--距离参数

float Temperature=0;
uint _Canshu_Biangencishu=0;
uint _Canshu_Wendu=30;
uint _Canshu_Juli=35;
uint _Canshu_Wendu_youxiao=30;
uint _Canshu_Juli_youxiao=35;

uint Distance=0;
bit bo_flag=0;

uchar _s12_longpress_flag=0;
uchar _s13_longpress_flag=0;
uint _long_KEY_count=0;

uchar Rec_buffer[]="nihao\r\n";
uchar Error_arr[]="ERROR\r\n";
uchar Juli_Wendu_arr[20];
uint 	Receive_flag=0;
bit 	Receive_over=0;
uchar 	Receive_num=0;

bit DAC_OUT_PUT=0;

void main()
{
	allinit();
	UartInit();
	Timer2Init();
	Timer0Init();
	_Canshu_Biangencishu=EEPROM_Read(0x08);Delay_ms(10);
	while(1)
	{
		Temperature=Temper_Get();
		// Digbuf[0]=Temperature/1000;
		// Digbuf[1]=Temperature%1000/100+10;
		// Digbuf[2]=Temperature%100/10;
		// Digbuf[3]=Temperature%10;

		// DA_Write(255);
		CHAOSHENBO_FUNTION();
		CHUANKO_FUNTION();
		DAC_FUNCTION();
		// Digbuf[0]=Distance/100;
		// Digbuf[1]=Distance%100/10;
		// Digbuf[2]=Distance%10;
		LED_SHOW();
		keyscan16_2();
		DISPLAY_ALL();
	}
}
void CHAOSHENBO_FUNTION()
{
	if (bo_flag==1)
	{
		bo_flag=0;
		Distance=Get_Distance();
	}
}

void CHUANKO_FUNTION()
{
	if (Receive_over==1)
	{
		Receive_over=0;
		Receive_num=0;
		if ((Rec_buffer[0]=='S')&(Rec_buffer[1]=='T')&(Rec_buffer[2]=='\r')&(Rec_buffer[3]=='\n'))
		{
			sprintf((char*)Juli_Wendu_arr,"$%-2d,%-.2f\r\n",Distance,Temperature/100);
			send_buffer(Juli_Wendu_arr,20);
		}
		else if ((Rec_buffer[0]=='P')&(Rec_buffer[1]=='A')&(Rec_buffer[2]=='R')&(Rec_buffer[3]=='A')&(Rec_buffer[4]=='\r')&(Rec_buffer[5]=='\n'))
		{
			sprintf((char*)Juli_Wendu_arr,"#%-2d,%-2d\r\n",_Canshu_Juli,_Canshu_Wendu);
			send_buffer(Juli_Wendu_arr,20);
		}
		else
		{
			send_buffer(Error_arr,9);
		}
	}
}

void DISPLAY_ALL()
{
	if (INTERFACE==0)
	{
		if (Xianshi_Shuju_Qiehuan==0)
		{
			SHUJU_DISPLAY_Wendu();
		}
		else if (Xianshi_Shuju_Qiehuan==1)
		{
			SHUJU_DISPLAY_Juli();
		}
		else if (Xianshi_Shuju_Qiehuan==2)
		{
			SHUJU_DISPLAY_Biangencishu();
		}
		
	}
	else if (INTERFACE==1)
	{
		if (Canshu_Shuju_Qiehuan==0)
		{
			CANSHU_DISPLAY_Wendu();
		}
		else if (Canshu_Shuju_Qiehuan==1)
		{
			CANSHU_DISPLAY_Juli();
		}
		
	}
	
}
void SHUJU_DISPLAY_Wendu()
{
	Digbuf[0]=22;
	Digbuf[1]=21;
	Digbuf[2]=21;
	Digbuf[3]=21;
	Digbuf[4]=(uint)(Temperature)/1000;
	Digbuf[5]=(uint)(Temperature)%1000/100+10;
	Digbuf[6]=(uint)(Temperature)%100/10;
	Digbuf[7]=(uint)(Temperature)%10/1;
}
void SHUJU_DISPLAY_Juli()
{
	Digbuf[0]=23;
	Digbuf[1]=21;
	Digbuf[2]=21;
	Digbuf[3]=21;
	Digbuf[4]=21;
	Digbuf[5]=21;
	Digbuf[6]=Distance/10;
	Digbuf[7]=Distance%10;
}
void SHUJU_DISPLAY_Biangencishu()
{
	Digbuf[0]=24;
	Digbuf[1]=21;
	Digbuf[2]=21;
	if (_Canshu_Biangencishu>9999)
	{
		Digbuf[3]=_Canshu_Biangencishu/10000;
		Digbuf[4]=_Canshu_Biangencishu%10000/1000;
		Digbuf[5]=_Canshu_Biangencishu%1000/100;
		Digbuf[6]=_Canshu_Biangencishu%100/10;
		Digbuf[7]=_Canshu_Biangencishu%10;
	}
	else if ((_Canshu_Biangencishu<9999)&&(_Canshu_Biangencishu>999))
	{
		Digbuf[3]=21;
		Digbuf[4]=_Canshu_Biangencishu/1000;
		Digbuf[5]=_Canshu_Biangencishu%1000/100;
		Digbuf[6]=_Canshu_Biangencishu%100/10;
		Digbuf[7]=_Canshu_Biangencishu%10;
	}
	else if ((_Canshu_Biangencishu<999)&&(_Canshu_Biangencishu>99))
	{
		Digbuf[3]=21;
		Digbuf[4]=21;
		Digbuf[5]=_Canshu_Biangencishu/100;
		Digbuf[6]=_Canshu_Biangencishu%100/10;
		Digbuf[7]=_Canshu_Biangencishu%10;
	}
	else if ((_Canshu_Biangencishu<99)&&(_Canshu_Biangencishu>9))
	{
		Digbuf[3]=21;
		Digbuf[4]=21;
		Digbuf[5]=21;
		Digbuf[6]=_Canshu_Biangencishu/10;
		Digbuf[7]=_Canshu_Biangencishu%10;
	}
	else if (_Canshu_Biangencishu<9)
	{
		Digbuf[3]=21;
		Digbuf[4]=21;
		Digbuf[5]=21;
		Digbuf[6]=21;
		Digbuf[7]=_Canshu_Biangencishu;
	}
	
}

void CANSHU_DISPLAY_Wendu()
{
	Digbuf[0]=25;
	Digbuf[1]=21;
	Digbuf[2]=21;
	Digbuf[3]=1;
	Digbuf[4]=21;
	Digbuf[5]=21;
	Digbuf[6]=_Canshu_Wendu/10;
	Digbuf[7]=_Canshu_Wendu%10;
}
void CANSHU_DISPLAY_Juli()
{
	Digbuf[0]=25;
	Digbuf[1]=21;
	Digbuf[2]=21;
	Digbuf[3]=2;
	Digbuf[4]=21;
	Digbuf[5]=21;
	Digbuf[6]=_Canshu_Juli/10;
	Digbuf[7]=_Canshu_Juli%10;
}

void UartInit()		//4800bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x8F;		//设置定时初始值
	TH1 = 0xFD;		//设置定时初始值
	ET1 = 0;		//禁止定时器%d中断
	TR1 = 1;		//定时器1开始计时
	ES=1;
	EA=1;
}
void ser() interrupt 4
{
	if (RI)
	{
		Rec_buffer[Receive_num]=SBUF;
		Receive_num++;
		Receive_flag=1;
		RI=0;
	}
	
}
void send_buffer(unsigned char *p,unsigned char Length)
{
	uchar j=0;

	for ( j = 0; j < Length; j++)
	{
		SBUF=*(p+j);
		while(TI==0);
		TI=0;
	}
}
void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0;		//设置定时初始值
	TH0 = 0;		//设置定时初始值
	TF0 = 0;		//清除TF1标志
	TR0 = 0;		//定时器1开始计时
}
void send_wave()
{
	uchar i=8;
	do
	{
		TX=1;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;
		TX=0;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;

	} while (i--);
}
uint Get_Distance()
{
	TH0=0;
	TL0=0;
	send_wave();
	TR0=1;

	while((RX==1)&&(TF0==0));
	TR0=0;

	if (TF0==1)
	{
		TF0=0;
		Distance=999;
	}
	if (RX==0)
	{
		RX=1;
		Distance=(((TH0<<8)|(TL0))*0.017);
	}
	
	return Distance;
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
	static uint bo_tt=0;

	P2|=0xe0;
	P2&=0xff;
	P0=0xff;//????
	
	P2|=0xc0;
	P2&=0xdf;
	P0=0x01<<Digcom;
	
	P2|=0xe0;
	P2&=0xff;
	P0=tab[Digbuf[Digcom]];
	
	_long_KEY_count++;
	if (_s12_longpress_flag==1)
	{
		if (_long_KEY_count==1000)
		{
			_s12_longpress_flag=2;
		}
		
	}
	else if (_s13_longpress_flag==1)
	{
		if (_long_KEY_count==1000)
		{
			_s13_longpress_flag=2;
		}
	}
	

	if(++Digcom==8)
	{
		Digcom=0;
	}
	
	if (++bo_tt==200)
	{
		bo_tt=0;
		bo_flag=1;
	}
	if (Receive_flag>=1)
	{
		Receive_flag++;
		if (Receive_flag>=30)
		{
			Receive_flag=0;
			Receive_over=1;
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
void keyscan16_2()
{
	uchar temp;
	
	P3=0xef;P44=1;P42=1;
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
				case(0xeb)://s17
					if (INTERFACE==1)
					{
						if (Canshu_Shuju_Qiehuan==0)
						{
							_Canshu_Wendu+=2;
							if (_Canshu_Wendu>=100)
							{
								_Canshu_Wendu=0;
							}
						}
						else if (Canshu_Shuju_Qiehuan==1)
						{
							_Canshu_Juli+=5;
							if (_Canshu_Juli>=100)
							{
								_Canshu_Juli=0;
							}
							
						}
						
					}
					
					break;
				case(0xe7)://s16
					if (INTERFACE==1)
					{
						if (Canshu_Shuju_Qiehuan==0)
						{							
							if (_Canshu_Wendu==0)
							{
								_Canshu_Wendu=101;
							}
							_Canshu_Wendu-=2;
							
						}
						else if (Canshu_Shuju_Qiehuan==1)
						{
							if (_Canshu_Juli==0)
							{
								_Canshu_Juli=104;
							}
							_Canshu_Juli-=5;
							
						}
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
	P35=0;P34=1;
	if (P33==0)//s12
	{
		Delay_ms(5);
		if (P33==0)
		{
			if (INTERFACE==0)
			{
				INTERFACE=1;//进入参数设置界面
			}
			else if (INTERFACE==1)
			{
				INTERFACE=0;//进入数据显示界面
				if (_Canshu_Wendu_youxiao!=_Canshu_Wendu)
				{
					_Canshu_Biangencishu++;
				}
				if (_Canshu_Juli_youxiao!=_Canshu_Juli)
				{
					_Canshu_Biangencishu++;
				}
				_Canshu_Wendu_youxiao=_Canshu_Wendu;
				_Canshu_Juli_youxiao=_Canshu_Juli;
				EEPROM_Write(0x08,_Canshu_Biangencishu);Delay_ms(10);
			}
			_s12_longpress_flag=1;
			_long_KEY_count=0;
		}
		while (!P33)
		{
			if(_s12_longpress_flag==2)
			{
				Delay_ms(10);
				_Canshu_Biangencishu=0;
				EEPROM_Write(0x08,0);Delay_ms(10);
			}
		}
		_s12_longpress_flag=0;
	}
	else if (P32==0)
	{
		Delay_ms(5);
		if (P32==0)
		{
			if (INTERFACE==0)
			{
				if (Xianshi_Shuju_Qiehuan==0)
				{
					Xianshi_Shuju_Qiehuan=1;
				}
				else if (Xianshi_Shuju_Qiehuan==1)
				{
					Xianshi_Shuju_Qiehuan=2;
				}
				else if (Xianshi_Shuju_Qiehuan==2)
				{
					Xianshi_Shuju_Qiehuan=0;
				}
				
			}
			else if (INTERFACE==1)
			{
				if (Canshu_Shuju_Qiehuan==0)
				{
					Canshu_Shuju_Qiehuan=1;
				}
				else if (Canshu_Shuju_Qiehuan==1)
				{
					Canshu_Shuju_Qiehuan=0;
				}
				
			}
			_s13_longpress_flag=1;
			_long_KEY_count=0;

		}
		while (!P32)
		{
			if (_s13_longpress_flag==2)
			{
				Delay_ms(10);
				if (DAC_OUT_PUT==0)
				{
					DAC_OUT_PUT=1;
				}
				else if (DAC_OUT_PUT==1)
				{
					DAC_OUT_PUT=0;
				}
				
			}
			
		}
		_s13_longpress_flag=0;
	}
	
}
void DAC_FUNCTION()
{
	if (DAC_OUT_PUT==0)
	{
		DA_Write(20.4);
	}
	else if (DAC_OUT_PUT==1)
	{
		if (Distance<=_Canshu_Juli_youxiao)
		{
			DA_Write(102);
		}
		else if (Distance>_Canshu_Juli_youxiao)
		{
			DA_Write(204);
		}
		
	}
	
}
void LED_SHOW()
{
	if ((uint)(Temperature)>_Canshu_Wendu_youxiao)
	{
		LED_Bits&=0xfe;
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
	if (Distance<_Canshu_Juli_youxiao)
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
	if (DAC_OUT_PUT==1)
	{
		LED_Bits&=~0x04;
		P2|=0x80;
		P2&=0x9f;
		P0=LED_Bits;
	}
	else if (DAC_OUT_PUT==0)
	{
		LED_Bits|=0x04;
		P2|=0x80;
		P2&=0x9f;
		P0=LED_Bits;
	}
	
}
//????LED1
//LED_Bits&=0xfe;
//	P2|=0x80;
//	P2&=0x9f;
//	P0=LED_Bits;
//?¨??LED1
//LED_Bits|=0x01;
//P2|=0x80;
//P2&=0x9f;
//P0=LED_Bits;
