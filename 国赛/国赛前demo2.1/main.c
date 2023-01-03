#include <stc15f2k60s2.h>

unsigned char tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
						0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
															0xbf,0xff};
unsigned char Digbuf[]={21,21,21,21,21,21,21,21};
unsigned char Digcom=0;

void allinit();
void Fre_Get();
void Timer2Init(void);		//1??@11.0592MHz
void Timer0Init(void);		//1??@11.0592MHz

unsigned int fre=0;
bit fre_flag=0;
unsigned int fre_tt=0;
void main()
{
	allinit();
	Fre_Get();
	Timer2Init();
	Timer0Init();
	while(1)
	{
		Fre_Get();
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
}
void Fre_Get()
{
	if(fre_flag==1)
	{
		fre_tt=0;
		fre_flag=0;
		fre=(TH0<<8)|(TL0);
		TH0=0;
		TL0=0;
		TR0=1;
	}
}
void Timer2Init(void)		//1??@11.0592MHz
{
	AUXR |= 0x04;		//?????1T??
	T2L = 0xCD;		//???????
	T2H = 0xD4;		//???????
	AUXR |= 0x10;		//???2????
	IE2|=0x04;
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
	
	if(++fre_tt==800)
	{
		fre_flag=1;		TR0=0;

	}
}
void Timer0Init(void)		//1??@11.0592MHz
{
	AUXR |= 0x80;		//?????1T??
	TMOD &= 0xF0;		//???????
	TMOD |= 0x04;                    //?????0?16????????????
	TL0 = 0;		//???????
	TH0 = 0;		//???????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
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
