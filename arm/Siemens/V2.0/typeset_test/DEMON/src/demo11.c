#include <stdio.h>
#include <ctype.h>
#include <string.h>

//#include <conio.h>  
//#include <graph.h>

#include "demon.h"
#include "modbus.h"
#include "delay.h"

#define PC_MODE_B
//#define PC_MODE_A

char outStr0[80], outStr1[80], outStr2[80];
//UNSIGNED16 p100buf[2], p101buf[2], p208buf[2], p209buf[2];
UNSIGNED16 p100buf[17], p101buf[17], p208buf[17], p209buf[17];

uint8_t MVB_Initial_Regs_Read = 0;

UNSIGNED16 pc104_data_buff[17 * 32];

UNSIGNED16 pc104_data_old[17 * 32];

UNSIGNED16 temp_buff[17];
UNSIGNED8 reg_num[32] = {0};

TYPE_LP_PRT_CFG mvb_ts_cfg[32];
TYPE_LP_TS_CFG mvb;

UNSIGNED8 retWert, retWert100, retWert101, retWert208, retWert209;

void MVB_Service(void) {
	UNSIGNED16 age;
	UNSIGNED16 sink_port_count = 0;
	UNSIGNED8 pis = 0;
	UNSIGNED8 i = 0;

	UNSIGNED8 freshness = 0;
  
	if (MVB_START_FLAG == 0)
		return;
	if (MVB_Initial_Regs_Read == 0) {
	  for (pis = 0; pis < mvb.prt_count; pis ++) {
		  if (mvb_ts_cfg[pis].type == LP_CFG_SINK) {
				memset(temp_buff, 0, sizeof(temp_buff));
				MVBCGetPort(mvb_ts_cfg[pis].prt_addr, temp_buff, 0x0000, &age, 0);
				memcpy(&pc104_data_buff[pis*17], &temp_buff[1], mvb_ts_cfg[pis].size);
		    //MVBCGetPort(mvb_ts_cfg[pis].prt_addr, &pc104_data_old[pis * 17], 0x0000, &age, 0);
		  }
			reg_num[pis] = (mvb_ts_cfg[pis].size) / 2;
			/*
			if (mvb_ts_cfg[pis].size == 2) {
		    reg_num[pis] = 1;
		  } else if (mvb_ts_cfg[pis].size == 4) {
		    reg_num[pis] = 2;
		  } else if (mvb_ts_cfg[pis].size == 8) {
		    reg_num[pis] = 4;
		  } else if (mvb_ts_cfg[pis].size == 16) {
		    reg_num[pis] = 8;
		  } else if (mvb_ts_cfg[pis].size == 32) {
		    reg_num[pis] = 16;
		  } else {
	 		
 			}*/
		}
		MVB_Initial_Regs_Read = 1;
		
		return;
	}
	
  for (pis = 0; pis < mvb.prt_count; pis ++) {
	  if (mvb_ts_cfg[pis].type == LP_CFG_SINK) {
		  //MVBCGetPort(mvb_ts_cfg[pis].prt_addr, &pc104_data_buff[pis * 17], 0x0000, &age, 0);
			memset(temp_buff, 0, sizeof(temp_buff));
			MVBCGetPort(mvb_ts_cfg[pis].prt_addr, temp_buff, 0x0000, &age, 0);
			//delay_us(10);
		  memcpy(&pc104_data_buff[pis*17], &temp_buff[1], mvb_ts_cfg[pis].size);
			for (i = 0; i < reg_num[pis]; i++) {
			  if (pc104_data_old[pis * 17 + i] != pc104_data_buff[pis * 17 + i]) {
					// Data is different, freshness is true
					freshness = 1;
					// Update pc104_data_old
					pc104_data_old[pis * 17 + i] = pc104_data_buff[pis * 17 + i];
				}
			}
			freshness = 0;
			
			if (freshness == 1) {
			  pc104_data_buff[(pis + 1)* (reg_num[pis]) + pis * 17] = 0x0001;
				freshness = 0;
			} else {
			  // Data is same, freshness is false
			  pc104_data_buff[(pis + 1)* (reg_num[pis]) + pis * 17] = 0x0000;
			}
		} else {
			memcpy(&temp_buff[1], &pc104_data_buff[pis*17], mvb_ts_cfg[pis].size);
			//printf("Data buff addr: %x\r\n", mvb_ts_cfg[pis].prt_addr);
		  //MVBCPutPort(mvb_ts_cfg[pis].prt_addr, &pc104_data_buff[pis * 17], 0);
			MVBCPutPort(mvb_ts_cfg[pis].prt_addr, temp_buff, 0);
		}
	}
	// Set MVB watchdog 16384ms
	//MVBCRetriggerWd(0, 0x8200);
	MVBCIdle(0);
}

