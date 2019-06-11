/* ==========================================================================
 *
 *  File      : MUE_BC.C
 *
 *  Purpose   : MVB UART Emulation - Bus Controller Structure
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
#include <mue_bc.h>


/* ==========================================================================
 *
 *  Public Variables
 *
 * ==========================================================================
 */

/* ==========================================================================
 *
 *  Public Definitions
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Bus Controller Structure for MVB UART Emulation
 * --------------------------------------------------------------------------
 */
MUE_BUS_CTRL mue_bus_ctrl =                                                 \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  UART (static values)                                                \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* address of UART register RBR/THR                                 */  \
    (WORD32)0x4F8,                                                          \
                                                                            \
    /* address of UART register LSR                                     */  \
    (WORD32)0x4FD,                                                          \
                                                                            \
    /* size of a UART register (number of bytes)                        */  \
    (UNSIGNED32)1,                                                          \
                                                                            \
    /* timeout by loop (~10ms for MD16/MDFULL)                          */  \
    /* HW=D113L-MDFULL, OS=Win98, HOST=PC AMD K6-2 500MHz               */  \
    /* - 650000 => 1000ms                                               */  \
    (UNSIGNED32)6500                                                        \
};
