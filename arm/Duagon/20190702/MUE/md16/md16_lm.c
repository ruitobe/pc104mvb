/* ==========================================================================
 *
 *  File      : MD16_LM.C
 *
 *  Purpose   : Bus Controller Link Layer for MD16 - 
 *                  Link Layer Bus Controller Interface for Message Data
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
#if defined(TCN_MD) && defined(MUELL_MD16)


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
#include <tcn_md.h>
#include <tcn_lm.h>

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer
 * --------------------------------------------------------------------------
 */
#include <md16_bc.h>
#include <md16_lm.h>

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>
#include <mue_sv.h>
#include <mue_md.h>


/* ==========================================================================
 *
 *  Public Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
TCN_DECL_CONST \
    UNSIGNED16  md16_lm_frame_size = MD16_LM_FRAME_SIZE;


/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Endian conversion macros
 * --------------------------------------------------------------------------
 */
#define MD16_LM_MACRO_SWAP32(_address_)                                     \
    TCN_MACRO_START                                                         \
        WORD8       _buffer[4];                                             \
        WORD8       *_p = (WORD8*)(_address_);                              \
                                                                            \
        _buffer[0] = _p[0];                                                 \
        _buffer[1] = _p[1];                                                 \
        _buffer[2] = _p[2];                                                 \
        _buffer[3] = _p[3];                                                 \
                                                                            \
        _p[0] = _buffer[3];                                                 \
        _p[1] = _buffer[2];                                                 \
        _p[2] = _buffer[1];                                                 \
        _p[3] = _buffer[0];                                                 \
    TCN_MACRO_END

#ifdef TCN_LE
#   define MD16_LM_MACRO_CONVERT_CPU_TO_BE32(_x_) \
        MD16_LM_MACRO_SWAP32((_x_))
#   define MD16_LM_MACRO_CONVERT_BE32_TO_CPU(_x_) \
        MD16_LM_MACRO_SWAP32((_x_))
#else /* #ifdef TCN_LE */
#   define MD16_LM_MACRO_CONVERT_CPU_TO_BE32(_x_)
#   define MD16_LM_MACRO_CONVERT_BE32_TO_CPU(_x_)
#endif /* #else */

/* --------------------------------------------------------------------------
 *  Mutex for UART access
 * --------------------------------------------------------------------------
 */
#if (MD16_BC_MUTEX_UART == 1)

#define MD16_LM_MACRO_UART_MUTEX_LOCK(_p_osl_mutex_object_uart_)            \
    pthread_mutex_lock((_p_osl_mutex_object_uart_))

#define MD16_LM_MACRO_UART_MUTEX_UNLOCK(_p_osl_mutex_object_uart_)          \
    pthread_mutex_unlock((_p_osl_mutex_object_uart_))

#else /* #if (MD16_BC_MUTEX_UART == 1) */

#define MD16_LM_MACRO_UART_MUTEX_LOCK(_p_osl_mutex_object_uart_)
#define MD16_LM_MACRO_UART_MUTEX_UNLOCK(_p_osl_mutex_object_uart_)

#endif /* #else */


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : md16_lm_init (of procedure type 'LM_INIT')
 *
 *  Purpose   : Initialises a message data link layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              md16_lm_init
 *              (
 *                  void                *p_bus_ctrl,
 *                  LM_RECEIVE_INDICATE nm_receive_indicate,
 *                  LM_GET_PACK         nm_get_pack,
 *                  void                **owner,
 *                  LM_SEND_CONFIRM     nm_send_confirm,
 *                  LM_STATUS_INDICATE  nm_status_indicate
 *              );
 *
 *  Input     : p_bus_ctrl          - pointer to bus controller specific
 *                                    values
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
 *                                    signal an exception
 *                                    (e.g. inauguration)
 *
 *  Return    : any MD_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MD_RESULT
