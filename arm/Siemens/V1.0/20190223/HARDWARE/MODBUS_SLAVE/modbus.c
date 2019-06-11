#include "modbus.h"

// ����֡��С�����ms),������ʱ������Ϊ����һ֡
#define RS485_Frame_Interval 4
// �ӻ���ַ
uint8_t RS485_Addr = 1;
// MVB�˿����ݴ�С
uint8_t port_size[5] = {2, 4, 8, 16, 32};
// ֡�������
uint8_t RS485_FrameFlag=0;

uint16_t heart_beat = 0;
uint16_t startRegAddr;
uint16_t Multi_regNum;
uint8_t data_count;
uint16_t calCRC;
uint8_t MVB_START_FLAG = 0;

//CRCУ�� �Լ�������ӵ�
const uint8_t auchCRCHi[] = { 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
  0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
  0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40}; 


const uint8_t auchCRCLo[] = { 
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
  0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
  0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
  0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
  0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
  0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
  0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
  0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
  0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
  0x43, 0x83, 0x41, 0x81, 0x80, 0x40};


uint16_t CRC_Compute(uint8_t *puchMsg, uint16_t usDataLen) { 
	uint8_t uchCRCHi = 0xFF ; 
	uint8_t uchCRCLo = 0xFF ; 
	uint32_t uIndex ; 
	while (usDataLen--) { 
		uIndex = uchCRCHi ^ *puchMsg++ ; 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
		uchCRCLo = auchCRCLo[uIndex] ; 
	} 
	return ((uchCRCHi<< 8)  | (uchCRCLo)) ; 
}

void Modbus_Init(void) {
	// ��ʱ��7��ʼ�������ڼ��ӿ���ʱ��
  Timer7_Init();
}

// ��ʱ��7��ʼ��
void Timer7_Init(void) {
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  // TIM7ʱ��ʹ�� 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); 
  // TIM7��ʼ������
	// ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
  TIM_TimeBaseStructure.TIM_Period = RS485_Frame_Interval * 10;
	// ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ ���ü���Ƶ��Ϊ10kHz
  TIM_TimeBaseStructure.TIM_Prescaler =12399;
	// ����ʱ�ӷָ�:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	// IM���ϼ���ģʽ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); 
  
	TIM_ITConfig( TIM7, TIM_IT_Update, ENABLE );//TIM7 ��������ж�

  // TIM7�жϷ�������
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;  //TIM7�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
  NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���                                                            
}

// �ö�ʱ��7�жϽ��տ���ʱ�䣬������ʱ�����ָ��ʱ�䣬��Ϊһ֡����
// ��ʱ��7�жϷ������         
void TIM7_IRQHandler(void) {                                                                   
  if (TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET) {
    TIM_ClearITPendingBit(TIM7,TIM_IT_Update);//����жϱ�־
    TIM_Cmd(TIM7,DISABLE);//ֹͣ��ʱ��
    RS485_TX_EN = 1;//ֹͣ���գ��л�Ϊ����״̬
    RS485_FrameFlag = 1;//��λ֡�������
  }
}

void Heartbeat_Service(void) {
  uint8_t status;
	if (MVB_START_FLAG == 0)
		return;
	status = MVBCGetSCRIL(0);
	if (status == (uint8_t)(TM_SCR_IL_RUNNING)) {
	  heart_beat++; 
	}
	// else: MVB ״̬�쳣
}

void Modbus_Service(void) {
  uint16_t recCRC;
  if (RS485_FrameFlag == 1) {
    if (RS485_RX_BUFF[0] == RS485_Addr) {
      if ((RS485_RX_BUFF[1] == 0x03) || (RS485_RX_BUFF[1] == 0x06) ||
		    (RS485_RX_BUFF[1] == 0x10)) {
				  startRegAddr = (((uint16_t)RS485_RX_BUFF[2]) << 8) | RS485_RX_BUFF[3];
          if (startRegAddr > 0x630) {
					  calCRC = CRC_Compute(RS485_RX_BUFF, RS485_RX_CNT - 2);
            recCRC = RS485_RX_BUFF[RS485_RX_CNT - 1] | (((uint16_t)RS485_RX_BUFF[RS485_RX_CNT - 2]) << 8);
            if (calCRC == recCRC) {
              switch (RS485_RX_BUFF[1]) {
                case 0x03: {
									Multi_regNum = (((uint16_t)RS485_RX_BUFF[4]) << 8) | RS485_RX_BUFF[5];
                  Modbus_03_Solve();
                  break;
                }                                  
                case 0x06: {
                  Modbus_06_Solve();
                  break;
                }
                case 0x10: {
									Multi_regNum = (((uint16_t)RS485_RX_BUFF[4]) << 8) | RS485_RX_BUFF[5];
									data_count = RS485_RX_BUFF[6];
                  Modbus_16_Solve();
                  break;
                }
								
								default:
									break;
						  }
            } else {
							// TODO: �쳣�ظ�

					  }        
          } else {
            // TODO: �쳣�ظ�                      
          }                                                
        } else {
				  // TODO: �쳣�ظ� 
        }
      }
		
      RS485_FrameFlag = 0; //��λ֡������־
      RS485_RX_CNT = 0; //���ռ���������
      RS485_TX_EN = 0; //��������ģʽ                
  }                
}


