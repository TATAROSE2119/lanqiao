#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include "iic.h"
#include "ds1302.h"

#define Somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

#define uchar unsigned char
#define uint unsigned int

sbit TX=P1^0;
sbit RX=P1^1;

extern uchar shijian[];

uchar LED_Bits=0xff;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
				0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
														0xbf,0xff,0xc7,0xc6,0x8e,0x89,0x8c,0xfe,0xf7};//22--L,23--C,24--F,25--H,26--P,27--up,28--down
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

void allinit();
void Timer2Init(void);		//1??@12.000MHz
void Delay_ms(uchar ms);		//@12.000MHz
void keyscan16_1();

//bo//
void Timer0Init(void);		//1毫秒@11.0592MHz
uint Get_Dis();
void Send_Wave();
void Bo_Function();
uint dis=0;
uint distance=0;

uint dis_max=0;
uint dis_min=0;
float dis_ave=0;
uint dis_ave1=0;

void Display_All();
void shijian_shuju_display();
void juli_shuju_display();
void shuju_jilu_display();
void caijishijiancanshu_shezhi();
void julicanshu_shezhi();

bit interface=0;                                //0--数据显示界面；；1--参数设置界面
uchar interface_shuju_qiehuan=0;                //0--时间显示；；1--距离显示；；2--数据记录
bit interface_canshu_qiehuan=0;                 //0--采集时间参数；；1--距离参数
bit ceju_mode=0;                                //0--触发模式；；1--定时模式
uchar jilu_shuju_zhi=0;                         //0--最大值；；1--最小值；；2--平均值

uchar caiji_shijian=2;
uchar juli_canshu=10;

uchar Ligth=0;
uchar Ligth_last=0;
uchar Ligth_nom=0;

uchar dis_arr[6];
uchar dis_num=0;

uint AD_tt=0;
uint AD_flag=0;

void Compare();