md16_lm_init
(
    void                *p_bus_ctrl,
    LM_RECEIVE_INDICATE nm_receive_indicate,
    LM_GET_PACK         nm_get_pack,
    void                **owner,
    LM_SEND_CONFIRM     nm_send_confirm,
    LM_STATUS_INDICATE  nm_status_indicate
)
{
    MD_RESULT       md_result        = MD_OK;
    MD16_BUS_CTRL   *p_md16_bus_ctrl = (MD16_BUS_CTRL*)p_bus_ctrl;
    BLS_DESC_MD     *p_bls_desc_md;
    MUE_RESULT      mue_result;
    BITSET8         mue_md_control;
    BITSET8         mue_md_status;

    MD16_LM_MACRO_UART_MUTEX_LOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    /* ----------------------------------------------------------------------
     *  initialise SW-queue (make empty)
     * ----------------------------------------------------------------------
     */
    p_md16_bus_ctrl->lm_tx_fifo_size = MD16_BC_LM_TX_FIFO_SIZE;
    memset((void*)p_md16_bus_ctrl->lm_tx_fifo, 0, \
        sizeof(p_md16_bus_ctrl->lm_tx_fifo));
    p_md16_bus_ctrl->lm_tx_fifo_pos_read  = 0;
    p_md16_bus_ctrl->lm_tx_fifo_pos_write = 0;

    /* ----------------------------------------------------------------------
     *  clear subscribed procedures to the LMBI
     * ----------------------------------------------------------------------
     */
    p_bls_desc_md = (BLS_DESC_MD*)&(bls_desc_table[0]->desc_md);
    p_bls_desc_md->lm_receive_indicate = (LM_RECEIVE_INDICATE)NULL;
    p_bls_desc_md->lm_get_pack         = (LM_GET_PACK)NULL;
    p_bls_desc_md->lm_packet_pool      = (void**)NULL;
    p_bls_desc_md->lm_send_confirm     = (LM_SEND_CONFIRM)NULL;
    p_bls_desc_md->lm_status_indicate  = (LM_STATUS_INDICATE)NULL;

    /* ----------------------------------------------------------------------
     *  MD control and status
     * ----------------------------------------------------------------------
     */
    mue_md_control = (BITSET8)(mue_md_control_bit_ftx  | \
                                mue_md_control_bit_frx | \
                                mue_md_control_bit_reset_mask);
    mue_result = mue_md_control_and_status(p_bus_ctrl, mue_md_control, \
        &mue_md_status);
    if (MUE_RESULT_OK != mue_result)
    {
        /* store last result of MUE */
        p_md16_bus_ctrl->mue_result = mue_result;

        md_result = MD_REJECT;
    } /* if (MUE_RESULT_OK != mue_result) */

    /* ----------------------------------------------------------------------
     *  set subscribed procedures to the LMBI
     * ----------------------------------------------------------------------
     */
    if (MD_OK == md_result)
    {
        p_bls_desc_md = (BLS_DESC_MD*)&(bls_desc_table[0]->desc_md);
        p_bls_desc_md->lm_receive_indicate = nm_receive_indicate;
        p_bls_desc_md->lm_get_pack         = nm_get_pack;
        p_bls_desc_md->lm_packet_pool      = owner;
        p_bls_desc_md->lm_send_confirm     = nm_send_confirm;
        p_bls_desc_md->lm_status_indicate  = nm_status_indicate;
    } /* if (MD_OK == md_result) */

    MD16_LM_MACRO_UART_MUTEX_UNLOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    return(md_result);

} /* md16_lm_init */


/* --------------------------------------------------------------------------
 *  Procedure : md16_lm_get_dev_address
 *              (of procedure type 'LM_GET_DEV_ADDRESS')
 *
 *  Purpose   : Reads the own Device_Address.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              md16_lm_get_dev_address
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD32      *device_address
 *              );
 *
 *  Input     : p_bus_ctrl     - pointer to bus controller specific
 *                               values
 *
 *  Return    : any MD_RESULT
 *
 *  Output    : device_address - Device_Address of that device on this
 *                               link layer
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MD_RESULT
md16_lm_get_dev_address
(
    void        *p_bus_ctrl,
    WORD32      *device_address
)
{
    MD_RESULT       md_result         = MD_OK;
    MD16_BUS_CTRL   *p_md16_bus_ctrl  = (MD16_BUS_CTRL*)p_bus_ctrl;
    MUE_RESULT      mue_result;
    BITSET16        mue_device_config = 0x0000;

    MD16_LM_MACRO_UART_MUTEX_LOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    /* ----------------------------------------------------------------------
     *  get device config
     * ----------------------------------------------------------------------
     */
    mue_result = mue_sv_get_device_config(p_bus_ctrl, &mue_device_config);
    if (MUE_RESULT_OK != mue_result)
    {
        /* store last result of MUE */
        p_md16_bus_ctrl->mue_result = mue_result;

        md_result = MD_REJECT;
    } /* if (MUE_RESULT_OK != mue_result) */

    MD16_LM_MACRO_UART_MUTEX_UNLOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    if (MD_OK == md_result)
    {
        *device_address = \
            (WORD32)(mue_device_config & mue_sv_device_config_da_mask);
    } /* if (MD_OK == md_result) */

    return(md_result);

} /* md16_lm_get_dev_address */


