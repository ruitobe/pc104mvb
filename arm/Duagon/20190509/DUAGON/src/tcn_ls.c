/* ==========================================================================
 *
 *  File      : TCN_LS.C
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
#include <tcn_bls.h>
#include <tcn_ls.h>


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
TCN_DECL_CONST UNSIGNED16   ls_max_links = LS_MAX_LINKS;


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
 
 ENUM8 link_id_test = 0;
TCN_DECL_PUBLIC
LS_RESULT
ls_init
(
    ENUM8       link_id
)
{
    LS_RESULT   ls_result = LS_OK;

#ifdef TCN_CHECK_PARAMETER
	  link_id_test = link_id;
	  
    if (link_id >= 0x10) {
			TCN_OSL_PRINTF("LINK: link_id=%u\n", link_id);
      ls_result = LS_RANGE;
    } /* if (link_id >= ls_max_links) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (LS_OK == ls_result)
    {
        if (NULL == bls_desc_table[link_id])
        {
            ls_result = LS_UNKNOWN_LINK;
					  //TCN_OSL_PRINTF("LS_UNKNOWN_LINK --->\r\n");
        } /* if (NULL == bls_desc_table[link_id]) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        if (NULL == bls_desc_table[link_id]->desc_sv.ls_init)
        {
					  TCN_OSL_PRINTF("LS_UNKNOWN_LINK ***>\r\n");
            ls_result = LS_UNKNOWN_LINK;
        } /* if (NULL == bls_desc_table[link_id]->desc_sv.ls_init) */
    } /* if (LS_OK == ls_result) */
    //TCN_OSL_PRINTF("LS_UNKNOWN_LINK");
    if (LS_OK == ls_result)
    {
        ls_result =                                     \
            bls_desc_table[link_id]->desc_sv.ls_init    \
            (                                           \
                bls_desc_table[link_id]->p_bus_ctrl     \
            );
    } /* if (LS_OK == ls_result) */

    return(ls_result);

} /* ls_init */


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
)
{
    LS_RESULT   ls_result = LS_OK;

#ifdef TCN_CHECK_PARAMETER
    if (link_id >= ls_max_links)
    {
        ls_result = LS_RANGE;
    } /* if (link_id >= ls_max_links) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (LS_OK == ls_result)
    {
        if (NULL == bls_desc_table[link_id])
        {
            ls_result = LS_UNKNOWN_LINK;
        } /* if (NULL == bls_desc_table[link_id]) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        if (NULL == bls_desc_table[link_id]->desc_sv.ls_service_handler)
        {
            ls_result = LS_UNKNOWN_LINK;
        } /* if (NULL == ...) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        ls_result =                                             \
            bls_desc_table[link_id]->desc_sv.ls_service_handler \
            (                                                   \
                bls_desc_table[link_id]->p_bus_ctrl,            \
                service,                                        \
                p_parameter                                     \
            );
    } /* if (LS_OK == ls_result) */

    return(ls_result);

} /* ls_service_handler */


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
)
{
    BITSET16    link_bit = 0x0001;
    UNSIGNED16  link_id;

    if (NULL != link_set)
    {
        *link_set    = 0x0000;
    } /* if (NULL != link_set) */

    if (NULL != pd_link_set)
    {
        *pd_link_set = 0x0000;
    } /* if (NULL != pd_link_set) */

    if (NULL != md_link_set)
    {
        *md_link_set = 0x0000;
    } /* if (NULL != md_link_set) */

    for (link_id=0; link_id<BLS_MAX_LINKS; link_id++)
    {
        if (NULL != bls_desc_table[link_id])
        {
            if (NULL != bls_desc_table[link_id]->desc_sv.ls_init)
            {
                /* ----------------------------------------------------------
                 *  link_set
                 * ----------------------------------------------------------
                 */
                if (NULL != link_set)
                {
                    *link_set |= link_bit;
                } /* if (NULL != link_set) */

                /* ----------------------------------------------------------
                 *  pd_link_set
                 * ----------------------------------------------------------
                 */
                if (NULL != pd_link_set)
                {
                    if (NULL != bls_desc_table[link_id]->desc_pd.lp_init)
                    {
                        *pd_link_set |= link_bit;
                    } /* if (NULL != ...) */
                } /* if (NULL != pd_link_set) */

                /* ----------------------------------------------------------
                 *  md_link_set
                 * ----------------------------------------------------------
                 */
                if (NULL != md_link_set)
                {
                    if (NULL != bls_desc_table[link_id]->desc_md.lm_init)
                    {
                        *md_link_set |= link_bit;
                    } /* if (NULL != bls_desc_table[link_id]->desc_md.lm_init) */
                } /* if (NULL != md_link_set) */

            } /* if (NULL != bls_desc_table[link_id]->desc_sv.ls_init) */
        } /* if (NULL != bls_desc_table[link_id]) */

        link_bit = (BITSET16)(link_bit << 1);

    } /* for (link_id=0; link_id<BLS_MAX_LINKS; link_id++) */

    return(LS_OK);

} /* ls_read_link_set */


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
)
{
    LS_RESULT   ls_result = LS_OK;

#ifdef TCN_CHECK_PARAMETER
    if (link_id >= ls_max_links)
    {
        ls_result = LS_RANGE;
    } /* if (link_id >= ls_max_links) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (LS_OK == ls_result)
    {
        if (NULL == bls_desc_table[link_id])
        {
            *link_type = SV_LINK_TYPE_UNKNOWN;
            memset((void*)link_name, 0, 32);
            ls_result = LS_UNKNOWN_LINK;
        } /* if (NULL == bls_desc_table[link_id]) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        /* set type of the link */
        *link_type = bls_desc_table[link_id]->link_type;

        /* set name of the link */
        memcpy                                          \
        (                                               \
            (void*)link_name,                           \
            (void*)&bls_desc_table[link_id]->link_name, \
            32                                          \
        );
    } /* if (LS_OK == ls_result) */

    return(ls_result);

} /* ls_read_link_descriptor */
