#include <STC15F2K60S2.H>
#include "intrins.h"
#include "stdio.h"
#include "onewire.h"
#include "iic.h"

#define uchar unsigned  char
#define uint unsigned  int

#define Somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

sbit TX=P1^0;
sbit RX=P1^1;

uchar LED_Bits=0xff;
uchar code tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
					0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
													0xbf,0xff,0xc6,0xc7,0xc8,0x8c};//C--22,L--23,N--24,P--25
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

void allinit();
void Timer2Init(void);		//1毫秒@11.0592MHz
void Delay_ms(uint ms);		//@12.000MHz
void keyscan();
void LED_on(uchar LED_num);
void LED_off(uchar LED_num);

uint temperature=0;
uchar s12_short_or_long=2;//0--short,1--long
uchar s13_short_or_long=2;//0--short,1--long
bit s13_flag=0;
bit s12_flag=0;
uint s12_tt=0;
uint s13_tt=0;
uint bian_num=0;				//变更次数//
bit interface=0;			//界面切换
uchar shuju_type=0;			//数据类型
uchar canshu_type=0;		//参数类型
int temper_canshu=30;		//温度参数
int dis_canshu=35;			//距离参数
void DAC();
bit DAC_on=0;
uint temper_tt=0;
void LED_Display();

//串口//
void UartInit(void);		//4800bps@11.0592MHz
void Uart_Function();
void Send_Buffer(uchar *p,uchar length);
uchar Receive_flag=0;
bit Receive_over=0;
uchar Receive_num=0;
uchar Rec_buffer[]="123\r\n";
uchar send_arr[10];


//超声波//
void Bo_Function();
void Timer0Init(void);		//1毫秒@11.0592MHz
void Send_Wave();
uint Get_Dis();
uint dis=0;
uint distance=0;
bit bo_flag=0;