/* --------------------------------------------------------------------------
 *  Procedure : md16_lm_get_status (of procedure type 'LM_GET_STATUS')
 *
 *  Purpose   : Retrieves the link layer status.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              md16_lm_get_status
 *              (
 *                  void        *p_bus_ctrl,
 *                  BITSET8     selector,
 *                  BITSET8     reset,
 *                  BITSET8     *status
 *              );
 *
 *  Input     : p_bus_ctrl     - pointer to bus controller specific
 *                               values
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
md16_lm_get_status
(
    void        *p_bus_ctrl,
    BITSET8     selector,
    BITSET8     reset,
    BITSET8     *status
)
{
    MD_RESULT       md_result        = MD_OK;
    MD16_BUS_CTRL   *p_md16_bus_ctrl = (MD16_BUS_CTRL*)p_bus_ctrl;
    MUE_RESULT      mue_result;
    BITSET8         mue_md_control;
    BITSET8         mue_md_status    = 0x00;

    MD16_LM_MACRO_UART_MUTEX_LOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    /* ----------------------------------------------------------------------
     *  MD control and status
     * ----------------------------------------------------------------------
     */
    mue_md_control = (BITSET8)(reset & mue_md_control_bit_reset_mask);
    mue_result     = mue_md_control_and_status(p_bus_ctrl, mue_md_control, \
        &mue_md_status);
    if (MUE_RESULT_OK != mue_result)
    {
        /* store last result of MUE */
        p_md16_bus_ctrl->mue_result = mue_result;

        md_result = MD_REJECT;
    } /* if (MUE_RESULT_OK != mue_result) */

    MD16_LM_MACRO_UART_MUTEX_UNLOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    if (MD_OK == md_result)
    {
        *status = (BITSET8) \
            (mue_md_status & mue_md_control_bit_reset_mask & selector);
    } /* if (MD_OK == md_result) */

    return(md_result);

} /* md16_lm_get_status */


/* --------------------------------------------------------------------------
 *  Procedure : md16_lm_send_queue_flush
 *              (of procedure type 'LM_SEND_QUEUE_FLUSH')
 *
 *  Purpose   : Flushes the send queue.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              md16_lm_send_queue_flush
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : any MD_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MD_RESULT
md16_lm_send_queue_flush
(
    void        *p_bus_ctrl
)
{
    MD_RESULT       md_result        = MD_OK;
    MD16_BUS_CTRL   *p_md16_bus_ctrl = (MD16_BUS_CTRL*)p_bus_ctrl;
    MUE_RESULT      mue_result;
    BITSET8         mue_md_control   = mue_md_control_bit_ftx;
    BITSET8         mue_md_status    = 0x00;

    MD16_LM_MACRO_UART_MUTEX_LOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    /* ----------------------------------------------------------------------
     *  initialise SW-queue (make empty)
     * ----------------------------------------------------------------------
     */
    p_md16_bus_ctrl->lm_tx_fifo_size = MD16_BC_LM_TX_FIFO_SIZE;
    memset((void*)p_md16_bus_ctrl->lm_tx_fifo, 0, \
        sizeof(p_md16_bus_ctrl->lm_tx_fifo));
    p_md16_bus_ctrl->lm_tx_fifo_pos_read  = 0;
    p_md16_bus_ctrl->lm_tx_fifo_pos_write = 0;

    /* ----------------------------------------------------------------------
     *  MD control and status
     * ----------------------------------------------------------------------
     */
    mue_result = mue_md_control_and_status(p_bus_ctrl, mue_md_control, \
        &mue_md_status);
    if (MUE_RESULT_OK != mue_result)
    {
        /* store last result of MUE */
        p_md16_bus_ctrl->mue_result = mue_result;

        md_result = MD_REJECT;
    } /* if (MUE_RESULT_OK != mue_result) */

    MD16_LM_MACRO_UART_MUTEX_UNLOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    return(md_result);

} /* md16_lm_send_queue_flush */


