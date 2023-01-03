#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include "iic.h"

#define uchar unsigned char
#define uint unsigned int

#define Somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

sbit TX=P1^0;
sbit RX=P1^1;

uchar LED_Bits=0xff;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
				0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
														0xbf,0xff,0xc6,0x8e};//22--C;;23--F
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

void allinit();
void Timer2Init(void);		//1??@12.000MHz
void Delay_ms(uchar ms);		//@12.000MHz
void keyscan();

//超声波//
void Timer0Init(void);		//1毫秒@11.0592MHz
void Send_Wave();
void Bo_Function();
uint Get_Dis();
uint dis=0;
uint distance=0;
bit bo_flag=0;

void Display_All();
void Ceju_Display();
void Shuju_Huixian_Display();
void Canshu_Shezhi();

bit huixian_jiance=1;               //0--数据回显；；1--检测显示
bit canshu_shezhi_interface=0;      //0--显示界面；；1--参数设置界面
uchar shuju_jilu_arr[4];
uchar shuju_jilu_num=1;
uchar S0=20;
uchar jilu_cishu=0;
uchar dis_last=0;

void DAC_Out_Function();
uchar V_out=0;

bit LED1_flash=0;
bit fla=0;
uint LED_tt=0;
void LED_7_8();

void main()
{
	allinit();
	Timer2Init();
    Timer0Init();
    S0=EEPROM_Read(0x10);Delay_ms(10);
	while(1)
	{
		keyscan();
        //Bo_Function();
        // Digbuf[5]=dis/100;
        // Digbuf[6]=dis%100/10;
        // Digbuf[7]=dis%10;
        Display_All();
        DAC_Out_Function();
        LED_7_8();


	}
}
void LED_7_8()
{
    if (canshu_shezhi_interface==1)
    {
        LED_Bits&=~(0x01<<6);
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }
    else
    {
        LED_Bits|=(0x01<<6);
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }
    
    if (canshu_shezhi_interface==0)
    {
        if (huixian_jiance==0)
        {
            LED_Bits&=~(0x01<<7);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        else
        {
            LED_Bits|=(0x01<<7);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        
    }
    else
    {
        LED_Bits|=(0x01<<7);
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }
    
}
void DAC_Out_Function()
{
    if (dis<=S0)
    {
        V_out=0;
    }
    else if (dis>S0)
    {
        V_out=(dis-S0)*0.02;
    }
    if (V_out>5)
    {
        V_out=5;
    }
    DA_Write(V_out/1.96*100);
}

void Display_All()
{
    if (canshu_shezhi_interface==0)
    {
        if (huixian_jiance==1)
        {
            Ceju_Display();
        }
        else if (huixian_jiance==0)
        {
            Shuju_Huixian_Display();
        }
        
    }
    else if (canshu_shezhi_interface==1)
    {
        Canshu_Shezhi();
    }
    
}
void Ceju_Display()
{
    

    Digbuf[0]=22;
    Digbuf[1]=21;
    Digbuf[2]=dis/100;
    Digbuf[3]=dis%100/10;
    Digbuf[4]=dis%10;
    Digbuf[5]=dis_last/100;
    Digbuf[6]=dis_last%100/10;
    Digbuf[7]=dis_last%10;

}
void Shuju_Huixian_Display()
{
    Digbuf[0]=shuju_jilu_num;
    Digbuf[1]=21;
    Digbuf[2]=21;
    Digbuf[3]=21;
    Digbuf[4]=21;
    Digbuf[5]=shuju_jilu_arr[shuju_jilu_num-1]/100;
    Digbuf[6]=shuju_jilu_arr[shuju_jilu_num-1]%100/10;
    Digbuf[7]=shuju_jilu_arr[shuju_jilu_num-1]%10;
}
void Canshu_Shezhi()
{
    Digbuf[0]=23;
    Digbuf[1]=21;
    Digbuf[2]=21;
    Digbuf[3]=21;
    Digbuf[4]=21;
    Digbuf[5]=21;
    Digbuf[6]=S0/10;
    Digbuf[7]=S0%10;
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
void Send_Wave()
{
    uchar i=8;
    do
    {
        TX=1;
        Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;
        TX=0;
        Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;Somenop;
    } while (i--);
    
}
void Bo_Function()
{
    if (bo_flag==1)
    {
        bo_flag=0;
        
        dis_last=dis;
        
        dis=Get_Dis();
        

    }
    
}
uint Get_Dis()
{
    TH0=0;
    TL0=0;
    Send_Wave();
    TR0=1;
    while((RX==1)&&(TF0==0));
    TR0=0;
    if (TF0==1)
    {
        TF0=0;
        distance=9999;
    }
    if (RX==0)
    {
        distance=(((TH0<<8)|(TL0))*0.017);
    }
    return distance;
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
	
	if(++Digcom==8)
	{
		Digcom=0;
	}
	
    if (++bo_tt==200)
    {
        bo_tt=0;
        bo_flag=1;
    }
    
    if (LED1_flash==1)
    {
        LED_tt++;
        if (LED_tt%1000==0)
        {
            if (fla==0)
            {
                fla=1;
                LED_Bits&=0xfe;
                P2|=0x80;
                P2&=0x9f;
                P0=LED_Bits;
            }
            else if (fla==1)
            {
                fla=0;
                LED_Bits|=0x01;
                P2|=0x80;
                P2&=0x9f;
                P0=LED_Bits;
            }
            if (LED_tt==6000)
            {
                LED1_flash=0;
                LED_tt=0;

                LED_Bits|=0x01;
                P2|=0x80;
                P2&=0x9f;
                P0=LED_Bits;
            }
            
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
void keyscan()
{

	if(P30==0)//s7
	{
		Delay_ms(5);
		if(P30==0)
		{
			if (canshu_shezhi_interface==0)
            {
                if (huixian_jiance==0)
                {
                    shuju_jilu_num++;
                    if (shuju_jilu_num==5)
                    {
                        shuju_jilu_num=1;
                    }
                    
                }
                
            }
            else if (canshu_shezhi_interface==1)
            {
                S0+=10;
                if (S0==40)
                {
                    S0=10;
                }
                
            }
            EEPROM_Write(0x10,S0);
            Delay_ms(10);
		}
		while(!P30);
	}
	else if(P31==0)//s6
	{
		Delay_ms(5);
		if(P31==0)
		{
			if (canshu_shezhi_interface==0)
            {
                canshu_shezhi_interface=1;
            }
            else if (canshu_shezhi_interface==1)
            {
                canshu_shezhi_interface=0;
            }
            
		}
		while(!P31);
	}
	else if(P32==0)//s5
	{
		Delay_ms(5);
		if(P32==0)
		{
			if (canshu_shezhi_interface==0)
            {
                if (huixian_jiance==0)
                {
                    huixian_jiance=1;
                }
                else if (huixian_jiance==1)
                {
                    huixian_jiance=0;
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
			Bo_Function();

            LED1_flash=1;


            shuju_jilu_arr[jilu_cishu]=dis;
            jilu_cishu++;

            EEPROM_Write(0x01<<jilu_cishu,shuju_jilu_arr[jilu_cishu]);
            Delay_ms(10);
            if (jilu_cishu==4)
            {
                jilu_cishu=0;
            }
            
		}
		while(!P33);
	}
}
//????LED1
//LED_Bits&=0xfe;
//	P2|=0x80;
//	P2&=0x9f;
//	P0=LED_Bits;
//?¨??LED1
//LED_Bits|=0x01;
//	P2|=0x80;
//	P2&=0x9f;
//	P0=LED_Bits;
