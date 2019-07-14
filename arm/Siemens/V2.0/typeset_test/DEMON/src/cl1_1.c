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
** WORKFILE::  cl1_1.c
**---------------------------------------------------------------------
** TASK::
   Application Programmers Interface (API) for PC104 access.
   Functions for initialization / handling of MVBC and
   processing data in CLASS1.1

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
        MVBCIdle() or MVBCIdle11() when in MVBM-Mode
        MVBCGetDeviceAddress()
        MVBCRetriggerWd()
**---------------------------------------------------------------------
** NOTES::     -
**---------------------------------------------------------------------
  
**********************************************************************/

#ifdef O_LINUX
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/ioctl.h>
# include <errno.h>
#endif

#include "config.h"
#include "host_dep.h"
#include "dpr_dep.h"
#include "cl1_ret.h"
#include "cl1_1.h"

#ifdef O_LINUX
# include "../../drv/glo_inc/mvbdrv.h"
#endif


/*----------------- global defines ----------------------------------*/

/*---------- structure of the link layer process control block ------*/
/* This structure exists separately for each created traffic store.  */
/* The structure is accessed via the traffic store id.               */

TM_TYPE_WORD  test_src;

typedef struct  STR_LP_CB
{
    void *      pb_pit;         /* base ptr to port index table */
    void *      pb_pcs;         /* base ptr to port status & cntrl registers */
    void *      pb_prt;         /* base ptr to port data buffer */
    void *      pb_frc;         /* base ptr to force table */
    UNSIGNED16  prt_addr_max;   /* port address must not exceed this value */
    UNSIGNED16  prt_indx_max;   /* port index must not exceed this value */
    UNSIGNED8   ts_type;        /* type of traffic store */
    UNSIGNED8   hw_type;        /* hardware type */
    UNSIGNED8   ts_owner;       /* owner of traffic store */
    UNSIGNED8   pcs_power_of_2; /* size in powers of 2 */
    UNSIGNED8   irpt_sink;      /* handler for receive indication */
    UNSIGNED8   irpt_srce;      /* handler for sent indication is subscribed */
    UNSIGNED8   frc_flag;       /* forced data */
    UNSIGNED8   state;          /* init state */
} TYPE_LP_CB;

/*---------- macros and function prototypes which use TYPE_LP_CB ----*/
/* The macro returns the base pointer to the port control and status
   register. The parameter p_cb identifies the target traffic store.
   Return value:    pcs base pointer */
#define LPC_GET_PB_PCS(p_cb)  \
    (((TYPE_LP_CB*) (p_cb))->pb_pcs)

UNSIGNED16 temp_num;
/* The macro returns the pointer to the data buffer base.
   The parameter p_cb identifies the target traffic store.
   Return value:    data base pointer */
#define LPC_GET_PB_PRT(p_cb)  \
    (((TYPE_LP_CB*) (p_cb))->pb_prt)


/* The macro returns the size of the pcs in powers of 2.
   The parameter p_cb identifies the target traffic store.
   Return value:    size of pcs in powers of 2 */
#define LPC_GET_PCS_SIZE_IN_PWR_2(p_cb)  \
    (((TYPE_LP_CB*) (p_cb))->pcs_power_of_2)


/* The macro calculates and returns the byte offset between the base of
   the pcs array and the pcs defined by the port index in bytes.
   The parameter p_cb identifies the target traffic store.
   Return value:    byte offset */
#define LPL_GEN_PCS_OFFSET(p_cb, prt_indx)  \
    ((prt_indx) << LPC_GET_PCS_SIZE_IN_PWR_2(p_cb))


/* The macro returns the base pointer to the port index table.
   The parameter p_cb identifies the target traffic store.
   Return value:    PIT base pointer */
#define LPC_GET_PB_PIT(p_cb)  \
    (((TYPE_LP_CB*) (p_cb))->pb_pit)


/* The macro returns the highest supported port (dock) index within the
   traffic store. The parameter p_cb identifies the target traffic store.
   Return value:    port index */
#define LPC_GET_PRT_INDX_MAX(p_cb)  \
    (((TYPE_LP_CB*) (p_cb))->prt_indx_max)


/* The macro returns the traffic store type (A, B, C).
   The parameter p_cb identifies the target traffic store.
   Return value:    traffic store type */
#define LPC_GET_TS_TYPE(p_cb)  \
   (((TYPE_LP_CB*) (p_cb))->ts_type)


/* The macro returns the pit type (LP_PIT_TYPE_A, _B) for the port index
   table of the target traffic store. The parameter p_cb identifies the
   target traffic store.
   !!! Attention: relies on odd constants for 8 bit and even constants for
                 16 bit pit types!
   Return value:    pit type */
#define LPC_GET_PIT_TYPE(p_cb)  \
   (LPC_GET_TS_TYPE(p_cb) & 1)


/* The macro returns the highest supported port address within the traffic
   store. The parameter p_cb identifies the target traffic store.
   Return value:    port address */
#define LPC_GET_PRT_ADDR_MAX(p_cb)  \
   (((TYPE_LP_CB*) (p_cb))->prt_addr_max)


/* The macro returns the state of the traffic store initialisation.
   The parameter p_cb identifies the target traffic store.
   Return value:    state */
#define LPC_GET_STATE(p_cb)  \
    (((TYPE_LP_CB*) (p_cb))->state)


/* The macro saves the base pointer force table to the control block.
   The parameter p_cb identifies the target traffic store. */
#define LPC_PUT_PB_FRC(p_cb, p_frc)  \
    ( ((TYPE_LP_CB*) (p_cb))->pb_frc = p_frc )


/* The macro saves the base pointer port data buffer to the control block.
   The parameter p_cb identifies the target traffic store. */
#define LPC_PUT_PB_PRT(p_cb, p_prt)  \
    ( ((TYPE_LP_CB*) (p_cb))->pb_prt = p_prt )


/* The macro saves the base pointer port control & status to the control block.
   The parameter p_cb identifies the target traffic store. */
#define LPC_PUT_PB_PCS(p_cb, p_pcs)  \
    ( ((TYPE_LP_CB*) (p_cb))->pb_pcs = p_pcs )


/* The macro saves the base pointer port index table to the control block.
   The parameter p_cb identifies the target traffic store. */
#define LPC_PUT_PB_PIT(p_cb, p_pit)  \
    ( ((TYPE_LP_CB*) (p_cb))->pb_pit = p_pit )


/* The macro sets the ownership (own or alien traffic store). An access to
   an alien traffic store is regulated by the magic word mechanism.
   The parameter p_cb identifies the target traffic store. */
#define LPC_PUT_TS_OWNER(p_cb, value)  \
    ( ((TYPE_LP_CB*) (p_cb))->ts_owner = value )


/* The macro stores the hardware type for the target traffic store control
   block. The parameter p_cb identifies the target traffic store. */
#define LPC_PUT_HW_TYPE(p_cb, my_hw_type)  \
    ( ((TYPE_LP_CB*) (p_cb))->hw_type = my_hw_type )


/* The macro "generates" and returns the byte offset between the base
   address of the array port index table and the element referenced
   by the prt_addr dependent on the PIT type.
   The parameter p_cb identifies the target traffic store.
   Return value:    byte offset */
#define LPL_GEN_PIT_OFFSET(p_cb, prt_addr)  \
    (LPC_GET_PIT_TYPE(p_cb) == LP_PIT_TYPE_A ? (prt_addr) : ((prt_addr) << 1))


/*---------- traffic store control block ----------------------------*/
/* Describes location and size of supported Traffic Store            */
/* The structure is accessed via the traffic store id.               */

typedef struct
{
    void                 *p_tm;         /* Pointer to Traffic Memory */
    TM_TYPE_SERVICE_AREA *p_sa;         /* Pointer to Service Area   */
    UNSIGNED16            checklist;    /* Init state for checking   */
    UNSIGNED8             mvbc_type;    /* MVBC Controller type
                                           MVBC01-0
                                           MVBCS1-1                  */
} LCI_TYPE_CTRL_BLK;


/* values for checklist */
#define LCI_CHK_INIT  0x00 /* Initialized with lc_init               */
#define LCI_CHK_LC    0x01 /* Link Layer Common checked in           */


/* The macro calculates and returns the offset in bytes of the data port
   within the data port buffer.
   This is done using following algorithm:
   16 * (prt_indx - (prt_indx % 4)) + 8 * (prt_indx % 4)
   Return value:    byte offset */
#define LPL_GEN_PRT_OFFSET(prt_indx)  \
    ((((UNSIGNED16)(prt_indx) & 0xfffc) << 4) + (((prt_indx) & 0x3) << 3))


/* The macro checks whether port index max is an allowed value
   (count in fours)
   Return values:   MVB_LPS_OK
                    MVB_LPS_CONFIG if error */
#define LPL_CHK_PRT_INDX_MAX(prt_indx)  \
   ((((prt_indx) & LP_PRT_INDX_MAX_MASK) ==  \
     LP_PRT_INDX_MAX_MASK) ? MVB_LPS_OK : MVB_LPS_CONFIG)


/* The macro checks whether port address max is an allowed value
   (Count in twos)
   Return values:   MVB_LPS_OK
                    MVB_LPS_CONFIG if error */
#define LPL_CHK_PRT_ADDR_MAX(prt_addr)  \
   ((((prt_addr) & LP_PRT_ADDR_MAX_MASK) ==  \
     LP_PRT_ADDR_MAX_MASK) ? MVB_LPS_OK: MVB_LPS_CONFIG)


/*---------- global variables ---------------------------------------*/

#if defined (MVB_M)
extern UNSIGNED8    gMVBMState;
extern UNSIGNED8    gbAPIInternal;
#endif

#ifdef O_LINUX
int mvb_fd[LCX_TM_COUNT]; /* file descriptors for MVB driver access */

unsigned short port_sizes[TM_PORT_COUNT] = {0};
#else

/* Traffic Store Control Block */
static LCI_TYPE_CTRL_BLK lci_ctrl_blk[LCX_TM_COUNT];

/* The control block stores the process control information.
   Each traffic store is identified by the index "ts_id" (0....LCX_TM_COUNT-1).
*/
static TYPE_LP_CB        lp_cb[LCX_TM_COUNT];

#endif

/*---------- global constants ---------------------------------------*/

#ifdef O_LINUX
/* no consts */
#else
/* Waitstates for mvbc access */
static const UNSIGNED16 lci_waitstates  [LCX_TM_COUNT] = LCX_WAITSTATES;

/* Arbitration-Modes for mvbc access */
static const UNSIGNED16 lci_arb_modes  [LCX_TM_COUNT] = LCX_ARB_MODE;

/* Offset constants for Service Area */
static const UNSIGNED32 tm_sa_offs [] = TM_SERVICE_OFFSETS;

#endif

/******************************************************************************
*******************************************************************************
*******************************************************************************

                                 internal functions  

*******************************************************************************
*******************************************************************************
******************************************************************************/

#ifdef O_LINUX
/* none */
#else

/****************************************************************************
**  name         : lpl_tgl_prt_page
**===========================================================================
**  parameters   : p_pcs: Pointer to port control and status register

**  return value : -

**  description  : The pcs page bit is toggled

**  caveats      : -
*****************************************************************************/
static void lpl_tgl_prt_page(void *p_pcs)
{
    TM_TYPE_PCS_WORD1 pcs1;
    VOL int i = 1;
    pcs1.w = readWordFromTS((void*)(&((TM_TYPE_PCS *) p_pcs)->word1.w));
    pcs1.w ^= TM_PCS_VP_MSK;
    writeWordToTS((void*)(&((TM_TYPE_PCS *) p_pcs)->word1.w), pcs1.w);
    i = i + 1;
    writeWordToTS((void*)(&((TM_TYPE_PCS *) p_pcs)->word1.w), pcs1.w);
    return;
} /* lpl_tgl_prt_page */


