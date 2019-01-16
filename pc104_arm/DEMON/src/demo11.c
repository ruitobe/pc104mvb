#include <stdio.h>
#include <ctype.h>
#include <string.h>

//#include <conio.h>  
//#include <graph.h>


#include "config.h"
#include "host_dep.h"
#include "dpr_dep.h"
#include "cl1_ret.h"
#include "cl1_1.h"
#include "demon.h"

char outStr0[80], outStr1[80], outStr2[80];
UNSIGNED16 p100buf[2], p101buf[2], p208buf[2], p209buf[2];
TYPE_LP_PRT_CFG mvb_ts_cfg[4];
UNSIGNED8 retWert, retWert100, retWert101, retWert208, retWert209;
TYPE_LP_TS_CFG mvb;

int test_main(void)
{
  int ch, chi;
  int i = 0;  
  UNSIGNED16 iport;
  UNSIGNED16 age;
    
  mvb.pb_pit = 0xD0000L;       /* from NSDB: TRAFFIC-STORE */
  mvb.pb_pcs = 0xDC000L;       /*            "             */
  mvb.pb_prt = 0xD4000L;       /*            "             */
  mvb.pb_frc = 0xD8000L;       /*            "             */
  mvb.pb_def = 0;              /*            "             */
  mvb.ownership = 1;           /*            "             */
  mvb.ts_type = 1;             /*            "             */
  mvb.prt_addr_max = 4095;     /*            "             */
  mvb.prt_indx_max = 7;        /*            "             */
  mvb.prt_count = 4;           /*            "             */
  mvb.tm_start = 0xD0000L;     /* from NSDB: MVBC-INITIALISIERUNG */
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
  mvb_ts_cfg[0].size     = 2;
  mvb_ts_cfg[0].type     = LP_CFG_SRCE;
  	
  mvb_ts_cfg[1].prt_addr = 0x101;
  mvb_ts_cfg[1].prt_indx = 5;
  mvb_ts_cfg[1].size     = 2;
  mvb_ts_cfg[1].type     = LP_CFG_SRCE;
  	
  mvb_ts_cfg[2].prt_addr = 0x208;
  mvb_ts_cfg[2].prt_indx = 6;
  mvb_ts_cfg[2].size     = 2;
  mvb_ts_cfg[2].type     = LP_CFG_SINK;
  	
  mvb_ts_cfg[3].prt_addr = 0x209;
  mvb_ts_cfg[3].prt_indx = 7;
  mvb_ts_cfg[3].size     = 2;
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
		    retWert100 = MVBCPutPort(0x100, p100buf, 0);
				retWert101 = MVBCPutPort(0x101, p101buf, 0);
				
				while(1) {
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
					delay_ms(1000);
				}
		  } while (1);
	
  	  MVBCStop(0);
    }
  }

	return 0;
}