// Modbus������03�������
void Modbus_03_Solve(void) {
	uint16_t i = 0;
	uint16_t data = 0;
	uint16_t req_addr = 0;
	uint16_t calCRC;
	if (startRegAddr + Multi_regNum > 0x885)
		// TODO: �쳣�ظ�
		return;
	
	RS485_TX_BUFF[0] = RS485_RX_BUFF[0];
  RS485_TX_BUFF[1] = RS485_RX_BUFF[1];
	RS485_TX_BUFF[2] = (uint8_t)(Multi_regNum & 0xFF) << 1;
	
	for (i = 0; i < Multi_regNum; i++) {
		//data = (((uint16_t)RS485_RX_BUF[7 + i*2])<<8)|RS485_RX_BUF[8 + i*2];
	  req_addr = startRegAddr + i;
		
		if (req_addr == 0x630 || req_addr == 0x640 || 
			req_addr == 0x641 || req_addr == 0x642 ||
		  req_addr == 0x643 || (req_addr >= 0x645 && startRegAddr < 0x665)) {
		  // TODO: �Ĵ�����֧�� 	
		  return;
		} else if (req_addr == 0x644) {
			// �����Ĵ���
			// TODO: ��ȡMVB��״̬
			RS485_TX_BUFF[3 + (i << 1)] = (uint8_t)(heart_beat >> 8);
			RS485_TX_BUFF[4 + (i << 1)] = (uint8_t)(heart_beat & 0xFF);
			
	  } else if (req_addr >= 0x665 && req_addr < (0x665 + 16 * 33)) {
			RS485_TX_BUFF[3 + (i << 1)] = (uint8_t)(pc104_data_buff[req_addr - 0x665] >> 8);
			RS485_TX_BUFF[4 + (i << 1)] = (uint8_t)(pc104_data_buff[req_addr - 0x665] & 0xFF);
    } else {
		  return;
		}
	}
	
	calCRC = CRC_Compute(RS485_TX_BUFF, ((Multi_regNum << 1) + 3));
	RS485_TX_BUFF[(Multi_regNum << 1) + 3] = (uint8_t)(calCRC >> 8);
	RS485_TX_BUFF[(Multi_regNum << 1) + 4] = (uint8_t)(calCRC & 0xFF);
	RS485_Send_Data(RS485_TX_BUFF, ((Multi_regNum << 1) + 5));
}

void Handle_control_reg(uint16_t data) {
	if ((MVB_START_FLAG == 0) && (data & 0x1) && ((data & 0x2) == 0)) {
	  UNSIGNED8 retWert;
	  retWert = MVBCInit(&mvb, 0);
    if (retWert == MVB_NO_ERROR) {
      retWert = MVBCStart(0);
	    if (retWert == MVB_NO_ERROR) {
			  MVB_START_FLAG = 1;
		  }
	  }
	} else if ((MVB_START_FLAG == 1) && ((data & 0x1) == 0) && (data & 0x2)) {
	  MVBCStop(0);
		MVB_START_FLAG = 0;
	}
}

