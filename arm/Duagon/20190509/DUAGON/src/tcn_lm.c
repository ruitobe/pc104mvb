/* ==========================================================================
 *
 *  File      : TCN_LM.C
 *
 *  Purpose   : Link Layer Interface for Message Data
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
#include <tcn_bls.h>
#include <tcn_md.h>
#include <tcn_lm.h>


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
TCN_DECL_CONST UNSIGNED16   lm_max_busses = LM_MAX_BUSSES;


/* ==========================================================================
 *
 *  Common Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : lm_init
 *
 *  Purpose   : Initialises a message data link layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              lm_init
 *              (
 *                  ENUM8               bus_id,
 *                  LM_RECEIVE_INDICATE nm_receive_indicate,
 *                  LM_GET_PACK         nm_get_pack,
 *                  void                **owner,
 *                  LM_SEND_CONFIRM     nm_send_confirm,
 *                  LM_STATUS_INDICATE  nm_status_indicate
 *              );
 *
 *  Input     : bus_id              - bus identifier (0..15)
 *              nm_receive_indicate - procedure of the network layer
 *                                    which the link layer calls each
 *                                    time it receives a packet
 *              nm_get_pack         - procedure of the packet pool which
 *                                    the link layer calls each time it
 *                                    needs a void packet
 *              owner               - packet pool the link layer use
 *                                    to get packets
 *              nm_send_confirm     - procedure of the packet pool which
 *                                    the link layer calls to dispose of
 *                                    a packet
 *              nm_status_indicate  - procedure of the network layer
 *                                    which the link layer calls to
 *                                    signal an exception (e.g. inauguration)
 *
 *  Return        : any MD_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MD_RESULT
lm_init
(
    ENUM8               bus_id,
    LM_RECEIVE_INDICATE nm_receive_indicate,
    LM_GET_PACK         nm_get_pack,
    void                **owner,
    LM_SEND_CONFIRM     nm_send_confirm,
    LM_STATUS_INDICATE  nm_status_indicate
)
{
    MD_RESULT   md_result = MD_OK;

#ifdef TCN_CHECK_PARAMETER
    if (bus_id >= lm_max_busses)
    {
        md_result = MD_REJECT;
    } /* if (bus_id >= lm_max_busses) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id])
        {
            md_result = MD_REJECT;
        } /* if (NULL == bls_desc_table[bus_id]) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id]->desc_md.lm_init)
        {
            md_result = MD_REJECT;
        } /* if (NULL == bls_desc_table[bus_id]->desc_md.lm_init) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        md_result =                                 \
            bls_desc_table[bus_id]->desc_md.lm_init \
            (                                       \
                bls_desc_table[bus_id]->p_bus_ctrl, \
                nm_receive_indicate,                \
                nm_get_pack,                        \
                owner,                              \
                nm_send_confirm,                    \
                nm_status_indicate                  \
            );
    } /* if (MD_OK == md_result) */

    return(md_result);

} /* lm_init */


/* --------------------------------------------------------------------------
 *  Procedure : lm_get_dev_address
 *
 *  Purpose   : Reads the own Device_Address.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              lm_get_dev_address
 *              (
 *                  ENUM8       bus_id,
 *                  WORD32      *device_address
 *              );
 *
 *  Input     : bus_id         - bus identifier (0..15)
 *
 *  Return    : any MD_RESULT
 *
 *  Output    : device_address - Device_Address of that device on this
 *                               link layer
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MD_RESULT
lm_get_dev_address
(
    ENUM8       bus_id,
    WORD32      *device_address
)
{
    MD_RESULT   md_result = MD_OK;

#ifdef TCN_CHECK_PARAMETER
    if (bus_id >= lm_max_busses)
    {
        md_result = MD_REJECT;
    } /* if (bus_id >= lm_max_busses) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id])
        {
            md_result = MD_REJECT;
        } /* if (NULL == bls_desc_table[bus_id]) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id]->desc_md.lm_get_dev_address)
        {
            md_result = MD_REJECT;
        } /* if (NULL == ...) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        md_result =                                             \
            bls_desc_table[bus_id]->desc_md.lm_get_dev_address  \
            (                                                   \
                bls_desc_table[bus_id]->p_bus_ctrl,             \
                device_address                                  \
            );
    } /* if (MD_OK == md_result) */

    return(md_result);

} /* lm_get_dev_address */



/* --------------------------------------------------------------------------
 *  Procedure : lm_get_status
 *
 *  Purpose   : Retrieves the link layer status.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              lm_get_status
 *              (
 *                  ENUM8       bus_id,
 *                  BITSET8     selector,
 *                  BITSET8     reset,
 *                  BITSET8     *status
 *              );
 *
 *  Input     : bus_id         - bus identifier (0..15)
 *              selector       - selects the interesting bits returned
 *                               in status
 *              reset          - selects the bits which are to be
 *                               cleared afterwards
 *
 *  Return    : any MD_RESULT
 *
 *  Output    : status         - returns the value of the selected
 *                               status bits
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MD_RESULT
lm_get_status
(
    ENUM8       bus_id,
    BITSET8     selector,
    BITSET8     reset,
    BITSET8     *status
)
{
    MD_RESULT   md_result = MD_OK;

#ifdef TCN_CHECK_PARAMETER
    if (bus_id >= lm_max_busses)
    {
        md_result = MD_REJECT;
    } /* if (bus_id >= lm_max_busses) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id])
        {
            md_result = MD_REJECT;
        } /* if (NULL == bls_desc_table[bus_id]) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id]->desc_md.lm_get_status)
        {
            md_result = MD_REJECT;
        } /* if (NULL == bls_desc_table[bus_id]->desc_md.lm_get_status) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        md_result =                                         \
            bls_desc_table[bus_id]->desc_md.lm_get_status   \
            (                                               \
                bls_desc_table[bus_id]->p_bus_ctrl,         \
                selector,                                   \
                reset,                                      \
                status                                      \
            );
    } /* if (MD_OK == md_result) */

    return(md_result);

} /* lm_get_status */


