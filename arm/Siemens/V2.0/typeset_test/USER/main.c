#include "sys.h"
#include "delay.h"
#include "led.h"
#include "timer.h"
#include "usart.h"
#include "rs485.h"
#include "localbus.h"
#include "modbus.h"
#include "stm32f4xx_it.h"
#include "iwdg.h"

uint8_t RS485_BUF[64];
uint8_t RS485_len;

uint32_t test_addr = 0x555555;
uint16_t  test_data = 0x1234;
uint16_t  read_data;
uint8_t test_rs485[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

uint16_t data =0;

uint16_t  test_data1 = 0x1345;
uint16_t  test_data2 = 0xA2A2;
uint16_t  test_data3 = 0x3456;


uint16_t  res_data1;
uint16_t  res_data2;

uint32_t recode_time1 = 0;
uint32_t recode_time2 = 0;
uint32_t recode_time3 = 0;
uint8_t test_data111;

extern bool is_sys_act;

int main(void)
{ 
	uint32_t i = 0;
	
	u8 rs485buf[5]; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(248);   //初始化延时函数
	uart_init(115200);	//初始化串口波特率为115200
	/*Localbus_GPIO_Init();*/
	RS485_Init(115200);		//初始化RS485串口2
	MVB_Parameter_Init();
	Modbus_Init();
	Localbus_Exit_Init();
	PC104_ARM_REFRESH = 1;
	MVBC_RESET = 1;
	delay_ms(500);
	//TIM3_Int_Init();
  TIM4_Int_Init();
  Localbus_MEM_R = 1;
	Localbus_ARM_CS = 1;
	Localbus_MEM_W = 1;
	Localbus_ARM_ADDR_WAIT = 1;
	//PC104_ARM_REFRESH = 1;
	MVBC_RESET = 0;
	test_mvb_init();
    delay_ms(200);
	IWDG_Init(4,500);
	init_typeset_service();
  while(1) {
		if (millis1() % 4 == 0) {
		    MVB_Service();
			test_event_check();
			mvb_typetest_service();
		}
	}
	MVBCStop(0);		
	return 0;
}