/****************************************************************************
**  name         : lpc_put_state
**===========================================================================
**  parameters   : p_cb: Pointer to traffic store control block
                   state: to save

**  return value : -

**  description  : This function is used during initialisation to declare a 
                   traffic store "in use".

**  caveats      : -
*****************************************************************************/
static void lpc_put_state (
    void *p_cb,             /* Pointer to traffic store control block */
    UNSIGNED8 state         /* state to save */
)
{
    if (state <= LPC_STATE_MAX)
    {
        ((TYPE_LP_CB*) p_cb)->state = state;
    }
    else
    {
        ((TYPE_LP_CB*) p_cb)->state = LPC_TS_ERROR;
    }
    return;
} /* lpc_put_state */


/****************************************************************************
**  name         : lpc_put_ts_type
**===========================================================================
**  parameters   : p_cb: Pointer to control block
                   ts_type: Type of Traffic Store

**  return value : MVB_LPS_OK
                   MVB_LPS_ERROR

**  description  : The function decodes the traffic store type and stores it as 
                   type of the individual traffic store elements pit, pcs and prt.
                   The pcs_power_of_2 is the coded size of the pcs in bytes (4/8 Bytes).

**  caveats      :  -
*****************************************************************************/
static UNSIGNED8 lpc_put_ts_type (
    void *p_cb,                 /* Pointer to control block */
    UNSIGNED8 ts_type           /* Type of Traffic Store */
)
{
    UNSIGNED8 result = MVB_LPS_OK;

    ((TYPE_LP_CB *)p_cb)->ts_type = ts_type;

    if  (ts_type == LP_TS_TYPE_B)
    {
        ((TYPE_LP_CB*) p_cb)->pcs_power_of_2 = LPL_PCS_B_SIZE_IN_PWR2;
    }
    else
    {
        result = MVB_LPS_ERROR;
    }

    return result;
} /* lpc_put_ts_type */


/****************************************************************************
**  name         : lpc_put_max
**===========================================================================
**  parameters   :  p_cb: Pointer to control block
                    prt_addr_max: Maximum Port Address
                    prt_indx_max: Maximum Port Index

**  return value : -

**  description  : The function stores max values for traffic store 
                   prt_max is used for range checking and to derive size of force table.

**  caveats      : Attention, prt_max must be the most significant port of
                   a 4 port dock set ( x x x x x x 1 1 ).
*****************************************************************************/
static void lpc_put_max (
    void *p_cb,             /* Pointer to control block */
    UNSIGNED16 prt_addr_max,
    UNSIGNED16 prt_indx_max
)
{
    ((TYPE_LP_CB *) p_cb)->prt_addr_max = prt_addr_max;
    ((TYPE_LP_CB *) p_cb)->prt_indx_max = prt_indx_max;
} /* lpc_put_max */


/****************************************************************************
**  name         : lpc_open
**===========================================================================
**  parameters   : ts_id: Traffic Store ID
                   p_cb: Pointer to pointer of control block

**  return value : MVB_LPS_OK
                   MVB_LPS_CONFIG
                   MVB_LPS_UNKNOWN_TS_ID

**  description  : This function checks whether the traffic store identifier is 
                   defined and within range.
                   It returns the pointer to the control block of the target TS
**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lpc_open (
    UNSIGNED16 ts_id,       /* Traffic Store ID */
    void **p_cb             /* Pointer to pointer of control block */
)
{
    UNSIGNED8 result;

    *p_cb = (void *) &lp_cb[ts_id];

    if (ts_id < LCX_TM_COUNT)
    {
        if (LPC_GET_STATE (*p_cb) != LPC_TS_IN_USE)
        {
            result = MVB_LPS_CONFIG;
        }
        else
        {
            result = MVB_LPS_OK;
        }
    }
    else
    {
        result = MVB_LPS_UNKNOWN_TS_ID;
    }
    return result;
} /* lpc_open */


/****************************************************************************
**  name         : lpl_get_prt_indx
**===========================================================================
**  parameters   : mcm:      Memory Control Mode
                   pb_pit:   Pointer to port index table
                   prt_addr: Port address

**  return value : port index

**  description  : This function reads the prt_indx stored under to the prt_addr in
                   the array of character or word port index table (dependent of mcm).

**  caveats      : -
*****************************************************************************/
static UNSIGNED16 lpl_get_prt_indx(
    UNSIGNED16 mcm,     /* Memory Control Mode */
    void      *pb_pit,  /* Pointer to port index table*/
    UNSIGNED16 prt_addr /* Port address */
)
{
    if ((mcm == TM_MCM_16K) || (mcm == TM_MCM_32K))
    {   /* Port odd or even */
        if ( (prt_addr) & 0x0001)  /* odd */
        {
            UNSIGNED16 pit_word;
            pit_word = readWordFromTS((UNSIGNED16*) (pb_pit) +
                ((prt_addr-1) >> 1));
            return pit_word >> 8;
        }
        else
        {
            UNSIGNED16 pit_word;
            pit_word = readWordFromTS((UNSIGNED16*) (pb_pit) +
                ((prt_addr) >> 1));
            return pit_word & 0x00FF;
        }
    }
    else
    {
        return readWordFromTS((UNSIGNED16*) (pb_pit) + prt_addr);
    }
} /* lpl_get_prt_indx */


/****************************************************************************
**  name         : lpl_get_prt_fcode
**===========================================================================
**  parameters   : p_pcs: Pointer to PCS

**  return value : fcode

**  description  : The pcs fcode is read

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lpl_get_prt_fcode(
    void *p_pcs         /* Pointer to port control and status register */
)
{               
    UNSIGNED16 temp = readWordFromTS((void*)(&((TM_TYPE_PCS *) p_pcs)->word0.w));
    temp = (temp & TM_PCS_FCODE_MSK) >> TM_PCS_FCODE_OFF;
    return (UNSIGNED8)temp;
} /* lpl_get_prt_fcode */


/****************************************************************************
**  name         : lpl_put_prt_size
**===========================================================================
**  parameters   : p_pcs: Pointer to PCS
                   size:  Size

**  return value : -

**  description  : The function writes the function code of the port size to the pcs

**  caveats      : -
*****************************************************************************/
static void lpl_put_prt_size (
    void      *p_pcs,      /* Pointer to port control and status register */
    UNSIGNED16 size
)
{
    UNSIGNED16 fcode = 0;

    size = size >> 1;
    while ((size != 1) && (size != 0))
    {
        size = size >> 1;
        fcode++;
    }
    LPL_PUT_PRT_FCODE (p_pcs, fcode);
    return;
} /* lpl_put_prt_size */


/****************************************************************************
**  name         : lpl_put_prt_indx
**===========================================================================
**  parameters   : mcm:      Memory Control Mode
                   pb_pit:   Pointer to port index table
                   prt_addr: Port address
                   prt_indx: Port index

**  return value : -

**  description  : The 8 or 16 bit port index table (dependent of mcm) is loaded
                   with the port index under port address.

**  caveats      : -
*****************************************************************************/
static void lpl_put_prt_indx(
    UNSIGNED16  mcm,        /* Memory Control Mode */
    UNSIGNED16 *pb_pit,     /* Pointer to port index table */
    UNSIGNED16  prt_addr,   /* Port address */
    UNSIGNED16  prt_indx    /* Port index */
)
{
    if ((mcm == TM_MCM_16K) || (mcm == TM_MCM_32K))
    {   /* Port odd or even */
        if ( (prt_addr) & 0x0001)
        {
            UNSIGNED16 pit_word;
            pit_word = readWordFromTS((UNSIGNED16*) (pb_pit) +
                ((prt_addr-1) >> 1));
            pit_word &= 0x00FF;
            pit_word |= (UNSIGNED16)(prt_indx) << 8;
            writeWordToTS(((UNSIGNED16*) (pb_pit) + ((prt_addr-1) >> 1)),
                pit_word);
        }
        else
        {
            UNSIGNED16 pit_word;
            pit_word = readWordFromTS((UNSIGNED16*) (pb_pit) +
                ((prt_addr) >> 1));
            pit_word &= 0xFF00;
            pit_word |= (prt_indx);
            writeWordToTS(((UNSIGNED16*) (pb_pit) + ((prt_addr) >> 1)),
                pit_word);
        }
    }
    else /* other memory model */
    {
        writeWordToTS((UNSIGNED16*) (pb_pit) + prt_addr, prt_indx);
    }
    return;
} /* lpl_put_prt_indx */


/****************************************************************************
**  name         : lpl_put_def_values
**===========================================================================
**  parameters   : p_cb:  Pointer to control block
                   p_def: Long with pointer to default buffer

**  return value : -

**  description  : The function copies the default value buffer to the prt-table.

**  caveats      : -
*****************************************************************************/
static void lpl_put_def_values (
    void      *p_cb,    /* Pointer to control block */
    UNSIGNED32 p_def    /* Long with pointer to default buffer */
)
{
    UNSIGNED16 i;     /* loop control for port sets   */
    UNSIGNED16 j;     /* loop control for port size */
    UNSIGNED8 *p_prt;

    i     = (LPC_GET_PRT_INDX_MAX (p_cb) + 1) >> 2;
    p_prt =  LPC_GET_PB_PRT (p_cb);

    while (i > 0)
    {
        --i;
        j = sizeof(TYPE_LPL_PRT_PGE) >> 1;
        while (j > 0)
        {
            --j;
            writeWordToTS(&((TYPE_LPL_PRT_SET *) p_prt)->page_0,
                *P_OF_ULONG(UNSIGNED16*, p_def));
            writeWordToTS(&((TYPE_LPL_PRT_SET *) p_prt)->page_1,
                *P_OF_ULONG(UNSIGNED16*, p_def));
            p_def+=sizeof(UNSIGNED16);
        }
        p_prt += (2 * sizeof (TYPE_LPL_PRT_PGE));
    }
    return;
} /* lpl_put_def_values */


/****************************************************************************
**  name         : lpa_memset
**===========================================================================
**  parameters   : p_dest: Pointer to destination
                   value:  word value to write
                   size:   size in bytes

**  return value : -

**  description  : The function writes the parameter "value" to the traffic 
                   store of size.

**  caveats      : Attention: size must be even!
*****************************************************************************/
static void lpa_memset (
    void        *p_dest,    /* Pointer to destination */
    TM_TYPE_WORD value,     /* word value to write */
    UNSIGNED16   size       /* size in bytes */
)
{
    UNSIGNED16    count;
    TM_TYPE_WORD *p_trgt;

    p_trgt = (TM_TYPE_WORD*)p_dest;
    count  = size >> 1;

    SRVWDT();

    while (count > 0)
    {
        --count;
        writeWordToTS(p_trgt++, value);
    }

    SRVWDT();

    return;
} /* lpa_memset */
/*{
    The function writes the parameter "value" to the traffic store of size.
    !!! Attention: size must be even!
}*/


/****************************************************************************
**  name         : lci_check_ts_valid
**===========================================================================
**  parameters   : ts_id: Traffic Store ID

**  return value : MVB_LC_OK
                   MVB_LC_REJECT

**  description  : The function checks if ts_id is within specified boundaries 
                   and that the TS is properly configured

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lci_check_ts_valid (
    UNSIGNED16 ts_id         /* Traffic Store ID */
)
{
    UNSIGNED8 result = (ts_id < LCX_TM_COUNT) ? MVB_LC_OK : MVB_LC_REJECT;

    if (result == MVB_LC_OK)
    {
        result = (lci_ctrl_blk[ts_id].checklist != LCI_CHK_INIT) ?
            MVB_LC_OK : MVB_LC_REJECT;
    }
    return result;
} /* lci_check_ts_valid */


