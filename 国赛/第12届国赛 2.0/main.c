#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include "iic.h"
#include "ds1302.h"

#define uchar unsigned char 
#define uint unsigned int

#define Somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

sbit TX=P1^0;
sbit RX=P1^1;

extern unsigned char shijian[];

uchar LED_Bits=0xFF;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
                0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
                                                        0xbf,0xff,0xc7,0x8e,0xc6,0x89,0x8c,0xfe,0xf7};//L--22,F--23,C--24,H--25,P--26,上杠--27，下杠--28
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

void allinit();
void Timer2Init(void);		//1毫秒@12.000MHz
void Delay_ms(uint ms);		//@12.000MHz
void Keyscan();

uchar Light=0;
uchar Last_Light=0;
uchar Now_Light=0;

//超声波//
void Bo_Function();
void Send_Wave();
uint Get_Dis();
void Timer1Init(void);		//1毫秒@12.000MHz
uint dis=0;
bit bo_flag=0;
uint distance=0;
//数码管显示//
void Display_All();
void Shijian_Xianshi_Display();
void Juli_Xianshi_Display();
void Shuju_Jilu_Xianshi_Display();
void Caijishijian_Shezhi_Display();
void Julicanshu_Shezhi_Display();
//选择型变量//
bit shujuxianshi_canshushezhi=0;            //0--数据显示界面；；1--参数设置界面
uchar qiehuan_shujuxianshi=0;               //0--时间显示；；1--距离显示；；2--数据记录
bit qiehuan_caijicanshu_julicanshu=0;       //0--采集时间参数；；1--距离参数
bit ceju_mode=0;                            //0--触发模式；；1--定时模式
uchar shujujilu_zhi=0;                      //0--max;;1--min;;2--ave
uchar caijishijian_canshu=2;                //2,3,5,7,9;
uchar juli_canshu=10;                        //10~80;
//LED显示// 
void Led_Show_All();
//退出设置的有效变量//
uchar caijishijian_canshu_youxiao=2;
uchar juli_canshu_youxiao=10;

//触发模式的 从亮变暗 判断
uchar Liang_An_Panduan();
//在触发模式下连续 测量三次的数据 在距离参数 +-5cm 范围内的 判断
uchar Fanwei_Panduan();
uchar Lianxian_3_Juli[3];
//DAC输出功能
void DAC_Out_Function();
uchar DA_Out_value=0;

void main()
{
    allinit();
    Timer2Init();
    DS1302_Init();
    while (1)
    {
        DS1302_Get();
        Keyscan();
        Display_All();
        Bo_Function();
        Led_Show_All();
        // Digbuf[5]=AD_Read(0x01)/100;
        // Digbuf[6]=AD_Read(0x01)%100/10;
        // Digbuf[7]=AD_Read(0x01)%10;
        // Digbuf[0]=shijian[0]/10;
        // Digbuf[1]=shijian[0]%10;

    }
    
}
void DAC_Out_Function()
{
    if (dis<10)
    {
        DA_Write(51);
    }
    else if (dis>80)
    {
        DA_Write(255);
    }
    else if ((dis<=80)&&(dis>=10))
    {
        DA_Out_value=(5.71*dis+42.8)/1.96;
        DA_Write(DA_Out_value);
    }
}

uchar Fanwei_Panduan()
{
    if (((Lianxian_3_Juli[0]<(juli_canshu_youxiao+5))&&(Lianxian_3_Juli[0]>(juli_canshu_youxiao-5)))&&((Lianxian_3_Juli[1]<(juli_canshu_youxiao+5))&&(Lianxian_3_Juli[1]>(juli_canshu_youxiao-5)))&&((Lianxian_3_Juli[2]<(juli_canshu_youxiao+5))&&(Lianxian_3_Juli[2]>(juli_canshu_youxiao-5))))
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}

