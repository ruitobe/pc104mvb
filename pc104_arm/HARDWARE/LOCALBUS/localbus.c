#include "sys.h"	
#include "localbus.h"
#include "delay.h"
#include "rs485.h"

#define PC_MODE_A

uint16_t temp[100];
uint16_t i = 0;

volatile uint8_t Localbus_ARM_WAIT = 0;
uint16_t count_inter = 0;
uint32_t PC104_addr = 0xFFFF;
uint32_t data_t[16] = {0};



#if defined(PC_MODE_A)
  uint8_t m_mode = 0;
#elif defined(PC_MODE_B)
  uint8_t m_mode = 1;
#elif defined(PC_MODE_C)
  uint8_t m_mode = 2;
#else
  uint8_t m_mode = 0;
#endif

#ifdef DATA_BUS_8
GPIO_TypeDef* Localbus_ADDR_GPIO_type[18] = {GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOD, GPIOD, GPIOD, GPIOD, GPIOE, GPIOE, GPIOE, GPIOE};
uint16_t Localbus_ADDR_GPIO_pins[18] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15, 
                                        GPIO_Pin_14, GPIO_Pin_15, GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10};
uint32_t Localbus_ADDR_mask[18] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000};

GPIO_TypeDef* Localbus_WR_GPIO_type[8] = {GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOD, GPIOD, GPIOD};
uint16_t Localbus_WR_GPIO_pins[8] = {GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10};
uint8_t Localbus_WR_mask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
#else
GPIO_TypeDef* Localbus_ADDR_GPIO_type[12] = {GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOG, GPIOG};
uint16_t Localbus_ADDR_GPIO_pins[12] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15, GPIO_Pin_0, GPIO_Pin_1};
uint32_t Localbus_ADDR_mask[12] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800};

GPIO_TypeDef* Localbus_WR_GPIO_type[16] = {GPIOD, GPIOD, GPIOD, GPIOD, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOD, GPIOD, GPIOD};
uint16_t Localbus_WR_GPIO_pins[16] = {GPIO_Pin_14, GPIO_Pin_15, GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, 
                                         GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10};
uint16_t Localbus_WR_mask[16] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000};

#endif

void Localbus_GPIO_Init(void) { 	
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOF, ENABLE);//使能PD,PE时钟
	
#ifdef DATA_BUS_8  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;//PE7~15,AF OUT
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_Init(GPIOF, &GPIO_InitStructure);	
#else
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//PE7~15,AF OUT
  GPIO_Init(GPIOG, &GPIO_InitStructure);//初始
#endif
	
#ifdef DATA_BUS_8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
#else	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//偷漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化  
#endif

#ifdef DATA_BUS_8
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//PE11~15,AF OUT
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始
#else
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//PE7~15,AF OUT
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始
#endif
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//LED0和LED1对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIO
}

void Localbus_Exit_Init(void) {
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
  Localbus_GPIO_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource2);
	
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line3;//LINE10
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE10
  EXTI_Init(&EXTI_InitStructure);//配置
	
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//外部中断10
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//外部中断10
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
}

void Localbus_WriteGPIO(uint16_t data) {
	uint8_t pis = 0;
	uint8_t bit = 0;
  for (pis = 0; pis < 16; pis ++) {
	  bit = (uint8_t)((data & Localbus_WR_mask[pis]) >> pis);
		GPIO_WriteBit(Localbus_WR_GPIO_type[pis], Localbus_WR_GPIO_pins[pis], bit);
    //data = data << 1;
	}
}

