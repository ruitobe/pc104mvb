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
uint32_t test_addrs = 0x0;
uint8_t m_is_first_time = 0;
uint16_t m_localbus;

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
GPIO_TypeDef* Localbus_ADDR_GPIO_type[12] = {GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF};
uint16_t Localbus_ADDR_GPIO_pins[12] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11};
uint32_t Localbus_ADDR_mask[12] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800};

GPIO_TypeDef* Localbus_WR_GPIO_type[16] = {GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE};

uint16_t Localbus_WR_GPIO_pins[16] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, 
                                         GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};
uint16_t Localbus_WR_mask[16] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000};

#endif

void Localbus_GPIO_Init(void) { 	
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��PD,PEʱ��
	
	/*Data pin's bidirection should work with OD mode/No pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;// Pull down
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  //GPIOF9,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//LED0��LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIO
}

void Timer5_Init(void) {
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  // TIM7ʱ��ʹ�� 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 
  // TIM7��ʼ������
	// ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
  TIM_TimeBaseStructure.TIM_Period = 9;
	// ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ ���ü���Ƶ��Ϊ10kHz
  TIM_TimeBaseStructure.TIM_Prescaler = 12399;
	// ����ʱ�ӷָ�:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	// IM���ϼ���ģʽ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); 
  TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE );//TIM5 ��������ж�

	TIM_Cmd(TIM5,DISABLE); //ʹ�ܶ�ʱ��4
  // TIM7�жϷ�������
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM5�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
  NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

void Localbus_Exit_Init(void) {
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
  Localbus_GPIO_Init();
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource7);
	
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;// | EXTI_Line3;//LINE10
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�����ش��� 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE10
  EXTI_Init(&EXTI_InitStructure);//����
	
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//�ⲿ�ж�10
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	//
	//TIM_Cmd(TIM5, DISABLE);
}

void Localbus_WriteGPIO(uint8_t data) {
#if 0
	GPIO_Write(GPIOE, data);
#else 
	uint8_t pis = 0;
	uint8_t bit = 0;
  for (pis = 0; pis < 8; pis ++) {
	  bit = (uint8_t)((data & Localbus_WR_mask[pis]) >> pis);
		GPIO_WriteBit(Localbus_WR_GPIO_type[pis], Localbus_WR_GPIO_pins[pis], bit);
	}
#endif
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
	uint16_t addr_offset;
	
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
#if 1
	uint8_t pis = 0;
	uint8_t bit = 0;
#endif
	uint32_t addr_r;
	uint16_t addr_h, addr_l;
	
	//addr_r = Localbus_ConvertAddr(m_mode, addr);

	addr_r = addr_r & 0x7FFFFF;
	
	addr_h = (uint16_t)((addr_r & 0xFFF000)>>12);
	//addr_h = (uint16_t)(0x000);
	
	addr_l = (uint16_t)(addr_r & 0xFFF);
	Localbus_ARM_ADDR_WAIT = 1;

#if 0
	GPIO_Write(GPIOF, addr_l);
#else
	for (pis = 0; pis < 12; pis ++) {
		bit = (uint8_t)((addr_l & Localbus_ADDR_mask[pis]) >> pis);
		GPIO_WriteBit(Localbus_ADDR_GPIO_type[pis], Localbus_ADDR_GPIO_pins[pis], bit); 
  }
#endif
	
	delay_us(10);
	Localbus_ARM_ADDR_WAIT = 0;
#if 0
	GPIO_Write(GPIOF, addr_h);
#else
	for (pis = 0; pis < 12; pis ++) {
		bit = (uint8_t)((addr_h & Localbus_ADDR_mask[pis]) >> pis);
		GPIO_WriteBit(Localbus_ADDR_GPIO_type[pis], Localbus_ADDR_GPIO_pins[pis], bit);
  }
#endif
}
void Localbus_WriteData(uint32_t addr, const uint8_t* data) {
	uint8_t i = 50;
	Localbus_WriteAddr(addr);
  Localbus_WriteGPIO(*data);
	Localbus_MEM_W = 0;
	Localbus_ARM_CS = 0;
	
	TIM_Cmd(TIM5,ENABLE);
	//while(i--) {}
	while (!Localbus_ARM_WAIT);
	TIM_Cmd(TIM5,DISABLE);
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	TIM_SetCounter(TIM5, 0);
	Localbus_MEM_W = 1;
	Localbus_ARM_CS = 1 ;
	Localbus_ARM_WAIT = 0; 
}

void Creat_localbus_write_signal(){
    
}


/*uint32_t *Localbus_ReadGPIO(void) {
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
}*/


uint8_t Localbus_ReadData(uint32_t addr) {
	uint8_t i = 50;
	uint8_t data = 0;
	uint8_t pis = 0;
	Localbus_WriteAddr(addr);
	test_addrs = addr;
	Localbus_MEM_R = 0;
	Localbus_ARM_CS = 0;
	
 
  TIM_Cmd(TIM5,ENABLE);
  while (!Localbus_ARM_WAIT) ;
	//while(i--) {}
	TIM_Cmd(TIM5,DISABLE);
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	TIM_SetCounter(TIM5, 0);
	data_t[0] = (GPIOE->IDR & GPIO_Pin_0);
	data_t[1] = (GPIOE->IDR & GPIO_Pin_1);
	data_t[2] = (GPIOE->IDR & GPIO_Pin_2);
	data_t[3] = (GPIOE->IDR & GPIO_Pin_3);
	data_t[4] = (GPIOE->IDR & GPIO_Pin_4);
	data_t[5] = (GPIOE->IDR & GPIO_Pin_5);
	data_t[6] = (GPIOE->IDR & GPIO_Pin_6);
	data_t[7] = (GPIOE->IDR & GPIO_Pin_7);
	Localbus_MEM_R = 1;
	Localbus_ARM_CS = 1;
	Localbus_ARM_WAIT = 0;
	
	for (pis = 7; pis > 0; pis--) {
    if (data_t[pis] != 0)
      data |= 1;
    data = data << 1;
	}
  if (data_t[pis] != 0)
    data |= 1;
  return data;
}

void TIM5_IRQHandler(void) {                                                                   
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //����ж�
	{
    TIM_ClearITPendingBit(TIM5,TIM_IT_Update);//����жϱ�־
    TIM_Cmd(TIM5,DISABLE);//ֹͣ��ʱ��
		m_localbus++;
		//printf("TIM5:%d\n", m_localbus);
    Localbus_ARM_WAIT = 1;
  }
}


void EXTI9_5_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
	  Localbus_ARM_WAIT = 1;
	  count_inter++;
	  EXTI_ClearITPendingBit(EXTI_Line7);//���LINE4�ϵ��жϱ�־λ 
  }		
}