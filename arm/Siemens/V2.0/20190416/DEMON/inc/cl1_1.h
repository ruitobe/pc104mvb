/**********************************************************************
Copyright (C) Siemens AG 1997-2004 All rights reserved. Confidential

Permission is hereby granted, without written agreement and without
license or royalty fees, to use and modify this software and its
documentation for the use with SIEMENS PC104 cards.

IN NO EVENT SHALL SIEMENS AG BE LIABLE TO ANY PARTY FOR DIRECT,
INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF SIEMENS AG
HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

SIEMENS AG SPECIFICALLY DISCLAIMS ANY WARRANTIES,INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN
"AS IS" BASIS, AND SIEMENS AG HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
**=====================================================================
** PROJECT::   TCN
** MODULE::    PC104 CLASS1.1-1.3
** WORKFILE::  cl1_1.h
**---------------------------------------------------------------------
** TASK::
   Header for Application Programmers Interface (API) for PC104 access
   in CLASS1.1

**---------------------------------------------------------------------
** AUTHOR::    e09renu0 
** CREATED::   30.04.1997
** REVISION::  2.10.00
**---------------------------------------------------------------------
** CONTENTS::
      functions:
         MVBCInit()
         MVBCStart()
         MVBCStop()
         MVBCGetPort()
         MVBCPutPort()
         MVBCSetDSW()
         MVBCRetriggerWd()
    #if defined (MVB_M)
         MVBCIdle11()
    #else
         MVBCIdle()
    #endif
         MVBCGetDeviceAddress()
**---------------------------------------------------------------------
** NOTES::     -
**---------------------------------------------------------------------
  
**********************************************************************/

#ifndef CL1_1_H_
#define CL1_1_H_


/*---------- port configuration list --------------------------------*/
/* The parameters required to open a traffic store port are          */
/* configured in the port configuration list.                        */

typedef struct STR_LP_PRT_CFG
{
    UNSIGNED16  prt_addr; /* The port index table is an array of port_nr.
                             This is the virtual connection between the
                             port address and the port number           */
    UNSIGNED16  prt_indx; /* The port index defines the port to use     */
    UNSIGNED16  size;     /* Defines the port size in bytes             */
    UNSIGNED16  type;     /* Defines the port type (sink / source)      */
} TYPE_LP_PRT_CFG;

/*---------- traffic store configuration ----------------------------*/
/* The parameters required to open a traffic store port are          */
/* configured in this type of list.                                  */

typedef struct STR_LP_TS_CFG
{
    UNSIGNED32  pb_pit;     /* External representation of a pointer to
                               the base address of the port index table */
    UNSIGNED32  pb_pcs;     /* External representation of a pointer to the
                               port control & status field base address */
    UNSIGNED32  pb_prt;     /* External representation of a pointer to
                               the port base address */
    UNSIGNED32  pb_frc;     /* External representation of a pointer to
                               the force table base address */
    UNSIGNED32  pb_def;     /* External representation of a pointer to
                               buffer with port default values.
                               NULL: Initialise all port bits to '1' */
    UNSIGNED8   ownership;  /* LP_CFG_TS_OWNED */
    UNSIGNED8   ts_type;    /* traffic store type */
    UNSIGNED16  dev_addr;   /* MVB Address */
    UNSIGNED32  tm_start;   /* Traffic store base address */
    UNSIGNED16  mcm;        /* Memory configuration mode */
    UNSIGNED16  msq_offset; /* Message queue offset */
    UNSIGNED16  mft_offset; /* Master frame table offset */
    UNSIGNED16  line_cfg;   /* Line configuration */
    UNSIGNED16  reply_to;   /* Reply timeout coefficients */
    UNSIGNED16  prt_addr_max; /* Port index table range is
                                 0 .... prt_addr_max */
    UNSIGNED16  prt_indx_max; /* Port range is 0 .... prt_indx_max */
    UNSIGNED16  prt_count;  /* The structure is terminated after
                               prt_count elements */
    UNSIGNED32  p_prt_cfg;  /* External representation of a pointer the
                                   port configuration (struct STR_LP_PRT_CFG)*/
} TYPE_LP_TS_CFG;


/*---------- values for access to control block ---------------------*/
#define LPC_TS_FREE    0
#define LPC_TS_IN_USE  1
#define LPC_TS_ERROR   2
#define LPC_STATE_MAX  3

