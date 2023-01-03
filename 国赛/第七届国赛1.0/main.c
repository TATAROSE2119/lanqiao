#include <stc15f2k60s2.h>
#include <intrins.h>
#include "ds1302.h"
#include "iic.h"

#define uchar unsigned char
#define uint unsigned int
	
extern uchar shijian[];
uchar shijian_change[7];

uchar code tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
					0x40,0x19,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
																0xbf,0xff};
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

uint AD=0;
uint Vol=0;
uint vol_now=0;
uint vol_last=0;

void allinit();
void Timer2Init(void);		//1??@11.0592MHz
void keyscan();
void Delay_ms(uint ms);		//@11.0592MHz
void DISPLAY_ALL();
void READ_DAT();
int v_l=0;					//可视改变值
int v_h=0;					//可视改变值
int v_l_used=0;				//实际生效值
int v_h_used=0;				//实际生效值
uchar interface=0;			//0--时间界面关，1--时间界面开,2--时钟设置界面,3--电压测量功能，4--频率测量,5--查询按键
uchar set_h_m_s=0;			//0--h,1--m,2--s;
uchar set_VL_VH=0;			//0--l.1--h;
uchar fre_perio=0;			//0--fre,1--perio
uchar chaxun_shunxu=0;
uchar happen_00_time[3];
uchar happen_01_time[3];
//频率//
void Timer0Init(void);		//1毫秒@11.0592MHz
void fre_get();
uint fre=0;
float fre1=0;
uint perio=0;
bit fre_get_on=0;
bit fre_tt_flag=0;
uint fre_get_tt=0;
void main()
{
	allinit();
	Timer2Init();
	Timer0Init();
	ds1302_init();
	READ_DAT();
	while(1)
	{
		AD=AD_Read(0x03);
		vol_last=vol_now;
		vol_now=AD*196/10;
		keyscan();
		ds1302_get();
		fre_get();
		DISPLAY_ALL();
	}
}
void fre_get()
{
	if ((fre_get_on==1)&&(fre_tt_flag==1))
	{
		fre_tt_flag=0;
		fre_get_tt=0;
		fre=(TH0<<8)|(TL0);
		fre1=fre;
		fre1=(float)(1000/fre1);
		fre1=fre1*1000;
		perio=(uint)fre1;

		TH0=0;
		TL0=0;
		TR0=1;
	}
}
void DISPLAY_ALL()
{
	if (interface==1)
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
	else if (interface==2)
	{
		if (set_h_m_s==0)//小时闪烁
		{

			if (shijian[0]%2==0)
			{
				Digbuf[0]=shijian_change[2]/10;
				Digbuf[1]=shijian_change[2]%10;
			}
			else
			{
				Digbuf[0]=21;
				Digbuf[1]=21;
			}
			Digbuf[2]=20;
			Digbuf[3]=shijian_change[1]/10;
			Digbuf[4]=shijian_change[1]%10;
			Digbuf[5]=20;
			Digbuf[6]=shijian_change[0]/10;
			Digbuf[7]=shijian_change[0]%10;
		}
		else if (set_h_m_s==1)
		{
			if (shijian[0]%2==0)
			{
				Digbuf[3]=shijian_change[1]/10;
				Digbuf[4]=shijian_change[1]%10;
			}
			else
			{
				Digbuf[3]=21;
				Digbuf[4]=21;
			}					
			Digbuf[0]=shijian_change[2]/10;
			Digbuf[1]=shijian_change[2]%10;
			Digbuf[2]=20;
			Digbuf[5]=20;
			Digbuf[6]=shijian_change[0]/10;
			Digbuf[7]=shijian_change[0]%10;
		}
		else if (set_h_m_s==2)
		{
			if (shijian[0]%2==0)
			{
				Digbuf[6]=shijian_change[0]/10;
				Digbuf[7]=shijian_change[0]%10;
			}
			else
			{
				Digbuf[6]=21;
				Digbuf[7]=21;
			}
			Digbuf[0]=shijian_change[2]/10;
			Digbuf[1]=shijian_change[2]%10;
			Digbuf[2]=20;
			Digbuf[3]=shijian_change[1]/10;
			Digbuf[4]=shijian_change[1]%10;
			Digbuf[5]=20;
		}
		
	}
	else if (interface==3)
	{
		if (set_VL_VH==2)
		{
			Vol=AD*196/10;
			Digbuf[0]=20;
			Digbuf[1]=1;			
			Digbuf[2]=20;			
			Digbuf[3]=21;			
			Digbuf[4]=Vol/1000;			
			Digbuf[5]=Vol%1000/100;			
			Digbuf[6]=Vol%100/10;			
			Digbuf[7]=Vol%10;			
		}
		else if (set_VL_VH==0)
		{
			if (shijian[0]%2==0)
			{
				Digbuf[0]=v_h/1000;
				Digbuf[1]=v_h%1000/100;
				Digbuf[2]=v_h%100/10;
				Digbuf[3]=v_h%10;
			}
			else
			{
				Digbuf[0]=21;
				Digbuf[1]=21;
				Digbuf[2]=21;
				Digbuf[3]=21;
			}
			
			Digbuf[4]=v_l/1000;
			Digbuf[5]=v_l%1000/100;
			Digbuf[6]=v_l%100/10;
			Digbuf[7]=v_l%10;
		}
		else if (set_VL_VH==1)
		{
			if (shijian[0]%2==0)
			{
				Digbuf[4]=v_l/1000;
				Digbuf[5]=v_l%1000/100;
				Digbuf[6]=v_l%100/10;
				Digbuf[7]=v_l%10;
			}
			else
			{
				Digbuf[4]=21;
				Digbuf[5]=21;
				Digbuf[6]=21;
				Digbuf[7]=21;
			}
			
			Digbuf[0]=v_h/1000;
			Digbuf[1]=v_h%1000/100;
			Digbuf[2]=v_h%100/10;
			Digbuf[3]=v_h%10;
		}
		
	}
	else if (interface==4)
	{
		if (fre_perio==0)
		{
			Digbuf[0]=20;
			Digbuf[1]=2;
			Digbuf[2]=20;
			Digbuf[3]=fre/10000;
			Digbuf[4]=fre%10000/1000;
			Digbuf[5]=fre%1000/100;
			Digbuf[6]=fre%100/10;
			Digbuf[7]=fre%10;
		}
		else if (fre_perio==1)
		{
			Digbuf[0]=20;
			Digbuf[1]=2;
			Digbuf[2]=20;
			Digbuf[3]=perio/10000;
			Digbuf[4]=perio%10000/1000;
			Digbuf[5]=perio%1000/100;
			Digbuf[6]=perio%100/10;
			Digbuf[7]=perio%10;
		}
		
	}
	else if (interface==5)
	{
		if (chaxun_shunxu==0)//事件类型00
		{
			Digbuf[0]=21;
			Digbuf[1]=21;
			Digbuf[2]=21;
			Digbuf[3]=21;
			Digbuf[4]=21;
			Digbuf[5]=21;
			Digbuf[6]=0;
			Digbuf[7]=0;
		}
		else if (chaxun_shunxu==1)//00的发生时间
		{
			Digbuf[0]=happen_00_time[2]/10;
			Digbuf[1]=happen_00_time[2]%10;
			Digbuf[2]=20;
			Digbuf[3]=happen_00_time[1]/10;
			Digbuf[4]=happen_00_time[1]%10;
			Digbuf[5]=20;
			Digbuf[6]=happen_00_time[0]/10;
			Digbuf[7]=happen_00_time[0]%10;
		}
		else if (chaxun_shunxu==2)//事件类型01
		{
			Digbuf[0]=21;
			Digbuf[1]=21;
			Digbuf[2]=21;
			Digbuf[3]=21;
			Digbuf[4]=21;
			Digbuf[5]=21;
			Digbuf[6]=0;
			Digbuf[7]=1;
		}
		else if (chaxun_shunxu==3)//01的发生时间
		{
			Digbuf[0]=happen_01_time[2]/10;
			Digbuf[1]=happen_01_time[2]%10;
			Digbuf[2]=20;
			Digbuf[3]=happen_01_time[1]/10;
			Digbuf[4]=happen_01_time[1]%10;
			Digbuf[5]=20;
			Digbuf[6]=happen_01_time[0]/10;
			Digbuf[7]=happen_01_time[0]%10;
		}
		
	}
	
}

