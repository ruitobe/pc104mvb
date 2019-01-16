#include "sys.h"
#include "delay.h"
//#include "usart.h"
#include "led.h"
#include "rs485.h"
#include "localbus.h"
#include "timer.h"
#include "usart.h"

#include "demon.h"

uint8_t RS485_BUF[64];
uint8_t RS485_len;

uint32_t test_addr = 0x555555;
uint16_t  test_data = 0x1234;
uint16_t  read_data;
uint8_t test_rs485[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

uint16_t data =0;

uint16_t  test_data1 = 0x5a5a;
uint16_t  test_data2 = 0x5678;

uint16_t  res_data1;
uint16_t  res_data2;

int main(void)
{ 
	uint32_t i = 0;
	
	u8 rs485buf[5]; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(248);   //初始化延时函数
	uart_init(115200);	//初始化串口波特率为115200
	MVBC_RESET = 1;
	//Localbus_GPIO_Init();
	//LED_Init();					//初始化LED 
	RS485_Init(115200);		//初始化RS485串口2
	delay_ms(500);
	Localbus_Exit_Init();
	//TIM3_Int_Init(10-1,83); 									  
  Localbus_MEM_R = 1;
	Localbus_ARM_CS = 1;
	Localbus_MEM_W = 1;
	Localbus_ARM_ADDR_WAIT = 1;
	PC104_ARM_REFRESH = 1;
	MVBC_RESET = 0;
  delay_ms(1);
	//while (1) {
		//USART_SendData(USART3, test_rs485[0]);
		//while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
	//  RS485_Send_Data(test_rs485, 10);
		//printf("test PC104 usart");
	//	delay_ms(1000);
	//}
	//test_main();
	//while (1) {
	//  RS485_Receive_Data(RS485_BUF, &RS485_len);
	
	//}
	//while (1) {
		
		// Read test
		// Write test
   
		//Localbus_WriteData(0x6d55a, &test_data1);
		//Localbus_WriteData(0xfffff, &test_data1);
		//delay_ms(10);
		//Localbus_WriteData(0x6d55c, &test_data2);
		//Localbus_WriteData(0x00000, &test_data2);
		//delay_ms(10);
    
		//res_data1 = Localbus_ReadData(0x6d55a);
		//res_data1 = Localbus_ReadData(0xfffff);
		//res_data1 = Localbus_ReadData(0xD3F80);
		//res_data1 = Localbus_ReadData(0x69fc0);
		//delay_ms(10);
		//res_data2 = Localbus_ReadData(0x6d55c);
		//res_data2 = Localbus_ReadData(0x00000);
		//res_data2 = Localbus_ReadData(0x69fc2);
		//delay_ms(10);
		//res_data1 = 0;
		//res_data2 = 0;
	//}
	
	test_main();
	
	//for (i = 0; i < 1000; i++)
	//	  data = GPIOD->IDR & GPIO_Pin_10;// GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10);
			
	return 0;
	//Localbus_TEST = 1;
  /*while (1) {
		Localbus_WriteData(test_addr, &test_data);
		delay_ms(1000);
		read_data = Localbus_ReadData(&test_addr);
	  for (i = 0; i < 1000; i++)
		  data = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10);
		
	}*/
}