/* --------------------------------------------------------------------------
 *  Procedure : md16_lm_send_request (of procedure type 'LM_SEND_REQUEST')
 *
 *  Purpose   : Requests to send a packet.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              md16_lm_send_request
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED32  source,
 *                  UNSIGNED32  destination,
 *                  MD_PACKET   *packet
 *              );
 *
 *  Input     : p_bus_ctrl  - pointer to bus controller specific values
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
md16_lm_send_request
(
    void        *p_bus_ctrl,
    UNSIGNED32  source,
    UNSIGNED32  destination,
    MD_PACKET   *packet
)
{
    MD_RESULT                   md_result          = MD_OK;
    MD16_BUS_CTRL               *p_md16_bus_ctrl   = \
                                    (MD16_BUS_CTRL*)p_bus_ctrl;
    WORD32                      device_address;
    MD_PACKET_LINK_HEADER_MVB   *p_link_header_mvb;
    MD16_BC_LM_FIFO_PACKET      *p_tx_fifo         = NULL;

    if (0 == source)
    {
        /* get own Device_Address */
        md_result =                         \
            md16_lm_get_dev_address         \
            (                               \
                p_bus_ctrl,                 \
                (WORD32*)&device_address    \
            );
        if (MD_OK == md_result)
        {
            source = device_address;
        } /* if (MD_OK == md_result) */
    } /* if (0 == source) */

    /* ----------------------------------------------------------------------
     *  build MVB Link_Header of Frame_Data
     * ----------------------------------------------------------------------
     */
    if (MD_OK == md_result)
    {
        p_link_header_mvb = \
            (MD_PACKET_LINK_HEADER_MVB*)&(packet->frame_data.link_header[0]);
        p_link_header_mvb->mode = MD_PACKET_LINK_HEADER_MVB_MODE_SINGLE_CAST;
        p_link_header_mvb->dd   = (WORD16)destination;
        p_link_header_mvb->pt   = MD_PACKET_LINK_HEADER_MVB_PT_TCN;
        p_link_header_mvb->sd   = (WORD16)source;

        MD16_LM_MACRO_CONVERT_CPU_TO_BE32(p_link_header_mvb);

        packet->next = NULL;
    } /* if (MD_OK == md_result) */

    /* ----------------------------------------------------------------------
     *  put packet to SW-queue
     * ----------------------------------------------------------------------
     */
    if (MD_OK == md_result)
    {
        p_tx_fifo = &p_md16_bus_ctrl->lm_tx_fifo \
            [p_md16_bus_ctrl->lm_tx_fifo_pos_write];
        if (TRUE == p_tx_fifo->ready)
        {
            md_result = MD_REJECT;
        } /* if (TRUE == p_tx_fifo->ready) */
    }
    if (MD_OK == md_result)
    {
        memcpy                                  \
        (                                       \
            (void*)&(p_tx_fifo->frame_data),    \
            (void*)&(packet->frame_data),       \
            md16_lm_frame_size                  \
        );
        p_tx_fifo->ready = TRUE;
        if (++(p_md16_bus_ctrl->lm_tx_fifo_pos_write) >= \
            p_md16_bus_ctrl->lm_tx_fifo_size)
        {
            p_md16_bus_ctrl->lm_tx_fifo_pos_write = 0;
        } /* if (++(p_md16_bus_ctrl->lm_tx_fifo_pos_write) >= ...) */

        /* set packet status */
        packet->status = MD_SENT;

        /* return packet to pool */
        bls_desc_table[0]->desc_md.lm_send_confirm(packet);
    } /* if (MD_OK == md_result) */

    return(md_result);

} /* md16_lm_send_request */