void Handle_port_config_reg(uint8_t port_index, uint16_t data) {
  mvb_ts_cfg[port_index].prt_addr = (data & 0xFFF);
	mvb_ts_cfg[port_index].size = port_size[((data & 0x7000)>>12)];
	mvb_ts_cfg[port_index].type = (data & 0x8000)== 0 ? LP_CFG_SINK : LP_CFG_SRCE;
	
	if (port_index != 0) {
		if (mvb_ts_cfg[port_index-1].size == 16) {
	    mvb_ts_cfg[port_index].prt_indx = mvb_ts_cfg[port_index-1].prt_indx + 2;
			//if (mvb_ts_cfg[port_index].prt_indx % 2 != 0)
			//	mvb_ts_cfg[port_index].prt_indx = mvb_ts_cfg[port_index].prt_indx + 1;
	  } else if (mvb_ts_cfg[port_index-1].size == 32) {
		  mvb_ts_cfg[port_index].prt_indx = mvb_ts_cfg[port_index-1].prt_indx + 4;
			//if (mvb_ts_cfg[port_index].prt_indx % 4 != 0)
			//	mvb_ts_cfg[port_index].prt_indx = mvb_ts_cfg[port_index].prt_indx + 4 - (mvb_ts_cfg[port_index].prt_indx % 4);
		} else {
			mvb_ts_cfg[port_index].prt_indx =  mvb_ts_cfg[port_index-1].prt_indx + 1;
			/*if (mvb_ts_cfg[port_index].size == 16) {
			  if (mvb_ts_cfg[port_index].prt_indx % 2 != 0)
				  mvb_ts_cfg[port_index].prt_indx = mvb_ts_cfg[port_index].prt_indx + 1;
			} else if (mvb_ts_cfg[port_index].size == 32){
			  if (mvb_ts_cfg[port_index].prt_indx % 4 != 0)
				  mvb_ts_cfg[port_index].prt_indx = mvb_ts_cfg[port_index].prt_indx + 4 - (mvb_ts_cfg[port_index].prt_indx % 4);
			}*/	
		}
	} else {
	  mvb_ts_cfg[port_index].prt_indx = 4;
	}
	
	if (mvb_ts_cfg[port_index].size == 16) {
		if (mvb_ts_cfg[port_index].prt_indx % 2 != 0)
			mvb_ts_cfg[port_index].prt_indx = mvb_ts_cfg[port_index].prt_indx + 1;
	} else if (mvb_ts_cfg[port_index].size == 32){
		if (mvb_ts_cfg[port_index].prt_indx % 4 != 0)
			mvb_ts_cfg[port_index].prt_indx = mvb_ts_cfg[port_index].prt_indx + 4 - (mvb_ts_cfg[port_index].prt_indx % 4);
	}		
	/*
	if (port_index == 0) {
	  mvb_ts_cfg[port_index].prt_indx = 4;
	} else if (port_index == 1) {
	  mvb_ts_cfg[port_index].prt_indx = 8;
	} else if (port_index == 2) {
	  mvb_ts_cfg[port_index].prt_indx = 12;
	} else if (port_index == 3) {
	  mvb_ts_cfg[port_index].prt_indx = 16;
	} else if (port_index == 4) {
	  mvb_ts_cfg[port_index].prt_indx = 20;
	} else {
	
	}
	*/
	/*
	if (port_index == 0) {
	  mvb_ts_cfg[port_index].prt_indx = 4;
	} else if (port_index == 1) {
	  mvb_ts_cfg[port_index].prt_indx = 5;
	} else if (port_index == 2) {
	  mvb_ts_cfg[port_index].prt_indx = 6;
	} else if (port_index == 3) {
	  mvb_ts_cfg[port_index].prt_indx = 8;
	} else if (port_index == 4) {
	  mvb_ts_cfg[port_index].prt_indx = 16;
	} else {
	
	}
	*/
	/*
	if (mvb_ts_cfg[port_index].size == 16) {
	  mvb_ts_cfg[port_index].prt_indx = (port_index+2)*2;
	} else if (mvb_ts_cfg[port_index].size == 32) {
		mvb_ts_cfg[port_index].prt_indx = (port_index+1)*4;
	} else {
		if (port_index != 0) {
			mvb_ts_cfg[port_index].prt_indx =  mvb_ts_cfg[port_index-1].prt_indx + 1;
		} else {
		  mvb_ts_cfg[port_index].prt_indx = 4;
		}
	}
	*/
  
}

void Handle_MVB_addr_reg(uint16_t mvb_addr) {
  mvb.dev_addr = mvb_addr;
}

void Handle_MVB_port_count_reg(uint16_t prt_count) {
  mvb.prt_count = prt_count;

}

