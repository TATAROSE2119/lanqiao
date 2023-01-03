#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include <stc15f2k60s2.h>
#include <intrins.h>

sbit DQ = P1^4;  

unsigned int rd_temperature(void);  

#endif