uint32_t Localbus_ConvertAddr(uint8_t mode, uint32_t addr) {
  // Rui:
	// To integrate Siemens API, address MUST be converted here:
	// PC mode A: base address: "0xD0000"
	// PC mode B: base address: "0xE0000"
	// PC mode C: base address: "0xF00000"
	// Currently, we are using PC mode A: 
	// The base address: "0xD0000"
	// So SA16 = 1, LA17 = 0, LA18 = 1, LA19 = 1, LA20 - LA23 = 0
	// When base address is fixed, the next step is to calc the offset,
	// The offset which needs SA1 - SA15
	// The given addr is "byte address", MVB is running 16bits access to TM
	// So, the corresponding address lines must be calculated by right shift (" >> 1"), 
	// then, SA1 to SA15 is bit mapping to addr
	uint32_t addr_real = 0xFFFFFFFF;
	uint16_t addr_offset, addr_base;
	
	// Calculate offset address
	addr_offset = addr & 0xFFFF;
	addr_offset = addr_offset >> 1;
	
	// Calculate base address
	if (mode == 0) {
	  // PC mode A: base address: "0xD0000"
		if (0xD == (uint8_t)((addr & 0x000FFFFF) >> 16)) {
		  addr_real = 0x00068000 + addr_offset;
		}
	} else if (mode == 1) {
	  // PC mode B: base address: "0xE0000"
		if (0xE == (uint8_t)((addr & 0x000FFFFF) >> 16)) {
		  addr_real = 0x00070000 + addr_offset;
		}
	} else if (mode == 2) {
	  // PC mode C: base address: "0xF00000"
		if (0xF == (uint8_t)((addr & 0x00FFFFFF) >> 20)) {
			addr_real = 0x00780000 + addr_offset;
		}
	}
	
	return addr_real;
	
}

void Localbus_WriteAddr(uint32_t addr) {
	uint8_t pis = 0;
	uint8_t bit = 0;
	uint32_t addr_r;
	uint16_t addr_h, addr_l;
	
	addr_r = Localbus_ConvertAddr(m_mode, addr);
	
	//addr = addr & 0x7FFFFF;
	addr_r = addr_r & 0x7FFFFF;
	//addr_h = (uint16_t)((addr & 0xFFF000)>>12);
	//addr_l = (uint16_t)(addr & 0xFFF);
	
	addr_h = (uint16_t)((addr_r & 0xFFF000)>>12);
	addr_l = (uint16_t)(addr_r & 0xFFF);
	Localbus_ARM_ADDR_WAIT = 1;
	//delay_ms(1);
	//
	for (pis = 0; pis < 12; pis ++) {
		bit = (uint8_t)((addr_l & Localbus_ADDR_mask[pis]) >> pis);
		GPIO_WriteBit(Localbus_ADDR_GPIO_type[pis], Localbus_ADDR_GPIO_pins[pis], bit);
    //addr = addr << 1;
  }
	delay_ms(1);
	Localbus_ARM_ADDR_WAIT = 0;
	for (pis = 0; pis < 12; pis ++) {
		bit = (uint8_t)((addr_h & Localbus_ADDR_mask[pis]) >> pis);
		GPIO_WriteBit(Localbus_ADDR_GPIO_type[pis], Localbus_ADDR_GPIO_pins[pis], bit);
    //addr = addr << 1;
  }
  //Localbus_ARM_ADDR_WAIT = 1;	
	
}
void Localbus_WriteData(uint32_t addr, const uint16_t* data) {
	Localbus_WriteAddr(addr);
  Localbus_WriteGPIO(*data);
	Localbus_MEM_W = 0;
	Localbus_ARM_CS = 0;
	while (!Localbus_ARM_WAIT);
	Localbus_MEM_W = 1;
	Localbus_ARM_CS = 1 ;
	Localbus_ARM_WAIT = 0; 
}

void Creat_localbus_write_signal(){
    
}


