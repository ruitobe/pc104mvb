#ifndef MUE_ACCR_H
#define MUE_ACCR_H

/* ==========================================================================
 *
 *  File      : MUE_ACCR.H
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
#ifdef TCN_PRJ
#   include <tcn_prj.h>
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


/* ==========================================================================
 *
 *  Function Prototypes
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_accr_init
 *
 *  Purpose   : Initialises the access to the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
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
MUE_RESULT
mue_accr_init
(
    void        *p_bus_ctrl
);


/* --------------------------------------------------------------------------
 *  Procedure : mue_accr_get_reg
 *
 *  Purpose   : Get a 8-bit value from a UART register.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              WORD8
 *              mue_accr_get_reg
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  index
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
WORD8
mue_accr_get_reg
(
    void        *p_bus_ctrl,
    UNSIGNED16  index
);


/* --------------------------------------------------------------------------
 *  Procedure : mue_accr_get_reg_lsr
 *
 *  Purpose   : Get a 8-bit value from UART register LSR.
 *
 *  Syntax    : TCN_DECL_PUBLIC
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
WORD8
mue_accr_get_reg_lsr
(
    void        *p_bus_ctrl
);


/* --------------------------------------------------------------------------
 *  Procedure : mue_accr_put_reg_thr
 *
 *  Purpose   : Puts a 8-bit value to UART register THR.
 *
 *  Syntax    : TCN_DECL_PUBLIC
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
void
mue_accr_put_reg_thr
(
    void        *p_bus_ctrl,
    WORD8       value8
);


/* --------------------------------------------------------------------------
 *  Procedure : mue_accr_get_reg_rbr
 *
 *  Purpose   : Gets a 8-bit value from UART register RBR.
 *
 *  Syntax    : TCN_DECL_PUBLIC
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
WORD8
mue_accr_get_reg_rbr
(
    void        *p_bus_ctrl
);


#endif /* #ifndef MUE_ACCR_H */
