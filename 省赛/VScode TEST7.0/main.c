#include <STC15F2K60S2.H>
#include <INTRINS.H>

#define uchar unsigned char
#define uint unsigned int

uchar LED_Bits=0xff;
uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
				0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
														0xbf,0xff};
uchar Digbuf[]={21,21,21,21,21,21,21,21};
uchar Digcom=0;

void allinit();
void Timer2Init(void);		//1??@12.000MHz
void Delay_ms(uchar ms);		//@12.000MHz
void keyscan();
void keyscan16_1();
void keyscan16_2();

void main()
{
	allinit();
	Timer2Init();
	while(1)
	{
		// keyscan();
		// keyscan16_1();
		keyscan16_2();
	}
}
void allinit()
{
	P2|=0xa0;
	P2&=0xbf;
	P0=0x00;//�������̵���
	
	P2|=0x80;
	P2&=0x9f;
	P0=0xff;//LED
	
	P2|=0xc0;
	P2&=0xdf;
	P0=0xff;//λѡ
	
	P2|=0xe0;
	P2&=0xff;
	P0=0xff;//��ѡ
	
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
	P0=0xff;//��ѡ
	
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
	if(P30==0)
	{
		Delay_ms(5);
		if(P30==0)
		{
			Digbuf[0]=1;
		}
		while(!P30);
	}
	else if(P31==0)
	{
		Delay_ms(5);
		if(P31==0)
		{
			Digbuf[1]=1;
		}
		while(!P31);
	}
	else if(P32==0)
	{
		Delay_ms(5);
		if(P32==0)
		{
			Digbuf[2]=1;
		}
		while(!P32);
	}
	else if(P33==0)
	{
		Delay_ms(5);
		if(P33==0)
		{
			Digbuf[3]=1;
		}
		while(!P33);
	}
}
void keyscan16_1()
{
	P44=0;P42=1;P3=0x7f;
	if(!P44&&!P30)
	{
		Delay_ms(5);
		if(!P44&&!P30)
		{
			Digbuf[0]=1;
		}
		while(!P44&&!P30);
	}
	P44=1;P42=0;P3=0xbf;
	if(!P42&&!P30)
	{
		Delay_ms(5);
		if(!P42&&!P30)
		{
			Digbuf[0]=2;
		}
		while(!P44&&!P30);
	}
	P44=1;P42=1;P3=0xdf;
	if(!P35&&!P30)
	{
		Delay_ms(5);
		if(!P35&&!P30)
		{
			Digbuf[0]=3;
		}
		while(!P35&&!P30);
	}
	P44=1;P42=1;P3=0xef;
	if(!P34&&!P30)
	{
		Delay_ms(5);
		if(!P34&&!P30)
		{
			Digbuf[0]=4;
		}
		while(!P34&&!P30);
	}
}
void keyscan16_2()
{
	uchar temp;
	
	P3=0x7f;P44=0;P42=1;
	temp=P3&0x0f;
	if(temp!=0x0f)
	{
		Delay_ms(5);
		temp=P3&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case(0x7e):
					Digbuf[0]=1;break;
				case(0x7d):
					Digbuf[1]=2;break;
				case(0x7b):
					Digbuf[2]=3;break;
				case(0x77):
					Digbuf[3]=4;break;
				default:
					break;
			}
		}
		while(temp!=0x0f)
		{
			temp=P3&0x0f;
		}
	}
	P3=0xbf;P44=1;P42=0;
	temp=P3&0x0f;
	if(temp!=0x0f)
	{
		Delay_ms(5);
		temp=P3&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case(0xbe):
					Digbuf[0]=5;break;
				case(0xbd):
					Digbuf[1]=6;break;
				case(0xbb):
					Digbuf[2]=7;break;
				case(0xb7):
					Digbuf[3]=8;break;
				default:
					break;

			}
		}
		while(temp!=0x0f)
		{
			temp=P3&0x0f;
		}
	}
	P3=0xdf;P44=1;P42=1;
	temp=P3&0x0f;
	if(temp!=0x0f)
	{
		Delay_ms(5);
		temp=P3&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case(0xde):
					Digbuf[4]=1;break;
				case(0xdd):
					Digbuf[5]=2;break;
				case(0xdb):
					Digbuf[6]=3;break;
				case(0xd7):
					Digbuf[7]=4;break;
				default:
					break;
			}
		}
		while(temp!=0x0f)
		{
			temp=P3&0x0f;
		}
	}
	P3=0xef;P44=0;P42=1;
	temp=P3&0x0f;
	if(temp!=0x0f)
	{
		Delay_ms(5);
		temp=P3&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case(0xee):
					Digbuf[4]=11;break;
				case(0xed):
					Digbuf[5]=12;break;
				case(0xeb):
					Digbuf[6]=13;break;
				case(0xe7):
					Digbuf[7]=14;break;
				default:
					break;
			}
		}
		while(temp!=0x0f)
		{
			temp=P3&0x0f;
		}
	}
}
//����LED1
//LED_Bits&=0xfe;
//	P2|=0x80;
//	P2&=0x9f;
//	P0=LED_Bits;
//Ϩ��LED1
//LED_Bits|=0x01;
//	P2|=0x80;
//	P2&=0x9f;
//	P0=LED_Bits;
