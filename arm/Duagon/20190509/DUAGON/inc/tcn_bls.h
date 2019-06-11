#ifndef TCN_BLS_H
#define TCN_BLS_H

/* ==========================================================================
 *
 *  File      : TCN_BLS.H
 *
 *  Purpose   : Bus Controller Link Layer Selector
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
#include <tcn_ls.h>
#include <tcn_lp.h>
#include <tcn_lm.h>


/* ==========================================================================
 *
 *  Constants and Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous Constants
 * --------------------------------------------------------------------------
 */
#define BLS_MAX_LINKS 16


/* --------------------------------------------------------------------------
 *  Structured Type   : BLS_DESC_SV
 *
 *  Purpose           : Describes the supervision service elements of
 *                      a bus controller link layer.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          * -----------------------------------------------
 *                          *  LSBI procedures
 *                          * -----------------------------------------------
 *                          LS_INIT             ls_init;
 *                          LS_SERVICE_HANDLER  ls_service_handler;
 *                      }   BLS_DESC_SV;
 *
 *  Element           : ls_init            - a procedure of type 'LS_INIT'
 *                      ls_service_handler - a procedure of type
 *                                           'LS_SERVICE_HANDLER'
 * --------------------------------------------------------------------------
 */
typedef struct
{
    /* ----------------------------------------------------------------------
     *  LSBI procedures
     * ----------------------------------------------------------------------
     */
    LS_INIT             ls_init;
    LS_SERVICE_HANDLER  ls_service_handler;
}   BLS_DESC_SV;


/* --------------------------------------------------------------------------
 *  Structured Type   : BLS_DESC_PD
 *
 *  Purpose           : Describes the process data service elements of
 *                      a bus controller link layer.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          * -----------------------------------------------
 *                          *  LPBI procedures
 *                          * -----------------------------------------------
 *                          LP_INIT             lp_init;
 *                          LP_MANAGE           lp_manage;
 *                          LP_PUT_DATASET      lp_put_dataset;
 *                          LP_GET_DATASET      lp_get_dataset;
 *                      }   BLS_DESC_PD;
 *
 *  Element           : lp_init        - a procedure of type 'LP_INIT'
 *                      lp_manage      - a procedure of type 'LP_MANAGE'
 *                      lp_put_dataset - a procedure of type 'LP_PUT_DATASET'
 *                      lp_get_dataset - a procedure of type 'LP_GET_DATASET'
 * --------------------------------------------------------------------------
 */
typedef struct
{
    /* ----------------------------------------------------------------------
     *  LPBI procedures
     * ----------------------------------------------------------------------
     */
    LP_INIT             lp_init;
    LP_MANAGE           lp_manage;
    LP_PUT_DATASET      lp_put_dataset;
    LP_GET_DATASET      lp_get_dataset;
}   BLS_DESC_PD;


/* --------------------------------------------------------------------------
 *  Structured Type   : BLS_DESC_MD
 *
 *  Purpose           : Describes the message data service elements of
 *                      a bus controller link layer.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          * -----------------------------------------------
 *                          *  LMBI procedures
 *                          * -----------------------------------------------
 *                          LM_INIT             lm_init;
 *                          LM_GET_DEV_ADDRESS  lm_get_dev_address;
 *                          LM_GET_STATUS       lm_get_status;
 *                          LM_SEND_QUEUE_FLUSH lm_send_queue_flush;
 *                          LM_SEND_REQUEST     lm_send_request;
 *                          LM_RECEIVE_POLL     lm_receive_poll;
 *
 *                          * -----------------------------------------------
 *                          *  subscribed procedures to the LMBI
 *                          * -----------------------------------------------
 *                          LM_RECEIVE_INDICATE lm_receive_indicate;
 *                          LM_GET_PACK         lm_get_pack;
 *                          void                **lm_packet_pool;
 *                          LM_SEND_CONFIRM     lm_send_confirm;
 *                          LM_STATUS_INDICATE  lm_status_indicate;
 *                      }   BLS_DESC_MD;
 *
 *  Element           : lm_init             - a procedure of type
 *                                            'LM_INIT'
 *                      lm_get_dev_address  - a procedure of type
 *                                            'LM_GET_DEV_ADDRESS'
 *                      lm_get_status       - a procedure of type
 *                                            'LM_GET_STATUS'
 *                      lm_send_queue_flush - a procedure of type
 *                                            'LM_SEND_QUEUE_FLUSH'
 *                      lm_send_request     - a procedure of type
 *                                            'LM_SEND_REQUEST'
 *                      lm_receive_poll     - a procedure of type
 *                                            'LM_RECEIVE_POLL'
 *
 *                      lm_receive_indicate - a procedure of type
 *                                            'LM_RECEIVE_INDICATE'
 *                      lm_get_pack         - a procedure of type
 *                                            'LM_GET_PACK'
 *                      lm_packet_pool      - identifies the packet pool
 *                                            used by the link layer
 *                      lm_send_confirm     - a procedure of type
 *                                            'LM_SEND_CONFIRM'
 *                      lm_status_indicate  - a procedure of type
 *                                            'LM_STATUS_INDICATE'
 * --------------------------------------------------------------------------
 */
