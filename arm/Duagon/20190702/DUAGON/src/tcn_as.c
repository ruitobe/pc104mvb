/* ==========================================================================
 *
 *  File      : TCN_AS.C
 *
 *  Purpose   : Application Layer Interface for Supervision
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
 *  Include Files
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  TCN Software Architecture
 * --------------------------------------------------------------------------
 */
#include <tcn_def.h>
#include <tcn_osl.h>
#include <tcn_sv.h>
#include <tcn_ls.h>
#include <tcn_as.h>


/* ==========================================================================
 *
 *  General Constants and Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous Constants
 * --------------------------------------------------------------------------
 */
TCN_DECL_CONST UNSIGNED16   as_max_links = AS_MAX_LINKS;


/* ==========================================================================
 *
 *  Initialisation
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : as_init
 *
 *  Purpose   : Initialises the supervision application layer and
 *              the connected link layers.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AS_RESULT
 *              as_init (void);
 *
 *  Return    : result code; any AS_RESULT
 *
 *  Remarks   : - This procedure has to be called at system initialisation
 *                before calling any other 'as_xxx' procedure.
 *              - This procedure shall be called only one time.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AS_RESULT
as_init (void)
{
    LS_RESULT   ls_result = LS_OK;
    ENUM8       link_id;

    link_id = 0;
    while ((link_id < ls_max_links) && (LS_OK == ls_result))
    {
        ls_result = ls_init(link_id);
        /* ignore LS_UNKNOWN_LINK */
        if (LS_UNKNOWN_LINK == ls_result)
        {
            ls_result = LS_OK;
        } /* if (LS_UNKNOWN_LINK == ls_result) */
        link_id++;
    } /* while ((link_id < ls_max_links) && (LS_OK == ls_result)) */

    return((AS_RESULT)ls_result);

} /* as_init */


/* ==========================================================================
 *
 *  Link Layer Management
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : as_read_link_set
 *
 *  Purpose   : Retrieves the connected link layers and their supported
 *              services.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AS_RESULT
 *              as_read_link_set
 *              (
 *                  BITSET16    *link_set,
 *                  BITSET16    *pd_link_set,
 *                  BITSET16    *md_link_set
 *              );
 *
 *  Return    : result code; any AS_RESULT
 *
 *  Output    : link_set    - one bit for each connected link layer; the
 *                            first bit (MSB) corresponds to link_layer0
 *              pd_link_set - one bit for each connected link layer
 *                            supporting process data
 *              md_link_set - one bit for each connected link layer
 *                            supporting message data
 *
 *  Remarks   : - The link_layer0 corresponds to link_id=0
 *                (link_id=bus_id=ts_id).
 *              - Typically a connected link layer will support at minimum
 *                process data.
 *              - This procedure is intended for the network management
 *                agent.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AS_RESULT
as_read_link_set
(
    BITSET16    *link_set,
    BITSET16    *pd_link_set,
    BITSET16    *md_link_set
)
{
    LS_RESULT   ls_result;

    ls_result = ls_read_link_set(link_set, pd_link_set, md_link_set);

    return((AS_RESULT)ls_result);

} /* as_read_link_set */



/* --------------------------------------------------------------------------
 *  Procedure : as_read_link_descriptor
 *
 *  Purpose   : Retrieves the description (type and name) of a connected
 *              link layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AS_RESULT
 *              as_read_link_descriptor
 *              (
 *                  ENUM8       link_id,
 *                  ENUM8       *link_type,
 *                  STRING32    *link_name
 *              );
 *
 *  Input     : link_id   - link identifier (0..15)
 *
 *  Return    : result code; any AS_RESULT
 *
 *              link_type - type of the link (any SV_LINK_TYPE)
 *              link_name - name of the link (null terminated character
 *                          string)
 *
 *  Remarks   : - This procedure is intended for the network management
 *                agent.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AS_RESULT
as_read_link_descriptor
(
    ENUM8       link_id,
    ENUM8       *link_type,
    STRING32    *link_name
)
{
    LS_RESULT   ls_result;

    ls_result = ls_read_link_descriptor(link_id, link_type, link_name);

    return((AS_RESULT)ls_result);

} /* as_read_link_descriptor */


/* --------------------------------------------------------------------------
 *  Procedure : as_service_handler
 *
 *  Purpose   : Handles link layer specific services.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AS_RESULT
 *              as_service_handler
 *              (
 *                  ENUM8       link_id,
 *                  ENUM8       service,
 *                  void        *p_parameter
 *              );
 *
 *  Input     : link_id     - link identifier (0..15)
 *              service     - service identifier
 *                            (any link layer specific supervision service
 *                            identifier, e.g. SV_MVB_SERVICE)
 *
 *  In-/Output: p_parameter - pointer to service specific parameter
 *                            (structured type)
 *
 *  Return    : result code; any AS_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AS_RESULT
as_service_handler
(
    ENUM8       link_id,
    ENUM8       service,
    void        *p_parameter
)
{
    LS_RESULT   ls_result;

    ls_result = ls_service_handler(link_id, service, p_parameter);

    return((AS_RESULT)ls_result);

} /* as_service_handler */
