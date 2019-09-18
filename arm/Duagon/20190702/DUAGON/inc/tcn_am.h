#ifndef TCN_AM_H
#define TCN_AM_H

/* ==========================================================================
 *
 *  File      : TCN_AM.H
 *
 *  Purpose   : Application Layer Interface for Messages
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
#define AM_MAX_BUSSES       16
#define AM_MAX_CREDIT       7
#define AM_SYSTEM_ADDR      128
#define AM_SAME_NODE        0
#define AM_ROUTER_FCT       251
#define AM_AGENT_FCT        253
#define AM_MANAGER_FCT      254
#define AM_SAME_STATION     0
#define AM_UNKNOWN          255
#define AM_ANY_TOPO         0
#define AM_MAX_TOPO         63


/* --------------------------------------------------------------------------
 *  Enumerated Type   : AM_RESULT
 *
 *  Purpose           : Result of a procedure.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    AM_OK                   = 0,    /* successful termination              */
    AM_FAILURE              = 1,    /* unspecified failure                 */
    AM_BUS_ERR              = 2,    /* no bus transmission possible        */
    AM_REM_CONN_OVF         = 3,    /* too many incoming connections       */
    AM_CONN_TMO_ERR         = 4,    /* Connect_Request not answered        */
    AM_SEND_TMO_ERR         = 5,    /* send time-out (connect was OK)      */
    AM_REPLY_TMO_ERR        = 6,    /* no reply received                   */
    AM_ALIVE_TMO_ERR        = 7,    /* no complete message received        */
    AM_NO_LOC_MEM_ERR       = 8,    /* not enough memory or timers         */
    AM_NO_REM_MEM_ERR       = 9,    /* no more memory or timer (partner)   */
    AM_REM_CANC_ERR         = 10,   /* cancelled by partner                */
    AM_ALREADY_USED         = 11,   /* same operation already done         */
    AM_ADDR_FMT_ERR         = 12,   /* address format error                */
    AM_NO_REPLY_EXP_ERR     = 13,   /* no such reply expected              */
    AM_NR_OF_CALLS_OVF      = 14,   /* too many calls requested            */
    AM_REPLY_LEN_OVF        = 15,   /* Reply_Message too long              */
    AM_DUPL_LINK_ERR        = 16,   /* duplicated conversation error       */
    AM_MY_DEV_UNKNOWN_ERR   = 17,   /* my device unknown or not valid      */
    AM_NO_READY_INST_ERR    = 18,   /* no ready Replier instance           */
    AM_NR_OF_INST_OVF       = 19,   /* too many Replier instances          */
    AM_CALL_LEN_OVF         = 20,   /* Call_Message too long               */
    AM_UNKNOWN_DEST_ERR     = 21,   /* partner device unknown              */
    AM_INAUG_ERR            = 22,   /* train inauguration occurred         */
    AM_TRY_LATER_ERR        = 23,   /* (internally used only)              */
    AM_FIN_NOT_REG_ERR      = 24,   /* final not registered                */
    AM_GW_FIN_NOT_REG_ERR   = 25,   /* final not registered in router      */
    AM_GW_ORI_NOT_REG_ERR   = 26,   /* origin not registered in router     */
    AM_MAX_ERR              = 31    /* highest system code;                */
                                    /* user codes are higher than 31       */
}   AM_RESULT;


