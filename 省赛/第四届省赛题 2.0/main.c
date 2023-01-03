#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include "iic.h"
#include "DS1302.h"

#define uchar unsigned char
#define uint unsigned int

uchar LED_Bits=0xff;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
                0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
                                                            0xbf,0xff};
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

extern unsigned char shijian[];

bit work_mode=1;//0为手动，1为自动//
uchar AD=0;
uchar shidu_moni=0;
bit buzz=1;
bit threshold_set=0;
uchar Threshold=50;
bit relay=0;

void allinit();
void Timer2Init(void);
void Delay_ms(uint ms);		//@12.000MHz
void Keyscan();

void main()
{
    allinit();
    Timer2Init();
    DS1302_Init();
    EEPROM_Write(0x01,50);
    Delay_ms(5);
    Threshold=EEPROM_Read(0x01);
    while (1)
    {
        DS1302_Get();
        AD=AD_Read(0x03);
        shidu_moni=AD*99/255;
        // Digbuf[6]=shidu_moni/10;
        // Digbuf[7]=shidu_moni%10;
        Keyscan();
        if (threshold_set==0)
        {
            Digbuf[0]=shijian[2]/10;
            Digbuf[1]=shijian[2]%10;
            Digbuf[2]=20;
            Digbuf[3]=shijian[1]/10;
            Digbuf[4]=shijian[1]%10;
            Digbuf[5]=21;
            Digbuf[6]=shidu_moni/10;
            Digbuf[7]=shidu_moni%10;
        }
        else if (threshold_set==1)
        {
            Digbuf[0]=20;
            Digbuf[1]=20;
            Digbuf[2]=21;
            Digbuf[3]=21;
            Digbuf[4]=21;
            Digbuf[5]=21;
            Digbuf[6]=Threshold/10;
            Digbuf[7]=Threshold%10;
        }
        
        if (work_mode==0)
        {
            P2=0x80;
            P0=~0x02;
            if ((shidu_moni<Threshold)&&(buzz==0))
            {
                if (relay==1)
                {
                    P2|=0xa0;
                    P2&=0xbf;
                    P0=0x10;
                }
                else if (relay==0)
                {
                    P2|=0xa0;
                    P2&=0xbf;
                    P0=0x00;
                }
                
            }
            else if ((shidu_moni<Threshold)&&(buzz==1))
            {
                if (relay==1)
                {
                    P2|=0xa0;
                    P2&=0xbf;
                    P0=0x50;
                }
                else if (relay==0)
                {
                    P2|=0xa0;
                    P2&=0xbf;
                    P0=0x40;
                }
            }
            else if (shidu_moni>Threshold)
            {
                if (relay==1)
                {
                    P2|=0xa0;
                    P2&=0xbf;
                    P0=0x10;
                }
                else if (relay==0)
                {
                    P2|=0xa0;
                    P2&=0xbf;
                    P0=0x00;
                }
            }
            
        }
        else if (work_mode==1)
        {
            P2=0x80;
            P0=~0x01;

            if (shidu_moni<Threshold)
            {
                P2=0xa0;
                P0=0x10;
            }
            else if (shidu_moni>Threshold)
            {
                P2=0xa0;
                P0=0x00;
            }
            
        }
        
    }
    

}

void Keyscan()
{
    if (P30==0)//s7
    {
        Delay_ms(5);
        if (P30==0)
        {
            if (work_mode==1)
            {
                work_mode=0;
            }
            else if (work_mode==0)
            {
                work_mode=1;
            }
            
        }
        while(!P30);
    }
    else if (P31==0)//s6
    {
        Delay_ms(5);
        if (P31==0)
        {
            if (work_mode==0)
            {
                if (buzz==0)
                {
                    buzz=1;
                }
                else if (buzz==1)
                {
                    buzz=0;
                }
                
            }
            else if (work_mode==1)
            {
                if (threshold_set==0)
                {
                    threshold_set=1;
                }
                else if (threshold_set==1)
                {
                    EEPROM_Write(0x01,Threshold);
                    threshold_set=0;
                }
                
            }
            
        }
        while(!P31);
    }
    else if (P32==0)//s5
    {
        Delay_ms(5);
        if (P32==0)
        {
            if (work_mode==0)
            {
                relay=1;
            }
            else if (work_mode==1)
            {
                if (threshold_set==1)
                {
                    Threshold++;
                }
                
            }
            
        }
        while(!P32);
    }
    else if (P33==0)//s4
    {
        Delay_ms(5);
        if (P33==0)
        {
            if (work_mode==0)
            {
                relay=0;
                
            }
            else if (work_mode==1)
            {
                if (threshold_set==1)
                {
                    Threshold--;
                }
                
            }
        }
        while(!P33);
    }
}
void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初始值
	T2H = 0xD1;		//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时

    IE2 |= 0x04;                    //开定时器2中断
    EA = 1;

}
void t2int() interrupt 12           //中断入口
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

    if (++Digcom==8)
    {
        Digcom=0;
    }
    
}
void Delay_ms(uint ms)		//@12.000MHz
{
	unsigned char i, j,k;
    for ( k = 0; k < ms; k++)
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