/* --------------------------------------------------------------------------
 *  Procedure : md16_lm_send_fifo_packets
 *
 *  Purpose   : Tries to send the MD packets
 *              (i.e. put from SW-queue to HW-queue).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              md16_lm_send_fifo_packets (void);
 *
 *  Return    : any MD_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MD_RESULT
md16_lm_send_fifo_packets (void)
{
    MD_RESULT               md_result        = MD_OK;
    ENUM8                   bus_id;
    MD16_BUS_CTRL           *p_md16_bus_ctrl;
    MD16_BC_LM_FIFO_PACKET  *p_tx_fifo;
    MUE_RESULT              mue_result;
    BITSET8                 mue_md_control   = 0x00;
    BITSET8                 mue_md_status    = 0x00;

    for (bus_id=0; bus_id<lm_max_busses; bus_id++)
    {
        if (NULL != bls_desc_table[bus_id])
        {
            if (md16_lm_init == bls_desc_table[bus_id]->desc_md.lm_init)
            {
                p_md16_bus_ctrl = (MD16_BUS_CTRL*) \
                    bls_desc_table[bus_id]->p_bus_ctrl;

                p_tx_fifo = &(p_md16_bus_ctrl->lm_tx_fifo) \
                    [p_md16_bus_ctrl->lm_tx_fifo_pos_read];

                while ((TRUE == p_tx_fifo->ready) && (MD_OK == md_result))
                {
                    MD16_LM_MACRO_UART_MUTEX_LOCK( \
                        &p_md16_bus_ctrl->osl_mutex_object_uart);

                    /* ------------------------------------------------------
                     *  MD control and status
                     * ------------------------------------------------------
                     */
                    mue_result =                    \
                        mue_md_control_and_status   \
                        (                           \
                            (void*)p_md16_bus_ctrl, \
                            mue_md_control,         \
                            &mue_md_status          \
                        );
                    if (MUE_RESULT_OK != mue_result)
                    {
                        /* store last result of MUE */
                        p_md16_bus_ctrl->mue_result = mue_result;

                        md_result = MD_REJECT;
                    } /* if (MUE_RESULT_OK != mue_result) */

                    if (MD_OK == md_result)
                    {
                        /* check, if transmit queue is full */
                        if (mue_md_status & mue_md_status_bit_txqf)
                        {
                            /* transmit queue is full */
                            md_result = MD_REJECT;
                        } /* if (mue_md_status & mue_md_status_bit_txqf) */
                    } /* if (MD_OK == md_result) */

                    /* ------------------------------------------------------
                     *  MD transmit
                     * ------------------------------------------------------
                     */
                    if (MD_OK == md_result)
                    {
                        mue_result =                            \
                            mue_md_transmit                     \
                            (                                   \
                                (void*)p_md16_bus_ctrl,         \
                                (void*)&(p_tx_fifo->frame_data) \
                            );
                        if (MUE_RESULT_OK != mue_result)
                        {
                            /* store last result of MUE */
                            p_md16_bus_ctrl->mue_result = mue_result;

                            md_result = MD_REJECT;
                        } /* if (MUE_RESULT_OK != mue_result) */
                    } /* if (MD_OK == md_result) */

                    MD16_LM_MACRO_UART_MUTEX_UNLOCK( \
                        &p_md16_bus_ctrl->osl_mutex_object_uart);

                    if (MD_OK == md_result)
                    {
                        p_tx_fifo->ready = FALSE;

                        if (++(p_md16_bus_ctrl->lm_tx_fifo_pos_read) >= \
                            p_md16_bus_ctrl->lm_tx_fifo_size)
                        {
                            p_md16_bus_ctrl->lm_tx_fifo_pos_read = 0;
                        } /* if (...) */

                        p_tx_fifo = &(p_md16_bus_ctrl->lm_tx_fifo) \
                            [p_md16_bus_ctrl->lm_tx_fifo_pos_read];

                    } /* if (MD_OK == md_result) */

                } /* while ((TRUE == p_tx_fifo->ready) && (...)) */
            } /* if (md16_lm_init == ...) */
        } /* if (NULL != bls_desc_table[bus_id]) */
    } /* for (bus_id=0; bus_id<lm_max_busses; bus_id++) */

    return(md_result);

} /* md16_lm_send_fifo_packets */