void MVB_Parameter_Init(void) {
#if defined(PC_MODE_B)
	
	mvb.pb_pit = 0xE0000L;       /* from NSDB: TRAFFIC-STORE */
  mvb.pb_pcs = 0xEC000L;       /*            "             */
  mvb.pb_prt = 0xE4000L;       /*            "             */
  mvb.pb_frc = 0xE8000L;       /*            "             */
  mvb.pb_def = 0;              /*            "             */
  mvb.ownership = 1;           /*            "             */
  mvb.ts_type = 1;             /*            "             */
  mvb.prt_addr_max = 4095;     /*            "             */
  mvb.prt_indx_max = 31;       /*            "             */
  
	
	// Port count is from config via Modbus
	mvb.prt_count = 4;           /*            "             */
	
	
  mvb.tm_start = 0xE0000L;     /* from NSDB: MVBC-INITIALISIERUNG */
  mvb.mcm = 2;                 /*                "                */
  mvb.msq_offset = 0;          /*                "                */
  mvb.mft_offset = 0;          /*                "                */
  mvb.line_cfg = 2;            /*                "                */
  mvb.reply_to = 1;            /*                "                */
	// Device address is from config via Modbus
  mvb.dev_addr = 0x0A;         /*                "                */
  
#elif defined(PC_MODE_A)	
	mvb.pb_pit = 0xD0000L;       /* from NSDB: TRAFFIC-STORE */
  mvb.pb_pcs = 0xDC000L;       /*            "             */
  mvb.pb_prt = 0xD4000L;       /*            "             */
  mvb.pb_frc = 0xD8000L;       /*            "             */
  mvb.pb_def = 0;              /*            "             */
  mvb.ownership = 1;           /*            "             */
  mvb.ts_type = 1;             /*            "             */
  mvb.prt_addr_max = 4095;     /*            "             */
  mvb.prt_indx_max = 255;      /*            "             */
  
	
	// Port count is from config via Modbus
	//mvb.prt_count = 4;           /*            "             */
	
	
  mvb.tm_start = 0xD0000L;     /* from NSDB: MVBC-INITIALISIERUNG */
  mvb.mcm = 2;                 /*                "                */
  mvb.msq_offset = 0;          /*                "                */
  mvb.mft_offset = 0;          /*                "                */
  mvb.line_cfg = 2;            /*                "                */
  mvb.reply_to = 1;            /*                "                */
	// Device address is from config via Modbus
  mvb.dev_addr = 0x0A;         /*                "                */
  //mvb.p_prt_cfg = (UNSIGNED32) mvb_ts_cfg;
#endif
  mvb.p_prt_cfg = (UNSIGNED32) mvb_ts_cfg;
}