void main()
{
	allinit();
	Timer2Init();
    Timer0Init();
    Ds1302_Init();
	while(1)
	{
        if (AD_flag==1)
        {
            AD_flag=0;
            if (dis<=10)
            {
                DA_Write(51);
            }
            else if (dis>=80)
            {
                DA_Write(255);
            }
            else if ((dis<80)&&(dis>10))
            {
                DA_Write(dis*42.85+5.71);
            }
        }
        


        Ds1302_Get();
		keyscan16_1();
        // Digbuf[0]=shijian[0]/10;
        // Digbuf[1]=shijian[0]%10;
        // Digbuf[0]=dis/100;
        // Digbuf[1]=dis%100/10;
        // Digbuf[2]=dis%10;
        Display_All();

        Ligth_last=Ligth_nom;
        Ligth= AD_Read(0x01);
        Ligth_nom=Ligth;

        if (ceju_mode==1)
        {
            if (shijian[0]%caiji_shijian==0)
            {
                Bo_Function();
            }

            if (ceju_mode==1)
            {
                dis_arr[dis_num]=dis;
                dis_num++;
                if (dis_num==3)
                {
                    dis_num=0;
                }
            }
    


            if ((((dis_arr[0]<juli_canshu+5)&&(dis_arr[0]>juli_canshu-5))&&
                ((dis_arr[1]<juli_canshu+5)&&(dis_arr[1]>juli_canshu-5))&&
                ((dis_arr[2]<juli_canshu+5)&&(dis_arr[2]>juli_canshu-5))))
            {
                LED_Bits&=~(0x01<<4);
                P2|=0x80;
                P2&=0x9f;
                P0=LED_Bits;
            }
            else
            {
                LED_Bits|=(0x01<<4);
                P2|=0x80;
                P2&=0x9f;
                P0=LED_Bits;
            }
            
            
        }
        else if (ceju_mode==0)
        {
            if ((Ligth_nom<Ligth_last)&&(Ligth==180))
            {
                Bo_Function();
            }
            if (ceju_mode==0)
            {
                dis_arr[dis_num+3]=dis;
                dis_num++;
                if (dis_num==6)
                {
                    dis_num=0;
                }
            }
            LED_Bits|=(0x01<<4);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;

            
        }
        
        
        if (Ligth>=180)
        {
            LED_Bits&=~(0x01<<5);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        else
        {
            LED_Bits|=(0x01<<5);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        
        if( (interface==0)&&(interface_shuju_qiehuan==1)&&(ceju_mode==0))
        {
            LED_Bits&=~(0x01<<3);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        else
        {
            LED_Bits|=(0x01<<3);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        
        if ((interface==0)&&(interface_shuju_qiehuan==0))
        {
            LED_Bits&=~(0x01<<0);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        else
        {
            LED_Bits|=(0x01<<0);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        if ((interface==0)&&(interface_shuju_qiehuan==1))
        {
            LED_Bits&=~(0x01<<1);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        else
        {
            LED_Bits|=(0x01<<1);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        if ((interface==0)&&(interface_shuju_qiehuan==2))
        {
            LED_Bits&=~(0x01<<2);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        else
        {
            LED_Bits|=(0x01<<2);
            P2|=0x80;
            P2&=0x9f;
            P0=LED_Bits;
        }
        
	}
}
void Compare()
{
    uchar i,j;
    uchar compare_arr[6];
    for ( i = 0; i < 5; i++)
    {
        compare_arr[i]=dis_arr[i];
    }
    
    for ( i = 0; i < 6-1; i++)
    {
        for ( j = 0; j < 6-1-i; j++)
        {
            if (compare_arr[j]>compare_arr[j+1])
            {
                uchar temp=compare_arr[j];
                compare_arr[j]=compare_arr[j+1];
                compare_arr[j+1]=temp;
            }
            
        }
        
    }
    dis_max=compare_arr[0];
    dis_min=compare_arr[5];
}
void Display_All()
{
    if (interface==0)
    {
        if (interface_shuju_qiehuan==0)
        {
            shijian_shuju_display();
        }
        else if (interface_shuju_qiehuan==1)
        {
            juli_shuju_display();
        }
        else if (interface_shuju_qiehuan==2)
        {
            shuju_jilu_display();
        }
        
    }
    else if (interface==1)
    {
        if (interface_canshu_qiehuan==0)
        {
            caijishijiancanshu_shezhi();
        }
        else if (interface_canshu_qiehuan==1)
        {
            julicanshu_shezhi();
        }
        
    }
    
}
void shijian_shuju_display()
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
void juli_shuju_display()
{
    Digbuf[0]=22;
    if (ceju_mode==0)
    {
        Digbuf[1]=23;
    }
    else if (ceju_mode==1)
    {
        Digbuf[1]=24;
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
    else if ((dis>9)&&(dis<=99))
    {
        Digbuf[5]=21;
        Digbuf[6]=dis/10;
        Digbuf[7]=dis%10;
    }
    else if (dis<=9)
    {
        Digbuf[5]=21;
        Digbuf[6]=21;
        Digbuf[7]=dis;
    }
    
}
void shuju_jilu_display()
{
    Digbuf[0]=25;
    if (jilu_shuju_zhi==0)
    {
        Digbuf[1]=27;

        Digbuf[4]=21;
        if (dis_max>99)
        {
            Digbuf[5]=dis_max/100;
            Digbuf[6]=dis_max%100/10;
            Digbuf[7]=dis_max%10;
        }
        else if ((dis_max>9)&&(dis_max<=99))
        {
            Digbuf[5]=21;
            Digbuf[6]=dis_max/10;
            Digbuf[7]=dis_max%10;
        }
        else if (dis_max<=9)
        {
            Digbuf[5]=21;
            Digbuf[6]=21;
            Digbuf[7]=dis_max;
        }
        
    }
    else if (jilu_shuju_zhi==1)
    {
        Digbuf[1]=28;
        
        Digbuf[4]=21;
        if (dis_min>99)
        {
            Digbuf[5]=dis_min/100;
            Digbuf[6]=dis_min%100/10;
            Digbuf[7]=dis_min%10;
        }
        else if ((dis_min>9)&&(dis_min<=99))
        {
            Digbuf[5]=21;
            Digbuf[6]=dis_min/10;
            Digbuf[7]=dis_min%10;
        }
        else if (dis_min<=9)
        {
            Digbuf[5]=21;
            Digbuf[6]=21;
            Digbuf[7]=dis_min;
        }
    }
    else if (jilu_shuju_zhi==2)
    {
        Digbuf[1]=20;
        dis_ave1=(uint)dis_ave*10;
        if (dis_ave>99)
        {
            Digbuf[4]=dis_ave1/1000;
            Digbuf[5]=dis_ave1%1000/100;
            Digbuf[6]=dis_ave1%100/10+10;
            Digbuf[7]=dis_ave1%10;
        }
        else if ((dis_ave>9)&&(dis_ave<=99))
        {
            Digbuf[4]=21;
            Digbuf[5]=dis_ave1/100;
            Digbuf[6]=dis_ave1%100/10+10;
            Digbuf[7]=dis_ave1%10;
        }
        else if (dis_ave<=9)
        {
            Digbuf[4]=21;
            Digbuf[5]=21;
            Digbuf[6]=dis_ave1/10+10;
            Digbuf[7]=dis_ave1%10;
        }
    }
    Digbuf[2]=21;
    Digbuf[3]=21;

}
void caijishijiancanshu_shezhi()
{
    Digbuf[0]=26;
    Digbuf[1]=1;
    Digbuf[2]=21;
    Digbuf[3]=21;
    Digbuf[4]=21;
    Digbuf[5]=21;
    Digbuf[6]=caiji_shijian/10;
    Digbuf[7]=caiji_shijian%10;
}
void julicanshu_shezhi()
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

uint Get_Dis()
{
    TH0=0;
    TL0=0;
    Send_Wave();
    TR0=1;
    while((RX==1)&&(TF0==0));
    TR0=0;
    if (RX==0)
    {
        distance=(((TH0<<8)|(TL0))*0.017);
    }
    if (TF0==1)
    {
        TF0=0;
        distance=9999;
    }
    return distance;
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
    dis=Get_Dis();

    
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
	
    if (++AD_tt==499)
    {
        AD_flag=1;
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
void keyscan16_1()
{
	P44=0;P42=1;

	if(!P44&&!P32)//s5
	{
		Delay_ms(5);
		if(!P44&&!P32)
		{
			if (interface==0)
            {
                if (interface_shuju_qiehuan==0)
                {
                    interface_shuju_qiehuan=1;
                }
                else if (interface_shuju_qiehuan==1)
                {
                    interface_shuju_qiehuan=2;
                }
                else if (interface_shuju_qiehuan==2)
                {
                    interface_shuju_qiehuan=0;
                }
                
            }
            else if (interface==1)
            {
                if (interface_canshu_qiehuan==0)
                {
                    interface_canshu_qiehuan=1;
                }
                else if (interface_canshu_qiehuan==1)
                {
                    interface_canshu_qiehuan=0;
                }
                
            }
            
		}
		while(!P44&&!P32);
	}
    if (!P44&&!P33)//s4
    {
		Delay_ms(5);
		if(!P44&&!P33)
        {
            if (interface==0)
            {
                interface=1;
            }
            else if (interface==1)
            {
                interface=0;
            }
            
        }
		while(!P44&&!P33);
    }
    
	P44=1;P42=0;
	if(!P42&&!P32)//s9
	{
		Delay_ms(5);
		if(!P42&&!P32)
		{
			if (interface==1)
            {
                if (interface_canshu_qiehuan==0)
                {
                    if (caiji_shijian==2)
                    {
                        caiji_shijian=3;
                    }
                    else if (caiji_shijian>=3)
                    {
                        caiji_shijian+=2;
                    }
                    if (caiji_shijian>9)
                    {
                        caiji_shijian=2;
                    }
                    
                }
                else if (interface_canshu_qiehuan==1)
                {
                    juli_canshu+=10;
                    if (juli_canshu>80)
                    {
                        juli_canshu=10;
                    }
                    
                }
                
            }
            
		}
		while(!P42&&!P32);
	}
    if (!P42&&!P33)//s8
    {
		Delay_ms(5);
		if(!P42&&!P33)
        {
            if (interface==0)
            {
                if (interface_shuju_qiehuan==1)
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
                if (interface_shuju_qiehuan==2)
                {
                    if (jilu_shuju_zhi==0)
                    {
                        jilu_shuju_zhi=1;
                    }
                    else if (jilu_shuju_zhi==1)
                    {
                        jilu_shuju_zhi=2;
                    }
                    else if (jilu_shuju_zhi==2)
                    {
                        jilu_shuju_zhi=0;
                    }
                    
                }
                
            }
            
        }
		while(!P42&&!P33);
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
