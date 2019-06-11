#ifndef TCN_LS_H
#define TCN_LS_H

/* ==========================================================================
 *
 *  File      : TCN_LS.H
 *
 *  Purpose   : Link Layer Interface for Supervision
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
#include <tcn_sv.h>


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
#define LS_MAX_LINKS 16
TCN_DECL_PUBLIC TCN_DECL_CONST UNSIGNED16   ls_max_links;


/* --------------------------------------------------------------------------
 *  Enumerated Type : LS_RESULT
 *
 *  Purpose         : Result of a procedure.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    LS_OK                       = 0,  /* correct termination               */
    LS_SERVICE_NOT_SUPPORTED    = 1,  /* service not supported             */
    LS_ERROR                    = 2,  /* unspecified error                 */
    LS_CONFIG                   = 3,  /* configuration error               */
    LS_MEMORY                   = 4,  /* not enough memory                 */
    LS_UNKNOWN_LINK             = 5,  /* unknown link (e.g. not connected) */
    LS_RANGE                    = 6,  /* memory address error              */
    LS_DATA_TYPE                = 7,  /* unsupported data type             */
    LS_BUSY                     = 9,  /* try again later                   */
    LS_CALL_SEQUENCE            = 10, /* wrong command sequence            */
    LS_MAX_ERR                  = 15  /* highest system code; service      */
                                      /* specific codes are higher than 15 */
}   LS_RESULT;


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure Type: LS_INIT
 *
 *  Purpose       : Initialises a link layer as a whole (incl. process and
 *                  message data).
 *
 *  Syntax        : typedef LS_RESULT
 *                  (*LS_INIT)
 *                  (
 *                      void        *p_bus_ctrl
 *                  );
 *
 *  Input         : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return        : result code; any LS_RESULT
 *
 *  Remarks       : - A link layer is identified by 'p_bus_ctrl'.
 *                  - This procedure has to be called at system
 *                    initialisation before calling any other 'LS_xxx',
 *                    'LP_xxx' or 'LM_xxx' procedure referencing the same
 *                    link layer (parameter 'p_bus_ctrl').
 *                  - This procedure shall be called only one time for each
 *                    link layer.
 * --------------------------------------------------------------------------
 */
typedef LS_RESULT
(*LS_INIT)
(
    void        *p_bus_ctrl
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LS_SERVICE_HANDLER
 *
 *  Purpose       : Handles link layer specific services.
 *
 *  Syntax        : typedef LS_RESULT
 *                  (*LS_SERVICE_HANDLER)
 *                  (
 *                      void        *p_bus_ctrl,
 *                      ENUM8       service,
 *                      void        *p_parameter
 *                  );
 *
 *  Input         : p_bus_ctrl  - pointer to bus controller specific values
 *                  service     - service identifier (any link layer
 *                                specific supervision service identifier,
 *                                e.g. SV_MVB_SERVICE)
 *
 *  In-/Output    : p_parameter - pointer to a memory buffer which contains
 *                                service specific parameters
 *
 *  Return        : result code; any LS_RESULT
 *
 *  Remarks       : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
typedef LS_RESULT
(*LS_SERVICE_HANDLER)
(
    void        *p_bus_ctrl,
    ENUM8       service,
    void        *p_parameter
);


/* ==========================================================================
 *
 *  Common Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ls_init
 *
 *  Purpose   : Initialises a link layer as a whole (incl. process and
 *              message data).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              ls_init
 *              (
 *                  ENUM8       link_id
 *              );
 *
 *  Input     : link_id - link identifier (0..15)
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'link_id'.
 *              - This procedure has to be called at system
 *                initialisation before calling any other 'ls_xxx',
 *                'lp_xxx' or 'lm_xxx' procedure referencing the same
 *                link layer (parameter 'link_id').
 *              - This procedure shall be called only one time for each
 *                link layer.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
ls_init
(
    ENUM8       link_id
);


/* --------------------------------------------------------------------------
 *  Procedure : ls_service_handler
 *
 *  Purpose   : Handles link layer specific services.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              ls_service_handler
 *              (
 *                  ENUM8       link_id,
 *                  ENUM8       service,
 *                  void        *p_parameter
 *              );
 *
 *  Input     : link_id     - link layer identifier (0..15)
 *              service     - service identifier (any link layer specific
 *                            supervision service identifier,
 *                            e.g. SV_MVB_SERVICE)
 *
 *  In-/Output: p_parameter - pointer to a memory buffer which contains
 *                            service specific parameters
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'link_id'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
ls_service_handler
(
    ENUM8       link_id,
    ENUM8       service,
    void        *p_parameter
);


/* ==========================================================================
 *
 *  Common Management Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ls_read_link_set
 *
 *  Purpose   : Retreives the connected link layers and
 *              their supported services
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              ls_read_link_set
 *              (
 *                  BITSET16    *link_set,
 *                  BITSET16    *pd_link_set,
 *                  BITSET16    *md_link_set
 *              );
 *
 *  Output    : link_set    - one bit for each connected link layer; the
 *                            first bit (MSB) corresponds to link_layer0
 *              pd_link_set - one bit for each connected link layer
 *                            supporting process data
 *              md_link_set - one bit for each connected link layer
 *                            supporting message data
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - The link_layer0 corresponds to link_id=0
 *                (link_id=bus_id=ts_id).
 *              - Typically a connected link layer will support at minimum
 *                process data.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
ls_read_link_set
(
    BITSET16    *link_set,
    BITSET16    *pd_link_set,
    BITSET16    *md_link_set
);


/* --------------------------------------------------------------------------
 *  Procedure : ls_read_link_descriptor
 *
 *  Purpose   : Retreives the description of a connected link layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              ls_read_link_descriptor
 *              (
 *                  ENUM8       link_id,
 *                  ENUM8       *link_type,
 *                  STRING32    *link_name
 *              );
 *
 *  Input     : link_id   - link identifier (0..15)
 *
 *  Return    : result code; any LS_RESULT
 *
 *              link_type - type of the link (any SV_LINK_TYPE)
 *              link_name - name of the link (null terminated character
 *                          string)
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
ls_read_link_descriptor
(
    ENUM8       link_id,
    ENUM8       *link_type,
    STRING32    *link_name
);


#endif /* #ifndef TCN_LS_H */
