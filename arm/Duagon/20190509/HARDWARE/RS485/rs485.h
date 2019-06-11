#ifndef __RS485_H
#define __RS485_H

#include "sys.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "stm32f4xx_rcc.h"

// ���ջ���, ���2048���ֽ�
// ���ͻ�����, ���2048���ֽ�
extern uint8_t RS485_RX_BUFF[2048];
extern uint8_t RS485_TX_BUFF[2048];
// ���յ������ݳ���
extern uint16_t RS485_RX_CNT; 

//ģʽ����
#define RS485_TX_EN		PAout(12)	//485ģʽ����.0,����;1,����.
//����봮���жϽ��գ�����EN_USART2_RXΪ1����������Ϊ0
#define EN_USART1_RX 	1			//0,������;1,����.

void RS485_Init(uint32_t bound);
void RS485_Send_Data(uint8_t *buf, uint8_t len);
void RS485_Receive_Data(uint8_t *buf, uint8_t *len);		 
#endif	   
