void main()
{
	allinit();
	Timer2Init();
	Timer0Init();
	UartInit();
	bian_num=EEPROM_Read(0x01);
	while(1)
	{
		Bo_Function();
		Uart_Function();
		DAC();
		keyscan();
		LED_Display();
		if(interface==0)//数据界面//
		{
			//温度界面//
			if(shuju_type==0)
			{
				Digbuf[0]=22;
				Digbuf[1]=21;
				Digbuf[2]=21;
				Digbuf[3]=21;
				Digbuf[4]=temperature/1000;
				Digbuf[5]=temperature%1000/100+10;
				Digbuf[6]=temperature%100/10;
				Digbuf[7]=temperature%10;
			}
			//距离界面//
			else if(shuju_type==1)
			{
				Digbuf[0]=23;
				Digbuf[1]=21;
				Digbuf[2]=21;
				Digbuf[3]=21;
				Digbuf[4]=21;
				Digbuf[5]=21;
				Digbuf[6]=dis/10;
				Digbuf[7]=dis%10;
			}
			//变更次数界面//
			else if(shuju_type==2)
			{
				Digbuf[0]=24;
				Digbuf[1]=21;
				Digbuf[2]=21;
				if(bian_num>9999)
				{
					Digbuf[3]=bian_num/10000;
					Digbuf[4]=bian_num%10000/1000;
					Digbuf[5]=bian_num%1000/100;
					Digbuf[6]=bian_num%100/10;
					Digbuf[7]=bian_num%10/1;
				}
				else if((bian_num>999)&&(bian_num<=9999))
				{
					Digbuf[3]=21;
					Digbuf[4]=bian_num/1000;
					Digbuf[5]=bian_num%1000/100;
					Digbuf[6]=bian_num%100/10;
					Digbuf[7]=bian_num%10/1;
				}
				else if((bian_num>99)&&(bian_num<=999))
				{
					Digbuf[3]=21;
					Digbuf[4]=21;
					Digbuf[5]=bian_num/100;
					Digbuf[6]=bian_num%100/10;
					Digbuf[7]=bian_num%10/1;
				}
				else if((bian_num>9)&&(bian_num<=99))
				{
					Digbuf[3]=21;
					Digbuf[4]=21;
					Digbuf[5]=21;
					Digbuf[6]=bian_num/10;
					Digbuf[7]=bian_num%10;
				}
				else if(bian_num<=9)
				{
					Digbuf[3]=21;
					Digbuf[4]=21;
					Digbuf[5]=21;
					Digbuf[6]=21;
					Digbuf[7]=bian_num;
				}
			}
			
		}
		else if(interface==1)//参数界面
		{
			if(canshu_type==0)
			{
				Digbuf[0]=25;
				Digbuf[1]=21;
				Digbuf[2]=21;
				Digbuf[3]=1;
				Digbuf[4]=21;
				Digbuf[5]=21;
				Digbuf[6]=temper_canshu/10;
				Digbuf[7]=temper_canshu%10;
			}
			else if(canshu_type==1)
			{
				Digbuf[0]=25;
				Digbuf[1]=21;
				Digbuf[2]=21;
				Digbuf[3]=2;
				Digbuf[4]=21;
				Digbuf[5]=21;
				Digbuf[6]=dis_canshu/10;
				Digbuf[7]=dis_canshu%10;
			}
		}
	}
}
void keyscan()
{
	uchar temp;
	P3=0xef;P44=1;P42=1;//s16,s17
	temp=P3&0x0f;
	if(temp!=0x0f)
	{
		Delay_ms(5);
		temp=P3&0x0f;
		if(temp!=0x0f)
		{
			
			if(temp==0x0b)//s17short
			{
				if(interface==1)
				{
					if(canshu_type==0)
					{
						temper_canshu+=2;
						if(temper_canshu>99)
						{
							temper_canshu=0;
						}
					}
					else if(canshu_type==1)
					{
						dis_canshu+=5;
						if(dis_canshu>99)
						{
							dis_canshu=0;
						}
					}
				}
			}
			else if(temp==0x07)//s16short
			{
				if(interface==1)
				{
					if(canshu_type==0)
					{
						temper_canshu-=2;
						if(temper_canshu<0)
						{
							temper_canshu+=99;
						}
						
					}
					else if(canshu_type==1)
					{
						dis_canshu-=5;
						if(dis_canshu<0)
						{
							dis_canshu=99;
						}
					}
				}
			}
			if(interface==1)
			{
				bian_num++;
				Delay_ms(10);
				EEPROM_Write(0x01,bian_num);
				Delay_ms(10);
			}
		}
		while(temp!=0x0f)
		{
			temp=P3&0x0f;
		}
	}
	P3=0xdf;P44=1;P42=1;
	if(P33==0)//s12
	{
		Delay_ms(2);
		if(P33==0)
		{
			s12_flag=1;
		}
	}
	if(P33==1)
	{
		Delay_ms(2);
		if(P33==1)
		{
			s12_flag=0;
			if(s12_tt>=10)
			{
				
				if(s12_tt>=1000)
				{
					s12_short_or_long=1;
				}
				else if(s12_tt<1000)
				{
					s12_short_or_long=0;
				}
				s12_tt=0;
			}
		}
	}
	if(P32==0)//s13
	{
		Delay_ms(2);
		if(P32==0)
		{
			s13_flag=1;
		}
	}
	if(P32==1)
	{
		Delay_ms(2);
		if(P32==1)
		{
			s13_flag=0;
			if(s13_tt>=10)
			{
				
				if(s13_tt>=1000)
				{
					s13_short_or_long=1;
				}
				else if(s13_tt<1000)
				{
					s13_short_or_long=0;
				}
				s13_tt=0;
			}
		}
	}
	
	if(s12_short_or_long==1)//s12long
	{
		bian_num=0;
		Delay_ms(10);
		EEPROM_Write(0x01,bian_num);
		Delay_ms(10);
	}
	else if(s12_short_or_long==0)//s12short
	{
		s12_short_or_long=2;
		if(interface==0)
		{
			if(shuju_type==0){shuju_type=1;}
			else if(shuju_type==1){shuju_type=2;}
			else if(shuju_type==2){shuju_type=0;}
		}
		else if(interface==1)
		{
			if(canshu_type==0){canshu_type=1;}
			else if(canshu_type==1){canshu_type=0;}
		}			
	}
	
	
	if(s13_short_or_long==1)//s13long
	{
		s13_short_or_long=2;
		if(DAC_on==0){DAC_on=1;}
		else if(DAC_on==1){DAC_on=0;}
	
	}
	else if(s13_short_or_long==0)//s13short
	{
		s13_short_or_long=2;
		if(interface==0){interface=1;}				//界面切换功能
		else if(interface==1){interface=0;}
			
	}
}
void UartInit(void)		//4800bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xC0;		//设置定时初始值
	TH1 = 0xFD;		//设置定时初始值
	ET1 = 0;		//禁止定时器%d中断
	TR1 = 1;		//定时器1开始计时


    ES=1;
    EA=1;
}
void rec_ser() interrupt 4
{
    if (RI==1)
    {
        Rec_buffer[Receive_num]=SBUF;
        Receive_num++;
        Receive_flag=1;
        RI=0;
    }
    
}
void send_buffer(uchar *p,uchar Length)
{
    uchar i=0;
    for ( i = 0; i < Length; i++)
    {
        SBUF=*(p+i);
        while(TI==0);
        TI=0;
    }
    
}
void Uart_Function()
{
    if (Receive_over==1)
    {
        Receive_over=0;
        
        if ((Rec_buffer[0]=='S')&(Rec_buffer[1]=='T')&(Rec_buffer[2]=='\r')&(Rec_buffer[3]=='\n')&&(Receive_num==4))
        {
            sprintf((char*)send_arr,"$%d,%d.%d\r\n",dis,temperature/100,temperature%100);
            send_buffer(send_arr,11);
        }
		else if((Rec_buffer[0]=='P')&(Rec_buffer[1]=='A')&(Rec_buffer[2]=='R')&(Rec_buffer[3]=='A')&(Rec_buffer[4]=='\r')&(Rec_buffer[5]=='\n')&&(Receive_num==6))
		{
            sprintf((char*)send_arr,"#%d,%d\r\n",dis_canshu,temper_canshu);
            send_buffer(send_arr,8);
		}
		else
		{
            sprintf((char*)send_arr,"ERROR\r\n");
            send_buffer(send_arr,7);
		}
		
        Receive_num=0;
    }
    
}

