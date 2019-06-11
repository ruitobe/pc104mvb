/* ==========================================================================
 *
 *  File      : MUE_ACCR.C
 *
 *  Purpose   : MVB UART Emulation - Access Interface
 *              - HOST: Intel 386 compatible PC platform
 *              - OS  : eCos for Intel 386 (GNU GCC)
 *              - UART: MVB UART Emulation with FIFO (e.g. MDFULL)
 *                      - 8 registers a 8-bit
 *              - NOTE: Standard access to UART registers.
 *
 *  Project   : General TCN Driver Software
 *              - MVB UART Emulation Protocol (d-000206-nnnnnn)
 *              - TCN Software Architecture   (d-000487-nnnnnn)
 *
 *  Version   : d-000543-nnnnnn
 *
 *  Licence   : Duagon Software Licence (see file 'licence.txt')
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon GmbH, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * ==========================================================================
 */


/* ==========================================================================
 *
 *  Project specific Definitions used for Conditional Compiling
 *
 * ==========================================================================
 */
 
#include "tcn_common.h"
#ifdef TCN_PRJ
#   include <tcn_prj.h>
#endif

#include "localbus.h"
#include "delay.h"


/* ==========================================================================
 *
 *  Definitions
 *
 * ==========================================================================
 */
#define MUE_ACCR_DECL_INLINE

//#define MUE_ACCR_PRINT

//#define MUE_OSL_PRINTF printf
/* ==========================================================================
 *
 *  Inline Procedures
 *
 * ==========================================================================
 */

/* ==========================================================================
 *
 *  File      : MUE_ACCR.INL
 *
 *  Purpose   : MVB UART Emulation - Access Interface
 *              - HOST: Intel 386 compatible PC platform
 *              - OS  : eCos for Intel 386 (GNU GCC)
 *              - UART: MVB UART Emulation with FIFO (e.g. MDFULL)
 *                      - 8 registers a 8-bit
 *              - NOTE: Standard access to UART registers.
 *
 *  Project   : General TCN Driver Software
 *              - MVB UART Emulation Protocol (d-000206-nnnnnn)
 *              - TCN Software Architecture   (d-000487-nnnnnn)
 *
 *  Version   : d-000543-nnnnnn
 *
 *  Licence   : Duagon Software Licence (see file 'licence.txt')
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon GmbH, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * ==========================================================================
 */


/* ==========================================================================
 *
 *  Pre-processor Definitions:
 *  --------------------------
 *  - MUE_ACCR_PCI    - determine base address of UART from PCI device
 *  - MUE_ACCR_PRINT  - debug printouts
 *
 * ==========================================================================
 */


/* ==========================================================================
 *
 *  Project specific Definitions used for Conditional Compiling
 *
 * ==========================================================================
 */
#ifdef TCN_PRJ
#   include <tcn_prj.h>
#endif


/* ==========================================================================
 *
 *  Definitions
 *
 * ==========================================================================
 */
#ifndef MUE_ACCR_DECL_INLINE
#   define MUE_ACCR_DECL_INLINE TCN_DECL_INLINE
#endif


/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  TCN Software Architecture
 * --------------------------------------------------------------------------
 */
#include <tcn_def.h>

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>
#ifdef MUE_ACCR_PRINT
#   include <mue_osl.h>
#endif
#include <mue_bc.h>

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
#ifdef MUE_ACCR_PCI
#   include <pci_lib.h>
#   include <string.h>      /* memset                          */
#endif


/* ==========================================================================
 *
 *  Public Function Prototypes
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_accr_init
 *
 *  Purpose   : Initialises the access to the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACCR_DECL_INLINE
 *              MUE_RESULT
 *              mue_accr_init
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACCR_DECL_INLINE
MUE_RESULT
mue_accr_init
(
    void        *p_bus_ctrl
)
{
    MUE_RESULT  mue_result;

    /* avoid warnings */
    p_bus_ctrl = p_bus_ctrl;

    mue_result = MUE_RESULT_OK;

