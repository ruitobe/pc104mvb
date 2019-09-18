#ifndef TCN_LM_H
#define TCN_LM_H

/* ==========================================================================
 *
 *  File      : TCN_LM.H
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
#include <tcn_md.h>


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
#define LM_MAX_BUSSES                       16
TCN_DECL_PUBLIC TCN_DECL_CONST UNSIGNED16   lm_max_busses;


/* ==========================================================================
 *
 *  Subscribe and Indication Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure Type: LM_RECEIVE_INDICATE
 *
 *  Purpose       : Indicates that a packet was received.
 *
 *  Syntax        : typedef void
 *                  (*LM_RECEIVE_INDICATE)
 *                  (
 *                      ENUM8       bus_id
 *                  );
 *
 *  Input         : bus_id - bus identifier (0..15)
 * --------------------------------------------------------------------------
 */
typedef void
(*LM_RECEIVE_INDICATE)
(
    ENUM8       bus_id
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LM_GET_PACK
 *
 *  Purpose       : gets a void packet.
 *
 *  Syntax        : typedef void
 *                  (*LM_GET_PACK)
 *                  (
 *                      void        **owner,
 *                      MD_PACKET   **packet
 *                  );
 *
 *  Input         : owner  - identifies the pool from which the packet
 *                           is taken
 *                  packet - pointer to a data structure where the packet
 *                           is stored
 * --------------------------------------------------------------------------
 */
typedef void
(*LM_GET_PACK)
(
    void        **owner,
    MD_PACKET   **packet
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LM_SEND_CONFIRM
 *
 *  Purpose       : Indicates that a packet was sent.
 *
 *  Syntax        : typedef void
 *                  (*LM_SEND_CONFIRM)
 *                  (
 *                      MD_PACKET   *packet
 *                  );
 *
 *  Input         : packet - Pointer to a data structure which contains
 *                           the packet sent or flushed. The size and
 *                           status of the packet is contained in the
 *                           packet itself.
 * --------------------------------------------------------------------------
 */
typedef void
(*LM_SEND_CONFIRM)
(
    MD_PACKET   *packet
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LM_STATUS_INDICATE
 *
 *  Purpose       : Indicates a change of link layer status.
 *
 *  Syntax        : typedef void
 *                  (*LM_STATUS_INDICATE)
 *                  (
 *                      ENUM8       bus_id,
 *                      MD_RESULT   status
 *                  );
 *
 *  Input         : bus_id - bus identifier (0..15)
 *                  status - any MD_RESULT
 * --------------------------------------------------------------------------
 */
typedef void
(*LM_STATUS_INDICATE)
(
    ENUM8       bus_id,
    MD_RESULT   status
);


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure Type: LM_INIT
 *
 *  Purpose       : Initialises a message data link layer.
 *
 *  Syntax        : typedef MD_RESULT
 *                  (*LM_INIT)
 *                  (
 *                      void                *p_bus_ctrl,
 *                      LM_RECEIVE_INDICATE nm_receive_indicate,
 *                      LM_GET_PACK         nm_get_pack,
 *                      void                **owner,
 *                      LM_SEND_CONFIRM     nm_send_confirm,
 *                      LM_STATUS_INDICATE  nm_status_indicate
 *                  );
 *
 *  Input         : p_bus_ctrl          - pointer to bus controller specific
 *                                        values
 *                  nm_receive_indicate - procedure of the network layer
 *                                        which the link layer calls each
 *                                        time it receives a packet
 *                  nm_get_pack         - procedure of the packet pool which
 *                                        the link layer calls each time it
 *                                        needs a void packet
 *                  owner               - packet pool the link layer use
 *                                        to get packets
 *                  nm_send_confirm     - procedure of the packet pool which
 *                                        the link layer calls to dispose of
 *                                        a packet
 *                  nm_status_indicate  - procedure of the network layer
 *                                        which the link layer calls to
 *                                        signal an exception
 *                                        (e.g. inauguration)
 *
 *  Return        : any MD_RESULT
 * --------------------------------------------------------------------------
 */
typedef MD_RESULT
(*LM_INIT)
(
    void                *p_bus_ctrl,
    LM_RECEIVE_INDICATE nm_receive_indicate,
    LM_GET_PACK         nm_get_pack,
    void                **owner,
    LM_SEND_CONFIRM     nm_send_confirm,
    LM_STATUS_INDICATE  nm_status_indicate
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LM_GET_DEV_ADDRESS
 *
 *  Purpose       : Reads the own Device_Address.
 *
 *  Syntax        : typedef MD_RESULT
 *                  (*LM_GET_DEV_ADDRESS)
 *                  (
 *                      void        *p_bus_ctrl,
 *                      WORD32      *device_address
 *                  );
 *
 *  Input         : p_bus_ctrl     - pointer to bus controller specific
 *                                   values
 *
 *  Return        : any MD_RESULT
 *
 *  Output        : device_address - Device_Address of that device on this
 *                                   link layer
 * --------------------------------------------------------------------------
 */
typedef MD_RESULT
(*LM_GET_DEV_ADDRESS)
(
    void        *p_bus_ctrl,
    WORD32      *device_address
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LM_GET_STATUS
 *
 *  Purpose       : Retrieves the link layer status.
 *
 *  Syntax        : typedef MD_RESULT
 *                  (*LM_GET_STATUS)
 *                  (
 *                      void        *p_bus_ctrl,
 *                      BITSET8     selector,
 *                      BITSET8     reset,
 *                      BITSET8     *status
 *                  );
 *
 *  Input         : p_bus_ctrl     - pointer to bus controller specific
 *                                   values
 *                  selector       - selects the interesting bits returned
 *                                   in status
 *                  reset          - selects the bits which are to be
 *                                   cleared afterwards
 *
 *  Return        : any MD_RESULT
 *
 *  Output        : status         - returns the value of the selected
 *                                   status bits
 * --------------------------------------------------------------------------
 */
typedef MD_RESULT
(*LM_GET_STATUS)
(
    void        *p_bus_ctrl,
    BITSET8     selector,
    BITSET8     reset,
    BITSET8     *status
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LM_SEND_QUEUE_FLUSH
 *
 *  Purpose       : Flushes the send queue.
 *
 *  Syntax        : typedef MD_RESULT
 *                  (*LM_SEND_QUEUE_FLUSH)
 *                  (
 *                      void        *p_bus_ctrl
 *                  );
 *
 *  Input         : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return        : any MD_RESULT
 * --------------------------------------------------------------------------
 */
typedef MD_RESULT
(*LM_SEND_QUEUE_FLUSH)
(
    void        *p_bus_ctrl
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LM_SEND_REQUEST
 *
 *  Purpose       : Requests to send a packet.
 *
 *  Syntax        : typedef MD_RESULT
 *                  (*LM_SEND_REQUEST)
 *                  (
 *                      void        *p_bus_ctrl,
 *                      UNSIGNED32  source,
 *                      UNSIGNED32  destination,
 *                      MD_PACKET   *packet
 *                  );
 *
 *  Input         : p_bus_ctrl  - pointer to bus controller specific values
 *                  source      - Device_Address of the source device.
 *                                If 'source' is zero, the link layer
 *                                includes into the packet its own
 *                                Device_Address.
 *                  destination - Device_Address of the destination device,
 *                                or a broadcast address if set to the
 *                                highest possible Device_Address.
 *                  packet      - Pointer to a data structure which contains
 *                                the packet. The size and status of the
 *                                packet are contained in the packet itself.
 *
 *  Return        : any MD_RESULT
 * --------------------------------------------------------------------------
 */
typedef MD_RESULT
(*LM_SEND_REQUEST)
(
    void        *p_bus_ctrl,
    UNSIGNED32  source,
    UNSIGNED32  destination,
    MD_PACKET   *packet
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LM_RECEIVE_POLL
 *
 *  Purpose       : Polls for received packets.
 *
 *  Syntax        : typedef MD_RESULT
 *                  (*LM_RECEIVE_POLL)
 *                  (
 *                      void        *p_bus_ctrl,
 *                      UNSIGNED32  *source,
 *                      UNSIGNED32  *destination,
 *                      MD_PACKET   **packet
 *                  );
 *
 *  Input         : p_bus_ctrl  - pointer to bus controller specific values
 *
 *  Return        : any MD_RESULT
 *
 *  Output        : source      - Device_Address of the source device
 *                  destination - Device_Address of the destination device
 *                                or 'broadcast'
 *                  packet      - Pointer to a data structure which contains
 *                                the received packet, or NULL if the queue
 *                                is empty. The size of the packet is
 *                                contained in the packet itself.
 * --------------------------------------------------------------------------
 */
typedef MD_RESULT
(*LM_RECEIVE_POLL)
(
    void        *p_bus_ctrl,
    UNSIGNED32  *source,
    UNSIGNED32  *destination,
    MD_PACKET   **packet
);


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
);


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
);


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
);


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
);


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
);


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
);


#endif /* #ifndef TCN_LM_H */
