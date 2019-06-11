#ifndef __LOCALBUS_H
#define __LOCALBUS_H		
#include "sys.h"	 
#include "stdlib.h" 

//#define Localbus_ARM_CS    PDout(3)
#define Localbus_MEM_W          PDout(4)
#define Localbus_MEM_R          PDout(5)    
#define Localbus_ARM_CS         PDout(3)
#define PC104_ARM_REFRESH       PBout(15)
#define MVBC_RESET              PBout(14)
#define Localbus_ARM_ADDR_WAIT  PGout(10)


#define Localbus_Read_IRQHandler EXTI9_5_IRQHandler
#define Localbus_Read_IRQn EXTI2_IRQn

#define PC104_Read_IRQHandler EXTI3_IRQHandler
//#define DATA_BUS_8 0


void Localbus_Exit_Init(void);

void Localbus_GPIO_Init(void);

void Localbus_WriteData(uint32_t addr, const uint8_t* data);
void Local_put_bytes(uint32_t addr, const uint16_t *data, uint16_t len);
uint8_t Localbus_ReadData(uint32_t addr);
void Timer5_Init(void);
extern uint16_t m_localbus;
#endif  	 