#ifdef MUE_ACCR_PCI
{
    /* ----------------------------------------------------------------------
     *  get IO resource of PCI device
     * ----------------------------------------------------------------------
     */
    PCI_RESULT  pci_result;
    PCI_CONFIG  pci_cfg;
    PCI_UINT16  found;

    memset((void*)&pci_cfg, 0, sizeof(PCI_CONFIG));

    if (MUE_RESULT_OK == mue_result)
    {
        pci_result = pci_init();
        if (PCI_OK != pci_result)
        {
            mue_result = MUE_RESULT_ERROR;
        } /* if (PCI_OK != pci_result) */
    } /* if (MUE_RESULT_OK == mue_result) */

    if (MUE_RESULT_OK == mue_result)
    {
        /* search for... */
        pci_cfg.vendor_id = 0x9710; /* D213 */
        pci_cfg.device_id = 0x9845; /* D213 */
        /* start search at... */
        pci_cfg.bus  = 0; /* 0 - 255 */
        pci_cfg.dev  = 0; /* 0 -  31 */
        /* search for... */
        pci_cfg.func = 0; /* 0 -   7 */
        found = FALSE;

        pci_result = pci_search(&pci_cfg, &found);
        if ((PCI_OK != pci_result) || (FALSE == found))
        {
            mue_result = MUE_RESULT_ERROR;
        } /* if ((PCI_OK != pci_result) || (FALSE == found)) */
    } /* if (MUE_RESULT_OK == mue_result) */

    if (MUE_RESULT_OK == mue_result)
    {
        pci_result =            \
            pci_probe           \
            (                   \
                pci_cfg.bus,    \
                pci_cfg.dev,    \
                pci_cfg.func,   \
                0, /* print  */ \
                1, /* sizing */ \
                &pci_cfg        \
            );
        if (FALSE == pci_result)
        {
            mue_result = MUE_RESULT_ERROR;
        } /* if (FALSE == pci_result) */
    } /* if (MUE_RESULT_OK == mue_result) */

    if (MUE_RESULT_OK == mue_result)
    {
        /* type of PCI resource BAR0 must be IO */
        if (0 == (pci_cfg.base[0] & 1))
        {
            mue_result = MUE_RESULT_ERROR;
        } /* if (0 == (pci_cfg.base[0] & 1)) */
    } /* if (MUE_RESULT_OK == mue_result) */

    if (MUE_RESULT_OK == mue_result)
    {
        /* size of PCI resource BAR0 must be 8 */
        if (pci_cfg.size[0] != 8)
        {
            mue_result = MUE_RESULT_ERROR;
        } /* if (pci_cfg.size[0] != 8) */
    } /* if (MUE_RESULT_OK == mue_result) */

    if (MUE_RESULT_OK == mue_result)
    {
        /* set UART resources (static values) */
        ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_rbr_thr = \
            (WORD32)(pci_cfg.base[0] & 0xFFFC);
        ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_lsr     = \
            (WORD32)((pci_cfg.base[0] & 0xFFFC) + 5);
        ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_size    = \
            (WORD32)1;
    } /* if (MUE_RESULT_OK == mue_result) */
}
#endif /* #ifdef MUE_ACCR_PCI */

    return(mue_result);

} /* mue_accr_init */


/* --------------------------------------------------------------------------
 *  Procedure : mue_accr_get_reg
 *
 *  Purpose   : Get a 8-bit value from a UART register.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACCR_DECL_INLINE
 *              WORD8
 *              mue_accr_get_reg
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      index
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              index      - index to UART register (0..7)
 *
 *  Return    : 8-bit value get from UART register LSR
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACCR_DECL_INLINE
WORD8
mue_accr_get_reg
(
    void        *p_bus_ctrl,
    UNSIGNED16  index
)
{
    WORD8       mue_accr_reg;
    WORD32      address;

    address = (WORD32)(                                     \
        (((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_rbr_thr) +    \
        (((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_size * index));

    /* get 8-bit value from a UART register */
   // HAL_READ_UINT8(address, mue_accr_reg);
    mue_accr_reg = Localbus_ReadData(address);
	  //delay_ms(1);
	  //printf("<%02X>", mue_accr_reg);
