#include "sys.h"
#include "delay.h"
#include "led.h"
#include "timer.h"
#include "usart.h"
#include "rs485.h"
#include "localbus.h"
#include "modbus.h"
#include "stm32f4xx_it.h"
#include "tcn_common.h"


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
uint8_t test_data12 = 0x01;
uint8_t test_data13 = 0x02;

uint32_t modbus_process_time = 0;
uint32_t mvb_process_time = 0;
uint32_t heartbeat_process_time = 0;

uint8_t test_data111;

uint8_t test_data112;
int main(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(248);   //初始化延时函数
	uart_init(115200);	//初始化打印串口，波特率为115200
	RS485_Init(115200);		//初始化RS485串口，波特率115200
	//MVB_Parameter_Init(); //初始化MVB参数
	Modbus_Init();
	//TIM4_Int_Init();
	Localbus_Exit_Init(); //初始化localbus
	PC104_ARM_REFRESH = 1;
	MVBC_RESET = 1;
	delay_ms(500);
  Localbus_MEM_R = 1;
	Localbus_ARM_CS = 1;
	Localbus_MEM_W = 1;
	Localbus_ARM_ADDR_WAIT = 1;
	MVBC_RESET = 0;
  delay_ms(5000);
	Timer5_Init();
	TIM4_Int_Init();
#if 0	
	while(1) {
		//delay_ms(1);
	  test_data111 = Localbus_ReadData(0x4FD);
		//delay_ms(1);
	  test_data112 = Localbus_ReadData(0x4FD);
	}
#endif
  //tcn_demo();
	//tcn_test();
	//mue_test();
	//mue_calibrate();

	while(1) {
	  if (millis() - modbus_process_time > 5) {
			modbus_process_time = millis();
		  Modbus_Service();
		}
		
		if (millis() - mvb_process_time > 50) {
			mvb_process_time = millis();
		  MVB_Service();
		}
		
		if (millis() - heartbeat_process_time > 100) {
		  heartbeat_process_time = millis();
			Heartbeat_Service();
		}
	}	
}