/* --------------------------------------------------------------------------
 *  Enumerated Type   : AM_ADDRESS
 *
 *  Purpose           : Identifies an application address (caller or
 *                      replier).
 *
 *  Syntax            : typedef struct
 *                      {
 *                          UNSIGNED8   sg_node;
 *                          UNSIGNED8   func_or_stat;
 *                          UNSIGNED8   next_station;
 *                          UNSIGNED8   topo_counter;
 *                      }   AM_ADDRESS;
 *
 *  Element           : sg_node      - Bit 0 = 0 indicates a user address,
 *                                     bit 0 = 1 indicates a system address
 *                                     (see constant AM_SYSTEM_ADDR).
 *                                     NOTE: Bit 0 is also known as 'snu'
 *                                           (system, not user).
 *                                     Bit 1 = 0 indicates an individual
 *                                     node address (always 0).
 *                                     NOTE: Bit 1 is also known as 'gni'
 *                                           (group, not individual).
 *                                     Bits 2..7 specify always a node
 *                                     address
 *                                     (see also constant AM_SAME_NODE).
 *                      func_or_stat - If bit 0 of 'sg_node' is 0, this is
 *                                     a function identifier.
 *                                     If bit 0 of 'sg_node' is 1, this is
 *                                     a station identifier.
 *                                     See also function directory and
 *                                     constants AM_ROUTER_FCT,
 *                                     AM_AGENT_FCT, AM_MANAGER_FCT.
 *                      next_station - station identifier of next station
 *                                     (see also constants AM_SAME_STATION,
 *                                     AM_UNKNOWN)
 *                      topo_counter - bits 0..1 are always 0; bits 2..5
 *                                     specify the topology counter
 *                                     (see also constant AM_ANY_TOPO and
 *                                     AM_MAX_TOPO)
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_AM_ADDRESS 4
typedef struct
{
    UNSIGNED8   sg_node;
    UNSIGNED8   func_or_stat;
    UNSIGNED8   next_station;
    UNSIGNED8   topo_counter;
}   AM_ADDRESS;


/* ==========================================================================
 *
 *  Initialisation
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : am_init
 *
 *  Purpose   : Initialises the message data related layers
 *              (incl. sub-layers => the messenger).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_init (void);
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - This procedure has to be called at system initialisation
 *                before calling any other 'am_xxx' procedure.
 *              - This procedure shall be called only one time.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_init (void);


/* --------------------------------------------------------------------------
 *  Procedure : am_announce_device
 *
 *  Purpose   : Configures a device for message data communication.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_announce_device
 *              (
 *                  UNSIGNED16  max_call_number,
 *                  UNSIGNED16  max_inst_number,
 *                  UNSIGNED16  default_reply_timeout,
 *                  UNSIGNED8   my_credit
 *              );
 *
 *  Input     : max_call_number       - number of simultaneous calls on this
 *                                      device
 *              max_inst_number       - number of simultaneous instances for
 *                                      any replier on this device
 *                                      (default is three)
 *              default_reply_timeout - default reply time-out for call
 *                                      requests
 *              my_credit             - maximal (accepted) credit for all
 *                                      connections ending on this device
 *                                      and is cut to AM_MAX_CREDIT
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - This procedure has to be called at system initialisation.
 *              - This procedure shall be called only one time.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_announce_device
(
    UNSIGNED16  max_call_number,
    UNSIGNED16  max_inst_number,
    UNSIGNED16  default_reply_timeout,
    UNSIGNED8   my_credit
);


/* --------------------------------------------------------------------------
 *  Procedure : am_show_busses
 *
 *  Purpose   : Retrieves the number of link layers supporting message data
 *              connected to this station and lists their bus identifiers.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_show_busses
 *              (
 *                  UNSIGNED8   *nr_of_busses,
 *                  UNSIGNED8   *link_id_list
 *              );
 *
 *  Output    : nr_of_busses - number of connected link layers supporting
 *                             message data (NOTE: the max. number of
 *                             connected link layers is 16)
 *              link_id_list - list of connected link layers supporting
 *                             message data, with at least the 'bus_id' of
 *                             each (NOTE: range of bus_id=0..15)
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - The application has to provide a buffer for the link
 *                identifier list (parameter 'link_id_list'). The size of
 *                this buffer should be 16, which is the max. number of
 *                connected link layers.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_show_busses
(
    UNSIGNED8   *nr_of_busses,
    UNSIGNED8   *link_id_list
);


/* --------------------------------------------------------------------------
 *  Procedure : am_set_current_tc
 *
 *  Purpose   : Indicates to the messenger the current topology counter.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_set_current_tc
 *              (
 *                  UNSIGNED8   this_topo
 *              );
 *
 *  Input     : this_topo - topology counter or AM_ANY_TOPO if the topology
 *                          counter is unknown
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - The parameter 'this_topo' accept the range
 *                AM_ANY_TOPO <= this_topo <= AM_MAX_TOPO.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_set_current_tc
(
    UNSIGNED8   this_topo
);


/* --------------------------------------------------------------------------
 *  Structured Type   : AM_MESSENGER_STATUS
 *
 *  Purpose           : Defines the messenger status information.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          STRING32    messenger_name;
 *                          UNSIGNED8   send_time_out;
 *                          UNSIGNED8   alive_time_out;
 *                          UNSIGNED8   ack_time_out;
 *                          UNSIGNED8   credit;
 *                          WORD8       reserved;
 *                          UNSIGNED8   packet_size;
 *                          UNSIGNED8   instances;
 *                          UNSIGNED8   multicast_window;
 *                          UNSIGNED32  messages_sent;
 *                          UNSIGNED32  messages_received;
 *                          UNSIGNED32  packets_sent;
 *                          UNSIGNED32  packet_retries;
 *                          UNSIGNED32  multicast_retries;
 *                      }   AM_MESSENGER_STATUS;
 *
 *  Element           : messenger_name    - version of the messenger
 *                                          software, preferably in format:
 *                                          xxxx-Vz.z-dd.mm.yy
 *                      send_time_out     - time-out after which producer
 *                                          retires, in multiplies of 64ms
 *                      alive_time_out    - time-out after which the consumer
 *                                          disconnects, in seconds
 *                      ack_time_out      - time-out after which replier
 *                                          acknowledges all received data
 *                                          packets, in multi-ples of 64 ms
 *                      credit            - number of data packets the
 *                                          producer may send before it
 *                                          receives an acknowledgement for
 *                                          any of them
 *                      reserved          - reserved (=0)
 *                      packet_size       - size of the packet in octets
 *                      instances         - number of supported instances
 *                                          for each replier
 *                      multicast_window  - window size for the multicast
 *                                          mechanism (if 0, no multicast
 *                                          is supported)
 *                      messages_sent     - counter (with wrap-around)
 *                                          counting the number of messages
 *                                          sent by this Station
 *                      messages_received - counter (with wrap-around)
 *                                          counting the number of messages
 *                                          received on this Station
 *                      packets_sent      - counter (with wrap-around)
 *                                          counting the number of packets
 *                                          sent on this Station
 *                      packet_retries    - counter (with wrap-around)
 *                                          counting the number of retried
 *                                          packets in the single-cast
 *                                          protocol
 *                      multicast_retries - counter (with wrap-around)$
 *                                          counting the number of retried
 *                                          packets in the multicast protocol
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_AM_MESSENGER_STATUS 60
typedef struct
{
    STRING32    messenger_name;
    UNSIGNED8   send_time_out;
    UNSIGNED8   alive_time_out;
    UNSIGNED8   ack_time_out;
    UNSIGNED8   credit;
    WORD8       reserved;
    UNSIGNED8   packet_size;
    UNSIGNED8   instances;
    UNSIGNED8   multicast_window;
    UNSIGNED32  messages_sent;
    UNSIGNED32  messages_received;
    UNSIGNED32  packets_sent;
    UNSIGNED32  packet_retries;
    UNSIGNED32  multicast_retries;
}   AM_MESSENGER_STATUS;


/* --------------------------------------------------------------------------
 *  Procedure : am_read_messenger_status
 *
 *  Purpose   : Retrieves the status of the messenger and its statistics
 *              counters.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_read_messenger_status
 *              (
 *                  AM_MESSENGER_STATUS *messenger_status
 *              );
 *
 *  Output    : messenger_status - messenger status information
 *                                 (the application has to provide a memory
 *                                 buffer where the messenger status is
 *                                 copied to)
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - This procedure is intended for the network management
 *                agent.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_read_messenger_status
(
    AM_MESSENGER_STATUS *messenger_status
);


/* --------------------------------------------------------------------------
 *  Structured Type   : AM_MESSENGER_CONTROL
 *
 *  Purpose           : Defines the messenger control information.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          STRING32    messenger_name;
 *                          UNSIGNED8   send_time_out;
 *                          UNSIGNED8   alive_time_out;
 *                          UNSIGNED8   ack_time_out;
 *                          UNSIGNED8   credit;
 *                          WORD8       reserved;
 *                          UNSIGNED8   packet_size;
 *                          BITSET8     clear_counter;
 *                          UNSIGNED8   multicast_window;
 *                      }   AM_MESSENGER_CONTROL;
 *
 *  Element           : messenger_name   - version of the messenger
 *                                         software, preferably in format:
 *                                         xxxx-Vz.z-dd.mm.yy
 *                      send_time_out    - time-out after which producer
 *                                         retires, in multiplies of 64ms
 *                      alive_time_out   - time-out after which the consumer
 *                                         disconnects, in seconds
 *                      ack_time_out     - time-out after which replier
 *                                         acknowledges all received data
 *                                         packets, in multiples of 64 ms
 *                      credit           - number of data packets the
 *                                         producer may send before it
 *                                         receives an acknowledgement for
 *                                         any of them
 *                      reserved         - reserved (=0)
 *                      packet_size      - size of the packet in octets
 *                      clear_counter    - clear counters specified by bit
 *                                         constants 'AM_MSG_CLR_CNT_BIT_xxx'
 *                      multicast_window - window size for the multicast
 *                                         mechanism (if 0, no multicast is
 *                                         supported)
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_AM_MESSENGER_CONTROL 40
typedef struct
{
    STRING32    messenger_name;
    UNSIGNED8   send_time_out;
    UNSIGNED8   alive_time_out;
    UNSIGNED8   ack_time_out;
    UNSIGNED8   credit;
    WORD8       reserved;
    UNSIGNED8   packet_size;
    BITSET8     clear_counter;
    UNSIGNED8   multicast_window;
}   AM_MESSENGER_CONTROL;


/* --------------------------------------------------------------------------
 *  Bit Constants : AM_MSG_CLR_CNT_BIT_xxx
 *
 *  Purpose       : Used for element 'clear_counter' of structured
 *                  type 'AM_MESSENGER_CONTROL'.
 * --------------------------------------------------------------------------
 */