/* --------------------------------------------------------------------------
 *  Procedure : lm_send_queue_flush
 *
 *  Purpose   : Flushes the send queue.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              lm_send_queue_flush
 *              (
 *                  ENUM8       bus_id
 *              );
 *
 *  Input     : bus_id         - bus identifier (0..15)
 *
 *  Return    : any MD_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MD_RESULT
lm_send_queue_flush
(
    ENUM8       bus_id
)
{
    MD_RESULT   md_result = MD_OK;

#ifdef TCN_CHECK_PARAMETER
    if (bus_id >= lm_max_busses)
    {
        md_result = MD_REJECT;
    } /* if (bus_id >= lm_max_busses) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id])
        {
            md_result = MD_REJECT;
        } /* if (NULL == bls_desc_table[bus_id]) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id]->desc_md.lm_send_queue_flush)
        {
            md_result = MD_REJECT;
        } /* if (NULL == ...) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        md_result =                                             \
            bls_desc_table[bus_id]->desc_md.lm_send_queue_flush \
            (                                                   \
                bls_desc_table[bus_id]->p_bus_ctrl              \
            );
    } /* if (MD_OK == md_result) */

    return(md_result);

} /* lm_send_queue_flush */


/* --------------------------------------------------------------------------
 *  Procedure : lm_send_request
 *
 *  Purpose   : Requests to send a packet.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              lm_send_request
 *              (
 *                  ENUM8       bus_id,
 *                  UNSIGNED32  source,
 *                  UNSIGNED32  destination,
 *                  MD_PACKET   *packet
 *              );
 *
 *  Input     : bus_id      - bus identifier (0..15)
 *              source      - Device_Address of the source device.
 *                            If 'source' is zero, the link layer
 *                            includes into the packet its own
 *                            Device_Address.
 *              destination - Device_Address of the destination device,
 *                            or a broadcast address if set to the
 *                            highest possible Device_Address.
 *              packet      - Pointer to a data structure which contains
 *                            the packet. The size and status of the
 *                            packet are contained in the packet itself.
 *
 *  Return    : any MD_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MD_RESULT
lm_send_request
(
    ENUM8       bus_id,
    UNSIGNED32  source,
    UNSIGNED32  destination,
    MD_PACKET   *packet
)
{
    MD_RESULT   md_result = MD_OK;

#ifdef TCN_CHECK_PARAMETER
    if (bus_id >= lm_max_busses)
    {
        md_result = MD_REJECT;
    } /* if (bus_id >= lm_max_busses) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id])
        {
            md_result = MD_REJECT;
        } /* if (NULL == bls_desc_table[bus_id]) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id]->desc_md.lm_send_request)
        {
            md_result = MD_REJECT;
        } /* if (NULL == bls_desc_table[bus_id]->desc_md.lm_send_request) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        md_result =                                         \
            bls_desc_table[bus_id]->desc_md.lm_send_request \
            (                                               \
                bls_desc_table[bus_id]->p_bus_ctrl,         \
                source,                                     \
                destination,                                \
                packet                                      \
            );
    } /* if (MD_OK == md_result) */

    return(md_result);

} /* lm_send_request */



/* --------------------------------------------------------------------------
 *  Procedure : lm_receive_poll
 *
 *  Purpose   : Polls for received packets.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              lm_receive_poll
 *              (
 *                  ENUM8       bus_id,
 *                  UNSIGNED32  *source,
 *                  UNSIGNED32  *destination,
 *                  MD_PACKET   **packet
 *              );
 *
 *  Input     : bus_id      - bus identifier (0..15)
 *
 *  Return    : any MD_RESULT
 *
 *  Output    : source      - Device_Address of the source device
 *              destination - Device_Address of the destination device
 *                            or 'broadcast'
 *              packet      - Pointer to a data structure which contains
 *                            the received packet, or NULL if the queue
 *                            is empty. The size of the packet is
 *                            contained in the packet itself.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MD_RESULT
lm_receive_poll
(
    ENUM8       bus_id,
    UNSIGNED32  *source,
    UNSIGNED32  *destination,
    MD_PACKET   **packet
)
{
    MD_RESULT   md_result = MD_OK;

#ifdef TCN_CHECK_PARAMETER
    if (bus_id >= lm_max_busses)
    {
        md_result = MD_REJECT;
    } /* if (bus_id >= lm_max_busses) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id])
        {
            md_result = MD_REJECT;
        } /* if (NULL == bls_desc_table[bus_id]) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        if (NULL == bls_desc_table[bus_id]->desc_md.lm_receive_poll)
        {
            md_result = MD_REJECT;
        } /* if (NULL == bls_desc_table[bus_id]->desc_md.lm_receive_poll) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        md_result =                                         \
            bls_desc_table[bus_id]->desc_md.lm_receive_poll \
            (                                               \
                bls_desc_table[bus_id]->p_bus_ctrl,         \
                source,                                     \
                destination,                                \
                packet                                      \
            );
    } /* if (MD_OK == md_result) */

    return(md_result);

} /* lm_receive_poll */


#endif /* #ifdef TCN_MD */
