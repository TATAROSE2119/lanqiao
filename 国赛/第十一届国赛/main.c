#include "STC15F2K60S2.H"
#include "INTRINS.H"
#include "ds1302.h"
#include "iic.h"
#include "onewire.h"

#define uint        unsigned int
#define uchar       unsigned char

extern uchar shijian[];

uchar LED_Bits=0xff;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
                0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
                                                        0xbf,0xff
                                                        ,0xc6,0x86,0x8c};//22--C,23--E,24--P
uchar Digbuf[]={21,21,21,21,21,21,21,21};                 
uchar Digcom=0;

uchar   AD=0;
float   AD_1=0;
uint    Temperature=0;

bit     INTERFACE_SWITCH=0; //0--数据界面，1--参数界面
uchar   SHUJU_3=0;          //0--时间数据，1--温度数据，2--亮暗数据
uchar   CANSHU_3=0;         //0--时间参数，1--温度参数，2--指示灯参数

uchar   _CANSHU_time=17;          
uchar   _CANSHU_temperature=25;
uchar   _CANSHU_lednum=4;

uchar   _CANSHU_time_use=17;             //生效值
uchar   _CANSHU_temperature_use=25;
uchar   _CANSHU_lednum_use=4;

bit     LIGHT_STATU=0;
uint    light_tt=0;

void Timer2Init(void);		//1毫秒@12.000MHz
void allinit(void);
void Delay_ms(uint ms);		//@12.000MHz
void keyscan16_2(void);

void _DISPLAY_time(void);
void _DISPLAY_temperature(void);
void _DISPLAY_light(void);

void _DISPLAY_CANSHU_time(void);
void _DISPLAY_CANSHU_temperature(void);
void _DISPLAY_CANSHU_light(void);

void _DISPLAY_ALL(void);

void LED_SHOW(void);

void main(void)
{
    allinit();
    Timer2Init();
    ds1302_Init();
    while (1)
    {
        AD=AD_Read(0x01);
        AD_1=AD*1.96;
        Temperature=Temper_Get();
        ds1302_Get();
        keyscan16_2();
        _DISPLAY_ALL();
        LED_SHOW();
        // Digbuf[0]=Temperature/10;
        // Digbuf[1]=Temperature%10;
    }
    
}
void _DISPLAY_time(void)
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
void _DISPLAY_temperature(void)
{
    Digbuf[0]=22;
    Digbuf[1]=21;
    Digbuf[2]=21;
    Digbuf[3]=21;
    Digbuf[4]=21;
    Digbuf[5]=Temperature/100;
    Digbuf[6]=Temperature%100/10+10;
    Digbuf[7]=Temperature&10;
}
void _DISPLAY_light(void)
{
    Digbuf[0]=23;
    Digbuf[1]=21;
    Digbuf[2]=(uint)AD_1/100+10;
    Digbuf[3]=(uint)AD_1%100/10;
    Digbuf[4]=(uint)AD_1%10;
    Digbuf[5]=21;
    Digbuf[6]=21;
    if (AD_1>350)
    {
        Digbuf[7]=1;
        LIGHT_STATU=1;
    }
    else if (AD_1<=350)
    {
        Digbuf[7]=0;        
        LIGHT_STATU=0;
    }
    
}

void _DISPLAY_CANSHU_time(void)
{
    Digbuf[0]=24;
    Digbuf[1]=1;
    Digbuf[2]=21;
    Digbuf[3]=21;
    Digbuf[4]=21;
    Digbuf[5]=21;
    Digbuf[6]=_CANSHU_time/10;
    Digbuf[7]=_CANSHU_time%10;
}
void _DISPLAY_CANSHU_temperature(void)
{
    Digbuf[0]=24;
    Digbuf[1]=2;
    Digbuf[2]=21;
    Digbuf[3]=21;
    Digbuf[4]=21;
    Digbuf[5]=21;
    Digbuf[6]=_CANSHU_temperature/10;
    Digbuf[7]=_CANSHU_temperature%10;
}
void _DISPLAY_CANSHU_light(void)
{
    Digbuf[0]=24;
    Digbuf[1]=3;
    Digbuf[2]=21;
    Digbuf[3]=21;
    Digbuf[4]=21;
    Digbuf[5]=21;
    Digbuf[6]=21;
    Digbuf[7]=_CANSHU_lednum;
}
void _DISPLAY_ALL(void)
{
    if (INTERFACE_SWITCH==0)
    {
        if (SHUJU_3==0)
        {
            _DISPLAY_time();
        }
        else if (SHUJU_3==1)
        {
            _DISPLAY_temperature();
        }
        else if (SHUJU_3==2)
        {
            _DISPLAY_light();
        }
    }
    else if (INTERFACE_SWITCH==1)
    {
        if (CANSHU_3==0)
        {
            _DISPLAY_CANSHU_time();
        }
        else if (CANSHU_3==1)
        {
            _DISPLAY_CANSHU_temperature();
        }
        else if (CANSHU_3==2)
        {
            _DISPLAY_CANSHU_light();
        }
        
    }
    
}
void LED_SHOW(void)
{
    if (_CANSHU_time_use>8)
    {
        if(((shijian[2]>_CANSHU_time_use)&&(shijian[2]<24))||((shijian[2]>=0)&&(shijian[2]<8)))
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
        
    }
    else if (_CANSHU_time_use<8)
    {
        if ((shijian[2]>_CANSHU_time_use)&&(shijian[2]<8))
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
        
    }
    else if (_CANSHU_time_use==8)
    {
        LED_Bits|=0x01;
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }

    if (Temperature<_CANSHU_temperature_use)
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
    
    //**************LED3见定时器*********************//

    if (LIGHT_STATU==0)
    {
        LED_Bits&=~(0x01<<(_CANSHU_lednum_use-1));
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }
    else if (LIGHT_STATU==1)
    {
        LED_Bits|=0x01<<(_CANSHU_lednum_use-1);
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }
    
}