#define AM_MSG_CLR_CNT_BIT_MCR  0x10    /* multicast retries counter    */
#define AM_MSG_CLR_CNT_BIT_PKR  0x08    /* packet    retries counter    */
#define AM_MSG_CLR_CNT_BIT_PKS  0x04    /* sent      packets counter    */
#define AM_MSG_CLR_CNT_BIT_MGR  0x02    /* received  messages counter   */
#define AM_MSG_CLR_CNT_BIT_MGS  0x01    /* send      messages counter   */


/* --------------------------------------------------------------------------
 *  Procedure : am_write_messenger_control
 *
 *  Purpose   : Sets parameters in the messenger.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_write_messenger_control
 *              (
 *                  AM_MESSENGER_CONTROL    messenger_control
 *              );
 *
 *  Input     : messenger_control - messenger control information
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - This procedure is intended for the network management
 *                agent.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_write_messenger_control
(
    AM_MESSENGER_CONTROL    messenger_control
);


/* ==========================================================================
 *
 *  Station Directory
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : AM_STADI_ENTRY
 *
 *  Purpose           : Defines a station directory entry.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          UNSIGNED8   station;
 *                          UNSIGNED8   next_station;
 *                          ENUM8       bus_id;
 *                          WORD32      device_adr;
 *                      }   AM_STADI_ENTRY;
 *
 *  Element           : station      - station identifier (key to retrieve
 *                                     next_station)
 *                      next_station - station identifier of next station
 *                                     (for a directly reachable station,
 *                                     next_station is equal to the station
 *                                     identifier)
 *                      bus_id       - bus identifier
 *                      device_adr   - device address (bus dependent)
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_AM_STADI_ENTRY 8
typedef struct
{
    UNSIGNED8   station;
    UNSIGNED8   next_station;
    ENUM8       bus_id;
    WORD32      device_adr;
}   AM_STADI_ENTRY;


/* --------------------------------------------------------------------------
 *  Procedure : am_stadi_write
 *
 *  Purpose   : Writes the station directory.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_stadi_write
 *              (
 *                  const AM_STADI_ENTRY    entries[],
 *                  UNSIGNED8               nr_of_entries
 *              );
 *
 *  Input     : entires       - list of new station directory entries
 *              nr_of_entries - number of elements in 'entries'
 *
 *  Return    : result code; any AM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_stadi_write
(
    const AM_STADI_ENTRY    entries[],
    UNSIGNED8               nr_of_entries
);


/* --------------------------------------------------------------------------
 *  Procedure : am_stadi_read
 *
 *  Purpose   : Writes the station directory.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_stadi_read
 *              (
 *                  AM_STADI_ENTRY  entries[],
 *                  UNSIGNED8       nr_of_entries
 *              );
 *
 *  Input     : entries[].station      - entries to be read
 *              nr_of_entries          - number of entries
 *
 *  Output    : entries[].next_station - the fields entries[].next_station
 *                                       is the output information.
 *
 *  Return    : result code; any AM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_stadi_read
(
    AM_STADI_ENTRY  entries[],
    UNSIGNED8       nr_of_entries
);


/* ==========================================================================
 *
 *  Function Directory
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : AM_DIR_ENTRY
 *
 *  Purpose           : Defines a function directory entry.
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_AM_DIR_ENTRY 2
typedef struct
{
    UNSIGNED8   function;
    UNSIGNED8   station;
}   AM_DIR_ENTRY;


/* --------------------------------------------------------------------------
 *  Procedure : am_clear_dir
 *
 *  Purpose   : Initialises the function directory.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_clear_dir (void);
 *
 *  Return    : result code; any AM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_clear_dir (void);


/* --------------------------------------------------------------------------
 *  Procedure : am_insert_dir_entries
 *
 *  Purpose   : Records the station identifier of a list of functions.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_insert_dir_entries
 *              (
 *                  AM_DIR_ENTRY    *function_list,
 *                  UNSIGNED8       number_of_entries
 *              );
 *
 *  Input     : function_list     - function directory list
 *              number_of_entries - number of elements in this list
 *
 *  Return    : result code; any AM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_insert_dir_entries
(
    AM_DIR_ENTRY    *function_list,
    UNSIGNED8       number_of_entries
);


/* --------------------------------------------------------------------------
 *  Procedure : am_remove_dir_entries
 *
 *  Purpose   : Removes a list of functions.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_remove_dir_entries
 *              (
 *                  AM_DIR_ENTRY    *function_list,
 *                  UNSIGNED8       number_of_entries
 *              );
 *
 *  Input     : function_list     - function directory list
 *              number_of_entries - number of elements in this list
 *
 *  Return    : result code; any AM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_remove_dir_entries
(
    AM_DIR_ENTRY    *function_list,
    UNSIGNED8       number_of_entries
);


/* --------------------------------------------------------------------------
 *  Procedure : am_get_dir_entry
 *
 *  Purpose   : Retrieves the station identifier of a given function.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_get_dir_entry
 *              (
 *                  UNSIGNED8   function,
 *                  UNSIGNED8   *station
 *              );
 *
 *  Input     : function - function identifier (key)
 *
 *  Output    : station  - identifier of the station where the function is
 *                         executed or AM_UNKNOWN if the function has no
 *                         assigned station
 *
 *  Return    : result code; any AM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_get_dir_entry
(
    UNSIGNED8   function,
    UNSIGNED8   *station
);


/* ==========================================================================
 *
 *  Caller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure Type: AM_CALL_CONFIRM
 *
 *  Purpose       : Type of the procedure called when the reply arrives.
 *
 *  Syntax        : typedef void
 *                  (*AM_CALL_CONFIRM)
 *                  (
 *                      UNSIGNED8           caller_function,
 *                      void                *am_caller_ref,
 *                      const AM_ADDRESS    *replier,
 *                      void                *in_msg_adr,
 *                      UNSIGNED32          in_msg_size,
 *                      AM_RESULT           status
 *                  );
 *
 *  Input         : caller_function - function identifier of the caller
 *                  am_caller_ref   - returned value which was specified in
 *                                    the related procedure 'am_call_request'
 *                  replier         - application address of the replier
 *                                    function or station
 *                  in_msg_adr      - pointer to a buffer which contains the
 *                                    received reply message (it is NULL if
 *                                    the caller did not supply a buffer for
 *                                    the reply message and if at the same
 *                                    time 'in_msg_size' is 0)
 *                  in_msg_size     - total length in octets of the reply
 *                                    message (it is 0 if an error occurred
 *                                    or if the replier application only
 *                                    replies with a status)
 *                  status          - gives an error code < AM_MAX_ERR or on
 *                                    success the status supplied by the
 *                                    replier
 *
 *  Remarks       : - The procedure 'call_confirm' has to be previously
 *                    subscribed by procedure 'am_call_request'.
 *                  - AM_MANAGER_FCT is returned if the replier address is
 *                    a system address.
 *                  - Call confirmation implicitly returns the call message
 *                    buffer to the caller.
 *                  - A reply message buffer which was allocated by the
 *                    messenger has to be returned with procedure
 *                    'am_buffer_free' after use.
 * --------------------------------------------------------------------------
 */