typedef struct
{
    /* ----------------------------------------------------------------------
     *  LMBI procedures
     * ----------------------------------------------------------------------
     */
    LM_INIT             lm_init;
    LM_GET_DEV_ADDRESS  lm_get_dev_address;
    LM_GET_STATUS       lm_get_status;
    LM_SEND_QUEUE_FLUSH lm_send_queue_flush;
    LM_SEND_REQUEST     lm_send_request;
    LM_RECEIVE_POLL     lm_receive_poll;

    /* ----------------------------------------------------------------------
     *  subscribed procedures to the LMBI
     * ----------------------------------------------------------------------
     */
    LM_RECEIVE_INDICATE lm_receive_indicate;
    LM_GET_PACK         lm_get_pack;
    void                **lm_packet_pool;
    LM_SEND_CONFIRM     lm_send_confirm;
    LM_STATUS_INDICATE  lm_status_indicate;
}   BLS_DESC_MD;


/* --------------------------------------------------------------------------
 *  Structured Type   : BLS_DESC
 *
 *  Purpose           : Describes a bus controller link layer.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          * -----------------------------------------------
 *                          *  bus controller description
 *                          * -----------------------------------------------
 *                          void            *p_bus_ctrl;
 *
 *                          * -----------------------------------------------
 *                          *  link description
 *                          * -----------------------------------------------
 *                          ENUM8           link_type;
 *                          STRING32        link_name;
 *
 *                          * -----------------------------------------------
 *                          *  service descriptors
 *                          * -----------------------------------------------
 *                          BLS_DESC_SV     desc_sv;
 *                          BLS_DESC_PD     desc_pd;
 *                          BLS_DESC_MD     desc_md;
 *                      }   BLS_DESC;
 *
 *  Element           : p_bus_ctrl    - pointer to bus controller
 *                                      specific values
 *
 *                      link_type     - type of the link (any SV_LINK_TYPE;
 *                                      default=SV_LINK_TYPE_UNKNOWN)
 *                      link_name     - name of the link
 *                                      (null terminated character string;
 *                                      default=empty string)
 *
 *                      desc_sv       - supervision service descriptor
 *                      desc_pd       - process data service descriptor
 *                      desc_md       - message data service descriptor
 * --------------------------------------------------------------------------
 */
typedef struct
{
    /* ----------------------------------------------------------------------
     *  bus controller description
     * ----------------------------------------------------------------------
     */
    void        *p_bus_ctrl;

    /* ----------------------------------------------------------------------
     *  link description
     * ----------------------------------------------------------------------
     */
    ENUM8       link_type;
    STRING32    link_name;

    /* ----------------------------------------------------------------------
     *  service descriptors
     * ----------------------------------------------------------------------
     */
    BLS_DESC_SV desc_sv;
    BLS_DESC_PD desc_pd;
    BLS_DESC_MD desc_md;

}   BLS_DESC;


/* ==========================================================================
 *
 *  Public Variables
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Public Variable   : bls_desc_table
 *
 *  Purpose           : Points to the connected bus controller link layer
 *                      descriptors.
 *
 *  Remarks           : - The link identifier (e.g. link_id) is the index to
 *                        the table of bus controller link layer descriptor
 *                        pointers. If a pointer is NULL, then there is no
 *                        connected bus controller link layer.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC BLS_DESC    *bls_desc_table[BLS_MAX_LINKS];


#endif /* #ifndef TCN_BLS_H */