uint Get_Dis()
{
	TH0=0;
	TL0=0;
	Send_Wave();
	TR0=1;
	while((TF0==0)&&(RX==1));
	TR0=0;
	if(TF0==1)
	{
		TF0=0;
		distance=99;
	}
	if(RX==0)
	{
		RX=1;
		distance=(((TH0<<8)|(TL0))*0.017);
		if(distance>=99)
		{
			distance=99;
		}
	}
	return distance;
}	

void Bo_Function()
{
	if(bo_flag==1)
	{
		bo_flag=0;
		dis=Get_Dis();
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

void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0;		//设置定时初始值
	TH0 = 0;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 0;		//定时器0开始计时
}



void LED_on(uchar LED_num)
{
	LED_Bits&=~(0x01<<LED_num-1);
	P2|=0x80;
	P2&=0x9f;
	P0=LED_Bits;
}
void LED_off(uchar LED_num)
{
	LED_Bits|=(0x01<<LED_num-1);
	P2|=0x80;
	P2&=0x9f;
	P0=LED_Bits;

}

void Timer2Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xCD;		//设置定时初始值
	T2H = 0xD4;		//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
	
	IE2|=0x04;
	EA=1;
}
void t2_ser() interrupt 12
{
	static uint bo_tt=0;
	
	static uint temper_tt=0;
	
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
	
	if(++bo_tt==500)
	{
		bo_tt=0;
		bo_flag=1;
	}
    if (Receive_flag>=1)
    {
        Receive_flag++;
        if (Receive_flag>=20)
        {
            Receive_over=1;
            Receive_flag=0;
        }
        
    }
	
	if(s12_flag==1)
	{
		s12_tt++;
	}
	if(s13_flag==1)
	{
		s13_tt++;
	}
	
	
	if(++temper_tt==100)
	{
		temperature=Temper_Get();
		temper_tt=0;
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
void DAC()
{
	if(DAC_on==1)
	{
		if(dis>dis_canshu)
		{
			DA_Write(204);
		}
		else if(dis<=dis_canshu)
		{
			DA_Write(102);
		}
		LED_on(3);
	}
	else if(DAC_on==0)
	{
		DA_Write(20);
		LED_off(3);
	}
}
void LED_Display()
{
	if(interface==0)
	{
		if(temperature>temper_canshu){LED_on(1);}
		else {LED_off(1);}
		if(dis<dis_canshu){LED_on(2);}
		else {LED_off(2);}
		
	}
}