typedef void
(*AM_CALL_CONFIRM)
(
    UNSIGNED8           caller_function,
    void                *am_caller_ref,
    const AM_ADDRESS    *replier,
    void                *in_msg_adr,
    UNSIGNED32          in_msg_size,
    AM_RESULT           status
);


/* --------------------------------------------------------------------------
 *  Procedure : am_call_request
 *
 *  Purpose   : Caller sends a whole message.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              void
 *              am_call_request
 *              (
 *                  UNSIGNED8           caller_function,
 *                  const AM_ADDRESS    *replier,
 *                  void                *out_msg_adr,
 *                  UNSIGNED32          out_msg_size,
 *                  void                *in_msg_adr,
 *                  UNSIGNED32          in_msg_size,
 *                  UNSIGNED16          reply_timeout,
 *                  AM_CALL_CONFIRM     call_confirm,
 *                  void                *caller_ref
 *              );
 *
 *  Input     : caller_function - function identifier of the caller
 *              replier         - application address of the replier
 *                                function or station
 *              out_msg_adr     - pointer to the call message to transmit
 *              out_msg_size    - total length in octets of the call message
 *              in_msg_adr      - pointer to the buffer to put the reply
 *                                message
 *              in_msg_size     - maximal total length in octets of the
 *                                accepted reply message
 *              reply_timeout   - time-out value in multiples of 64 ms for
 *                                the reply after the transfer of the call
 *                                message
 *              call_confirm    - pointer to the call confirmation procedure
 *                                ('call_confirm' will be called unless
 *                                'am_call_request' is cancelled successfully
 *                                by procedure 'am_call_cancel')
 *              caller_ref      - external reference for the call to be
 *                                returned by 'call_confirm' (it can be used
 *                                in any way by the caller)
 *
 *  Remarks   : - Before calling procedure 'am_call_request', the procedures
 *                'am_init' and 'am_announce_device' have to be called.
 *              - If 'in_msg_adr' is NULL, the messenger allocates a buffer
 *                for the reply message. The caller is then responsible to
 *                return this buffer after use by calling procedure
 *                'am_buffer_free'.
 *              - The call is rejected if a conversation with the same
 *                caller and replier address (and in the same direction) is
 *                already established.
 *              - A function directory entry for the replier function has to
 *                be defined if the station identifier of the replier is
 *                AM_UNKNOWN.
 *              - No bus communication takes place if caller and replier are
 *                within the same station.
 *              - A call with a system address is rejected if the function
 *                identifier is different from 254 (manager).
 *              - The caller may not modify the message buffers until
 *                'call_confirm' is called.
 *              - The default reply time-out (specified with procedure
 *                'am_announce_device') is awaited if reply time-out is 0.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
void
am_call_request
(
    UNSIGNED8           caller_function,
    const AM_ADDRESS    *replier,
    void                *out_msg_adr,
    UNSIGNED32          out_msg_size,
    void                *in_msg_adr,
    UNSIGNED32          in_msg_size,
    UNSIGNED16          reply_timeout,
    AM_CALL_CONFIRM     call_confirm,
    void                *caller_ref
);


/* --------------------------------------------------------------------------
 *  Procedure : am_call_cancel
 *
 *  Purpose   : Cancel the conversation and discard reply message.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_call_cancel
 *              (
 *                  UNSIGNED8           caller_function,
 *                  const AM_ADDRESS    *replier
 *              );
 *
 *  Input     : caller_function - function identifier of the caller
 *              replier         - application address of the replier
 *                                function or station
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - The call confirmation procedure will not be called if the
 *                return value is AM_OK.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_call_cancel
(
    UNSIGNED8           caller_function,
    const AM_ADDRESS    *replier
);


/* ==========================================================================
 *
 *  Replier Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure Type: AM_RECEIVE_CONFIRM
 *
 *  Purpose       : Type of the procedure called when call is complete.
 *
 *  Syntax        : typedef void
 *                  (*AM_RECEIVE_CONFIRM)
 *                  (
 *                      UNSIGNED8           replier_function,
 *                      const AM_ADDRESS    *caller,
 *                      void *              in_msg_adr,
 *                      UNSIGNED32          in_msg_size,
 *                      void *              replier_ref
 *                  );
 *
 *  Input         : replier_function - function identifier of the replier as
 *                                     specified in the corresponding
 *                                     'am_receive_request'
 *                  caller           - application address of the caller
 *                  in_msg_adr       - pointer to a buffer which contains
 *                                     the call message
 *                  in_msg_size      - total length in octets of the
 *                                     received call message
 *                  replier_ref      - external reference as specified in
 *                                     the corresponding 'am_receive_request'
 *
 *  Remarks       : - The receive confirmation procedure 'receive_confirm'
 *                    has been previously subscribed by procedure
 *                    'am_bind_replier'.
 *                  - If the replier instance did not supply a buffer in
 *                    'am_receive_request', the reply buffer is supplied by
 *                    the messenger and the replier has to return it after
 *                    use with procedure 'am_buffer_free'.
 * --------------------------------------------------------------------------
 */