/*---------- constants for PCS size and PCS type --------------------*/
#define LP_CFG_TS_ALIEN                  0
#define LP_CFG_TS_OWNED                  1
#define LP_CFG_SINK                      1
#define LP_CFG_SRCE                      2
#define LP_CFG_BIDIRECTIONAL             3
#define LP_CFG_02_BYTES                  2
#define LP_CFG_04_BYTES                  4
#define LP_CFG_08_BYTES                  8
#define LP_CFG_16_BYTES                 16
#define LP_CFG_32_BYTES                 32

/*---------- constants for PCS configuration and state info ---------*/
#define LPL_PCS_PASSIVE                 0
#define LPL_PCS_SINK                    LP_CFG_SINK
#define LPL_PCS_SRCE                    LP_CFG_SRCE
#define LPL_PCS_BDIR                    LP_CFG_BIDIRECTIONAL

/*---------- size of PCS for LP_TS_TYPE_B ---------------------------*/
#define LPL_PCS_B_SIZE_IN_PWR2          3

/*---------- constants to identify the TS structure types -----------*/
#define LP_TS_TYPE_B                    0x0001   /* MVB, MVBC        */
#define LP_PIT_TYPE_A                   0x0000   /*  8 Bit Port Indx */
#define LP_PIT_TYPE_B                   0x0001   /* 16 Bit Port Indx */

/*---------- masks for check of port address and index --------------*/
#define LP_PRT_ADDR_MAX_MASK            1  /* for check of max. addr */
#define LP_PRT_INDX_MAX_MASK            3  /* for check of max. index */

/*---------- constant to identify the bus (controller) --------------*/
#define LP_HW_TYPE_MVBC                 0x0001  /* MVB new Bus Controller */

/*---------- constants to identify the MVBC Type -----------------------*/
#define MVBCS1                          1    /* MVBC type is S1 - MVBCS1 */
#define MVBC_01                         0    /* MVBC type is 01 - MVBC01 */

/*------------- Line Interface Variant ------------------------------*/
#define TM_LIV_EMD                      0x0000  /* EMD (trafo) */
#define TM_LIV_OPTO                     0x0001  /* Optocoupler */

/*---------- function prototypes for internal use -------------------*/
UNSIGNED8 lc_tmo_config (
    UNSIGNED16   ts_id,     /* ID of traffic store                     */
    TM_TYPE_WORD stsr,      /* value for Sinktime Supervision Register */
    TM_TYPE_WORD docks      /* number of docks to be supervised        */
);

/*---------- function prototypes ------------------------------------*/
/* LINUX and Class 1.1 is not possible call MVBCConfigure if using LINUX ! */
extern UNSIGNED8 MVBCInit (
    TYPE_LP_TS_CFG *Configuration,  /* Pointer to configuration data */
    UNSIGNED16      ts_id           /* ID of traffic store           */
);
/*{
    The function initializes MVBC and traffic store.
    Return values:  MVB_NO_ERROR
                    MVB_UNKNOWN_TRAFFICSTORE : wrong TS ID
                    MVB_ERROR_PARA           : inconsistent or wrong params
                    MVB_ERROR_NO_MVBC        : MVBC not found
                    MVB_ERROR_MVBC_RESET     : MVBC not correct resetted
                    MVB_ERROR_NSDB           : NSDB problem
}*/


extern UNSIGNED8 MVBCGetSCRIL(
    UNSIGNED16 ts_id     /* ID of traffic store */
);
/*{
    The function get MVBC IL status.
    Return values:  TM_SCR_IL_RUNNING
                    TM_SCR_IL_TEST
                    TM_SCR_IL_CONFIG
                    TM_SCR_IL_RESET
}*/

extern UNSIGNED8 MVBCStart(
    UNSIGNED16 ts_id     /* ID of traffic store */
);
/*{
    The function gives the last kick to MVBC.
    Return values:  MVB_NO_ERROR
                    MVB_UNKNOWN_TRAFFICSTORE : wrong TS ID
                    MVB_ERROR_MVBC_INIT      : MVBC not initialized
                    MVBM_CONFIG_UNDEF        : only MVB-M: no configuration      
}*/


extern UNSIGNED8 MVBCStop (
    UNSIGNED16 ts_id         /* ID of traffic store */
);
/*{
    The function stops MVBC operation.
    Return values:  MVB_NO_ERROR
                    MVB_UNKNOWN_TRAFFICSTORE : wrong TS ID
                    MVB_ERROR_MVBC_INIT      : MVBC not initialized
                    MVBM_CONFIG_UNDEF        : only MVB-M: no configuration      
}*/