uint32_t *Localbus_ReadGPIO(void) {
	//uint16_t data = 0;
	//uint8_t pis;
	data_t[0] = (GPIOD->IDR & GPIO_Pin_14);
	data_t[1] = (GPIOD->IDR & GPIO_Pin_15);
	data_t[2] = (GPIOD->IDR & GPIO_Pin_0);
	data_t[3] = (GPIOD->IDR & GPIO_Pin_1);
	data_t[4] = (GPIOE->IDR & GPIO_Pin_7);
	data_t[5] = (GPIOE->IDR & GPIO_Pin_8);
	data_t[6] = (GPIOE->IDR & GPIO_Pin_9);
	data_t[7] = (GPIOE->IDR & GPIO_Pin_10);
	data_t[8] = (GPIOE->IDR & GPIO_Pin_11);
	data_t[9] = (GPIOE->IDR & GPIO_Pin_12);
	data_t[10] = (GPIOE->IDR & GPIO_Pin_13);
	data_t[11] = (GPIOE->IDR & GPIO_Pin_14);
	data_t[12] = (GPIOE->IDR & GPIO_Pin_15);
	data_t[13] = (GPIOD->IDR & GPIO_Pin_8);
	data_t[14] = (GPIOD->IDR & GPIO_Pin_9);
	data_t[15] = (GPIOD->IDR & GPIO_Pin_10);
	//for (pis = 15; pis > 0; pis --) {
	//	data |= GPIO_ReadInputDataBit(Localbus_WR_GPIO_type[pis], Localbus_WR_GPIO_pins[pis]);
  //  data = data << 1;
	//}
	//data |= GPIO_ReadInputDataBit(Localbus_WR_GPIO_type[pis], Localbus_WR_GPIO_pins[pis]);
  //return data;
	return data_t;
}


uint16_t Localbus_ReadData(uint32_t addr) {
	uint8_t pis;
	uint16_t data = 0;
	uint32_t *temp;
	Localbus_WriteAddr(addr);
	Localbus_MEM_R = 0;
	Localbus_ARM_CS = 0;
  while (!Localbus_ARM_WAIT);
	//temp = Localbus_ReadGPIO();
	data_t[0] = (GPIOD->IDR & GPIO_Pin_14);
	data_t[1] = (GPIOD->IDR & GPIO_Pin_15);
	data_t[2] = (GPIOD->IDR & GPIO_Pin_0);
	data_t[3] = (GPIOD->IDR & GPIO_Pin_1);
	data_t[4] = (GPIOE->IDR & GPIO_Pin_7);
	data_t[5] = (GPIOE->IDR & GPIO_Pin_8);
	data_t[6] = (GPIOE->IDR & GPIO_Pin_9);
	data_t[7] = (GPIOE->IDR & GPIO_Pin_10);
	data_t[8] = (GPIOE->IDR & GPIO_Pin_11);
	data_t[9] = (GPIOE->IDR & GPIO_Pin_12);
	data_t[10] = (GPIOE->IDR & GPIO_Pin_13);
	data_t[11] = (GPIOE->IDR & GPIO_Pin_14);
	data_t[12] = (GPIOE->IDR & GPIO_Pin_15);
	data_t[13] = (GPIOD->IDR & GPIO_Pin_8);
	data_t[14] = (GPIOD->IDR & GPIO_Pin_9);
	data_t[15] = (GPIOD->IDR & GPIO_Pin_10);
	
	//Localbus_ReadGPIO();
	//GPIOD->BSRRH = GPIO_Pin_3;
	//GPIOD->BSRRH = GPIO_Pin_5;
	Localbus_MEM_R = 1;
	Localbus_ARM_CS = 1;
	Localbus_ARM_WAIT = 0;
	/*
	for (pis = 15; pis > 0; pis--) {
		data |= (*(temp + pis) & 0xffff);
    data = data << 1;
	}
	*/
    
    
	for (pis = 15; pis > 0; pis--) {
    if (data_t[pis] != 0)
      data |= 1;
    data = data << 1;
	}
  if (data_t[pis] != 0)
    data |= 1;
  return data;
}


void Localbus_Read_IRQHandler(void) {
	Localbus_ARM_WAIT = 1;
	count_inter ++;
	EXTI_ClearITPendingBit(EXTI_Line2);//清除LINE4上的中断标志位  
}

void PC104_Read_IRQHandler(void) {
	uint16_t data;
  data = Localbus_ReadData(PC104_addr);
	RS485_Send_Data((uint8_t *)&data, 2);
}

void Local_put_bytes(uint32_t addr, const uint16_t *data, uint16_t len) {
  while (len--) {
	  Localbus_WriteData(addr, data);
		addr++;
		data++;
  }
}