typedef void
(*AM_RECEIVE_CONFIRM)
(
    UNSIGNED8           replier_function,
    const AM_ADDRESS    *caller,
    void *              in_msg_adr,
    UNSIGNED32          in_msg_size,
    void *              replier_ref
);


/* --------------------------------------------------------------------------
 *  Procedure Type: AM_REPLY_CONFIRM
 *
 *  Purpose       : Type of the procedure called when reply sent.
 *
 *  Syntax        : typedef void
 *                  (*AM_REPLY_CONFIRM)
 *                  (
 *                      UNSIGNED8   replier_function,
 *                      void        *replier_ref
 *                  );
 *
 *  Input         : replier_function - function identifier of the replier as
 *                                     specified in the corresponding
 *                                     'am_receive_request'
 *                  replier_ref      - external reference as specified in
 *                                     the corresponding 'am_receive_request'
 *
 *  Remarks       : - 'reply_confirm' has been previously subscribed by
 *                    procedure 'am_bind_replier'.
 *                  - This procedure returns the reply message buffer back
 *                    to the Replier instance.
 *                  - Reply confirmation allows a further call of procedure
 *                    'am_receive_request' for the same replier instance.
 * --------------------------------------------------------------------------
 */
typedef void
(*AM_REPLY_CONFIRM)
(
    UNSIGNED8   replier_function,
    void        *replier_ref
);


