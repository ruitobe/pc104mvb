#ifndef TCN_SV_H
#define TCN_SV_H

/* ==========================================================================
 *
 *  File      : TCN_SV.H
 *
 *  Purpose   : Common Interface for Supervision - General
 *
 *  Project   : General TCN Driver Software
 *              - TCN Software Architecture (d-000487-nnnnnn)
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
#include <tcn_def.h>


/* ==========================================================================
 *
 *  Constants and Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Enumerated Type   : SV_LINK_TYPE
 *
 *  Purpose           : Type of a link layer.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    SV_LINK_TYPE_UNKNOWN    = 0,    /* unknown link     */
    SV_LINK_TYPE_MVB        = 1,    /* MVB              */
    SV_LINK_TYPE_WTB        = 2,    /* WTB              */
    SV_LINK_TYPE_MBX        = 3,    /* memory mailbox   */
    SV_LINK_TYPE_SER        = 4,    /* serial link      */
    SV_LINK_TYPE_CAN        = 5     /* CAN bus          */
}   SV_LINK_TYPE;


/* --------------------------------------------------------------------------
 *  Enumerated Type   : SV_LL_SERVICE
 *
 *  Purpose           : Link Layer services.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    SV_LL_SERVICE_READ_INFO = 128
}   SV_LL_SERVICE;


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_LL_INFO
 *
 *  Purpose           : Link Layer Information Object.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          * -----------------------------------------------
 *                          *  process data
 *                          * -----------------------------------------------
 *                          UNSIGNED16  pd_port_number_max;
 *                          UNSIGNED16  pd_port_size_number;
 *                          UNSIGNED8   pd_port_size_value[16];
 *
 *                          * -----------------------------------------------
 *                          *  message data
 *                          * -----------------------------------------------
 *                          UNSIGNED16  md_frame_size;
 *                          WORD16      reserved;
 *                      }   SV_LL_INFO;
 *
 *  Element           : pd_port_number_max  - max. number of process data
 *                                            ports (0..4095)
 *                      pd_port_size_number - number of supported process
 *                                            data port sizes
 *                      pd_port_size_value  - list of supported process data
 *                                            port sizes (number of bytes;
 *                                            ascending order)
 *                      md_frame_size       - size of a message data frame
 *                                            (number of bytes)
 *                      reserved            - reserved (=0)
 *
 *  Remarks           : - The link layer information object can be read from
 *                        a connected bus controller link layer by
 *                        a supervision service handler procedure
 *                        (e.g. as_service_handler or ls_service_handler)
 *                        using SV_LL_SERVICE_INFO as service identifier
 *                        (parameter 'service').
 *                      - The user has to provide a memory buffer of this
 *                        type for the link layer information object
 *                        (output parameter 'p_parameter').
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_LL_INFO 24
typedef struct
{
    /* ----------------------------------------------------------------------
     *  process data
     * ----------------------------------------------------------------------
     */
    UNSIGNED16  pd_port_number_max;
    UNSIGNED16  pd_port_size_number;
    UNSIGNED8   pd_port_size_value[16];

    /* ----------------------------------------------------------------------
     *  message data
     * ----------------------------------------------------------------------
     */
    UNSIGNED16  md_frame_size;
    WORD16      reserved;
}   SV_LL_INFO;


#endif /* #ifndef TCN_SV_H */