/****************************************************************************
**  name         : lc_set_device_status_word
**===========================================================================
**  parameters   : ts_id: Traffic Store ID
                   mask:  Device Status Word Mask Bits
                   value: New values for affected fields

**  return value : MVB_LC_OK
                   MVB_LC_REJECT

**  description  : The function modifies Device Status Word from Traffic Store Service Area

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lc_set_device_status_word(
    UNSIGNED16 ts_id,   /* Traffic Store ID               */
    UNSIGNED16 mask,    /* Device Status Word Mask Bits   */
    UNSIGNED16 value    /* New values for affected fields */
)
{
    UNSIGNED8 result = MVB_LC_OK;
    /* DSW accessed to TM: Data Area of Phys. Port FC15, Page 0, Word 0 */

    /* Data Area of Physical Ports */
    TM_TYPE_DATA *p_data = (TM_TYPE_DATA*)(lci_ctrl_blk[ts_id].p_sa->pp_data);

    /* Pointer to Device Status Word */
    TM_TYPE_WORD *p_dsw = &TM_1_DATA_WD(p_data,TM_PP_FC15,TM_PAGE_0,0);

    if (lci_check_ts_valid(ts_id) == MVB_LC_OK)
    {
        writeWordToTS(p_dsw,
            (TM_TYPE_WORD)((readWordFromTS(p_dsw) & ~(mask)) | ((value) & (mask))));
    }
    else
    {
        result = MVB_LC_REJECT;
    }
    return result;
} /* lc_set_device_status_word */


/****************************************************************************
**  name         : lc_set_laa_rld
**===========================================================================
**  parameters   :  -

**  return value : MVB_LC_OK
                   return values of lc_set_device_status_word()

**  description  : The function retrieves LAA and RLD from MVBC Decoder Register
                   and updates it in the Device Status Word

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lc_set_laa_rld (void)
{
    UNSIGNED16    i;                   /* Loop Index                        */
    TM_TYPE_WORD dr;                  /* Contents of Decoder Register      */
    UNSIGNED16   laa;                 /* Line A Active                     */
    UNSIGNED16   rld;                 /* Redundant Line Disturbed          */
    UNSIGNED8    result = MVB_LC_OK;

    for (i = 0; (i < LCX_TM_COUNT) && (result == MVB_LC_OK); i++)
    {   /* All control blocks */
        if (lci_ctrl_blk[i].checklist != LCI_CHK_INIT)
        {
            dr = readWordFromMVBC((void*)(&lci_ctrl_blk[i].p_sa->int_regs.dr.w));

            laa  = (dr & TM_DR_LAA) != 0 ? LC_DSW_LAA_SET : LC_DSW_LAA_CLR;
            rld  = (dr & TM_DR_RLD) != 0 ? LC_DSW_RLD_SET : LC_DSW_RLD_CLR;

            result = lc_set_device_status_word(
                i, LC_DSW_LAA_MSK | LC_DSW_RLD_MSK, (UNSIGNED16)(laa | rld) );
        }
    }
    return result;
} /* lc_set_laa_rld */


/****************************************************************************
**  name         : lp_ts_open_port
**===========================================================================
**  parameters   : mcm:   Memory Control Mode
                   p_cb:  Pointer to control block
                   p_cfg: Pointer to ts configuration

**  return value : MVB_LPS_OK
                   MVB_LPS_CONFIG

**  description  : This function completes PIT and PCS for each port in p_cfg

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lp_ts_open_port (
    UNSIGNED16             mcm,     /* Memory Control Mode           */
    void                  *p_cb,    /* Pointer to control block      */
    const TYPE_LP_PRT_CFG *p_cfg    /* Pointer to port configuration */
)
{
    UNSIGNED8  *p_pcs;     /*   temp pointer pcs */
    void       *pb_pit;
    UNSIGNED16  prt_addr;
    UNSIGNED16  prt_indx;
    UNSIGNED16  prt_type;
    UNSIGNED16  prt_size;
    UNSIGNED8   result = MVB_LPS_OK;

    prt_addr = p_cfg->prt_addr;
    prt_indx = p_cfg->prt_indx;
    prt_type = p_cfg->type;
    prt_size = p_cfg->size;

    pb_pit   = LPC_GET_PB_PIT (p_cb);
    p_pcs    = (((UNSIGNED8*) LPC_GET_PB_PCS (p_cb)) +
        LPL_GEN_PCS_OFFSET (p_cb, prt_indx));

    result = !((prt_addr <= LPC_GET_PRT_ADDR_MAX (p_cb)) &&
        (prt_indx <= LPC_GET_PRT_INDX_MAX (p_cb)));
    /* port address / port index ??? */
    if (result != MVB_LPS_OK)
    {
        return (MVB_LPS_CONFIG);
    }

    result = ((prt_type != LPL_PCS_PASSIVE) &&
        (prt_type != LPL_PCS_SINK   ) &&
        (prt_type != LPL_PCS_SRCE   ) &&
        (prt_type != LPL_PCS_BDIR   ));
    /* port type error               */

    if (result == MVB_LPS_OK)
    {
        lpl_put_prt_indx (mcm, pb_pit, prt_addr, prt_indx);
        lpl_put_prt_size (p_pcs, prt_size);
        LPL_PUT_PRT_TYPE (p_pcs, prt_type);
    }
    else
    {
        result = MVB_LPS_CONFIG;
    }

    return result;
} /* lp_ts_open_port */


/****************************************************************************
**  name         : lpl_clr_pit
**===========================================================================
**  parameters   : p_cb:  Pointer to control block

**  return value : -

**  description  : The function clears the port address table. This will have 
                   the affect of closing all ports in this traffic store

**  caveats      : -
*****************************************************************************/
static void lpl_clr_pit (
    void *p_cb      /* Pointer to control block */
)
{
    void   *pb_pit;
    UNSIGNED16  pit_size;

    if ((pb_pit  = LPC_GET_PB_PIT (p_cb)) != NULL)
    {
        pit_size = LPL_GEN_PIT_OFFSET (p_cb,
            LPC_GET_PRT_ADDR_MAX (p_cb) + 1);
        lpa_memset (pb_pit, 0, pit_size);
    }
    return;
} /* lpl_clr_pit */


#ifndef CLASS2 /* if CLASS2: the force area is used for message queues */
/****************************************************************************
**  name         : lp_ts_create_frc
**===========================================================================
**  parameters   : p_cb:  Pointer to control block
                   p_cfg: Pointer to ts configuration

**  return value : MVB_LPS_OK
                   MVB_LPS_CONFIG

**  description  : This function creates a zero-initialised force table array

**  caveats      : The forch table area is also used by Message data (if there any)
*****************************************************************************/
static UNSIGNED8 lp_ts_create_frc (
    void *p_cb,                 /* Pointer to control block    */
    const TYPE_LP_TS_CFG *p_cfg /* Pointer to ts configuration */
)
{
    UNSIGNED16  frc_size;
    void       *p_frc;
    UNSIGNED8   result = MVB_LPS_OK;

    frc_size = LPL_GEN_PRT_OFFSET (LPC_GET_PRT_INDX_MAX (p_cb) + 1);

    if ((p_frc = P_OF_ULONG(void*, p_cfg->pb_frc)) == NULL)
    {
        result = MVB_LPS_CONFIG;
    }

    LPC_PUT_PB_FRC (p_cb, p_frc);

    if (result == MVB_LPS_OK)
    {
        lpa_memset (p_frc, 0, frc_size);
    }

    return result;
} /* lp_ts_create_frc */
#endif


/****************************************************************************
**  name         : lp_ts_create_prt
**===========================================================================
**  parameters   : p_cb:  Pointer to control block
                   p_cfg: Pointer to ts configuration

**  return value : MVB_LPS_OK
                   MVB_LPS_CONFIG

**  description  : This function creates a one-initialised prt data buffer array

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lp_ts_create_prt (
    void *p_cb,                 /* Pointer to control block    */
    const TYPE_LP_TS_CFG *p_cfg /* Pointer to ts configuration */
)
{
    UNSIGNED16 prt_size;
    void      *p_prt;
    UNSIGNED8  result = MVB_LPS_OK;

    prt_size = LPL_GEN_PRT_OFFSET (LPC_GET_PRT_INDX_MAX (p_cb) + 1);

    if ((p_prt = P_OF_ULONG(void*, p_cfg->pb_prt)) == NULL)
    {
        result = MVB_LPS_CONFIG;
    }

    LPC_PUT_PB_PRT (p_cb, p_prt);

    if ((p_cfg->ownership == LP_CFG_TS_OWNED) &&
        (result           == MVB_LPS_OK))
    {
        if (p_cfg->pb_def != NULL)
        {
            lpl_put_def_values (p_cb, p_cfg->pb_def);
        }
        else
        {
            lpa_memset (p_prt, 0xFFFF, prt_size);
        }
    }
    return result;
} /* lp_ts_create_prt */


/****************************************************************************
**  name         : lp_ts_create_pcs
**===========================================================================
**  parameters   : p_cb:  Pointer to control block
                   p_cfg: Pointer to ts configuration

**  return value : MVB_LPS_OK
                   MVB_LPS_CONFIG

**  description  : This function creates a zero-initialised pcs array

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lp_ts_create_pcs (
    void *p_cb,                 /* Pointer to control block    */
    const TYPE_LP_TS_CFG *p_cfg /* Pointer to ts configuration */
)
{
    UNSIGNED16 pcs_size;
    void      *p_pcs;
    UNSIGNED8  result = MVB_LPS_OK;

    pcs_size = LPL_GEN_PCS_OFFSET (p_cb, LPC_GET_PRT_INDX_MAX (p_cb) + 1);

    if ((p_pcs = P_OF_ULONG(void*, p_cfg->pb_pcs)) == NULL)
    {
        result = MVB_LPS_CONFIG;
    }

    LPC_PUT_PB_PCS (p_cb, p_pcs);

    if ((p_cfg->ownership == LP_CFG_TS_OWNED) &&
        (result           == MVB_LPS_OK))
    {
        lpa_memset (p_pcs, 0, pcs_size);
    }

    return result;
} /* lp_ts_create_pcs */


/****************************************************************************
**  name         : lp_ts_create_pit
**===========================================================================
**  parameters   : p_cb:  Pointer to control block
                   p_cfg: Pointer to ts configuration

**  return value : MVB_LPS_OK
                   MVB_LPS_CONFIG

**  description  : This function creates a zero-initialised port index table

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lp_ts_create_pit (
    void *p_cb,                 /* Pointer to control block    */
    const TYPE_LP_TS_CFG *p_cfg /* Pointer to ts configuration */
)
{
    void      *p_pit;
    UNSIGNED8  result = MVB_LPS_OK;

    if ((p_pit = P_OF_ULONG(void*, p_cfg->pb_pit)) == NULL)
    {
        result = MVB_LPS_CONFIG;
    }

    LPC_PUT_PB_PIT (p_cb, p_pit);

    if ((p_cfg->ownership == LP_CFG_TS_OWNED) &&
        (result           == MVB_LPS_OK))
    {
        lpl_clr_pit (p_cb);
    }

    return result;
} /* lp_ts_create_pit */