int test_main(void)
{
  int ch, chi;
  int i = 0;  
  UNSIGNED16 iport;
  UNSIGNED16 age;
    
  mvb.pb_pit = 0xE0000L;       /* from NSDB: TRAFFIC-STORE */
  mvb.pb_pcs = 0xEC000L;       /*            "             */
  mvb.pb_prt = 0xE4000L;       /*            "             */
  mvb.pb_frc = 0xE8000L;       /*            "             */
  mvb.pb_def = 0;              /*            "             */
  mvb.ownership = 1;           /*            "             */
  mvb.ts_type = 1;             /*            "             */
  mvb.prt_addr_max = 4095;     /*            "             */
  mvb.prt_indx_max = 31;        /*            "             */
  mvb.prt_count = 4;           /*            "             */
  mvb.tm_start = 0xE0000L;     /* from NSDB: MVBC-INITIALISIERUNG */
  mvb.mcm = 2;                 /*                "                */
  mvb.msq_offset = 0;          /*                "                */
  mvb.mft_offset = 0;          /*                "                */
  mvb.line_cfg = 2;            /*                "                */
  mvb.reply_to = 1;            /*                "                */
  mvb.dev_addr = 0x0A;         /*                "                */
  mvb.p_prt_cfg = (UNSIGNED32) mvb_ts_cfg;

  /* from NSDB: TRAFFIC-STORE */
  mvb_ts_cfg[0].prt_addr = 0x100;
  mvb_ts_cfg[0].prt_indx = 4;
  mvb_ts_cfg[0].size     = 32;
  mvb_ts_cfg[0].type     = LP_CFG_SRCE;
  	
  mvb_ts_cfg[1].prt_addr = 0x101;
  mvb_ts_cfg[1].prt_indx = 8;
  mvb_ts_cfg[1].size     = 32;
  mvb_ts_cfg[1].type     = LP_CFG_SRCE;
  	
  mvb_ts_cfg[2].prt_addr = 0x208;
  mvb_ts_cfg[2].prt_indx = 12;
  mvb_ts_cfg[2].size     = 32;
  mvb_ts_cfg[2].type     = LP_CFG_SINK;
  	
  mvb_ts_cfg[3].prt_addr = 0x209;
  mvb_ts_cfg[3].prt_indx = 16;
  mvb_ts_cfg[3].size     = 32;
  mvb_ts_cfg[3].type     = LP_CFG_SINK;

  retWert = retWert100 = retWert101 = retWert208 = retWert209 = MVB_NO_ERROR;
  sprintf(outStr0,"         Port: Value / Err      Port: Value / Err\r\n");
  sprintf(outStr1,"Source:  100   1234 /  00       101   1234  / 00");
  sprintf(outStr1,"Sink:    208   1234 /  00       209   1234 /  00");

	p100buf[1] = 0xAAAA;
	p101buf[1] = 0x5555;

	printf("Test of PC/104 UL-API in Class 1.1\r\n");
	printf("Q: Quit program\r\n");
	printf("0: Input Value for Port 100\r\n");
	printf("1: Input Value for Port 101\r\n");
	
  retWert = MVBCInit(&mvb,0);
  printf("MVBCInit: %d\r\n",retWert);
  if (retWert == MVB_NO_ERROR) {
    retWert = MVBCStart(0);
    printf("MVBCStart: %d\r\n",retWert);
	  if (retWert == MVB_NO_ERROR) {
			do {
				
				iport = 0;
		    
				
				while(1) {
					retWert100 = MVBCPutPort(0x100, p100buf, 0);
				  retWert101 = MVBCPutPort(0x101, p101buf, 0);
					printf("Test #%d get source and sink port\r\n", i++);
					ch = i % 2;
					iport = iport * 16 + i % 2;
					if (ch == 0) {
					  p100buf[1] = iport;
				  } else {
					  p101buf[1] = iport;
				  }
				  retWert208 = MVBCGetPort(0x208, p208buf, 0x0000, &age, 0);
					retWert209 = MVBCGetPort(0x209, p209buf, 0x0000, &age, 0);
					sprintf(outStr1, "Source:  100   %04.4X /  %02.2d       101   %04.4X  / %02.2d\r\n", p100buf[1], retWert100, p101buf[1], retWert101);
    			sprintf(outStr2, "Sink:    208   %04.4X /  %02.2d       209   %04.4X /  %02.2d\r\n", p208buf[1], retWert208, p209buf[1], retWert209);
					printf("%s%s%s", outStr0, outStr1, outStr2);
					MVBCIdle(0);
					delay_ms(512);
				}
		  } while (1);
	
  	  MVBCStop(0);
    }
  }

	return 0;
}