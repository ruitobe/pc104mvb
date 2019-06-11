#ifndef MUE_OSL_H
#define MUE_OSL_H

/* ==========================================================================
 *
 *  File      : MUE_OSL.H
 *
 *  Purpose   : MVB UART Emulation - Operating System Libraries Interface
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
 *  - __ECOS__    - eCos using GNU GCC
 *  - _MSC_VER    - DOS, Windows using Microsoft Visual C++
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

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
#ifdef __ECOS__
#   include <cyg/infra/diag.h>  /* diag_printf  */
#endif
#ifdef _MSC_VER
#   include <stdio.h>           /* printf       */
#endif


/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Macro     : MUE_OSL_PRINTF
 *
 *  Purpose   : Print formatted output (used for diagnose messages).
 *
 *  Syntax    : #define MUE_OSL_PRINTF
 * --------------------------------------------------------------------------
 */
#ifdef __ECOS__
#   define MUE_OSL_PRINTF   diag_printf
#endif
#ifdef _MSC_VER
#   define MUE_OSL_PRINTF   printf
#endif


/* ==========================================================================
 *
 *  Initialisation
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_osl_init
 *
 *  Purpose   : Initialises all OSL resources.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_osl_init
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_osl_init
(
    void        *p_bus_ctrl
);


#endif /* #ifndef MUE_OSL_H */