/****************************************************************************
**  name         : lp_ts_create
**===========================================================================
**  parameters   : p_cb:  Pointer to control block
                   p_cfg: Pointer to ts configuration

**  return value : MVB_LPS_OK
                   MVB_LPS_CONFIG

**  description  : The function creates a traffic store

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lp_ts_create (
    void *p_cb,                 /* Pointer to control block    */
    const TYPE_LP_TS_CFG *p_cfg /* Pointer to ts configuration */
)
{
    UNSIGNED8 result = MVB_LPS_OK;

    result = (LPL_CHK_PRT_ADDR_MAX (p_cfg->prt_addr_max) != MVB_LPS_OK);
    /* port address max ??? */
    if (result != MVB_LPS_OK)
    {
        return (MVB_LPS_CONFIG);
    }
    result = (LPL_CHK_PRT_INDX_MAX (p_cfg->prt_indx_max) != MVB_LPS_OK);
    /* port index   max ??? */
    if (result != MVB_LPS_OK)
    {
        return (MVB_LPS_CONFIG);
    }

    lpc_put_max (p_cb, p_cfg->prt_addr_max, p_cfg->prt_indx_max);

    if (lpc_put_ts_type (p_cb, p_cfg->ts_type) != MVB_LPS_OK)
    {
        result = MVB_LPS_ERROR;
    }
    else if (lp_ts_create_pit (p_cb, p_cfg) != MVB_LPS_OK)
    {
        result = MVB_LPS_ERROR;
    }
    else if (lp_ts_create_pcs (p_cb, p_cfg) != MVB_LPS_OK)
    {
        result = MVB_LPS_ERROR;
    }
    else if (lp_ts_create_prt (p_cb, p_cfg) != MVB_LPS_OK)
    {
        result = MVB_LPS_ERROR;
    }
#ifndef CLASS2 /* if CLASS2: the force area is used for message queues */
    else if (lp_ts_create_frc (p_cb, p_cfg) != MVB_LPS_OK)
    {
        result = MVB_LPS_ERROR;
    }
#endif
    else
    {
        LPC_PUT_TS_OWNER (p_cb, p_cfg->ownership);
    }

    return result;
} /* lp_ts_create */


/****************************************************************************
**  name         : lpc_kill_ts
**===========================================================================
**  parameters   : p_cb: Pointer to control block

**  return value : -

**  description  : The function clears the control block for the opened ts_id

**  caveats      : -
*****************************************************************************/
static void lpc_kill_ts (
    void *p_cb      /* Pointer to control block */
)
{
    UNSIGNED16  count;
    UNSIGNED8  *p_trgt;

    p_trgt = (UNSIGNED8*) p_cb;
    count  = sizeof(TYPE_LP_CB);

    while (count > 0)
    {
        --count;
        *p_trgt++ = 0;
    }
    return;
} /* lpc_kill_ts */


/****************************************************************************
**  name         : lp_create
**===========================================================================
**  parameters   : ts_id:    Traffic Store ID
                   hw_type:  Type of Hardware
                   p_ts_cfg: Pointer to ts configuration

**  return value : MVB_LPS_OK
                   MVB_LPS_UNKNOWN_TS_ID
                   return values of lp_ts_create()
                   return values of lp_ts_open_port()

**  description  : The function creates a traffic store, announces the traffic 
                   store to control block and starts the bus hardware

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lp_create (
    UNSIGNED16            ts_id,    /* Traffic Store ID            */
    UNSIGNED8             hw_type,  /* Type of Hardware            */
    const TYPE_LP_TS_CFG *p_ts_cfg  /* Pointer to ts configuration */
)
{
    const TYPE_LP_PRT_CFG *p_prt_cfg;
    void                  *p_cb;
    UNSIGNED16             prt_count;
    TM_TYPE_MCR            local_mcr;/*Local copy of Memory Config. Reg.*/
    UNSIGNED8              result = MVB_LPS_OK;

    result = lpc_open (ts_id, &p_cb);

    if (result == MVB_LPS_UNKNOWN_TS_ID)
    {                                          /* Traffic Store ID ??? */
        return result;
    }
    lpc_kill_ts (p_cb);

    LPC_PUT_HW_TYPE (p_cb, hw_type);

    result = lp_ts_create (p_cb, p_ts_cfg);

    prt_count = p_ts_cfg->prt_count;
    p_prt_cfg = P_OF_ULONG(const TYPE_LP_PRT_CFG*, p_ts_cfg->p_prt_cfg);
    local_mcr.w = readWordFromMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.mcr.w));

    while   ((prt_count > 0) && (result == MVB_LPS_OK))
    {
        --prt_count;
        result    = lp_ts_open_port((UNSIGNED16)(GETMCRMCM(local_mcr)), p_cb, p_prt_cfg);
        p_prt_cfg = p_prt_cfg + 1;
    }

    if (result == MVB_LPS_OK)
    {
        lpc_put_state (p_cb, LPC_TS_IN_USE);
    }
    else
    {
        if (p_ts_cfg->prt_count != 0)
        {
            lpl_clr_pit (p_cb);
        }

        lpc_kill_ts (p_cb);
    }

    return result;
} /* lp_create */


/****************************************************************************
**  name         : lc_hardw_config
**===========================================================================
**  parameters   : ts_id: Traffic Store ID
                   line_config: MVB line configuration
                   line_config      = { LC_CH_A, LC_CH_B, LC_CH_BOTH }
                   treply_config: MVB reply timeout configuration
                   treply_config    = { LC_TREPLY_21US, LC_TREPLY_43US,
                                        LC_TREPLY_64US, LC_TREPLY_85US }
**  return value : MVB_LC_OK
                   MVB_LC_REJECT

**  description  : This function performs additional HW Configuration.

**  caveats      : Call is optional, not mandatory.  Default values are
                   LC_CH_BOTH and LC_REPLY_43US, active since MVBC reset.
                   Attention: lc_config must be called before.
*****************************************************************************/
static UNSIGNED8 lc_hardw_config (
    UNSIGNED16 ts_id,           /* Traffic Store ID                */
    UNSIGNED16 line_config,     /* MVB line configuration          */
    UNSIGNED16 treply_config    /* MVB reply timeout configuration */
)
{
    TM_TYPE_INT_REGS *p_ir;         /* MVBC Internal Registers           */
    TM_TYPE_SCR       local_scr;    /* Local copy of SCR                 */
    TM_TYPE_SCR       testm_scr;    /* Local copy of SCR, Test Mode set  */
    TM_TYPE_RWORD    *p_dr;         /* Pointer to Decoder Register       */
    /* Timeout after trying to switch line*/
    UNSIGNED16        limit = LCI_LS_LIMIT;

    UNSIGNED16        exp_laa;      /* Exp. value of LAA after swtiching */
    UNSIGNED8         result = MVB_LC_OK; /* Return Value, default: OK   */


    if ( lci_check_ts_valid(ts_id) == MVB_LC_OK )
    {
        p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);
        local_scr.w = readWordFromMVBC((void*)(&p_ir->scr.w));
        /*
        ******************************
        * Reconfigure Reply Time     *
        ******************************
        */

        if (treply_config <= LC_TREPLY_85US)
        {
            SETSCRTMO(local_scr, treply_config);
            writeWordToMVBC((void*)(&p_ir->scr.w), local_scr.w);

            if (treply_config > LC_TREPLY_43US)
            {
                (void)lc_set_device_status_word(ts_id,LC_DSW_ERD_MSK,LC_DSW_ERD_SET);
            }
        }
        else
        {
            result = MVB_LC_REJECT;   /* Invalid value in treply_config    */
        }

        /*
        ******************************
        * Select Line                *
        ******************************
        */
        /* 15.03.02 Check for the MVBC card type and changed SLM/LS access */


        /* Line Switching may be influenced by following factors:            */
        /* RTI or BTI by MVBC which leads to additional line switch or RLD   */
        /* Therefore, multiple attempts to perform a successful line switch  */
        /* and then freeze the decoder input may be necessary.               */

        p_dr      = (TM_TYPE_RWORD*) &(p_ir->dr);
        testm_scr = local_scr;
        SETSCRIL(testm_scr, TM_SCR_IL_TEST);

        if (line_config == LC_CH_BOTH)
        {
            writeWordToMVBC((void*)p_dr, 0);     /* Deactivate SLM                    */
        }
        else
        { /* 15.03.02 Check for the MVBC card type and changed SLM/LS access */
            if (lci_ctrl_blk[ts_id].mvbc_type == MVBCS1)
            {
                if ( line_config == LC_CH_A )
                {
                    writeWordToMVBC((void*)p_dr, (TM_DR_LAA | TM_DR_LS | TM_DR_SLM));
                }
                else
                {
                    writeWordToMVBC((void*)p_dr, TM_DR_LS | TM_DR_SLM);
                }
            }
            else
            {
                exp_laa   =  ( line_config == LC_CH_A ) ? TM_DR_LAA : 0;
                writeWordToMVBC((void*)(&p_ir->scr.w), testm_scr.w);/* Test Mode = quiet MVB   */
                do
                {
                    writeWordToMVBC((void*)p_dr, TM_DR_LS);    /* Line Switch, then       */
                    writeWordToMVBC((void*)p_dr, TM_DR_SLM);   /* lock again              */
                }
                while ( ((readWordFromMVBC((void*)p_dr) & TM_DR_LAA) != exp_laa) &&
                    (limit-- > 0) );

                writeWordToMVBC((void*)(&p_ir->scr.w), local_scr.w); /* Restore original SCR  */
                if (limit==0) /* Problems with line switching      */
                {
                    result = MVB_LC_REJECT; /* Unsuccessful switchover           */
                }
            } /* end of else if it is not a mvbc_type(MVBCS1) */

        } /* end of else if it not LC_CH_BOTH */
        result = lc_set_laa_rld();  /* Update changes in DSW             */

    } /* if (lci_check_ts_valid ... */
    else
    {
        result = MVB_LC_REJECT;
    }

    return result;
} /* lc_hardw_config */


/****************************************************************************
**  name         : lci_port_init
**===========================================================================
**  parameters   : p_srv_area: Pointer to service area

**  return value : -

**  description  : This function configures the Device Status Source Port (FC15)

**  caveats      : -
*****************************************************************************/
static void lci_port_init (
    TM_TYPE_SERVICE_AREA *p_srv_area    /* Pointer to service area */
)
{
    /*
    ******************************
    * Initialize Physical Ports  *
    ******************************
    */

    lpa_memset ((void*)(p_srv_area->pp_pcs), 0, sizeof(p_srv_area->pp_pcs));
    lpa_memset (p_srv_area->pp_data, 0, sizeof(p_srv_area->pp_data));

    /* All physical ports are passive now */

    /*
    ******************************
    * Device Status Port    FC15 *
    ******************************
    */
	  //printf("MVBC device status port FC15 address: 0x%x\r\n", (void*)(&p_srv_area->pp_pcs[TM_PP_FC15].word0.w));
    writeWordToTS (
        (void*)(&p_srv_area->pp_pcs[TM_PP_FC15].word0.w),
        W_FC15      |           /* F-Code 15; Mandatory                    */
        TM_PCS0_SRC |           /* Active Source                           */
        TM_PCS0_NUM);           /* Data is numeric                         */

    writeWordToTS (
        (void*)(&p_srv_area->pp_pcs[TM_PP_FC15].word1.w),
        TM_PCS1_VP0);           /* Addresses Data Area page 0 only         */

    writeWordToTS (                 /* Device Status Report: Initial Value */
        &TM_1_DATA_WD(p_srv_area->pp_data,TM_PP_FC15,TM_PAGE_0,0), /*Word 0*/
        LC_DSW_TYPE_SPECIAL_CLR   | /* No special device                   */
        LC_DSW_TYPE_BUS_ADMIN_CLR | /* No bus administrator yet attached   */
        LC_DSW_TYPE_BRIDGE_CLR    | /* No bridge/gateway SW yet attached   */
        LC_DSW_TYPE_CLASS_2_3_CLR | /* No Link-Layer Message yet attached  */
        LC_DSW_LAA_SET            | /* Line A Active                       */
        LC_DSW_DNR_SET);            /* Device not (yet) ready              */
    return;
} /* lci_port_init */