void keyscan16_2(void)
{
    uchar temp;
    P3=0x7f;P44=0;P42=1;
    temp=P3&0x0f;
    if (temp!=0x0f)
    {
        Delay_ms(5);
        temp=P3&0x0f;
        if (temp!=0x0f)
        {
            temp=P3;
            switch (temp)
            {
            case 0x7b://s5
                if (INTERFACE_SWITCH==0)
                {
                    if (SHUJU_3==0)
                    {
                        SHUJU_3=1;
                    }
                    else if (SHUJU_3==1)
                    {
                        SHUJU_3=2;
                    }
                    else if (SHUJU_3==2)
                    {
                        SHUJU_3=0;
                    }
                }
                else if (INTERFACE_SWITCH==1)
                {
                    if (CANSHU_3==0)
                    {
                        CANSHU_3=1;
                    }
                    else if (CANSHU_3==1)
                    {
                        CANSHU_3=2;
                    }
                    else if (CANSHU_3==2)
                    {
                        CANSHU_3=0;
                    }
                }
                break;
            case 0x77://s4
                if (INTERFACE_SWITCH==0)
                {
                    INTERFACE_SWITCH=1;
                }
                else if (INTERFACE_SWITCH==1)
                {
                    INTERFACE_SWITCH=0;
                    //退出参数界面时参数生效
                    _CANSHU_time_use=           _CANSHU_time;
                    _CANSHU_temperature_use=    _CANSHU_temperature;
                    _CANSHU_lednum_use=         _CANSHU_lednum;

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
    P3=0xbf;P44=1;P42=0;
    temp=P3&0x0f;
    if (temp!=0x0f)
    {
        Delay_ms(5);
        temp=P3&0x0f;
        if (temp!=0x0f)
        {
            temp=P3;
            switch (temp)
            {
            case 0xbb://s9
                if (INTERFACE_SWITCH==1)
                {
                    if (CANSHU_3==0)
                    {
                        _CANSHU_time++;                        
                        if (_CANSHU_time>23)
                        {
                            _CANSHU_time=0;
                        }
                    }
                    else if (CANSHU_3==1)
                    {
                        _CANSHU_temperature++;
                        if (_CANSHU_temperature>99)
                        {
                            _CANSHU_temperature=0;
                        }
                        
                    }
                    else if (CANSHU_3==2)
                    {
                        _CANSHU_lednum++;
                        if (_CANSHU_lednum>8)
                        {
                            _CANSHU_lednum=4;
                        }
                        
                    }
                    
                }
                
                break;
            case 0xb7://s8
                if (INTERFACE_SWITCH==1)
                {
                    if (CANSHU_3==0)
                    {                        
                        if (_CANSHU_time==0)
                        {
                            _CANSHU_time=24;
                        }
                        _CANSHU_time--;                        
                    }
                    else if (CANSHU_3==1)
                    {
                        if (_CANSHU_temperature==0)
                        {
                            _CANSHU_temperature=100;
                        }
                        _CANSHU_temperature--;                        
                        
                    }
                    else if (CANSHU_3==2)
                    {
                        if (_CANSHU_lednum==4)
                        {
                            _CANSHU_lednum=9;
                        }
                        _CANSHU_lednum--;                        
                        
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
void allinit(void)
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
    
    
    light_tt++;
    if (AD_1>350)
    {  
        if (light_tt==3000)
        {
            LED_Bits&=~0x04;
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;

            light_tt=0;
        }
        
    }
    else if (AD_1<=350)
    {
        if (light_tt==3000)
        {
            LED_Bits|=0x04;
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;

            light_tt=0;
        }
        
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