/* --------------------------------------------------------------------------
 *  Procedure : am_bind_replier
 *
 *  Purpose   : Announce a replier instance to the session layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_bind_replier
 *              (
 *                  UNSIGNED8           replier_function,
 *                  AM_RECEIVE_CONFIRM  receive_confirm,
 *                  AM_REPLY_CONFIRM    reply_confirm
 *              );
 *
 *  Input     : replier_function - function identifier of the replier to
 *                                 be bound
 *              receive_confirm  - receive confirmation procedure which
 *                                 will be called on completion of a receive
 *                                 request
 *              reply_confirm    - reply confirmation procedure which will
 *                                 be called on completion of a reply
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - Procedures 'am_init' and 'am_announce_device' have to be
 *                called before calling procedure 'am_bind_replier'.
 *              - Each replier has to be bound before it can issue any
 *                'am_receive_request'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_bind_replier
(
    UNSIGNED8           replier_function,
    AM_RECEIVE_CONFIRM  receive_confirm,
    AM_REPLY_CONFIRM    reply_confirm
);


/* --------------------------------------------------------------------------
 *  Procedure : am_unbind_replier
 *
 *  Purpose   : Cancel the above announcement.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_unbind_replier
 *              (
 *                  UNSIGNED8           replier_function
 *              );
 *
 *  Input     : replier_function - function identifier of the replier to
 *                                 be unbound
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - Calls which have been received before calling procedure
 *                'am_unbind_replier', but have not yet been replied to,
 *                are cancelled.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_unbind_replier
(
    UNSIGNED8   replier_function
);


/* --------------------------------------------------------------------------
 *  Procedure : am_receive_request
 *
 *  Purpose   : Announce instance is ready for the next call.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_receive_request
 *              (
 *                  UNSIGNED8   replier_function,
 *                  void        *in_msg_adr,
 *                  UNSIGNED32  in_msg_size,
 *                  void        *replier_ref
 *              );
 *
 *  Input     : replier_function - function identifier of the replier
 *                                 expecting a call (function identifier 253
 *                                 implies a system address)
 *              in_msg_adr       - pointer to a buffer for the incoming call
 *                                 message (The messenger allocates a buffer
 *                                 for the call message if 'in_msg_adr' is
 *                                 NULL. This buffer may not be modified
 *                                 until 'am_receive_request' is confirmed
 *                                 or cancelled.)
 *              in_msg_size      - maximum total size in octets of the call
 *                                 message which can be accepted
 *              replier_ref      - external reference returned with the
 *                                 related receive confirm procedure (it is
 *                                 at the same time an instance reference
 *                                 and distinguishes the replier instances
 *                                 which serve the same replier)
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - This procedure requires that the procedure
 *                'am_bind_replier' for the same replier has been called
 *                previously.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_receive_request
(
    UNSIGNED8   replier_function,
    void        *in_msg_adr,
    UNSIGNED32  in_msg_size,
    void        *replier_ref
);


/* --------------------------------------------------------------------------
 *  Procedure : am_reply_request
 *
 *  Purpose   : Called by the Replier instance to send back a reply message.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_reply_request
 *              (
 *                  UNSIGNED8   replier_function,
 *                  void        *out_msg_adr,
 *                  UNSIGNED32  out_msg_size,
 *                  void        *replier_ref,
 *                  AM_RESULT   status
 *              );
 *
 *  Input     : replier_function - function identifier of the replier as
 *                                 specified in the corresponding
 *                                 'am_receive_request'
 *              out_msg_adr      - pointer to the reply message buffer
 *                                 (This buffer shall not be modified until
 *                                 the reply request is confirmed. If
 *                                 'out_msg_adr' is NULL, only the status is
 *                                 transmitted to the caller.)
 *              out_msg_size     - total length in octets of the reply
 *                                 message
 *              status           - call execution result supplied by the
 *                                 replier, transmitted to the caller in
 *                                 addition to the reply message itself
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - Each received call has to be replied with prcoedure
 *                'am_reply_request' or cancelled with procedure
 *                'am_receive_cancel'.
 *              - This procedure returns before the reply message is
 *                transmitted.
 *              - The messenger transmits this reply message together with
 *                the specified status back to the caller.
 *              - The caller address is retrieved internally from the
 *                replier instance.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_reply_request
(
    UNSIGNED8   replier_function,
    void        *out_msg_adr,
    UNSIGNED32  out_msg_size,
    void        *replier_ref,
    AM_RESULT   status
);


/* --------------------------------------------------------------------------
 *  Procedure : am_receive_cancel
 *
 *  Purpose   : Cancels a ready or engaged replier instance.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_receive_cancel
 *              (
 *                  UNSIGNED8   replier_function,
 *                  void        *replier_ref
 *              );
 *
 *  Input     : replier_function - function identifier of the replier as
 *                                 specified in the corresponding
 *                                 'am_receive_request'
 *              replier_ref      - external reference as specified in the
 *                                 corresponding 'am_receive_request'
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - The confirmation procedure for a successfully cancelled
 *                'reply request' will not be called any more.
 *              - It is up to the user to distinguish if the receive request
 *                has completed (i.e. a call message has still been received)
 *                or not and to deallocated dynamic buffers for the call
 *                messages.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_receive_cancel
(
    UNSIGNED8   replier_function,
    void        *replier_ref
);


/* ==========================================================================
 *
 *  Buffer Handling
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : am_buffer_free
 *
 *  Purpose   : Recycles a dynamic message buffer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              am_buffer_free
 *              (
 *                  void        *in_msg_adr,
 *                  UNSIGNED32  size
 *              );
 *
 *  Input     : in_msg_adr - pointer to the released buffer
 *              size       - total length in octets of this buffer
 *
 *  Return    : result code; any AM_RESULT
 *
 *  Remarks   : - Buffer allocation is independent from the packet pool
 *                management.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AM_RESULT
am_buffer_free
(
    void        *in_msg_adr,
    UNSIGNED32  size
);


#endif /* #ifndef TCN_AM_H */