/****************************************************************************
**  name         : lci_mvbc_init
**===========================================================================
**  parameters   : ts_id: Traffic Store ID

**  return value : MVB_LC_OK
                   MVB_ERROR_NO_MVBC
                   MVB_ERROR_MVBC_RESET

**  description  : This function performs hard MVBC Configuration and Initialization
                   Correct reset of MVBC is tested with loopback test.

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lci_mvbc_init (
    UNSIGNED16 ts_id         /* Traffic Store ID */
)
{
    TM_TYPE_INT_REGS *p_ir = NULL;      /* Internal Registers            */
    TM_TYPE_SCR       local_scr;        /* Local copy of Status Control Reg. */
    short             i;                /* Loop Index                        */

    /*
    ******************************
    * Reset MVBC                 *
    ******************************
    */
    local_scr.w =                 /* Reset MVBC                          */
        TM_SCR_IL_RESET |         /* Reset Mode                      */
        lci_arb_modes[ts_id]     |/* Arbitration Strategy                    */
        TM_SCR_TMO_43US  |        /* Default Timeout: 42.7 us                */
        lci_waitstates[ts_id];    /* Waitstate Specification                 */

    /* Assume MVBC is in unknown MCM, but within 64K */

    for (i = TM_MCM_64K; i >= TM_MCM_16K; i--)
    {
        lci_ctrl_blk[ts_id].p_sa = LCX_PTR_ADD_SEG( lci_ctrl_blk[ts_id].p_tm,
            tm_sa_offs[i] );
        p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);
			  
        writeWordToMVBC((void*)(&p_ir->scr.w), local_scr.w);
    }
    printf("Reset MVBC, SCR 0x%x with 0x%x\r\n", (void*)(&p_ir->scr.w), local_scr.w);
    /*
    ******************************
    * Status Control Register    *
    ******************************
    */
    local_scr.w =                 /* Configure MVBC                          */
        TM_SCR_IL_CONFIG |        /* Configuration Mode                      */
        lci_arb_modes[ts_id]     |/* Arbitration Strategy                    */
        ((LCX_INTEL_MODE == 0) ? 0 : TM_SCR_IM ) | 
        /* Intel or Motorola Mode                  */
        TM_SCR_RCEV      |        /* Event Polling always permitted          */
        TM_SCR_TMO_43US  |        /* Default Timeout: 42.7 us                */
        lci_waitstates[ts_id];    /* Waitstate Specification                 */

    writeWordToMVBC((void*)(&p_ir->scr.w), local_scr.w);
    printf("Configure MVBC, SCR 0x%x with 0x%x\r\n", (void*)(&p_ir->scr.w), local_scr.w);
    /*
    ******************************
    * Test if MVBC+SCR are OK    *
    ******************************
    */

    writeWordToMVBC((void*)(&p_ir->dpr), LCI_ALL_1);/* Write access to a MVBC-characteristic   */
		printf("Write MVBC-characteristic, DPR 0x%x with 0x%x\r\n", (void*)(&p_ir->dpr), LCI_ALL_1);
    /* register to check that MVBC and not a   */
    /* memory is accessed. Use 14-bit reg. DPR */
		test_src = readWordFromMVBC((void*)(&p_ir->scr.w));
		printf("Read MVBC, SCR 0x%x with 0x%x\r\n", (void*)(&p_ir->scr.w), test_src);

#ifndef MVBCEMU
    if ((readWordFromMVBC((void*)(&p_ir->scr.w)) != local_scr.w) ||
        (readWordFromMVBC((void*)(&p_ir->dpr)) != TM_DPR_MASK))
    {
        return MVB_ERROR_NO_MVBC;      /* MVBC not found */
    }

    /*
    ******************************
    * MVBC Interrupt Logic       *
    ******************************
    */

    /* The interrupt controller of the MVBC is already initialized by        */
    /* setting SCR to zero.  See code above                                  */

    /*
    ******************************
    * MVBC loopback test         *
    * See, if MVBC is resetted   *
    * properly                   *
    ******************************
    */
    /* Configure test ports */
    {
        TM_TYPE_SERVICE_AREA *p_sa;

        p_sa = (TM_TYPE_SERVICE_AREA*)
            (LCX_PTR_ADD_SEG(lci_ctrl_blk[ts_id].p_tm,tm_sa_offs[0]));

        writeWordToTS((void*)(&p_sa->pp_pcs[TM_PP_TSRC].word0.w),
            (TM_TYPE_WORD)W_FC1|TM_PCS0_SRC|TM_PCS0_NUM);
        writeWordToTS((void*)(&p_sa->pp_pcs[TM_PP_TSNK].word0.w),
            (TM_TYPE_WORD)W_FC1|TM_PCS0_SINK|TM_PCS0_NUM);
        writeWordToTS((void*)(&p_sa->pp_pcs[TM_PP_TSRC].word1.w),
            (TM_TYPE_WORD)TM_PCS1_VP0);
        writeWordToTS((void*)(&p_sa->pp_pcs[TM_PP_TSNK].word1.w),
            (TM_TYPE_WORD)TM_PCS1_VP0);

        writeWordToTS(&TM_1_DATA_WD(p_sa->pp_data,TM_PP_TSRC,TM_PAGE_0,0),
            (TM_TYPE_WORD)0xa55a);
        writeWordToTS(&TM_1_DATA_WD(p_sa->pp_data,TM_PP_TSNK,TM_PAGE_1,0),
            (TM_TYPE_WORD)0x0000);

        local_scr.w =               /* Configure MVBC                          */
            TM_SCR_IL_TEST  |       /* Configuration Mode                      */
            lci_arb_modes[ts_id] |  /* Arbitration Strategy                    */
            ((LCX_INTEL_MODE == 0) ? 0 : TM_SCR_IM ) |
            /* Intel or Motorola Mode                  */
            TM_SCR_RCEV     |       /* Event Polling always permitted          */
            TM_SCR_TMO_43US |       /* Default Timeout: 42.7 us                */
            TM_SCR_MAS      |       /* Self test sends MF */
            TM_SCR_UTQ      |
            TM_SCR_UTS      |
            lci_waitstates[ts_id];  /* Waitstate Specification                 */

        /* loopback test mode on */
        writeWordToMVBC((void*)(&p_ir->scr.w), local_scr.w);
				printf("Loopback test, SCR 0x%x with 0x%x\r\n", (void*)(&p_ir->scr.w), local_scr.w);
        Wait2MS();
        printf("Send test master frame to TS 0x%x with 0x%x\r\n", &p_sa->mfs, (TM_TYPE_WORD)0x1001);
				printf("Write 0x%x with 0x%x\r\n", (void*)(&p_ir->mr.w), (TM_TYPE_WORD)TM_MR_SMFM);
        /* send test master frame manually */
        writeWordToTS(&p_sa->mfs, (TM_TYPE_WORD)0x1001);
        writeWordToMVBC((void*)(&p_ir->mr.w), (TM_TYPE_WORD)TM_MR_SMFM);
        
        /* wait for response or timeout */
        do {
            Wait100US();
        } while (readWordFromMVBC((void*)(&p_ir->mr.w)) & TM_MR_BUSY);

        if (readWordFromTS(&TM_1_DATA_WD(p_sa->pp_data,TM_PP_TSNK,TM_PAGE_1,0))
            != 0xa55a)
        {
            return MVB_ERROR_MVBC_RESET;
        }            
        /* additional wait loop til BTI occured */
        Wait2MS();
			  printf("Loopback test is OK\r\n");
    }
#endif

    local_scr.w =               /* Configure MVBC                          */
        TM_SCR_IL_CONFIG |      /* Configuration Mode                      */
        lci_arb_modes[ts_id] |  /* Arbitration Strategy                    */
        ((LCX_INTEL_MODE == 0) ? 0 : TM_SCR_IM ) |
        /* Intel or Motorola Mode                  */
        TM_SCR_RCEV      |      /* Event Polling always permitted          */
        TM_SCR_TMO_43US  |      /* Default Timeout: 42.7 us                */
        lci_waitstates[ts_id];  /* Waitstate Specification                 */

    /* loopback teset mode off */
    writeWordToMVBC((void*)(&p_ir->scr.w), local_scr.w);
    //printf("Turn off loopback test mode\r\n");
    /* Check, if MVBCS1 is available */
    if ((readWordFromMVBC((void *)(&p_ir->mcr.w)) & TM_MCR_VERS_MASK) == TM_MCR_VERS_S1)
    {
        lci_ctrl_blk[ts_id].mvbc_type = MVBCS1;
    }
    else
    {
        lci_ctrl_blk[ts_id].mvbc_type = MVBC_01;
    }

    if (lci_ctrl_blk[ts_id].mvbc_type == MVBCS1) 
    {
       if ((readWordFromMVBC((void *)(&p_ir->liv)) & TM_LIV_OPTO) == TM_LIV_OPTO) {
          writeWordToMVBC((void *)(&p_ir->conf),  
             (TM_TYPE_WORD)(readWordFromMVBC((void *)(&p_ir->conf)) & ~TM_CONF_EMD));
          /* RECORD_M_EVENT("MVBC ESD (optocoupler) mode"); */
       } else {
          writeWordToMVBC((void *)(&p_ir->conf), 
             (TM_TYPE_WORD)(readWordFromMVBC((void *)(&p_ir->conf)) | TM_CONF_EMD));
          /* RECORD_M_EVENT("MVBC EMD (trafo) mode"); */
       }   
    }

    return MVB_LC_OK;
} /* lci_mvbc_init */


