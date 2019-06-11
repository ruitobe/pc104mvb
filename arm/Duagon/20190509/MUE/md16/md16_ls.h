#ifndef MD16_LS_H
#define MD16_LS_H

/* ==========================================================================
 *
 *  File      : MD16_LS.H
 *
 *  Purpose   : Bus Controller Link Layer for MD16 - 
 *                  Link Layer Bus Controller Interface for Supervision
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

/* --------------------------------------------------------------------------
 *  TCN Software Architecture
 * --------------------------------------------------------------------------
 */
#include <tcn_def.h>
#include <tcn_ls.h>


/* ==========================================================================
 *
 *  Miscellaneous Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : md16_ls_errcnt_pd
 *
 *  Purpose   : Handles error counters of a process data port.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              void
 *              md16_ls_errcnt_pd
 *              (
 *                  MD16_BUS_CTRL   *p_md16_bus_ctrl,
 *                  WORD8           mue_port_handle,
 *                  BITSET8         mue_port_status
 *              );
 *
 *  Input     : p_md16_bus_ctrl   - pointer to MD16 bus controller
 *                                  specific values
 *              mue_port_handle   - port identifier (0..15)
 *              mue_port_status   - status information releated to a MVB
 *                                  process data port;
 *                                  any combination of constants
 *                                  'MUE_PD_PORT_STATUS_xxx' or constants
 *                                  'mue_pd_port_status_xxx'
 *
 *  Remarks   : - A link layer is identified by 'p_md16_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
void
md16_ls_errcnt_pd
(
    MD16_BUS_CTRL   *p_md16_bus_ctrl,
    WORD8           mue_port_handle,
    BITSET8         mue_port_status
);


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : md16_ls_init (of procedure type 'LS_INIT')
 *
 *  Purpose   : Initialises a link layer as a whole.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              md16_ls_init
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 *              - This procedure has to be called at system
 *                initialisation before calling any other 'md16_ls_xxx',
 *                'md16_lp_xxx' or ''md16_lm_xxx' procedure referencing
 *                the same link layer (parameter 'p_bus_ctrl').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
md16_ls_init
(
    void        *p_bus_ctrl
);


/* --------------------------------------------------------------------------
 *  Procedure : md16_ls_service_handler
 *              (of procedure type 'LS_SERVICE_HANDLER')
 *
 *  Purpose   : Handles link layer specific services.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              md16_ls_service_handler
 *              (
 *                  void        *p_bus_ctrl,
 *                  ENUM8       service,
 *                  void        *p_parameter
 *              );
 *
 *  Input     : p_bus_ctrl  - pointer to bus controller specific values
 *              service     - link layer service
 *
 *  In-/Output: p_parameter - pointer to a memory buffer which contains
 *                            service specific values
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
md16_ls_service_handler
(
    void        *p_bus_ctrl,
    ENUM8       service,
    void        *p_parameter
);


#endif /* #ifndef MD16_LS_H */