void Handle_data_tx_reg(uint16_t port_addr, uint16_t data) {
	//printf("port_addr = 0x%2x data = 0x%2x\r\n", port_addr, data);
  pc104_data_buff[port_addr] = data;
}

void Modbus_06_Solve(void) {
	
	uint16_t data = (((uint16_t)RS485_RX_BUFF[4]) << 8) | RS485_RX_BUFF[5];
	if (startRegAddr == 0x630 || startRegAddr == 0x641 || 
		startRegAddr == 0x644) {
	  // ֻ���Ĵ���
		return;
	} else if	(startRegAddr == 0x640) {
	  Handle_control_reg(data);
	} else if (startRegAddr == 0x642) {
		Handle_MVB_addr_reg(data);
	} else if (startRegAddr == 0x643) {
		Handle_MVB_port_count_reg(data);
	} else if (startRegAddr >= 0x645 && startRegAddr < 0x665) {
		Handle_port_config_reg(startRegAddr - 0x645, data);
  } else if (startRegAddr >= 0x665 && startRegAddr < (0x665 + 17*32)){
      Handle_data_tx_reg(startRegAddr - 0x665, data);

	} else {
	  return;
	}
	
	
		
	RS485_TX_BUFF[0] = RS485_RX_BUFF[0];
  RS485_TX_BUFF[1] = RS485_RX_BUFF[1];
	RS485_TX_BUFF[2] = RS485_RX_BUFF[2];
  RS485_TX_BUFF[3] = RS485_RX_BUFF[3];
	RS485_TX_BUFF[4] = RS485_RX_BUFF[4];
  RS485_TX_BUFF[5] = RS485_RX_BUFF[5];
	RS485_TX_BUFF[6] = RS485_RX_BUFF[6];
	RS485_TX_BUFF[7] = RS485_RX_BUFF[7];
	RS485_Send_Data(RS485_TX_BUFF, 8);
}


void Modbus_16_Solve(void) {
	//uint8_t temp_flag = 0;
	//UNSIGNED16 temp_buff[17];
	uint16_t i = 0;
	uint16_t data = 0;
	uint16_t req_addr = 0;
	uint16_t calCRC;
	
	for (i = 0; i < Multi_regNum; i++) {
		data = (((uint16_t)RS485_RX_BUFF[7 + i*2])<<8)|RS485_RX_BUFF[8 + i*2];
	  req_addr = startRegAddr + i;
		if (req_addr == 0x640){
	    Handle_control_reg(data);
	  } else if (req_addr == 0x641) {
		  ;
	  } else if (req_addr == 0x642) {
		  Handle_MVB_addr_reg(data);
	  } else if (req_addr == 0x643) {
		  Handle_MVB_port_count_reg(data);
	  } else if (req_addr == 0x644) {
		  return;
	  } else if (req_addr >= 0x645 && startRegAddr < 0x665) {
	  	Handle_port_config_reg(req_addr - 0x645, data);
    } else {
	    if (req_addr >= 0x665 && req_addr < (0x665 + 17*32)) {
        Handle_data_tx_reg(req_addr - 0x665, data);
				//temp_flag = 1;
			}
	  }
	}
	
	//if (temp_flag) {
	 // memcpy(&temp_buff[1], &pc104_data_buff[1*17], 32);
	  //printf("Data buff addr: %x\r\n", mvb_ts_cfg[pis].prt_addr);
		  //MVBCPutPort(mvb_ts_cfg[pis].prt_addr, &pc104_data_buff[pis * 17], 0);
		//MVBCPutPort(mvb_ts_cfg[1].prt_addr, temp_buff, 0);
	//}
	RS485_TX_BUFF[0] = RS485_RX_BUFF[0];
  RS485_TX_BUFF[1] = RS485_RX_BUFF[1];
	RS485_TX_BUFF[2] = RS485_RX_BUFF[2];
  RS485_TX_BUFF[3] = RS485_RX_BUFF[3];
	RS485_TX_BUFF[4] = RS485_RX_BUFF[4];
  RS485_TX_BUFF[5] = RS485_RX_BUFF[5];
	calCRC = CRC_Compute(RS485_TX_BUFF, 6);
	RS485_TX_BUFF[6] = (uint8_t)(calCRC >> 8);
	RS485_TX_BUFF[7] = (uint8_t)(calCRC & 0xFF);
	RS485_Send_Data(RS485_TX_BUFF, 8);
}