/****************************************************************************
**  name         : lc_config
**===========================================================================
**  parameters   : ts_id:           Traffic Store ID
                   p_tm_start_addr: Pointer to start of traffic store
                   mcm:             Memory Configuration Mode of MVBC
                   qo:              Queue Offset Value
                   mo:              Master-Frame Offset Value

**  return value : MVB_LC_OK
                   MVB_LC_REJECT

**  description  : This function initializes the LC Basic Structure

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lc_config (
    UNSIGNED16 ts_id,           /* Traffic Store ID                  */
    void      *p_tm_start_addr, /* Pointer to start of traffic store */
    UNSIGNED16 mcm,             /* Memory Configuration Mode of MVBC */
    UNSIGNED16 qo,              /* Queue Offset Value                */
    UNSIGNED16 mo               /* Master-Frame Offset Value         */
)
{
    UNSIGNED8 result = MVB_LC_REJECT;   /* Return Value, default: Reject     */

    TM_TYPE_WORD   *p_la_pit;           /* LA Port Index Table               */
    TM_TYPE_WORD   *p_da_pit;           /* DA Port Index Table               */
    TM_TYPE_WORD   *p_pcs;              /* Port Control + Status Register    */

    TM_TYPE_SERVICE_AREA  *p_srv_area;  /* Local pointer to Service Area  */
    TM_TYPE_MCR            local_mcr;   /* Local copy of Memory Config. Reg. */

    /* Offset values, specific to Traffic Memory structure */

    static const UNSIGNED32  la_pcs_addr[] = TM_LA_PCS_OFFSETS;
    static const UNSIGNED32  da_pcs_addr[] = TM_DA_PCS_OFFSETS;
    static const UNSIGNED32  la_pit_addr[] = TM_LA_PIT_OFFSETS;
    static const UNSIGNED32  da_pit_addr[] = TM_DA_PIT_OFFSETS;
    static const UNSIGNED16  pit_size   [] = TM_PIT_BYTE_SIZES;

    int mvbcinit;  /* counter for several resets */

    if (ts_id < LCX_TM_COUNT)
    {

        /* Initialize the Control Block */

        lci_ctrl_blk[ts_id].p_tm      = p_tm_start_addr;
        lci_ctrl_blk[ts_id].checklist = LCI_CHK_LC;

        /*
        ******************************
        * Configure MVBC             *
        ******************************
        */

        /* Try to init MVBC for some times if not fully reset */
        mvbcinit = 10;
        do
        {
            result = lci_mvbc_init(ts_id); /* Hard Initialization */
        } while (result != MVB_LC_OK && --mvbcinit);
        printf("Hard init MVBC result = %d\r\n", result);
        /* Check if MCM is valid according to maximum available TM size */

        if ( ((UNSIGNED16) mcm) > TM_MCM_64K)
        {
            result = MVB_LC_REJECT;
        }

        if (result == MVB_LC_OK)
        {
            local_mcr.w = 0;
            SETMCRMO(local_mcr, mo);
            SETMCRQO(local_mcr, qo);
            SETMCRMCM(local_mcr, mcm);
            printf("Config MCR 0x%x with 0x%x\r\n", (void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.mcr.w), local_mcr.w);
            writeWordToMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.mcr.w),
                local_mcr.w);
            /*
            ******************************
            * Compute Serv. Area Address *
            ******************************
            */

            p_srv_area = LCX_PTR_ADD_SEG( p_tm_start_addr, tm_sa_offs[mcm] );
            lci_ctrl_blk[ts_id].p_sa = p_srv_area;

            /*
            ******************************
            * Clear Port Index Tables    *
            ******************************
            */

            p_la_pit = LCX_PTR_ADD_SEG(p_tm_start_addr,la_pit_addr[mcm]);
            p_da_pit = LCX_PTR_ADD_SEG(p_tm_start_addr,da_pit_addr[mcm]);

            lpa_memset (p_la_pit, 0, pit_size[mcm]);
            /* Harmless action if MCM=0: p_la_pit = p_da_pit */
            lpa_memset (p_da_pit, 0, pit_size[mcm]);

            /*
            ******************************
            * Make default ports inactive*
            ******************************
            */

            /* Port Index 0 in the logical address space */

            p_pcs = LCX_PTR_ADD_SEG(p_tm_start_addr, la_pcs_addr[mcm]);
            writeWordToTS(p_pcs, 0); /* delete LA PCS(Port 0), Word 0 */
            p_pcs++;
            writeWordToTS(p_pcs, 0); /* delete LA PCS(Port 0), Word 1 */

            /* Port Index 0 in the device  address space  */
            /* Harmless action if MCM=0: p_pcs = p_la_pit */

            p_pcs = LCX_PTR_ADD_SEG(p_tm_start_addr, da_pcs_addr[mcm]);
            writeWordToTS(p_pcs, 0); /* delete DA PCS(Port 0), Word 0 */
            p_pcs++;
            writeWordToTS(p_pcs, 0); /* delete DA PCS(Port 0), Word 1 */

            /*
            ******************************
            * Initialize Physical Ports  *
            ******************************
            */

            lci_port_init( p_srv_area );

        } /* if (result... */
    }     /* if (ts_id...   */

    return result;
} /* lc_config */


/****************************************************************************
**  name         : lc_init
**===========================================================================
**  parameters   : ts_id: Traffic Store ID

**  return value : -

**  description  : This function initializes the LC Basic Structure
                   Clears entire control block for traffic store.

**  caveats      : MVBCs and Traffic Stores are not yet accessible.
*****************************************************************************/
static void lc_init (
    UNSIGNED16 ts_id     /* Traffic Store ID */
)
{
    UNSIGNED16  count;
    UNSIGNED8  *p_trgt;

    p_trgt = (UNSIGNED8*) &lci_ctrl_blk[ts_id];
    count  = sizeof(LCI_TYPE_CTRL_BLK);

    while (count > 0)
    {
        --count;
        *p_trgt++ = 0;
    }
    return;
} /* lc_init */


/****************************************************************************
**  name         : lpc_init
**===========================================================================
**  parameters   : ts_id: Traffic Store ID

**  return value : -

**  description  : The function clears the control block for the TS ID.

**  caveats      : All information is lost.
*****************************************************************************/
static void lpc_init (
    UNSIGNED16 ts_id     /* Traffic Store ID */
)
{
    UNSIGNED16  count;
    UNSIGNED8  *p_trgt;

    p_trgt = (UNSIGNED8*) &lp_cb[ts_id];
    count  = sizeof(TYPE_LP_CB);

    while (count > 0)
    {
        --count;
        *p_trgt++ = 0;
    }
    return;
} /* lpc_init */


/****************************************************************************
**  name         : lc_go
**===========================================================================
**  parameters   : ts_id: Traffic Store ID

**  return value : MVB_LC_OK
                   return values of lci_check_ts_valid()

**  description  : The function starts MVBC operation

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lc_go (
    UNSIGNED16 ts_id         /* Traffic Store ID */
)
{
    TM_TYPE_SCR local_scr;              /* Local copy of SCR                 */
    UNSIGNED8   result  = lci_check_ts_valid(ts_id);
    /* Return Value, check for valid ts  */

    if (result == MVB_LC_OK)            /* MVB Communication Kickoff !       */
    {
        (void)lc_set_laa_rld();              /* Valid LAA and RLD                 */

        local_scr.w = readWordFromMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.scr.w));
        SETSCRIL(local_scr, TM_SCR_IL_RUNNING);
        writeWordToMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.scr.w), local_scr.w);
    }
    return result;
} /* lc_go */


/****************************************************************************
**  name         : lc_set_device_address
**===========================================================================
**  parameters   : ts_id:       Traffic Store ID
                   dev_address: MVBC device address

**  return value : MVB_LC_OK
                   MVB_LC_REJECT

**  description  : The function applies new Device Address to MVBC

**  caveats      : -
*****************************************************************************/
static UNSIGNED8 lc_set_device_address (
    UNSIGNED16 ts_id,               /* Traffic Store ID    */
    UNSIGNED16 dev_address          /* MVBC device address */
)
{
    TM_TYPE_INT_REGS *p_ir;               /* MVBC Internal Registers        */
    UNSIGNED8         result = MVB_LC_REJECT;/* Return Value, default: Reject  */

    if ( lci_check_ts_valid(ts_id) == MVB_LC_OK )
    {
        if ((dev_address & LCI_DA_MASK) == dev_address)
        {
            p_ir = &lci_ctrl_blk[ts_id].p_sa->int_regs;
            writeWordToMVBC((void*)(&p_ir->daor), dev_address);
            writeWordToMVBC((void*)(&p_ir->daok), TM_DAOK_ENABLE);

            if (readWordFromMVBC((void*)(&p_ir->daor)) == dev_address)
            {   /* Check if successful */
                result = MVB_LC_OK;
            }
        }
    }
    return result;
} /* lc_set_device_address */


#endif

/******************************************************************************
*******************************************************************************
*******************************************************************************

                                 external functions  

*******************************************************************************
*******************************************************************************
******************************************************************************/


/****************************************************************************
**  name         : lc_tmo_config
**===========================================================================
**  parameters   : ts_id: Traffic Store ID
                   stsr:  value for Sinktime Supervision Register
                   docks: number of docks to be supervised
                   
**  return value : MVB_LC_OK
                   return values of lci_check_ts_valid()

**  description  : The function sets Sinktime Supervision on MVBC

**  caveats      : only to be used in cl1_2u3.c, so it is not static
*****************************************************************************/
#ifndef O_LINUX
UNSIGNED8 lc_tmo_config (
    UNSIGNED16   ts_id,     /* Traffic Store ID                        */
    TM_TYPE_WORD stsr,      /* value for Sinktime Supervision Register */
    TM_TYPE_WORD docks      /* number of docks to be supervised        */
)
{
    TM_TYPE_STSR local_stsr;
    UNSIGNED8    result = lci_check_ts_valid(ts_id);

    if (result == MVB_LC_OK)
    {
        local_stsr.w = readWordFromMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.stsr));
        SETSTSRDOCKS(local_stsr, docks);
        SETSTSRSI(local_stsr, stsr);
        writeWordToMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.stsr), local_stsr.w);
    } /* Ende if MVB_LC_OK */

    return result;
} /* Ende lc_tmo_config */
#endif

/****************************************************************************
**  name         : MVBCInit
**===========================================================================
**  parameters   : Configuration: Pointer to port configuration
                   ts_id:         Traffic Store ID

**  return value : MVB_ERROR_PARA
                   MVB_NO_ERROR
                   MVB_ERROR_NO_MVBC
                   MVB_ERROR_MVBC_RESET
                   
**  description  : The function initializes MVBC and traffic store

**  caveats      : -
*****************************************************************************/
UNSIGNED8 MVBCInit (TYPE_LP_TS_CFG *Configuration, UNSIGNED16 ts_id)
{
#ifdef O_LINUX
    unsigned int i;
    UNSIGNED16  prt_addr;
    UNSIGNED16  prt_size;
#else
    UNSIGNED8 result;
#endif    

    if (ts_id >= LCX_TM_COUNT)
    {
        return MVB_ERROR_PARA;
    }

#ifdef O_LINUX
    /* initialize the port_sizes array */
    for (i = 0; i < Configuration->prt_count; i++) {
      prt_addr = (((TYPE_LP_PRT_CFG *)(Configuration->p_prt_cfg)) + i)->prt_addr;
      prt_size = (((TYPE_LP_PRT_CFG *)(Configuration->p_prt_cfg)) + i)->size;
      port_sizes[prt_addr] = prt_size;
    
    }
#else
    lpc_init(ts_id);

    lc_init(ts_id);


    result = lc_config(ts_id,
        P_OF_ULONG(void*, Configuration->tm_start),
        Configuration->mcm,
        Configuration->msq_offset,
        Configuration->mft_offset);
    if (result != MVB_LC_OK)
    {
        if ((result != MVB_ERROR_NO_MVBC) && (result != MVB_ERROR_MVBC_RESET))
        {
            result = MVB_ERROR_PARA;
        }
        return result;
    }

    (void)lc_hardw_config(ts_id,
        Configuration->line_cfg,
        Configuration->reply_to);


    if (lc_set_device_address(ts_id, Configuration->dev_addr) != MVB_LC_OK)
    {
        return MVB_ERROR_NO_MVBC;
    }

    result = lp_create (ts_id, LP_HW_TYPE_MVBC, Configuration);
    /* result = lp_create (ts_id, Configuration->ts_type, Configuration); */
    /* RG280198: Hardwaretype fixed to MVBC-Type. No other one will work on PC/104 */
    if (result != MVB_LPS_OK)
    {
        return MVB_ERROR_PARA;
    }

    result = lc_tmo_config (ts_id, TM_STSR_INTERV, TM_STSR_DOCKS);
    if (result != MVB_LC_OK)
    {
        return  MVB_ERROR_PARA;
    }
#endif
    return MVB_NO_ERROR;
} /* MVBCInit */

/****************************************************************************
**  name         : MVBCGetSCRIL
**===========================================================================
**  parameters   : Configuration: Pointer to port configuration
                   ts_id:         Traffic Store ID

**  return value : MVB_ERROR_PARA
                   MVB_NO_ERROR
                   MVB_ERROR_NO_MVBC
                   MVB_ERROR_MVBC_RESET
                   
**  description  : The function initializes MVBC and traffic store

**  caveats      : -
*****************************************************************************/
UNSIGNED8 MVBCGetSCRIL(UNSIGNED16 ts_id) {
	UNSIGNED8  result;
  TM_TYPE_SCR local_scr;       /* Local copy of SCR                 */

  result = lci_check_ts_valid(ts_id);
  /* Return Value, check for valid ts  */
  if (result == MVB_LC_OK) {
    local_scr.w = readWordFromMVBC(
      (void*)(&(lci_ctrl_blk[ts_id].p_sa->int_regs.scr.w)));
		return GETSCRIL(local_scr);
	} else {
		return 0xFF;
	}
}


