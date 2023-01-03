#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include "iic.h"
#include "onewire.h"

#define uchar unsigned char
#define uint unsigned int

uchar LED_Bits=0xff;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
				0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
														0xbf,0xff,0xc1,0x8e,0xc6,0x89,0x8c};//U--22,F--23,C--24,H--25,P--26
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

void allinit();
void Timer2Init(void);		//1??@12.000MHz
void Delay_ms(uchar ms);		//@12.000MHz
void keyscan();
void LED_ON(uchar num);
void LED_OFF(uchar num);

uint temperature=0;
uint AD=0;
bit If_b_dsp=0;
bit If_set=0;
uchar change_dsp_status=0;          //0--温度；1--电压；2--频率
uchar change_b_dsp_status=0;         //0--温度；1--电压；2--频率
void DSP_ALL();
void DAT_get();
void LED_DSP();

uint b_temper=0;
uint b_AD=0;
uint b_fre=0;
uchar temper_h=0;
uchar temper_l=0;
uchar AD_m=0;
uchar fre_h2=0;
uchar fre_m2=0;
uchar fre_l1=0;

uchar v_threshold=1;
uchar s6_flag=0;
uint s6_tt=0;
bit LED8_flash=0;
uint led_tt=0;
bit fla=0;
//***********************频率//
void Timer0Init(void);		//1毫秒@11.0592MHz
uint fre_tt=0;
uint fre_flag=0;
void Fre_Get();
uint fre=0;

void main()
{
	allinit();
	Timer2Init();
    Timer0Init();
    DAT_get();
	while(1)
	{
        temperature=Temper_Get();
        AD=(AD_Read(0x03)*196)/100;
		keyscan();
        Fre_Get();        
        DSP_ALL();
        LED_DSP();
	}
}

