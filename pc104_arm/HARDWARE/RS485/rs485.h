#ifndef __RS485_H
#define __RS485_H

#include "sys.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "stm32f4xx_rcc.h"

// 接收缓冲, 最大2048个字节
// 发送缓冲区, 最大2048个字节
extern uint8_t RS485_RX_BUFF[2048];
extern uint8_t RS485_TX_BUFF[2048];
// 接收到的数据长度
extern uint16_t RS485_RX_CNT; 

//模式控制
#define RS485_TX_EN		PAout(12)	//485模式控制.0,接收;1,发送.
//如果想串口中断接收，设置EN_USART2_RX为1，否则设置为0
#define EN_USART1_RX 	1			//0,不接收;1,接收.

void RS485_Init(uint32_t bound);
void RS485_Send_Data(uint8_t *buf, uint8_t len);
void RS485_Receive_Data(uint8_t *buf, uint8_t *len);		 
#endif	   
