/****************************************************************************
**  name         : MVBCStart
**===========================================================================
**  parameters   : ts_id: Traffic Store ID

**  return value : MVB_NO_ERROR
                   MVBM_CONFIG_UNDEF
                   MVB_ERROR_MVBC_INIT
                   MVB_UNKNOWN_TRAFFICSTORE
                   
**  description  : The function starts MVBC operation

**  caveats      : -
*****************************************************************************/
UNSIGNED8 MVBCStart(UNSIGNED16 ts_id)
{
    UNSIGNED8   result;
#ifdef O_LINUX

    result = MVB_NO_ERROR;
# if defined(MVB_M)
    if (gMVBMState == MVBM_CONFIG_UNDEF)
    {
        return MVBM_CONFIG_UNDEF;
    }
# endif
    if (result == MVB_NO_ERROR) {
        int exit_st = ioctl(mvb_fd[ts_id], MVB_START, 0);
        if (exit_st != 0) {
            result = MVB_ERROR_MVBC_INIT;
        }
    }
#else
    TM_TYPE_SCR local_scr;       /* Local copy of SCR                 */

    result = lci_check_ts_valid(ts_id);
    /* Return Value, check for valid ts  */

    if (result == MVB_LC_OK)    /* MVB Communication Kickoff !       */
    {
        local_scr.w = readWordFromMVBC(
            (void*)(&(lci_ctrl_blk[ts_id].p_sa->int_regs.scr.w)));
        /* MVBC is initialized ? */
        if (GETSCRIL(local_scr) != TM_SCR_IL_RESET)
        {
            (void)lc_set_device_status_word(ts_id, LC_DSW_DNR_MSK, LC_DSW_DNR_CLR);
            (void)lc_go(ts_id);
            result = MVB_NO_ERROR;
#if defined(MVB_M)
            if (gMVBMState == MVBM_CONFIG_UNDEF)
            {
                result = MVBM_CONFIG_UNDEF;
            }
#endif
        }
        else
        {
            result = MVB_ERROR_MVBC_INIT;
        }
    }
    else
    {
        result = MVB_UNKNOWN_TRAFFICSTORE;
    }
#endif
    return result;
} /* MVBCStart */


/****************************************************************************
**  name         : MVBCStop
**===========================================================================
**  parameters   : ts_id: Traffic Store ID

**  return value : MVB_NO_ERROR
                   MVB_ERROR_MVBC_INIT
                   MVB_UNKNOWN_TRAFFICSTORE
                   
**  description  : The function stops MVBC operation

**  caveats      : -
*****************************************************************************/
UNSIGNED8 MVBCStop (UNSIGNED16 ts_id)
{
    UNSIGNED8    result;
#ifdef O_LINUX
    result = MVB_NO_ERROR;
    if (result == MVB_NO_ERROR) {
        int exit_st = ioctl(mvb_fd[ts_id], MVB_STOP, 0);
        if (exit_st != 0) {
            result = MVB_ERROR_MVBC_INIT;
        }
    }
#else
    TM_TYPE_SCR  local_scr;      /* Local copy of SCR                 */
    
    result = lci_check_ts_valid(ts_id);
    /* Return Value, check for valid ts  */

    if (result == MVB_LC_OK)        /* MVB Communication Kickoff !       */
    {
        local_scr.w = readWordFromMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.scr.w));
        local_scr.w &= ~TM_SCR_IL_TEST;
        writeWordToMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.scr.w), local_scr.w);
        result = MVB_NO_ERROR;
    }
    else
    {
        result = MVB_UNKNOWN_TRAFFICSTORE;
    }
#endif
    return result;
} /* MVBCStop */


/****************************************************************************
**  name         : MVBCGetPort
**===========================================================================
**  parameters   : Port: Port number
                   PortBuffer: ptr to port-buffer
                   Tack: Limit for age of data
                   Age: ptr to Real age of data
                   ts_id: Traffic Store ID

**  return value : MVB_NO_ERROR: success
                   MVBM_CONFIG_UNDEF
                   MVB_WARNING_NO_SINK
                   MVB_ERROR_PORT_UNDEF
                   MVB_UNKNOWN_TRAFFICSTORE
                   MVB_ERROR_MVBC_STOP
                   MVB_WARNING_OLD_DATA
                   
**  description  : The function reads data from MVBC port into the port buffer

**  caveats      : uses interrupt lock to protect against concurrent accesses
*****************************************************************************/
UNSIGNED8 MVBCGetPort(UNSIGNED16  Port,        /* Port number            */
                      UNSIGNED16 *PortBuffer,  /* Pointer to port buffer */
                      UNSIGNED16  Tack,        /* Limit for age of data  */
                      UNSIGNED16 *Age,         /* Real age of data       */
                      UNSIGNED16  ts_id)       /* Traffic Store ID       */
#ifdef O_LINUX
{
    int retval;
    mvb_port my_port;

#if defined(MVB_M)
    if (!gbAPIInternal && (gMVBMState == MVBM_CONFIG_UNDEF))
    {
        return MVBM_CONFIG_UNDEF;
    }
#endif

    my_port.type = MVB_PD;
    my_port.port = Port;  

    retval = read(mvb_fd[ts_id], &my_port, port_sizes[Port]);
    if (port_sizes[Port] == retval) {
        PortBuffer++; /* first word in buffer contains sinktime value */
        memcpy(PortBuffer, my_port.data, port_sizes[Port]); 
        return MVB_NO_ERROR;
    }
    switch (retval) {
        case EIO:       return MVB_WARNING_NO_SINK;
        case EBADF:     return MVB_ERROR_PORT_UNDEF;
        case EINVAL:    return MVB_UNKNOWN_TRAFFICSTORE;
        case ENETDOWN:  return MVB_ERROR_MVBC_STOP;
        case ETIMEDOUT: return MVB_WARNING_OLD_DATA;
        default:        return MVB_UNKNOWN_TRAFFICSTORE;
    }  
} /* MVBCGetPort */
#else
{
    void       *pb_pit;     /* Address  Port Index Table (PIT) */
    UNSIGNED16  prt_indx;   /* Contents PIT byte */
    TYPE_LP_CB *p_cb = &lp_cb[ts_id];
    TM_TYPE_SCR local_scr;  /* Local copy of SCR                 */
    TM_TYPE_MCR local_mcr;  /* Local copy of Memory Config. Reg. */
    UNSIGNED8   result = MVB_NO_ERROR;  /* Return value */

#if defined(MVB_M)
    if (!gbAPIInternal && (gMVBMState == MVBM_CONFIG_UNDEF))
    {
        return MVBM_CONFIG_UNDEF;
    }
#endif

    if ( lci_check_ts_valid(ts_id) != MVB_LC_OK )
    {
        return MVB_UNKNOWN_TRAFFICSTORE;
    }

#ifndef MVBCEMU
    local_scr.w = readWordFromMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.scr.w));
    /* MVBC is running? */
    if (GETSCRIL(local_scr) != TM_SCR_IL_RUNNING)
    {
			  // Rui:
			  //printf("MVBCGetPort: MVBC is stopped!\r\n");
        return MVB_ERROR_MVBC_STOP;
    }
#endif

    if (Port > LPC_GET_PRT_ADDR_MAX(p_cb))
    {
        return  MVB_ERROR_PORT_UNDEF;
    }

    pb_pit = LPC_GET_PB_PIT(p_cb);
    local_mcr.w = readWordFromMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.mcr.w));
    prt_indx = lpl_get_prt_indx((UNSIGNED16)(GETMCRMCM(local_mcr)), pb_pit, Port);

    if (prt_indx > LPC_GET_PRT_INDX_MAX (p_cb))
    {
        return MVB_ERROR_PORT_UNDEF;
    }

    /* Check existence of port */
    if (prt_indx != 0)
    {
        UNSIGNED8 *p_pcs;   /* Address  Port Control and Status Register (PCS) */
        UNSIGNED8  f_code;      /* Function code */
        UNSIGNED8  frame_size;  /* Port size in word */
        UNSIGNED16 *addr_prt;
        UNSIGNED16 *p_sha;

        p_pcs = (((UNSIGNED8*) LPC_GET_PB_PCS (p_cb)) +
                           LPL_GEN_PCS_OFFSET (p_cb, prt_indx));

        /* Get data with warning if port is not sink (value 01) */
        if (LPL_GET_PRT_TYPE(p_pcs) != TM_PCS_TYPE_SNK)
        {
            result = MVB_WARNING_NO_SINK;
        }

        /* Function code  to calculate the port size in words */
        f_code = lpl_get_prt_fcode(p_pcs);

        frame_size = (UNSIGNED8)1 << f_code; /* Port size in words */

        MVB_INT_DISABLE();
        p_sha = (UNSIGNED16*)PortBuffer;

        *Age = LPL_GET_PRT_TACK(p_pcs);
        if (*Age < Tack)
        {
            /* Keep in mind: TACK check result */
            *(p_sha++) = MVB_WARNING_OLD_DATA;
            if (result == MVB_NO_ERROR)
            {   /* no overwrite of error */
                result = MVB_WARNING_OLD_DATA;
            }
        }
        else
        {
            /* Keep in mind: TACK check result */
            *(p_sha++) = MVB_NO_ERROR;
        }

        /* Address  port (data area) */
        addr_prt = (UNSIGNED16*)((UNSIGNED8*)LPC_GET_PB_PRT(p_cb) +
                                 LPL_GEN_PRT_OFFSET(prt_indx) +
                                 LPL_GET_PGE_OFFSET_RD(p_pcs));
        /* Copy all data */
				// Rui:
				//printf("MVBCGetPort: frame_size = %d\r\n", frame_size);
        while (frame_size > 0)
        {
					  temp_num = readWordFromTS(addr_prt++);
            *(p_sha++) = (temp_num >> 8) | ((temp_num & 0xFF) << 8);
            frame_size--;
        }
        MVB_INT_ENABLE();
    }
    else
    {  /* Error: port not defined */
        result = MVB_ERROR_PORT_UNDEF;
    }
    return result;
} /* MVBCGetPort */
#endif


/****************************************************************************
**  name         : MVBCPutPort
**===========================================================================
**  parameters   : Port: Port number
                   PortBuffer: ptr to port-buffer
                   ts_id: Traffic Store ID

**  return value : MVB_NO_ERROR: success
                   MVBM_CONFIG_UNDEF
                   MVB_ERROR_NO_SRC
                   MVB_UNKNOWN_TRAFFICSTORE
                   MVB_ERROR_PORT_UNDEF
                   
**  description  : The function writes data from the port buffer into MVBC port

**  caveats      : uses interrupt lock to protect against concurrent accesses
*****************************************************************************/
UNSIGNED8 MVBCPutPort(UNSIGNED16  Port,        /* Port number */
                      UNSIGNED16 *PortBuffer,  /* Pointer to port buffer  */
                      UNSIGNED16  ts_id)       /* Traffic Store ID */
