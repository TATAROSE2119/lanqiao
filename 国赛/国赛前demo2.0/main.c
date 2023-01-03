//长按键*******************************//
//s4长按计数快速加，短按+1//
//s5长按加2，短按加1//
#include <stc15f2k60s2.h>
#include <intrins.h>
code unsigned char tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
							0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
															0xbf,0xff};
unsigned char Digbuf[]={21,21,21,21,21,21,21,21};
unsigned char Digcom=0;

void allinit();
void keyscan();
void Timer2Init(void);		//1??@11.0592MHz
void Delay_ms(unsigned int ms);		//@11.0592MHz


unsigned char s4_long_flag=0;
unsigned char s5_flag=0;
unsigned char s5_long_flag=2;

unsigned char count4=0;
unsigned char count5=0;
unsigned int s4_tt=0;
unsigned int s5_tt=0;

void main()
{
	Timer2Init();
	allinit();
	while(1)
	{
		keyscan();
		Digbuf[0]=count4/10;
		Digbuf[1]=count4%10;
		Digbuf[6]=count5/10;
		Digbuf[7]=count5%10;
	}
}
void keyscan()
{
	if(P33==0)//s4
	{
		Delay_ms(5);
		if(P33==0)//s4
		{
			s4_long_flag=1;
			count4++;
			if(count4>=50)
			{
				count4=0;
			}
			
		}
		while(!P33)
		{
			if(s4_long_flag==2)
			{
				Delay_ms(60);
				count4++;
				if(count4>=50)
				{
					count4=0;
				}
			}
			Digbuf[0]=count4/10;
			Digbuf[1]=count4%10;
		}
		s4_long_flag=0;
		s4_tt=0;
	}		
	
	if(P32==0)
	{
		Delay_ms(5);
		if(P32==0)
		{
			s5_flag=1;
		}
	}
	if(P32==1)
	{
		Delay_ms(5);
		if(P32==1)
		{
			if(s5_flag==1)
			{
				s5_flag=0;
				if(s5_tt<200)
				{
					s5_long_flag=0;
				}
				else if(s5_tt>=200)
				{
					s5_long_flag=1;
				}
				s5_tt=0;
			}
		}
	}
	if(s5_long_flag==0)
	{
		count5++;
		if(count5>70){count5=0;}
		s5_long_flag=2;
	}
	else if(s5_long_flag==1)
	{
		count5+=10;
		if(count5>70){count5=0;}
		s5_long_flag=2;
	}
}
void Timer2Init(void)		//1??@11.0592MHz
{
	AUXR |= 0x04;		//?????1T??
	T2L = 0xCD;		//???????
	T2H = 0xD4;		//???????
	AUXR |= 0x10;		//???2????
	IE2=0x04;
	EA=1;
}

void t2_ser() interrupt 12
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
	if(++Digcom==8){Digcom=0;}
	
	if(s4_long_flag==1)
	{
		s4_tt++;
		if(s4_tt>=1000)
		{
			s4_long_flag=2;
		}
	}
	if(s5_flag==1)
	{
		s5_tt++;
	}
}
void Delay_ms(unsigned int ms)		//@11.0592MHz
{
	unsigned char i, j,k;
	for(k=0;k<ms;k++)
	{
		_nop_();
		_nop_();
		_nop_();
		i = 11;
		j = 190;
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