void Led_Show_All()
{
    if (shujuxianshi_canshushezhi==0)
    {
        if (qiehuan_shujuxianshi==0)
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
        if (qiehuan_shujuxianshi==1)
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
        if (qiehuan_shujuxianshi==2)
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
    
    if (ceju_mode==0)
    {
        LED_Bits&=~0x08;
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }
    else
    {
        LED_Bits|=0x08;
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }
    if (ceju_mode==1)
    {
        if (Fanwei_Panduan()==1)
        {
            LED_Bits&=~0x10;
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        else
        {
            LED_Bits|=0x10;
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        
    }
    else
    {
        LED_Bits|=0x10;
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }
    
    if (AD_Read(0x01)>=180)
    {
        LED_Bits&=~0x20;
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }
    else
    {
        LED_Bits|=0x20;
        P2|=0x80;
        P2&=0x9f;
        P0=LED_Bits;
    }
    

}

void Display_All()
{
    if (shujuxianshi_canshushezhi==0)
    {
        if (qiehuan_shujuxianshi==0)
        {
            Shijian_Xianshi_Display();
        }
        else if (qiehuan_shujuxianshi==1)
        {
            Juli_Xianshi_Display();
        }
        else if (qiehuan_shujuxianshi==2)
        {
            Shuju_Jilu_Xianshi_Display();
        }
        
    }
    else if (shujuxianshi_canshushezhi==1)
    {
        if (qiehuan_caijicanshu_julicanshu==0)
        {
            Caijishijian_Shezhi_Display();
        }
        else if (qiehuan_caijicanshu_julicanshu==1)
        {
            Julicanshu_Shezhi_Display();
        }
        
    }
    
}
void Shijian_Xianshi_Display()
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

void Juli_Xianshi_Display()
{
    Digbuf[0]=22;
    if (ceju_mode==0)
    {
        Digbuf[1]=24;
    }
    else if (ceju_mode==1)
    {
        Digbuf[1]=23;
    }
    Digbuf[2]=21;
    Digbuf[3]=21;
    Digbuf[4]=21;
    if (dis>99)
    {
        Digbuf[5]=dis/100;
        Digbuf[6]=dis%100/10;
        Digbuf[7]=dis%10;
    }
    else if ((dis>9)&&(dis<99))
    {
        Digbuf[5]=21;
        Digbuf[6]=dis/10;
        Digbuf[7]=dis%10;
    }
    else if (dis<9)
    {
        Digbuf[5]=21;
        Digbuf[6]=21;
        Digbuf[7]=dis;
    }
    DAC_Out_Function();
}
void Shuju_Jilu_Xianshi_Display()
{
    Digbuf[0]=25;
    if (shujujilu_zhi==0)
    {
        Digbuf[1]=27;
    }
    else if (shujujilu_zhi==1)
    {
        Digbuf[1]=28;
    }
    else if (shujujilu_zhi==2)
    {
        Digbuf[1]=20;
    }
    Digbuf[2]=21;
    Digbuf[3]=21;

}
void Caijishijian_Shezhi_Display()
{
    Digbuf[0]=26;
    Digbuf[1]=1;
    Digbuf[2]=21;
    Digbuf[3]=21;
    Digbuf[4]=21;
    Digbuf[5]=21;
    Digbuf[6]=caijishijian_canshu/10;
    Digbuf[7]=caijishijian_canshu%10;
}
void Julicanshu_Shezhi_Display()
{
    Digbuf[0]=26;
    Digbuf[1]=2;
    Digbuf[2]=21;
    Digbuf[3]=21;
    Digbuf[4]=21;
    Digbuf[5]=21;
    Digbuf[6]=juli_canshu/10;
    Digbuf[7]=juli_canshu%10;
}
uchar Liang_An_Panduan()
{
    Last_Light=Now_Light;
    Now_Light=AD_Read(0x01);
    if ((Now_Light==180)&&(Now_Light<Last_Light))
    {
        return 1;
    }
    return 0;
}

void Bo_Function()
{
    uchar i=0;
    if (ceju_mode==1)
    {
        DS1302_Get();
        if ((shijian[0]%caijishijian_canshu_youxiao)==0)
        {
            if (bo_flag==1)
            {
                bo_flag=0;
                dis=Get_Dis();

                Lianxian_3_Juli[i]=dis;
            }
            i++;
            if (i==3)
            {
                i=0;
            }
            
        }
        
    }
    else if (ceju_mode==0)
    {
        if (Liang_An_Panduan()==1)
        {
            if (bo_flag==1)
            {
                bo_flag=0;
                dis=Get_Dis();
            }
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
uint Get_Dis()
{
    TH1=0;
    TL1=0;
    Send_Wave();
    TR1=1;
    while((RX==1)&&(TF1==0));
    TR1=0;
    if (TF1==1)
    {
        TF1=0;
        distance=9999;
    }
    if (RX==0)
    {
        RX=1;
        distance=(((TH1<<8)|(TL1))*0.017);
    }
    return distance;
}

void Timer1Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0;		//设置定时初始值
	TH1 = 0;		//设置定时初始值
	TF1 = 0;		//清除TF1标志
	TR1 = 0;		//定时器1开始计时
}

void Keyscan()
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
                if (shujuxianshi_canshushezhi==0)
                {
                    if (qiehuan_shujuxianshi==0)
                    {
                        qiehuan_shujuxianshi=1;
                    }
                    else if (qiehuan_shujuxianshi==1)
                    {
                        qiehuan_shujuxianshi=2;
                    }
                    else if (qiehuan_shujuxianshi==2)
                    {
                        qiehuan_shujuxianshi=0;
                    }
                    
                }
                else if (shujuxianshi_canshushezhi==1)
                {
                    if (qiehuan_caijicanshu_julicanshu==0)
                    {
                        qiehuan_caijicanshu_julicanshu=1;
                    }
                    else if (qiehuan_caijicanshu_julicanshu==1)
                    {
                        qiehuan_caijicanshu_julicanshu=0;
                    }
                    
                }
                
                break;
            case 0x77://s4
                if (shujuxianshi_canshushezhi==0)
                {
                    shujuxianshi_canshushezhi=1;
                }
                else if (shujuxianshi_canshushezhi==1)
                {
                    shujuxianshi_canshushezhi=0;
                    juli_canshu_youxiao=juli_canshu;
                    caijishijian_canshu_youxiao=caijishijian_canshu;
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
                if (shujuxianshi_canshushezhi==1)
                {
                    if (qiehuan_caijicanshu_julicanshu==0)
                    {
                        if (caijishijian_canshu==2)
                        {
                            caijishijian_canshu=3;
                        }
                        else
                        {
                            caijishijian_canshu+=2;
                            if (caijishijian_canshu>9)
                            {
                                caijishijian_canshu=2;
                            }
                            
                        }
                        
                    }
                    else if (qiehuan_caijicanshu_julicanshu==1)
                    {
                        juli_canshu+=10;
                        if (juli_canshu>80)
                        {
                            juli_canshu=10;
                        }
                        
                    }
                    
                }
                
                break;
            case 0xb7://s8
                if (shujuxianshi_canshushezhi==0)
                {
                    if (qiehuan_shujuxianshi==1)
                    {
                        if (ceju_mode==0)
                        {
                            ceju_mode=1;
                        }
                        else if (ceju_mode==1)
                        {
                            ceju_mode=0;
                        }
                        
                    }
                    else if (qiehuan_shujuxianshi==2)
                    {
                        if (shujujilu_zhi==0)
                        {
                            shujujilu_zhi=1;
                        }
                        else if (shujujilu_zhi==1)
                        {
                            shujujilu_zhi=2;
                        }
                        else if (shujujilu_zhi==2)
                        {
                            shujujilu_zhi=0;
                        }
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
void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初始值
	T2H = 0xD1;		//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时

    IE2|=0x04;
    EA=1;
}
void T2_ser() interrupt 12
{
    static uint bo_tt=0;

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
    //超声波————部分//
    if (++bo_tt==200)
    {
        bo_tt=0;
        bo_flag=1;
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

