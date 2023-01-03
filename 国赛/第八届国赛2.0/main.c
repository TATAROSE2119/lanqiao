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
														0xbf,0xff,0xc6,0x8e};//C--22,F--23
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

void allinit();
void Timer2Init(void);		//1??@12.000MHz
void Delay_ms(uchar ms);		//@12.000MHz
void keyscan();
void LED_on(uchar num);
void LED_off(uchar num);
void DSP();
void Bo_Dat_init();
void DAC_out();
void LED_dsp();
bit bo_on=0;
uchar interface=0;            //0--数据显示，1--回显,2--set
uchar dac=0;
uint led_tt=0;
bit fla=0;
//超声波//
void Bo_Function();
void Timer0Init(void);		//1毫秒@11.0592MHz
void Send_Wave();
uint Dis_Get();
uint dis=0;
uint dis_last=0;
uint distance=0;
bit bo_flag=0;

uint dis_save[4];
uchar dis_num=0;
uchar eep_num=0;
bit bo_over=0;
uchar dis_canshu=0;


void main()
{
	allinit();
	Timer2Init();
    Timer0Init();
    Bo_Dat_init();
	while(1)
	{
        Bo_Function();
		keyscan();
        DSP();
        DAC_out();
        LED_dsp();
	}
}
void Bo_Function()
{
    if (interface==0)
    {
        if ((bo_flag==1)&&(bo_on==1))
        {
            if (dis_num==4)
            {
                dis_num=0;
            }
            if (eep_num==4)
            {
                eep_num=0;
            }
            
            bo_flag=0;bo_on=0;
            dis_last=dis;
            dis=Dis_Get();

            dis_save[dis_num]=dis;
            Delay_ms(8);
            EEPROM_Write(eep_num+1,dis_save[dis_num]);
            Delay_ms(8);
            dis_num++;eep_num++;

            bo_over=1;
            led_tt=0;
            fla=0;
        }
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
    } while (i--);
    
}
uint Dis_Get()
{
    TH0=0;
    TL0=0;
    Send_Wave();
    TR0=1;
    while((TF0==0)&&(RX==1));
    TR0=0;
    if (TF0==1)
    {
        distance=999;
        TF0=0;
    }
    if (RX==0)
    {
        RX=1;
        distance=(((TH0<<8)|(TL0))*0.017);
    }
    return distance;
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
    
    if (bo_over==1)
    {
        led_tt++;
        if (led_tt%1000==0)
        {
            if (fla==0)
            {
                fla=1;
                LED_on(1);
            }
            else if (fla==1)
            {
                fla=0;
                LED_off(1);
            }
        }
        if (led_tt==6000)
        {
            led_tt=0;
            bo_over=0;
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
	if(P30==0)//7
	{
		Delay_ms(5);
		if(P30==0)
		{
			if (interface==1)
            {
                dis_num++;
                if (dis_num==4)
                {
                    dis_num=0;
                }
                
            }
            else if (interface==2)
            {
                dis_canshu+=10;
                if (dis_canshu>30)
                {
                    dis_canshu=0;
                }
                Delay_ms(8);
                EEPROM_Write(5,dis_canshu);
                Delay_ms(8);
            }
            
		}
		while(!P30);
	}
	else if(P31==0)//6
	{
		Delay_ms(5);
		if(P31==0)
		{
			if (interface!=2)
            {
                interface=2;
            }
            else if (interface==2)
            {
                interface=0;
            }
            
		}
		while(!P31);
	}
	else if(P32==0)//5
	{
		Delay_ms(5);
		if(P32==0)
		{
			if (interface==0)
            {
                interface=1;
                dis_num=0;
            }
            else if (interface==1)
            {
                interface=0;
            }
            
		}
		while(!P32);
	}
	else if(P33==0)//4
	{
		Delay_ms(5);
		if(P33==0)
		{
			bo_on=1;
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
void LED_on(uchar num)
{
    LED_Bits&=~(0x01<<num-1);
	P2|=0x80;
	P2&=0x9f;
	P0=LED_Bits;
}
void LED_off(uchar num)
{
    LED_Bits|=(0x01<<num-1);
	P2|=0x80;
	P2&=0x9f;
	P0=LED_Bits;
    
}
void DSP()
{
    if (interface==0)
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
    else if (interface==1)
    {
        Digbuf[0]=dis_num+1;
        Digbuf[1]=21;
        Digbuf[2]=21;
        Digbuf[3]=21;
        Digbuf[4]=21;
        Digbuf[5]=dis_save[dis_num]/100;
        Digbuf[6]=dis_save[dis_num]%100/10;
        Digbuf[7]=dis_save[dis_num]%10;
    }
    else if (interface==2)
    {
        Digbuf[0]=23;
        Digbuf[1]=21;
        Digbuf[2]=21;
        Digbuf[3]=21;
        Digbuf[4]=21;
        Digbuf[5]=21;
        Digbuf[6]=dis_canshu/10;
        Digbuf[7]=dis_canshu%10;
    }
    

}
void Bo_Dat_init()
{
    dis_save[0]=EEPROM_Read(1);
    dis_save[1]=EEPROM_Read(2);
    dis_save[2]=EEPROM_Read(3);
    dis_save[3]=EEPROM_Read(4);
    dis_canshu=EEPROM_Read(5);
}
void DAC_out()
{
    if (dis<=dis_canshu)
    {
        dac=0;
    }
    else if (dis>dis_canshu)
    {
        dac=(dis-dis_canshu)*2/1.96;
        if (dac>255)
        {
            dac=255;
        }
        
    }
    DA_Write(dac);
}
void LED_dsp()
{
    if (interface==2)
    {
        LED_on(7);
    }
    else
    {
        LED_off(7);
    }
    if (interface==1)
    {
        LED_on(8);
    }
    else
    {
        LED_off(8);
    }
    
}
