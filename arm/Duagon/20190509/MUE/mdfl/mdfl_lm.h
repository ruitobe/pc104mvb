#ifndef MDFL_LM_H
#define MDFL_LM_H

/* ==========================================================================
 *
 *  File      : MDFL_LM.H
 *
 *  Purpose   : Bus Controller Link Layer for MDFULL - 
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
#include <tcn_lm.h>


/* ==========================================================================
 *
 *  Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
#define MDFULL_LM_FRAME_SIZE                32
TCN_DECL_PUBLIC TCN_DECL_CONST UNSIGNED16   mdfull_lm_frame_size;


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mdfull_lm_init (of procedure type 'LM_INIT')
 *
 *  Purpose   : Initialises a message data link layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              mdfull_lm_init
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
mdfull_lm_init
(
    void                *p_bus_ctrl,
    LM_RECEIVE_INDICATE nm_receive_indicate,
    LM_GET_PACK         nm_get_pack,
    void                **owner,
    LM_SEND_CONFIRM     nm_send_confirm,
    LM_STATUS_INDICATE  nm_status_indicate
);


/* --------------------------------------------------------------------------
 *  Procedure : mdfull_lm_get_dev_address
 *              (of procedure type 'LM_GET_DEV_ADDRESS')
 *
 *  Purpose   : Reads the own Device_Address.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              mdfull_lm_get_dev_address
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
mdfull_lm_get_dev_address
(
    void        *p_bus_ctrl,
    WORD32      *device_address
);


/* --------------------------------------------------------------------------
 *  Procedure : mdfull_lm_get_status (of procedure type 'LM_GET_STATUS')
 *
 *  Purpose   : Retrieves the link layer status.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              mdfull_lm_get_status
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
mdfull_lm_get_status
(
    void        *p_bus_ctrl,
    BITSET8     selector,
    BITSET8     reset,
    BITSET8     *status
);


/* --------------------------------------------------------------------------
 *  Procedure : mdfull_lm_send_queue_flush
 *              (of procedure type 'LM_SEND_QUEUE_FLUSH')
 *
 *  Purpose   : Flushes the send queue.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              mdfull_lm_send_queue_flush
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
mdfull_lm_send_queue_flush
(
    void        *p_bus_ctrl
);


/* --------------------------------------------------------------------------
 *  Procedure : mdfull_lm_send_request (of procedure type 'LM_SEND_REQUEST')
 *
 *  Purpose   : Requests to send a packet.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              mdfull_lm_send_request
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
mdfull_lm_send_request
(
    void        *p_bus_ctrl,
    UNSIGNED32  source,
    UNSIGNED32  destination,
    MD_PACKET   *packet
);


/* --------------------------------------------------------------------------
 *  Procedure : mdfull_lm_receive_poll (of procedure type 'LM_RECEIVE_POLL')
 *
 *  Purpose   : Polls for received packets.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MD_RESULT
 *              mdfull_lm_receive_poll
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
mdfull_lm_receive_poll
(
    void        *p_bus_ctrl,
    UNSIGNED32  *source,
    UNSIGNED32  *destination,
    MD_PACKET   **packet
);


#endif /* #ifndef MDFL_LM_H */