#ifdef O_LINUX
{
    int retval;
    mvb_port my_port;

#if defined(MVB_M)
    if (gMVBMState == MVBM_CONFIG_UNDEF)
    {
        return MVBM_CONFIG_UNDEF;
    }
#endif
    PortBuffer++;  /* 1st word contains sinktime data */
    my_port.type = MVB_PD;
    my_port.port = Port;  
    memcpy(my_port.data, PortBuffer, port_sizes[Port]); 

    retval = write(mvb_fd[ts_id], &my_port, port_sizes[Port]);
    if (port_sizes[Port] == retval) {

        return MVB_NO_ERROR;
    }

    switch (retval) {
        case -EIO   : return MVB_ERROR_NO_SRC;
        case -EBADF : return MVB_UNKNOWN_TRAFFICSTORE;
        case -EINVAL: return MVB_ERROR_PORT_UNDEF;
        default     : return MVB_ERROR_PORT_UNDEF;
    }
} /* MVBCPutPort */
#else
{
    void       *pb_pit;     /* Address  Port Index Table (PIT) */
    UNSIGNED16  prt_indx;   /* Contents PIT byte */
    TYPE_LP_CB *p_cb = &lp_cb[ts_id];
    TM_TYPE_MCR local_mcr;  /* Local copy of Memory Config. Reg. */
    UNSIGNED8   result = MVB_NO_ERROR;  /* Return value */

    if (lci_check_ts_valid(ts_id) != MVB_LC_OK)
    {
        return MVB_UNKNOWN_TRAFFICSTORE;
    }

    if (Port > LPC_GET_PRT_ADDR_MAX(p_cb))
    {
        return MVB_ERROR_PORT_UNDEF;
    }

    pb_pit = LPC_GET_PB_PIT(p_cb);
    local_mcr.w = readWordFromMVBC((void*)(&lci_ctrl_blk[ts_id].p_sa->int_regs.mcr.w));
    prt_indx = lpl_get_prt_indx((UNSIGNED16)(GETMCRMCM(local_mcr)), pb_pit, Port);

    if (prt_indx > LPC_GET_PRT_INDX_MAX (p_cb))
    {
        return MVB_ERROR_PORT_UNDEF;
    }

    /* Check existence of port */
    if (prt_indx != 0)
    {
        UNSIGNED8 *p_pcs;   /* Address  Port Control and Status Register (PCS) */
        UNSIGNED8  f_code;      /* Function code */
        UNSIGNED8  frame_size;  /* Port size in word */
        UNSIGNED16 *addr_prt;
        UNSIGNED16 *p_sha;

        p_pcs = (((UNSIGNED8*) LPC_GET_PB_PCS (p_cb)) +
                               LPL_GEN_PCS_OFFSET (p_cb, prt_indx));

        /* Get data only if port is sink (value 01) */
        if (LPL_GET_PRT_TYPE(p_pcs) != TM_PCS_TYPE_SRC)
        {
            return MVB_ERROR_NO_SRC;
        }

        /* Function code  to calculate the port size in words */
        f_code = lpl_get_prt_fcode(p_pcs);

        frame_size = (UNSIGNED8)1 << f_code; /* Port size in words */

        MVB_INT_DISABLE();
        /* Address  port (data area) */
        addr_prt = (UNSIGNED16*)((UNSIGNED8*)LPC_GET_PB_PRT(p_cb) +
                                 LPL_GEN_PRT_OFFSET(prt_indx) +
                                 LPL_GET_PGE_OFFSET_WT(p_pcs));
        p_sha = (UNSIGNED16*)PortBuffer;

				// Rui:
				//printf("MVBCPutPort: frame_size = %d\r\n", frame_size);
        /* Copy all data */
        p_sha++; /* skip TACK check result */
				//printf("MVBCPutPort: put data: ");
        while (frame_size > 0)
        {
					  // Rui:
					  //printf("0x%02x%02x ", *(UNSIGNED8*)p_sha, *((UNSIGNED8*)p_sha + 1));
					  temp_num = *(p_sha++);
					  temp_num = (temp_num >> 8) | ((temp_num & 0xFF) << 8);
            writeWordToTS(addr_prt++, temp_num);
            frame_size--;
        }
				//printf("\r\n");
        /* Make port valid: toggle page */
        lpl_tgl_prt_page(p_pcs);
        MVB_INT_ENABLE();

#if defined(MVB_M)
        if (gMVBMState == MVBM_CONFIG_UNDEF)
        {
            return MVBM_CONFIG_UNDEF;
        }
#endif
    }
    else
    {  /* Error: port not defined */
        result = MVB_ERROR_PORT_UNDEF;
    }
    return result;
} /* MVBCPutPort */
#endif


/****************************************************************************
**  name         : MVBCSetDSW
**===========================================================================
**  parameters   : ts_id: Traffic Store ID
                   mask:  mask to seperate the desired bits
                   value: value of the bits
                   
**  return value : MVB_NO_ERROR
                   MVB_UNKNOWN_TRAFFICSTORE

**  description  : The function sets the bits in the Device Status Word which
                   shall be set by the user.
                   i.e: Device Not Ready
                       Some Device Disturbed
                       Some System Disturbed

**  caveats      : -
*****************************************************************************/
UNSIGNED8 MVBCSetDSW(UNSIGNED16 ts_id,   /* Traffic Store ID               */
                     UNSIGNED16 mask,    /* Device Status Word Mask Bits   */
                     UNSIGNED16 value)   /* New values for affected fields */
{
    UNSIGNED8   result = MVB_NO_ERROR;  /* Return value */

    UNSIGNED16  lMask = mask & (LC_DSW_DNR_MSK |
        LC_DSW_SDD_MSK |
        LC_DSW_SSD_MSK);  /* Only these Bits shall be modified */

#ifdef O_LINUX
    UNSIGNED32  lMask32 = (lMask << 16) | value;

    if (result == MVB_NO_ERROR) {
        int exit_st = ioctl(mvb_fd[ts_id], MVB_WRITE_DSW, lMask32);
        if (exit_st != 0) {
            result = MVB_ERROR_MVBC_INIT;
        }
    }
#else    
    if ( lc_set_device_status_word(ts_id,
        lMask,
        value) != MVB_LC_OK )
    {
        result = MVB_UNKNOWN_TRAFFICSTORE;
    }
#endif
    return result;
} /* MVBCSetDSW */


/****************************************************************************
**  name         : MVBCIdle  (in MVBM: MVBCIdle11)
**===========================================================================
**  parameters   : ts_id: Traffic Store ID

**  return value : MVB_NO_ERROR
                   MVB_UNKNOWN_TRAFFICSTORE

**  description  : The function retrieves LAA and RLD from MVBC Decoder Register
                   and updates it in the Device Status Word.

**  caveats      : -
*****************************************************************************/
#if defined (MVB_M)
UNSIGNED8 MVBCIdle11(UNSIGNED16  ts_id)       /* Traffic Store ID */
#else
UNSIGNED8 MVBCIdle(UNSIGNED16  ts_id)       /* Traffic Store ID */
#endif
{
    UNSIGNED8   result;
#ifdef O_LINUX
    UNREFERENCED(ts_id);
    result = MVB_NO_ERROR;
    /* dummy function */
#else
    result = MVB_UNKNOWN_TRAFFICSTORE;  /* Return value */

    if ( lci_check_ts_valid(ts_id) == MVB_LC_OK )
    {
        (void)lc_set_laa_rld();
        result = MVB_NO_ERROR;
    }
#endif
    return result;
} /* MVBCIdle */


/****************************************************************************
**  name         : MVBCGetDeviceAddress
**===========================================================================
**  parameters   : ts_id    Traffic Store  Identification
                   devAddress  ptr where the address shall be written to
                   
**  return value : MVB_NO_ERROR             success
                   MVB_UNKNOWN_TRAFFICSTORE
                   MVB_ERROR_MVBC_INIT

**  description  : reads the current device address.

**  caveats      : -
*****************************************************************************/
UNSIGNED8 MVBCGetDeviceAddress(
    UNSIGNED16  *devAddress,
    UNSIGNED16  ts_id)        /* Traffic Store ID       */
{
    UNSIGNED8   result;
#ifdef O_LINUX

    result = MVB_NO_ERROR;
    if (result == MVB_NO_ERROR) {
        int exit_st = ioctl(mvb_fd[ts_id], MVB_READ_DEV_ADDR, devAddress);
        if (exit_st != 0) {
            return MVB_ERROR_MVBC_INIT;
        }
    }
#else    
    result = MVB_UNKNOWN_TRAFFICSTORE;  /* Return value */

    if ( lci_check_ts_valid(ts_id) == MVB_LC_OK )
    {
        *devAddress = readWordFromMVBC((void *)(&lci_ctrl_blk[ts_id].p_sa->int_regs.daor));
        result = MVB_NO_ERROR;
    }
#endif
    return result;
} /* MVBCGetDeviceAddress */


/****************************************************************************
**  name         : MVBCRetriggerWd
**===========================================================================
**  parameters   : ts_id    Traffic Store  Identification
                   trig_val  value to be written into the watchdog register

**  return value : status MVB_NO_ERROR,
                          MVB_UNKNOWN_TRAFFICSTORE,
                          MVB_ERROR_MVBC_INIT or 
                          MVB_WATCHDOG_NOT_AVAIL

**  description  : retriggers MVBCS1 watchdog 

**  caveats      : -
*****************************************************************************/
UNSIGNED8 MVBCRetriggerWd(UNSIGNED16 ts_id, UNSIGNED16 trig_val)
{
#ifdef O_LINUX
    int exit_st;
    exit_st = ioctl(mvb_fd[ts_id], MVB_RETRIGGER, trig_val);
    if (exit_st != 0) {
        return MVB_ERROR_MVBC_INIT;
    }
    return MVB_NO_ERROR;
#else
    if(lci_ctrl_blk[ts_id].mvbc_type == MVBCS1)
    {
        if (MVB_LC_OK != lci_check_ts_valid(ts_id)) 
        {
            return MVB_UNKNOWN_TRAFFICSTORE;
        }
        writeWordToMVBC((void *)(&lci_ctrl_blk[ts_id].p_sa->int_regs.wdc), trig_val);

        /* 26.03.02; If MVBCS1 watchdog is activated and the watchdog counter register reaches zero,the initialization
        level changes to configuration mode.Check the SCR-Initialization level and return the error if it is config mode */

        if ((readWordFromMVBC((void *)(&lci_ctrl_blk[ts_id].p_sa->int_regs.scr.w)) & TM_SCR_IL_MASK) == TM_SCR_IL_RUNNING) 
        {
            return MVB_NO_ERROR;
        }
        else
        { 
            if ((readWordFromMVBC((void *)(&lci_ctrl_blk[ts_id].p_sa->int_regs.scr.w)) & TM_SCR_IL_MASK) == TM_SCR_IL_CONFIG)
            {
                return MVB_ERROR_MVBC_INIT;
            }
        }  
        return MVB_NO_ERROR;
    }
    else
    {
        return MVB_WATCHDOG_NOT_AVAIL;
    }
#endif
} /* MVBCRetriggerWd() */


/****************************************************************************
**  name         : MVBCGetMvbFd
**===========================================================================
**  parameters   : ts_id: Traffic Store ID

**  return value : file descriptor

**  description  : return fd

**  caveats      : Do not call before MVBCConfigure()!
*****************************************************************************/
#ifdef O_LINUX
int MVBCGetMvbFd(UNSIGNED16  ts_id)  
{
   return mvb_fd[ts_id];
}
#endif


/****************************************************************************
**  name         : MVBCGetServiceAreaAddr
**===========================================================================
**  parameters   : in     ts_id            Traffic Store  Identification

**  return value : pointer to service area

**  used globals : mvb_ctrl_blk (read)

**  description  : gets address of Traffic Store Service Area

**  caveats      : -
*****************************************************************************/
#ifndef O_LINUX
VOL WORD16 *MVBCGetServiceAreaAddr(unsigned short ts_id)
{
  if (MVB_LC_OK != lci_check_ts_valid(ts_id)) {
    return NULL;
  }
 return (WORD16 *)lci_ctrl_blk[ts_id].p_sa;
}
#endif


/****************************************************************************
**  name         : MVBCGetTSStartAddr
**===========================================================================
**  parameters   : in     ts_id            Traffic Store  Identification

**  return value : pointer to service area

**  used globals : mvb_ctrl_blk (read)

**  description  : gets address of Traffic Store Service Area

**  caveats      : -
*****************************************************************************/
#ifndef O_LINUX
VOL WORD16 *MVBCGetTSStartAddr(unsigned short ts_id)
{
  if (MVB_LC_OK != lci_check_ts_valid(ts_id)) {
    return NULL;
  }
 return lci_ctrl_blk[ts_id].p_tm;
}
#endif
