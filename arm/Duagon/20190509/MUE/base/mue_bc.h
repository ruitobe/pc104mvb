#ifndef MUE_BC_H
#define MUE_BC_H

/* ==========================================================================
 *
 *  File      : MUE_BC.H
 *
 *  Purpose   : MVB UART Emulation - Bus Controller Structure
 *              - UART: MVB UART Emulation with FIFO (e.g. MDFULL)
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


/* ==========================================================================
 *
 *  Definitions
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : MUE_BUS_CTRL
 *
 *  Purpose           : Description of bus controller specific values.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          * -----------------------------------------------
 *                          *  static values
 *                          * -----------------------------------------------
 *                          WORD32      acc_reg_rbr_thr;
 *                          WORD32      acc_reg_lsr;
 *                          UNSIGNED32  acc_reg_size;
 *                          UNSIGNED32  acc_reg_timeout;
 *                      }   MUE_BUS_CTRL;
 *
 *  Element           : acc_reg_rbr_thr - address of UART register RBR/THR
 *                      acc_reg_lsr     - address of UART register LSR
 *                      acc_reg_size    - size of a UART register
 *                                        (number of bytes)
 *                      acc_reg_timeout - timeout by loop
 *                                        (~10ms  for MD16/MDFULL)
 *                                        (~100us for PD16       )
 *
 *  Remarks           : This bus controller structure is common for all types
 *                      of MVB UART Emulation (e.g. PD16, MD16, MDFULL).
 * --------------------------------------------------------------------------
 */
typedef struct
{
    /* ----------------------------------------------------------------------
     *  static values
     * ----------------------------------------------------------------------
     */
    WORD32      acc_reg_rbr_thr;
    WORD32      acc_reg_lsr;
    UNSIGNED32  acc_reg_size;
    UNSIGNED32  acc_reg_timeout;

}   MUE_BUS_CTRL;


#endif /* #ifndef MUE_BC_H */
