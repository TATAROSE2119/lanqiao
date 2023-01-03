#ifndef _IIC_H
#define _IIC_H

//???????¡Â
void IIC_Start(void); 
void IIC_Stop(void);  
void IIC_Ack(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
bit IIC_WaitAck(void);  
unsigned char IIC_RecByte(void); 

unsigned char AD_Read(unsigned char add);
unsigned char EEPROM_Read(unsigned char add);
void EEPROM_Write(unsigned char add,unsigned char dat);


#endif