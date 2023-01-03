#include <STC15F2K60S2.h>
#include <INTRINS.h>
#include "iic.h"

#define uchar unsigned char 
#define uint unsigned char 

uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
                0x40,0x70,0x20,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
                                                    0xbf,0xff,
                                                            0xc1,0x8c,0xab};//U P N//
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar DIgcom=0;
uchar AD=0;
uchar Voltage_P=0;
uchar Voltage=0;
uchar AD_LAST=0;
uchar AD_NOW=0;

uchar INTERFACE=0;//0-数据显示，1-参数界面，2-计数界面
uint Counter_Num=0;
uchar wuxiao_Button=0;
uint vol_Over_tt=0;

void chufa_jishu();

void allinit();
void keyscan16_2();
void Timer2Init(void);		//1毫秒@12.000MHz
void Delay_ms(uint ms);		//@12.000MHz
void main()
{
    allinit();
    Timer2Init();

    while (1)
    {
        chufa_jishu();

        AD=AD_Read(0x03);
        Voltage=AD*1.96;

        Voltage_P=EEPROM_Read(0x00);

        keyscan16_2();


        if (wuxiao_Button>=4)//******************************************LED3************************************************//
        {
            P2=0x80;
            P0=~0x04;
        }
        else
        {
            P2=0x80;
            P0=0xff;
        }
        if (vol_Over_tt>=5000)//******************************************LED1************************************************//
        {
            P2=0x80;
            P0=0xfe;
        }
        else
        {
            P2=0x80;
            P0=0xff;
        }
        
        if (INTERFACE==0)
        {
            Digbuf[0]=22;
            Digbuf[1]=21;
            Digbuf[2]=21;
            Digbuf[3]=21;
            Digbuf[4]=21;
            Digbuf[5]=Voltage/100+10;
            Digbuf[6]=Voltage%100/10;;
            Digbuf[7]=Voltage%10;
        }
        else if (INTERFACE==1)
        {
            Digbuf[0]=23;
            Digbuf[1]=21;
            Digbuf[2]=21;
            Digbuf[3]=21;
            Digbuf[4]=21;
            Digbuf[5]=Voltage_P/100+10;
            Digbuf[6]=Voltage_P%100/10;;
            Digbuf[7]=Voltage_P%10;
        }
        else if (INTERFACE==2)
        {
            Digbuf[0]=24;
            Digbuf[1]=21;
            Digbuf[2]=21;
            Digbuf[3]=21;
            Digbuf[4]=21;
            if((Counter_Num<=999)&&(Counter_Num>99))
            {
                Digbuf[5]=Counter_Num/100;
                Digbuf[6]=Counter_Num%100/10;
                Digbuf[7]=Counter_Num%10;
            }
            else if ((Counter_Num<=99)&&(Counter_Num>9))
            {
                Digbuf[5]=21;
                Digbuf[6]=Counter_Num/10;
                Digbuf[7]=Counter_Num%10;
            }
            else if (Counter_Num<=9)
            {
                Digbuf[5]=21;
                Digbuf[6]=21;
                Digbuf[7]=Counter_Num;
            }
            
            if (Counter_Num%2==1)//**************************************LED2*******************************************//
            {
                P2=0x80;
                P0=~0x02;
            }
            else
            {
                P2=0x80;
                P0=0xff;
            }

        }
        

    }
}
void keyscan16_2()
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
            case 0x7e://s7
                wuxiao_Button++;
                break;
            case 0x7d://s6
                wuxiao_Button++;
                break;
            case 0x7b://s5
                wuxiao_Button++;
                break;
            case 0x77://s4
                wuxiao_Button++;
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
            case 0xbe://s11
                wuxiao_Button++;
                break;
            case 0xbd://s10
                wuxiao_Button++;
                break;
            case 0xbb://s9
                wuxiao_Button++;
                break;
            case 0xb7://s8
                wuxiao_Button++;
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
    P3=0xdf;P44=1;P42=1;
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
            case 0xde://s15
                wuxiao_Button++;
                break;
            case 0xdd://s14
                wuxiao_Button++;
                break;
            case 0xdb://s13
                wuxiao_Button=1;

                if (INTERFACE==2)
                {
                    Counter_Num=0;
                }
                
                break;
            case 0xd7://s12
                wuxiao_Button=1;
                if (INTERFACE>=3)
                {
                    INTERFACE=0;
                }
                else
                {
                    INTERFACE++;
                }
                if (INTERFACE==3)
                {
                    EEPROM_Write(0x00,Voltage_P*10);
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
    P3=0xef;P44=0;P42=1;
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
            case 0xee://s19
                wuxiao_Button++;
                break;
            case 0xed://s18
                wuxiao_Button++;
                break;
            case 0xeb://s17
                wuxiao_Button=1;
                if (INTERFACE==1)
                {
                    if (Voltage_P<=0)
                    {
                        Voltage_P=500;
                    }
                    Voltage_P-=50;
                }
                break;
            case 0xe7://s16
                wuxiao_Button=1;
                if (INTERFACE==1)
                {
                    if (Voltage_P>500)
                    {
                        Voltage_P=0;
                    }
                    Voltage_P+=50;
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

void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初始值
	T2H = 0xD1;		//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
    
    IE2|= 0x04;                    //开定时器2中断
    EA = 1;
}
void TImer2_inter() interrupt 12
{
    P2|=0xe0;
    P2&=0xff;
    P0=0xff;

    P2|=0xc0;
    P2&=0xdf;
    P0=0x01<<DIgcom;

    P2|=0xe0;
    P2&=0xff;
    P0=tab[Digbuf[DIgcom]];

    if (++DIgcom==8)
    {
        DIgcom=0;
    }
    
    if (Voltage<Voltage_P)
    {
        vol_Over_tt++;
    }
    else
    {
        vol_Over_tt=0;
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

void chufa_jishu()
{
    AD_LAST=AD_NOW;
    AD_NOW=AD*100/50;

}