void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD|=0x04;
	TL0 = 0;		//设置定时初始值
	TH0 = 0;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void keyscan()
{
	uchar temp;
	P44=1;P42=0;P3=0x7f;
	temp=P3&0x0f;
	if(temp!=0x0f)
	{
		Delay_ms(5);
		temp=P3&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch (temp)
			{
			case 0x7e://s11//++
				if (interface==2)
				{
					if (set_h_m_s==0)
					{
						shijian_change[2]++;
						if (shijian_change[2]>23)
						{
							shijian_change[2]=0;
						}
						
					}
					else if (set_h_m_s==1)
					{
						shijian_change[1]++;
						if (shijian_change[1]>59)
						{
							shijian_change[1]=0;
						}
					}
					else if (set_h_m_s==2)
					{
						shijian_change[0]++;
						if (shijian_change[0]>59)
						{
							shijian_change[0]=0;
						}
					}
					
				}
				else if (interface==3)
				{
					if (set_VL_VH==0)//VH
					{
						v_h+=500;
						if (v_h>=10000)
						{
							v_h=0;
						}
						
					}
					else if (set_VL_VH==1)//VL
					{
						v_l+=500;
						if (v_l>=10000)
						{
							v_l=0;
						}
						
					}
					
				}
				
				shijian[0]=shijian_change[0];shijian[1]=shijian_change[1];shijian[2]=shijian_change[2];
				ds1302_init();
				
				break;
			case 0x7d://s10//--
				if (interface==2)
				{
					if (set_h_m_s==0)
					{
						if (shijian_change[2]==0)
						{
							shijian_change[2]=24;
						}
						shijian_change[2]--;
					}
					else if (set_h_m_s==1)
					{
						if (shijian_change[1]==0)
						{
							shijian_change[1]=60;
						}
						shijian_change[1]--;
					}
					else if (set_h_m_s==2)
					{
						if (shijian_change[0]==0)
						{
							shijian_change[0]=60;
						}
						shijian_change[0]--;
					}
				}
				else if (interface==3)
				{
					if (set_VL_VH==0)//VH
					{
						v_h-=500;
						if (v_h<0)
						{
							v_h=9500;
						}
					}
					else if (set_VL_VH==1)//VL
					{
						v_l-=500;
						if (v_l<0)
						{
							v_l=9500;
						}
						
					}
					
				}
				shijian[0]=shijian_change[0];shijian[1]=shijian_change[1];shijian[2]=shijian_change[2];
				ds1302_init();
				break;
			case 0x7b://s9
				if (interface==1)
				{
					interface=5;
					if ((vol_now==v_h_used)&&(vol_last<vol_now))
					{
						happen_01_time[0]=shijian[0];happen_01_time[1]=shijian[1];happen_01_time[2]=shijian[2];
					}
					else if ((vol_now==v_l_used)&&(vol_last>vol_now))
					{
						happen_00_time[0]=shijian[0];happen_00_time[1]=shijian[1];happen_00_time[2]=shijian[2];
					}
					
					Delay_ms(10);
					EEPROM_Write(3,happen_00_time[0]);
					Delay_ms(10);
					EEPROM_Write(4,happen_00_time[1]);
					Delay_ms(10);
					EEPROM_Write(5,happen_00_time[2]);
					Delay_ms(10);
					EEPROM_Write(6,happen_01_time[0]);
					Delay_ms(10);
					EEPROM_Write(7,happen_01_time[1]);
					Delay_ms(10);
					EEPROM_Write(8,happen_01_time[2]);
					Delay_ms(10);
				}
				
				break;
			
			default:
				break;
			}

		}
		while (temp!=0x0f)
		{
			temp=P3&0x0f;
		}
		
	}
	P44=0;P42=1;P3=0xbf;
	temp=P3&0x0f;
	if(temp!=0x0f)
	{
		Delay_ms(5);
		temp=P3&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch (temp)
			{
			case 0xbe://s7//时间显示界面
				if (interface==0)
				{
					interface=1;
				}
				else if (interface==2)
				{
					interface=1;
				}
				else if (interface==3)
				{
					interface=1;
				}
				else if (interface==4)
				{
					interface=1;
					fre_get_on=0;
				}
				else if (interface==5)
				{
					interface=1;
				}
				
				
				break;
			case 0xbd://s6//启动电压测量功能
				if (interface==1)
				{
					interface=3;
					set_VL_VH=2;
				}
				else if (interface==3)
				{
					if (set_VL_VH!=2)
					{
						set_VL_VH=2;
						v_l_used=v_l;v_h_used=v_h;			//结束设置时将 可视改变值 赋值给 实际生效值
						Delay_ms(10);EEPROM_Write(1,(((uchar)(v_l_used))/100));Delay_ms(10);
						Delay_ms(10);EEPROM_Write(2,(((uchar)(v_h_used))/100));Delay_ms(10);
					}
					
				}
				
				
				break;
			case 0xbb://s5//频率测量开启
				if (interface==1)
				{
					interface=4;
					fre_get_on=1;
				}
								
				break;
			case 0xb7://s4//时钟设置界面
				if (interface==1)
				{
					interface=2;
					shijian_change[0]=shijian[0];shijian_change[1]=shijian[1];shijian_change[2]=shijian[2];

				}
				else if (interface==2)
				{
					if (set_h_m_s==0)
					{
						set_h_m_s=1;
					}
					else if (set_h_m_s==1)
					{
						set_h_m_s=2;
					}
					else if (set_h_m_s==2)
					{
						set_h_m_s=0;
					}
				}
				else if (interface==3)
				{
					if (set_VL_VH==2)
					{
						set_VL_VH=0;
						v_h=v_h_used;v_l=v_l_used;
					}
					else if (set_VL_VH==0)
					{
						set_VL_VH=1;
					}
					else if (set_VL_VH==1)
					{
						set_VL_VH=0;
					}
					
				}
				else if (interface==4)
				{
					if (fre_perio==0)
					{
						fre_perio=1;
					}
					else if (fre_perio==1)
					{
						fre_perio=0;
					}
				}
				else if (interface==5)
				{
					if (chaxun_shunxu==0)
					{
						chaxun_shunxu=1;
					}
					else if (chaxun_shunxu==1)
					{
						chaxun_shunxu=2;
					}
					else if (chaxun_shunxu==2)
					{
						chaxun_shunxu=3;
					}
					else if (chaxun_shunxu==3)
					{
						chaxun_shunxu=0;
					}
					
				}
				
				break;
			default:
				break;
			}
		}
		while (temp!=0x0f)
		{
			temp=P3&0x0f;
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
	if(++fre_get_tt==800){fre_tt_flag=1;TR0=0;}
}
void Delay_ms(uint ms)		//@11.0592MHz
{
	unsigned char i, j,k;
	for ( k = 0; k < ms; k++)
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
void READ_DAT()
{
	Delay_ms(10);
	v_l_used=EEPROM_Read(1);Delay_ms(10);
	v_h_used=EEPROM_Read(2);Delay_ms(10);
	happen_00_time[0]=EEPROM_Read(3);Delay_ms(10);
	happen_00_time[1]=EEPROM_Read(4);Delay_ms(10);
	happen_00_time[2]=EEPROM_Read(5);Delay_ms(10);
	happen_01_time[0]=EEPROM_Read(6);Delay_ms(10);
	happen_01_time[1]=EEPROM_Read(7);Delay_ms(10);
	happen_01_time[2]=EEPROM_Read(8);Delay_ms(10);
}