extern UNSIGNED8 MVBCGetPort(
    UNSIGNED16  Port,        /* Port number            */
    UNSIGNED16 *PortBuffer,  /* Pointer to port buffer */
    UNSIGNED16  Tack,        /* Limit for age of data  */
    UNSIGNED16 *Age,         /* Real age of data       */
    UNSIGNED16  ts_id        /* ID of traffic store    */
);
/*{
    The function reads data from MVBC port into the port buffer.
    Return values:  MVB_NO_ERROR
                    MVB_UNKNOWN_TRAFFICSTORE : wrong TS ID
                    MVB_ERROR_MVBC_STOP      : MVBC is not running
                    MVB_ERROR_PORT_UNDEF     : port not found in TS or port
                                               number/index not ok
                    MVB_WARNING_NO_SINK      : not a sink port
                    MVB_WARNING_OLD_DATA     : read data old
                    MVBM_CONFIG_UNDEF        : only MVB-M: no configuration      
}*/

extern UNSIGNED8 MVBCPutPort(
    UNSIGNED16  Port,        /* Port number            */
    UNSIGNED16 *PortBuffer,  /* Pointer to port buffer */
    UNSIGNED16  ts_id        /* Traffic Store ID       */
);
/*{
    The function writes data from the port buffer into MVBC port.
    Return values:  MVB_NO_ERROR
                    MVB_UNKNOWN_TRAFFICSTORE : wrong TS ID
                    MVB_ERROR_PORT_UNDEF     : port not found in TS or port
                                               number/index not ok
                    MVB_ERROR_NO_SRC         : not a source port
                    MVBM_CONFIG_UNDEF        : only MVB-M: no configuration      
}*/


extern UNSIGNED8 MVBCSetDSW(
    UNSIGNED16 ts_id,   /* ID of Traffic Store            */
    UNSIGNED16 mask,    /* Device Status Word Mask Bits   */
    UNSIGNED16 value    /* New values for affected fields */
);                     
/*{
    The function sets the Device-Status-Word.
    Only the bits 1, 4 and 5 may be affected.
    The other bits shall be left.
    Return values:  MVB_NO_ERROR
                    MVB_UNKNOWN_TRAFFICSTORE : wrong TS ID
}*/


#if defined (MVB_M)
extern UNSIGNED8 MVBCIdle11(
#else
extern UNSIGNED8 MVBCIdle(
#endif
    UNSIGNED16  ts_id       /* Traffic Store ID */
);
/*{
    The function shall be called cyclicaly.
    It set the DSW-Bits LAA, RLD.
    Return values:  MVB_NO_ERROR
                    MVB_UNKNOWN_TRAFFICSTORE : wrong TS ID
}*/


extern UNSIGNED8 MVBCGetDeviceAddress(
    UNSIGNED16  *devAddress,
    UNSIGNED16  ts_id        /* Traffic Store ID       */
);
/*{
    The function resolves the current device-address.
 
    Return values:  MVB_NO_ERROR
                    MVB_UNKNOWN_TRAFFICSTORE : wrong TS ID
}*/

extern UNSIGNED8 MVBCRetriggerWd(
    UNSIGNED16 ts_id, 
    UNSIGNED16 trig_val
);
/*{
   The function retriggers the MVBCS1 watchdog. Details s. MVBCS1 datasheet
   
   Return values:  MVB_NO_ERROR
                   MVB_UNKNOWN_TRAFFICSTORE : wrong TS ID
                   MVB_ERROR_MVBC_INIT      : watchdog has run out, MVBC is in init mode
                   MVB_WATCHDOG_NOT_AVAIL   : If mvbcs1 is not available then no watchdog function
                                              and returns with this error 

}*/

#ifdef O_LINUX
extern int MVBCGetMvbFd(UNSIGNED16  ts_id);
/*{
   The function returns the file descriptor for the apprpriate traffic store.
   Do not call before MVBCConfigure()!
}*/
#endif

#ifndef O_LINUX
extern VOL WORD16 *MVBCGetServiceAreaAddr(unsigned short ts_id);
/*{
  returns the start address of the service area of the MVBC.
}*/
#endif

#ifndef O_LINUX
extern VOL WORD16 *MVBCGetTSStartAddr(unsigned short ts_id);
/*{
  returns the start address of the traffic memory.
}*/
#endif

#endif