/* --------------------------------------------------------------------------
 *  Procedure : md16_lm_receive_poll (of procedure type 'LM_RECEIVE_POLL')
 *
 *  Purpose   : Polls for received packets.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              md16_lm_receive_poll
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED32  *source,
 *                  UNSIGNED32  *destination,
 *                  MD_PACKET   **packet
 *              );
 *
 *  Input     : p_bus_ctrl  - pointer to bus controller specific values
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
md16_lm_receive_poll
(
    void        *p_bus_ctrl,
    UNSIGNED32  *source,
    UNSIGNED32  *destination,
    MD_PACKET   **packet
)
{
    MD_RESULT                   md_result          = MD_OK;
    MD16_BUS_CTRL               *p_md16_bus_ctrl   = \
                                    (MD16_BUS_CTRL*)p_bus_ctrl;
    MUE_RESULT                  mue_result;
    BITSET8                     mue_md_control;
    BITSET8                     mue_md_status;
    MD_PACKET                   *p                 = NULL;
    MD_PACKET_LINK_HEADER_MVB   *p_link_header_mvb = NULL;

    MD16_LM_MACRO_UART_MUTEX_LOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    /* ----------------------------------------------------------------------
     *  MD control and status
     * ----------------------------------------------------------------------
     */
    mue_md_control = (BITSET8) \
        (mue_md_control_bit_reset_rxac | mue_md_control_bit_reset_rxov);
    mue_md_status  = (BITSET8)0x00;
    mue_result     = mue_md_control_and_status(p_bus_ctrl, mue_md_control, \
        &mue_md_status);
    if (MUE_RESULT_OK != mue_result)
    {
        /* store last result of MUE */
        p_md16_bus_ctrl->mue_result = mue_result;

        md_result = MD_REJECT;
    } /* if (MUE_RESULT_OK != mue_result) */

    if (MD_OK == md_result)
    {
        /* check, if something received */
        if (0 == (mue_md_status & mue_md_status_bit_rxac))
        {
            /* receive queue is empty */
            md_result = MD_REJECT;
        } /* if (0 == (mue_md_status & mue_md_status_bit_rxac)) */
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        /* get packet from pool */
        bls_desc_table[0]->desc_md.lm_get_pack \
            (bls_desc_table[0]->desc_md.lm_packet_pool, &p);
        if (NULL == p)
        {
            /* no packet available */
            md_result = MD_REJECT;
        } /* if (NULL == p) */
    } /* if (MD_OK == md_result) */

    /* ----------------------------------------------------------------------
     *  MD receive
     * ----------------------------------------------------------------------
     */
    if (MD_OK == md_result)
    {
        mue_result = mue_md_receive(p_bus_ctrl, (void*)&(p->frame_data));
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_md16_bus_ctrl->mue_result = mue_result;

            md_result = MD_REJECT;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (MD_OK == md_result) */

    /* ----------------------------------------------------------------------
     *  extract MVB Link_Header from Frame_Data
     * ----------------------------------------------------------------------
     */
    if (MD_OK == md_result)
    {
        p_link_header_mvb = \
            (MD_PACKET_LINK_HEADER_MVB*)&(p->frame_data.link_header[0]);
    } /* if (MD_OK == md_result) */

    if (MD_OK == md_result)
    {
        MD16_LM_MACRO_CONVERT_BE32_TO_CPU(p_link_header_mvb);

        *source      = (UNSIGNED32)p_link_header_mvb->sd;
        *destination = (UNSIGNED32)p_link_header_mvb->dd;

        *packet      = p;
    } /* if (MD_OK == md_result) */

    MD16_LM_MACRO_UART_MUTEX_UNLOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    return(md_result);

} /* md16_lm_receive_poll */


#endif /* #if defined(TCN_MD) && defined(MUELL_MD16) */