void Fre_Get()
{
    if (fre_flag==1)
    {
        fre_tt=0;
        fre_flag=0;
        fre=(TH0<<8)|(TL0);
        TH0=0;
        TL0=0;
        TR0=1;
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
	
    if (++fre_tt==1000)
    {
        fre_flag=1;
        TR0=0;
    }
    
    if (s6_flag==1)
    {
        s6_tt++;
        if (s6_tt>=800)
        {
            s6_flag=2;
        }
        
    }
    if (LED8_flash==1)
    {
        led_tt++;
        if (led_tt==800)
        {
            led_tt=0;
            if (fla==0)
            {
                fla=1;
                LED_ON(8);
            }
            else if (fla==1)
            {
                fla=0;
                LED_OFF(8);
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
            if (If_b_dsp==0)
            {
                if (If_set==0){If_set=1;}
                else if (If_set==1){If_set=0;}                
            }
            
            
		}
		while(!P30);
	}
	else if(P32==0)//s5
	{
		Delay_ms(5);
		if(P32==0)
		{
            Delay_ms(8);
            EEPROM_Write(1,(uchar)(temperature/100));Delay_ms(8);
            EEPROM_Write(2,(uchar)(temperature%100));Delay_ms(8);
            EEPROM_Write(3,(uchar)AD_Read(0x03));Delay_ms(8);
            EEPROM_Write(4,fre/1000);Delay_ms(8);
            EEPROM_Write(5,fre%1000/10);Delay_ms(8);
            EEPROM_Write(6,fre%10);Delay_ms(8);
		}
		while(!P32);
	}
	else if(P33==0)//s4
	{
		Delay_ms(5);
		if(P33==0)
		{
			if ((If_set==0)&&(If_b_dsp==0))
            {
                if (change_dsp_status==0)
                {
                    change_dsp_status=1;
                }
                else if (change_dsp_status==1)
                {
                    change_dsp_status=2;
                }
                else if (change_dsp_status==2)
                {
                    change_dsp_status=0;
                }
            }
            else if ((If_set==0)&&(If_b_dsp==1))
            {
                if (change_b_dsp_status==0)
                {
                    change_b_dsp_status=1;
                }
                else if (change_b_dsp_status==1)
                {
                    change_b_dsp_status=2;
                }
                else if (change_b_dsp_status==2)
                {
                    change_b_dsp_status=0;
                }
            }
            
		}
		while(!P33);
	}


	else if(P31==0)//s6
	{
		Delay_ms(5);
		if(P31==0)
		{
			if (If_set==0)
            {
                if(If_b_dsp==0)
                {
                    If_b_dsp=1;

                    Delay_ms(8);
                    temper_h=EEPROM_Read(1);
                    Delay_ms(8);
                    temper_l=EEPROM_Read(2);
                    Delay_ms(8);
                    AD_m=EEPROM_Read(3);
                    Delay_ms(8);
                    fre_h2=EEPROM_Read(4);
                    Delay_ms(8);
                    fre_m2=EEPROM_Read(5);
                    Delay_ms(8);
                    fre_l1=EEPROM_Read(6);
                    Delay_ms(8);
                    b_temper=(uint)(temper_h*100+temper_l);
                    b_AD=(uint)((AD_m)*196/100);
                    b_fre=(uint)(fre_h2*1000+fre_m2*10+fre_l1);
                }
                else if (If_b_dsp==1){If_b_dsp=0;}


            }
            else if ((If_set==1)&&(If_b_dsp==0))
            {   
                s6_flag=1;
                v_threshold++;
                if (v_threshold>50)
                {
                    v_threshold=0;
                }
                
            }
		}
		while(!P31)
        {
            if ((If_set==1)&&(If_b_dsp==0))
            {
                if(s6_flag==2)
                {
                    Delay_ms(5000);
                    v_threshold++;
                    if (v_threshold>50)
                    {
                        v_threshold=0;
                    }
                }
            }
            Digbuf[6]=v_threshold/10+10;
            Digbuf[7]=v_threshold%10;
        }
        s6_flag=0;
        s6_tt=0;
	}
}
void LED_ON(uchar num)
{
    LED_Bits&=~(0x01<<num-1);
	P2|=0x80;
	P2&=0x9f;
	P0=LED_Bits;
}
void LED_OFF(uchar num)
{
    LED_Bits|=(0x01<<num-1);
	P2|=0x80;
	P2&=0x9f;
	P0=LED_Bits;
}
void DSP_ALL()
{
    if ((If_set==0)&&(If_b_dsp==0))
    {
        if (change_dsp_status==0)
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
        else if (change_dsp_status==1)
        {
            Digbuf[0]=23;
            Digbuf[1]=21;
            if (fre>99999)
            {
                Digbuf[2]=fre/100000;
                Digbuf[3]=fre%100000/10000;
                Digbuf[4]=fre%10000/1000;
                Digbuf[5]=fre%1000/100;
                Digbuf[6]=fre%100/10;
                Digbuf[7]=fre%10/1;
            }
            else if ((fre>9999)&&(fre<=99999))
            {
                Digbuf[2]=21;
                Digbuf[3]=fre/10000;
                Digbuf[4]=fre%10000/1000;
                Digbuf[5]=fre%1000/100;
                Digbuf[6]=fre%100/10;
                Digbuf[7]=fre%10/1;
            }
            else if((fre>999)&&(fre<=9999))
            {
                Digbuf[2]=21;
                Digbuf[3]=21;
                Digbuf[4]=fre/1000;
                Digbuf[5]=fre%1000/100;
                Digbuf[6]=fre%100/10;
                Digbuf[7]=fre%10/1;
            }
            else if ((fre>99)&&(fre<=999))
            {
                Digbuf[2]=21;
                Digbuf[3]=21;
                Digbuf[4]=21;
                Digbuf[5]=fre/100;
                Digbuf[6]=fre%100/10;
                Digbuf[7]=fre%10/1;
            }
            else if ((fre>9)&&(fre<=99))
            {
                Digbuf[2]=21;
                Digbuf[3]=21;
                Digbuf[4]=21;
                Digbuf[5]=21;
                Digbuf[6]=fre/10;
                Digbuf[7]=fre%10;
            }
            else if (fre<=9)
            {
                Digbuf[2]=21;
                Digbuf[3]=21;
                Digbuf[4]=21;
                Digbuf[5]=21;
                Digbuf[6]=21;
                Digbuf[7]=fre;
            }
            
        }
        else if (change_dsp_status==2)
        {
            Digbuf[0]=24;
            Digbuf[1]=21;
            Digbuf[2]=21;
            Digbuf[3]=21;
            Digbuf[4]=temperature/1000;
            Digbuf[5]=temperature%1000/100+10;
            Digbuf[6]=temperature%100/10;
            Digbuf[7]=temperature%10;
        }
        
    }
    else if ((If_set==0)&&(If_b_dsp==1))
    {
        Digbuf[0]=25;
        if (change_b_dsp_status==0)
        {
            Digbuf[1]=22;
            Digbuf[2]=21;
            Digbuf[3]=21;
            Digbuf[4]=21;
            Digbuf[5]=21;
            Digbuf[6]=b_AD/100+10;
            Digbuf[7]=b_AD%100/10;
        }
        else if (change_b_dsp_status==1)
        {
            Digbuf[1]=23;
            if (b_fre>99999)
            {
                Digbuf[2]=b_fre/100000;
                Digbuf[3]=b_fre%100000/10000;
                Digbuf[4]=b_fre%10000/1000;
                Digbuf[5]=b_fre%1000/100;
                Digbuf[6]=b_fre%100/10;
                Digbuf[7]=b_fre%10/1;
            }
            else if ((b_fre>9999)&&(b_fre<=99999))
            {
                Digbuf[2]=21;
                Digbuf[3]=b_fre/10000;
                Digbuf[4]=b_fre%10000/1000;
                Digbuf[5]=b_fre%1000/100;
                Digbuf[6]=b_fre%100/10;
                Digbuf[7]=b_fre%10/1;
            }
            else if((b_fre>999)&&(b_fre<=9999))
            {
                Digbuf[2]=21;
                Digbuf[3]=21;
                Digbuf[4]=b_fre/1000;
                Digbuf[5]=b_fre%1000/100;
                Digbuf[6]=b_fre%100/10;
                Digbuf[7]=b_fre%10/1;
            }
            else if ((b_fre>99)&&(b_fre<=999))
            {
                Digbuf[2]=21;
                Digbuf[3]=21;
                Digbuf[4]=21;
                Digbuf[5]=b_fre/100;
                Digbuf[6]=b_fre%100/10;
                Digbuf[7]=b_fre%10/1;
            }
            else if ((b_fre>9)&&(b_fre<=99))
            {
                Digbuf[2]=21;
                Digbuf[3]=21;
                Digbuf[4]=21;
                Digbuf[5]=21;
                Digbuf[6]=b_fre/10;
                Digbuf[7]=b_fre%10;
            }
            else if (b_fre<=9)
            {
                Digbuf[2]=21;
                Digbuf[3]=21;
                Digbuf[4]=21;
                Digbuf[5]=21;
                Digbuf[6]=21;
                Digbuf[7]=b_fre;
            }
        }
        else if (change_b_dsp_status==2)
        {
            Digbuf[1]=24;
            Digbuf[2]=21;
            Digbuf[3]=21;
            Digbuf[4]=b_temper/1000;
            Digbuf[5]=b_temper%1000/100+10;
            Digbuf[6]=b_temper%100/10;
            Digbuf[7]=b_temper%10;
        }
        
    }
    else if (If_set==1)
    {
        Digbuf[0]=26;
        Digbuf[1]=21;
        Digbuf[2]=21;
        Digbuf[3]=21;
        Digbuf[4]=21;
        Digbuf[5]=21;
        Digbuf[6]=v_threshold/10+10;
        Digbuf[7]=v_threshold%10;
    }
    
    
}
void DAT_get()
{
    Delay_ms(8);
    temper_h=EEPROM_Read(1);
    Delay_ms(8);
    temper_l=EEPROM_Read(2);
    Delay_ms(8);
    AD_m=EEPROM_Read(3);
    Delay_ms(8);
    fre_h2=EEPROM_Read(4);
    Delay_ms(8);
    fre_m2=EEPROM_Read(5);
    Delay_ms(8);
    fre_l1=EEPROM_Read(6);
    Delay_ms(8);
    b_temper=(uint)(temper_h*100+temper_l);
    b_AD=(uint)((AD_m)*196/100);
    b_fre=(uint)(fre_h2*1000+fre_m2*10+fre_l1);
}
void LED_DSP()
{
    if ((If_set==0)&&(If_b_dsp==0))
    {
        if (change_dsp_status==0)
        {
            LED_ON(1);
        }
        else
        {
            LED_OFF(1);
        }
        if (change_dsp_status==2)
        {
            LED_ON(3);
        }
        else
        {
            LED_OFF(3);
        }
        if (change_dsp_status==1)
        {
            LED_ON(2);
        }
        else
        {
            LED_OFF(2);
        }
        
    }
    else
    {
        LED_OFF(1);LED_OFF(2);LED_OFF(3);
    }
    if (AD>v_threshold*10)
    {
        LED8_flash=1;
    }
    else
    {
        LED8_flash=0;
        LED_OFF(8);
    }
    
}