#ifdef MUE_ACCR_PRINT
    MUE_OSL_PRINTF("<%02X>", mue_accr_reg);
#endif /* #ifdef MUE_ACCR_PRINT */

    return(mue_accr_reg);

} /* mue_accr_get_reg */


/* --------------------------------------------------------------------------
 *  Procedure : mue_accr_get_reg_lsr
 *
 *  Purpose   : Get a 8-bit value from UART register LSR.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACCR_DECL_INLINE
 *              WORD8
 *              mue_accr_get_reg_lsr
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : 8-bit value get from UART register LSR
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACCR_DECL_INLINE
WORD8
mue_accr_get_reg_lsr
(
    void        *p_bus_ctrl
)
{
    WORD8       mue_accr_reg_lsr;

    /* get 8-bit value from UART register LSR */
    /*HAL_READ_UINT8                                  \
    (                                               \
        ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_lsr,   \
        mue_accr_reg_lsr                            \
    );*/
  	mue_accr_reg_lsr = Localbus_ReadData(((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_lsr);
	  //delay_ms(1);
	  //mue_accr_reg_lsr = Localbus_ReadData(0x4fd);
    //if (0x60 != mue_accr_reg_lsr)
    //  printf("acc_reg_lsr 0x%x = (0x%02X)\r\n", ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_lsr, mue_accr_reg_lsr);
#ifdef MUE_ACCR_PRINT
	  
#endif /* #ifdef MUE_ACCR_PRINT */

    return(mue_accr_reg_lsr);

} /* mue_accr_get_reg_lsr */


/* --------------------------------------------------------------------------
 *  Procedure : mue_accr_put_reg_thr
 *
 *  Purpose   : Puts a 8-bit value to UART register THR.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACCR_DECL_INLINE
 *              void
 *              mue_accr_put_reg_thr
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD8       value8
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              value8     - 8-bit value to put to UART register THR
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACCR_DECL_INLINE
void
mue_accr_put_reg_thr
(
    void        *p_bus_ctrl,
    WORD8       value8
)
{

#ifdef MUE_ACCR_PRINT
    MUE_OSL_PRINTF("[%02X]", value8);
#endif /* #ifdef MUE_ACCR_PRINT */
    //printf("[%02X]", value8);
    /* put 8-bit value to UART register THR */
    //HAL_WRITE_UINT8(((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_rbr_thr, value8);
    Localbus_WriteData(((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_rbr_thr, &value8);
	  //delay_ms(1);
} /* mue_accr_put_reg_thr */


/* --------------------------------------------------------------------------
 *  Procedure : mue_accr_get_reg_rbr
 *
 *  Purpose   : Gets a 8-bit value from UART register RBR.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACCR_DECL_INLINE
 *              WORD8
 *              mue_accr_get_reg_rbr
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : 8-bit value get from UART register RBR
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACCR_DECL_INLINE
WORD8
mue_accr_get_reg_rbr
(
    void        *p_bus_ctrl
)
{
    WORD8       mue_accr_reg_rbr;

    /* get 8-bit value from UART register RBR */
    /*HAL_READ_UINT8                                      \
    (                                                   \
        ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_rbr_thr,   \
        mue_accr_reg_rbr                                \
    );*/
    mue_accr_reg_rbr = Localbus_ReadData(((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_rbr_thr);
	  //delay_ms(1);
#ifdef MUE_ACCR_PRINT
    MUE_OSL_PRINTF("(%02X)", mue_accr_reg_rbr);
#endif /* #ifdef MUE_ACCR_PRINT */

    return(mue_accr_reg_rbr);

} /* mue_accr_get_reg_rbr */


