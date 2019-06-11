#ifndef MUE_DEF_H
#define MUE_DEF_H

/* ==========================================================================
 *
 *  File      : MUE_DEF.H
 *
 *  Purpose   : MVB UART Emulation - Common Definitions
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
 *  Definitions
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Enumerated Type : MUE_RESULT
 *
 *  Purpose         : Result of a procedure.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    MUE_RESULT_OK         = 0,  /* correct termination  */
    MUE_RESULT_ERROR      = 1,  /* unspecified error    */
    MUE_RESULT_TIMEOUT    = 2,  /* timeout occured      */
    MUE_RESULT_PARAMETER  = 3   /* wrong parameter      */
}   MUE_RESULT;


#endif /* #ifndef MUE_DEF_H */
