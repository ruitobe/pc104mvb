/* ==========================================================================
 *
 *  File      : TCN_MM.C
 *
 *  Purpose   : Manager Interface (Train Communication Network)
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
#include "tcn_common.h"
#ifdef TCN_PRJ
#   include <tcn_prj.h>
#endif




/* ==========================================================================
 *
 *  Conditional Compiling
 *
 * ==========================================================================
 */
#ifdef TCN_MD


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
#include <tcn_tnm.h>
#include <tcn_mm.h>
#include <tcn_am.h>


/* ==========================================================================
 *
 *  Local Variables
 *
 * ==========================================================================
 */
TCN_DECL_LOCAL  MM_SERVICE_CONF tcn_mm_service_conf;


/* ==========================================================================
 *
 *  Initialisation
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mm_init
 *
 *  Purpose   : Initialises the Manager (e.g. starts the Manager process).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MM_RESULT
 *              mm_init
 *              (
 *                  MM_SERVICE_CONF service_conf
 *              );
 *
 *  Input     : service_conf - service confirm procedure
 *                             of type 'MM_SERVICE_CONF'
 *
 *  Return    : any MM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MM_RESULT
mm_init
(
    MM_SERVICE_CONF service_conf
)
{

    /* set local variables */
    tcn_mm_service_conf = service_conf;

    return(MM_OK);

} /* mm_init */


/* --------------------------------------------------------------------------
 *  Procedure : mm_service_req
 *
 *  Purpose   : Calls a remote service.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MM_RESULT
 *              mm_service_req
 *              (
 *                  UNSIGNED8           station_id,
 *                  const AM_ADDRESS    *agent_adr,
 *                  void                *mm_call
 *              );
 *
 *  Input     : station_id - Station_Identifier of this Station
 *              agent_adr  - Network_Address of the Agent
 *              mm_call    - pointer to body of the Call_Message
 *                           (a Call_Message always start with
 *                           MM_MSG_HEADER; the structure of
 *                           a Call_Message depends on the SIF_code)
 *
 *  Return    : any MM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MM_RESULT
mm_service_req
(
    UNSIGNED8           station_id,
    const AM_ADDRESS    *agent_adr,
    void                *mm_call
)
{

    /* to_do */

    return(MM_OK);

} /* mm_service_req */


#endif /* TCN_MD */
