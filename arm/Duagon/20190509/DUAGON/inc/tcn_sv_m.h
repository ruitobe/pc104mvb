#ifndef TCN_SV_M_H
#define TCN_SV_M_H

/* ==========================================================================
 *
 *  File      : TCN_SV_M.H
 *
 *  Purpose   : Common Interface for Supervision - Link Layer Services
 *                                                 for MVB
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
#define SV_MVB_DEVICE_NUMBER_MAX 4096


/* --------------------------------------------------------------------------
 *  Enumerated Type   : SV_MVB_SERVICE
 *
 *  Purpose           : MVB supervision services.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    /* ----------------------------------------------------------------------
     *  TCN (IEC 61375-1)
     * ----------------------------------------------------------------------
     */
    SV_MVB_SERVICE_READ_STATUS                = 10,
    SV_MVB_SERVICE_WRITE_CONTROL              = 11,
    SV_MVB_SERVICE_READ_DEVICES               = 12,
    SV_MVB_SERVICE_WRITE_ADMINISTRATOR        = 13,

    /* ----------------------------------------------------------------------
     *  Error Counters
     * ----------------------------------------------------------------------
     */
    SV_MVB_SERVICE_ERRCNT_GET_GLOBAL          = 192,
    SV_MVB_SERVICE_ERRCNT_GET_DS_PORT         = 193,
    SV_MVB_SERVICE_ERRCNT_GET_PD_PORT         = 194,

    /* ----------------------------------------------------------------------
     *  MVB UART Emulation
     * ----------------------------------------------------------------------
     */
    SV_MVB_SERVICE_MUE_SV_PUT_BA_CONFIG       = 224,
    SV_MVB_SERVICE_MUE_SV_GET_DEVICE_STATUS   = 225,
    SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA       = 226
}   SV_MVB_SERVICE;


/* ==========================================================================
 *
 *  MVB Device Status
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_DEVICE_STATUS
 *
 *  Purpose           : MVB device status.
 *
 *  Syntax            : typedef struct (big-endian representation)
 *                      {
 *                          * capabilities *
 *                          BITFIELD16  sp  : 1;
 *                          BITFIELD16  ba  : 1;
 *                          BITFIELD16  gw  : 1;
 *                          BITFIELD16  md  : 1;
 *                          * class specific *
 *                          BITFIELD16  cs0 : 1;
 *                          BITFIELD16  cs1 : 1;
 *                          BITFIELD16  cs2 : 1;
 *                          BITFIELD16  cs3 : 1;
 *                          * common flags *
 *                          BITFIELD16  lat : 1;
 *                          BITFIELD16  rld : 1;
 *                          BITFIELD16  ssd : 1;
 *                          BITFIELD16  sdd : 1;
 *                          BITFIELD16  erd : 1;
 *                          BITFIELD16  frc : 1;
 *                          BITFIELD16  dnr : 1;
 *                          BITFIELD16  ser : 1;
 *                      }   SV_MVB_DEVICE_STATUS;
 *
 *  Element           : sp  - special device (class1=0)
 *                      ba  - bus administrator capability
 *                      gw  - gateway capability
 *                      md  - message data capability
 *                      cs0 - first bit of class specific field
 *                            (depends on the capabilities)
 *                      cs1 - second bit of class specific field
 *                            (depends on the capabilities)
 *                      cs2 - third bit of class specific field
 *                            (depends on the capabilities)
 *                      cs3 - fourth bit of class specific field
 *                            (depends on the capabilities)
 *                      lat - line A trusted
 *                      rld - redundant line disturbed
 *                      ssd - some system disturbance
 *                      sdd - some device disturbance
 *                      erd - extended reply delay
 *                      frc - forced device
 *                      dnr - device not ready
 *                      ser - system reserved
 *
 *  Remarks           : - Instead of this type a BITSET16 can be used
 *                        together with the bit constants
 *                        'SV_MVB_DEVICE_STATUS_BIT_xxx'.
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_DEVICE_STATUS 2
#ifdef TCN_LE
    typedef struct /* little-endian representation */
    {
        /* common flags */
        BITFIELD16  ser : 1;
        BITFIELD16  dnr : 1;
        BITFIELD16  frc : 1;
        BITFIELD16  erd : 1;
        BITFIELD16  sdd : 1;
        BITFIELD16  ssd : 1;
        BITFIELD16  rld : 1;
        BITFIELD16  lat : 1;
        /* class specific */
        BITFIELD16  cs3 : 1;
        BITFIELD16  cs2 : 1;
        BITFIELD16  cs1 : 1;
        BITFIELD16  cs0 : 1;
        /* capabilities */
        BITFIELD16  md  : 1;
        BITFIELD16  gw  : 1;
        BITFIELD16  ba  : 1;
        BITFIELD16  sp  : 1;
    }   SV_MVB_DEVICE_STATUS;
#else /* #ifdef TCN_LE */
    typedef struct /* big-endian representation */
    {
        /* capabilities */
        BITFIELD16  sp  : 1;
        BITFIELD16  ba  : 1;
        BITFIELD16  gw  : 1;
        BITFIELD16  md  : 1;
        /* class specific */
        BITFIELD16  cs0 : 1;
        BITFIELD16  cs1 : 1;
        BITFIELD16  cs2 : 1;
        BITFIELD16  cs3 : 1;
        /* common flags */
        BITFIELD16  lat : 1;
        BITFIELD16  rld : 1;
        BITFIELD16  ssd : 1;
        BITFIELD16  sdd : 1;
        BITFIELD16  erd : 1;
        BITFIELD16  frc : 1;
        BITFIELD16  dnr : 1;
        BITFIELD16  ser : 1;
    }   SV_MVB_DEVICE_STATUS;
#endif /* #else */


/* --------------------------------------------------------------------------
 *  Bit Constants : SV_MVB_DEVICE_STATUS_BIT_xxx
 *
 *  Purpose       : MVB device status.
 * --------------------------------------------------------------------------
 */
/* capabilities */
#define SV_MVB_DEVICE_STATUS_BIT_SP  0x8000 /* special device (class1=0)   */
#define SV_MVB_DEVICE_STATUS_BIT_BA  0x4000 /* bus administrator           */
#define SV_MVB_DEVICE_STATUS_BIT_GW  0x2000 /* gateway                     */
#define SV_MVB_DEVICE_STATUS_BIT_MD  0x1000 /* message data                */

/* class specific (general) */
#define SV_MVB_DEVICE_STATUS_BIT_CS0 0x0800 /* first  bit ...  ...of       */
#define SV_MVB_DEVICE_STATUS_BIT_CS1 0x0400 /* second bit ...    class     */
#define SV_MVB_DEVICE_STATUS_BIT_CS2 0x0200 /* third  bit ...   specific   */
#define SV_MVB_DEVICE_STATUS_BIT_CS3 0x0100 /* fourth bit ...    field     */

/* class specific (bus administrator) */
#define SV_MVB_DEVICE_STATUS_BIT_AX1 0x0800 /* second last significant bit */
                                            /*  of the actualisation key   */
                                            /*  of the periodic list       */
#define SV_MVB_DEVICE_STATUS_BIT_AX0 0x0400 /* least significant bit of    */
                                            /*  the actualisation key of   */
                                            /*  the periodic list          */
#define SV_MVB_DEVICE_STATUS_BIT_ACT 0x0200 /* device is actualised and in */
                                            /*  condition of becoming      */
                                            /* master                      */
#define SV_MVB_DEVICE_STATUS_BIT_MAS 0x0100 /* device is the current       */
                                            /*  master                     */

/* class specific (gateway without bus administrator capability) */
#define SV_MVB_DEVICE_STATUS_BIT_STD 0x0800 /* static disturbance          */
#define SV_MVB_DEVICE_STATUS_BIT_DYD 0x0400 /* dynamic disturbance         */

/* common flags */
#define SV_MVB_DEVICE_STATUS_BIT_LAT 0x0080 /* line A trusted              */
#define SV_MVB_DEVICE_STATUS_BIT_RLD 0x0040 /* redundant line disturbed    */
#define SV_MVB_DEVICE_STATUS_BIT_SSD 0x0020 /* some system disturbance     */
#define SV_MVB_DEVICE_STATUS_BIT_SDD 0x0010 /* some device disturbance     */
#define SV_MVB_DEVICE_STATUS_BIT_ERD 0x0008 /* extended reply delay        */
#define SV_MVB_DEVICE_STATUS_BIT_FRC 0x0004 /* forced device               */
#define SV_MVB_DEVICE_STATUS_BIT_DNR 0x0002 /* device not ready            */
#define SV_MVB_DEVICE_STATUS_BIT_SER 0x0001 /* system reserved             */


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_STATUS
 *
 *  Purpose           : MVB_Status object.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          WORD16      reserved1;
 *                          WORD16      device_address;
 *                          STRING32    mvb_hardware_name;
 *                          STRING32    mvb_software_name;
 *                          BITSET16    device_status;
 *                          WORD8       reserved2;
 *                          UNSIGNED8   t_ignore;
 *                          UNSIGNED32  lineA_errors;
 *                          UNSIGNED32  lineB_errors;
 *                      }   SV_MVB_STATUS;
 *
 *  Element           : reserved1         - reserved (=0)
 *                      device_address    - address of the device on the bus
 *                                          (0..4095)
 *                      mvb_hardware_name - descriptor of the MVB controller,
 *                                          including the hardware version
 *                      mvb_software_name - descriptor of the MVB software
 *                                          version, in the format
 *                                          'TCN Driver d-xxxxxx-yyyyyy'
 *                      device_status     - copy of the 16-bit MVB
 *                                          Device_Status
 *                      reserved2         - reserved (=0)
 *                      t_ignore          - configured time-out value for
 *                                          the delay between a Master_Frame
 *                                          and a Slave_Frame on that device
 *                                          in multiples of 1.0us, default
 *                                          value (t_ignore = 0) =42.7us
 *                      lineA_errors      - 32-bit counter incremented each
 *                                          time an erroneous frame of any
 *                                          type is received over Line_A;
 *                                          this counter does not wraparound
 *                                          when reaching its highest value
 *                      lineB_errors      - 32-bit counter incremented each
 *                                          time an erroneous frame of any
 *                                          type is received over Line_B;
 *                                          this counter does not wraparound
 *                                          when reaching its highest value
 *
 *  Remarks           : - The MVB status object can be read from a connected
 *                        bus controller link layer by a supervision service
 *                        handler procedure (e.g. as_service_handler or
 *                        ls_service_handler) using
 *                        SV_MVB_SERVICE_READ_STATUS as service identifier
 *                        (parameter 'service').
 *                      - The user has to provide a memory buffer of this
 *                        type for the MVB status object (output parameter
 *                        'p_parameter').
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_STATUS 80
typedef struct
{
    WORD16      reserved1;
    WORD16      device_address;
    STRING32    mvb_hardware_name;
    STRING32    mvb_software_name;
    BITSET16    device_status;
    WORD8       reserved2;
    UNSIGNED8   t_ignore;
    UNSIGNED32  lineA_errors;
    UNSIGNED32  lineB_errors;
}   SV_MVB_STATUS;


/* ==========================================================================
 *
 *  MVB Control
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_CONTROL
 *
 *  Purpose           : MVB_Control object.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          WORD16      device_address;
 *                          WORD8       reserved1;
 *                          UNSIGNED8   t_ignore;
 *                          WORD8       reserved2;
 *                          BITSET8     command;
 *                          WORD16      reserved3;
 *                      }   SV_MVB_CONTROL;
 *
 *  Element           : device_address - device address to be set (0..4095);
 *                                       only if software modifiable
 *                      reserved1      - reserved (=0)
 *                      t_ignore       - value of the time-out T_ignore in
 *                                       multiples of 1.0us;default
 *                                       (t_ignore=0): 42.7us;
 *                                       recommended values:22.0us, 64.0us,
 *                                       86.0us,maximum value: 255.0us
 *                      reserved2      - reserved (=0)
 *                      command        - command to be performed; see bit
 *                                       constants 'SV_MVB_CTRL_CMD_BIT_xxx'
 *                      reserved3      - reserved (=0)
 *
 *  Remarks           : - The MVB control object can be written to a
 *                        connected bus controller link layer by a
 *                        supervision service handler procedure (e.g.
 *                        as_service_handler or ls_service_handler) using
 *                        SV_MVB_SERVICE_WRITE_CONTROL as service identifier.
 *                      - The user has to provide a memory buffer of this
 *                        type for the MVB control object (input parameter
 *                        'p_parameter').
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_CONTROL 8
typedef struct
{
    WORD16      device_address;
    WORD8       reserved1;
    UNSIGNED8   t_ignore;
    WORD8       reserved2;
    BITSET8     command;
    WORD16      reserved3;
}   SV_MVB_CONTROL;


/* --------------------------------------------------------------------------
 *  Bit Constants : SV_MVB_CTRL_CMD_BIT_xxx
 *
 *  Purpose       : Defines the commands for the MVB_Control object.
 *
 *  Remarks       : - Used for element 'command' of structured type
 *                    'SV_MVB_CONTROL'.
 * --------------------------------------------------------------------------
 */
#define SV_MVB_CTRL_CMD_BIT_AON 0x80 /* enables Bus_Administrator          */
#define SV_MVB_CTRL_CMD_BIT_AOF 0x40 /*  disables Bus_Administrator        */
                                     /*  (aon=aof=1: no action)            */
#define SV_MVB_CTRL_CMD_BIT_SPL 0x20 /* switch to new Periodic_List when   */
                                     /*  Bus_Administrator becomes master  */
#define SV_MVB_CTRL_CMD_BIT_TMS 0x10 /* transfer mastership to the next    */
                                     /*  Bus_Administrator                 */
#define SV_MVB_CTRL_CMD_BIT_SLA 0x08 /* trust Line_A, even if disturbed    */
                                     /*  (sla=slb=0: no action)            */
#define SV_MVB_CTRL_CMD_BIT_SLB 0x04 /* trust Line_B, even if disturbed    */
                                     /*  (sla=slb=1: normal redundancy)    */
#define SV_MVB_CTRL_CMD_BIT_CLA 0x02 /* reset frame error counter of LineA */
                                     /*  (cla=clb=0: no action)            */
#define SV_MVB_CTRL_CMD_BIT_CLB 0x01 /* reset frame error counter of LineB */
                                     /*  (cla=clb=1: reset both counters)  */


/* ==========================================================================
 *
 *  MVB Devices
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_DEVICES
 *
 *  Purpose           : MVB_Devices object.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          WORD16      device_address;
 *                          BITSET16    device_status;
 *                      }   SV_MVB_DEVICES_LIST;
 *
 *                      typedef struct
 *                      {
 *                          UNSIGNED16          nr_devices;
 *                          SV_MVB_DEVICES_LIST devices_list[1]; (dyn. size)
 *                      }   SV_MVB_DEVICES;
 *
 *  Element           : device_address - address of the device (0..4095)
 *                      device_status  - device status of the device;
 *                                       see bit constants
 *                                       'SV_MVB_DEVICE_STATUS_BIT_xxx' and
 *                                       structured type
 *                                       'SV_MVB_DEVICE_STATUS'
 *
 *                      nr_devices     - number of devices in this list
 *                                       (0..4096)
 *                      devices_list   - list of devices found during
 *                                       Devices_Scan
 *  Remarks           : - The MVB devices object can be read from a connected
 *                        bus controller link layer by a supervision service
 *                        handler procedure (e.g. as_service_handler or
 *                        ls_service_handler) using
 *                        SV_MVB_SERVICE_READ_DEVICES as service identifier.
 *                      - The user has to provide a pointer of this type for
 *                        the MVB devices object (output parameter
 *                        'p_parameter').
 *                      - The memory required for the MVB devices object
 *                        will be allocated by the bus controller link layer.
 *                        It must be released by the user.
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_DEVICES_LIST 4
typedef struct
{
    WORD16      device_address;
    BITSET16    device_status;
}   SV_MVB_DEVICES_LIST;

#define TCN_SIZEOF_STRUCT_SV_MVB_DEVICES \
    (2 + TCN_SIZEOF_STRUCT_SV_MVB_DEVICES_LIST)
typedef struct
{
    UNSIGNED16          nr_devices;
    SV_MVB_DEVICES_LIST devices_list[1]; /* dynamic size */
}   SV_MVB_DEVICES;


/* ==========================================================================
 *
 *  MVB Administrator
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_ADMINISTRATOR
 *
 *  Purpose           : MVB_Administrator object.
 *
 *  Remarks           : - See also 'MVB_Administrator' in IEC 61375-1,
 *                        Clause 3: Multifunction Vehicle Bus
 *                        and
 *                        'Write_MVB_Administrator' in IEC 61375-1,
 *                        Clause 5: Train Network Management.
 *                      - The MVB administrator object can be written to a
 *                        connected bus controller link layer by a
 *                        supervision service handler procedure (e.g.
 *                        as_service_handler or ls_service_handler) using
 *                        SV_MVB_SERVICE_WRITE_ADMINISTRATOR as service
 *                        identifier.
 *                      - The user has to provide a memory buffer of this
 *                        type for the MVB administrator object (input
 *                        parameter 'p_parameter').
 *                      - The element 'list' contains different lists
 *                        described by the other types below. There are also
 *                        macros defined for faster access of the different
 *                        lists (see below).
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR 40
typedef struct
{
    UNSIGNED16  checkword0;
    UNSIGNED16  actualisation_key;
    UNSIGNED16  t_reply_max;
    UNSIGNED16  macro_cycles;
    ENUM16      event_poll_strategy;
    UNSIGNED16  basic_period;
    UNSIGNED16  macrocycles_per_turn;
    ENUM16      devices_scan_strategy;
    WORD16      reserved2;
    WORD16      reserved3;
    WORD16      reserved4;
    WORD16      reserved5;
    UNSIGNED16  known_devices_list_offset;
    UNSIGNED16  reserved_list_offset;
    UNSIGNED16  periodic_list_offset;
    UNSIGNED16  bus_administrators_list_offset;
    UNSIGNED16  devices_scan_list_offset;
    UNSIGNED16  end_list_offset;
    WORD16      list[2]; /* dynamic size */
}   SV_MVB_ADMINISTRATOR;


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_C_KDL
 *
 *  Purpose   : Returns the number of entries of the 'known devices list'.
 *
 *  Input     : _p_base_ - pointer to base of MVB_Administrator object
 *
 *  Return    : number of entires of the 'known devices list'
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_C_KDL(_p_base_)                            \
        (UNSIGNED16)                                                        \
        (                                                                   \
            (                                                               \
                (_p_base_)->reserved_list_offset -                          \
                (_p_base_)->known_devices_list_offset                       \
            ) / 2                                                           \
        )


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_P_KDL
 *
 *  Purpose   : Returns the pointer to first entry
 *              of the 'known devices list' or NULL if list is empty.
 *
 *  Input     : _p_base_ - pointer to base of MVB_Administrator object
 *
 *  Return    : pointer to first entry of the 'known devices list'
 *              or NULL if list is empty
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_P_KDL(_p_base_)                            \
        (                                                                   \
            ((_p_base_)->known_devices_list_offset ==                       \
             (_p_base_)->reserved_list_offset)                              \
            ? (WORD16*)NULL                                                 \
            : (WORD16*)                                                     \
              (                                                             \
                  (UNSIGNED32)(_p_base_) +                                  \
                  (UNSIGNED32)(_p_base_)->known_devices_list_offset         \
              )                                                             \
        )


/* --------------------------------------------------------------------------
 *  Constants : SV_MVB_ADMINISTRATOR_xxx
 *
 *  Purpose   : Defines the max. number of 'cycle lists' and 'split lists'.
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_CL_MAX     11 /* max. number of 'cycle lists' */
#define SV_MVB_ADMINISTRATOR_SL_MAX     5  /* max. number of 'split lists' */


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_ADMINISTRATOR_CL
 *
 *  Purpose           : Defines the 'cycle list entry'
 *                      of the MVB_Administrator object.
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_CL 2
#ifdef TCN_LE
    typedef struct /* little-endian representation */
    {
        BITFIELD16  address : 12;
        BITFIELD16  f_code  : 4;
    }   SV_MVB_ADMINISTRATOR_CL;
#else /* #ifdef TCN_LE */
    typedef struct /* big-endian representation */
    {
        BITFIELD16  f_code  : 4;
        BITFIELD16  address : 12;
    }   SV_MVB_ADMINISTRATOR_CL;
#endif /* #else */


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_C_CL
 *
 *  Purpose   : Returns the number of entries of a 'cycle list'.
 *
 *  Input     : _p_pl_    - pointer to base of the 'periodic list'
 *              _i_cycle_ - index (0..10) which references a cycle list
 *
 *  Return    : number of entires of a 'cycle list'
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_C_CL(_p_pl_, _i_cycle_)                    \
        (UNSIGNED16)                                                        \
        (                                                                   \
            (                                                               \
                (_p_pl_)->cycle_list_offset[(_i_cycle_)+1] -                \
                (_p_pl_)->cycle_list_offset[(_i_cycle_)]                    \
            ) / 2                                                           \
        )


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_P_CL
 *
 *  Purpose   : Returns the pointer to first entry of a 'cycle list'
 *              or NULL if list is empty.
 *
 *  Input     : _p_pl_    - pointer to base of the 'periodic list'
 *              _i_cycle_ - index (0..10) which references a 'cycle list'
 *
 *  Return    : pointer to first entry of a 'cycle list'
 *              or NULL if list is empty
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_P_CL(_p_pl_, _i_cycle_)                    \
        (                                                                   \
            ((_p_pl_)->cycle_list_offset[(_i_cycle_)] ==                    \
             (_p_pl_)->cycle_list_offset[(_i_cycle_)+1])                    \
            ? (SV_MVB_ADMINISTRATOR_CL*)NULL                                \
            : (SV_MVB_ADMINISTRATOR_CL*)                                    \
              (                                                             \
                  (UNSIGNED32)(_p_pl_) +                                    \
                  (UNSIGNED32)(_p_pl_)->cycle_list_offset[(_i_cycle_)]      \
              )                                                             \
        )


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_ADMINISTRATOR_SL
 *
 *  Purpose           : Defines the 'split list entry'
 *                      of the MVB_Administrator object.
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_SL 2
#ifdef TCN_LE
    typedef struct /* little-endian representation */
    {
        UNSIGNED8   upper_count;
        UNSIGNED8   lower_count;
    }   SV_MVB_ADMINISTRATOR_SL;
#else /* #ifdef TCN_LE */
    typedef struct /* big-endian representation */
    {
        UNSIGNED8   lower_count;
        UNSIGNED8   upper_count;
    }   SV_MVB_ADMINISTRATOR_SL;
#endif /* #else */


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_C_SL
 *
 *  Purpose   : Returns the number of entries of a 'split list'.
 *
 *  Input     : _p_pl_    - pointer to base of the 'periodic list'
 *              _i_split_ - index (0..3) which references a 'split list'
 *
 *  Return    : number of entries of a 'split list'
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_C_SL(_p_pl_, _i_split_)                    \
        (UNSIGNED16)                                                        \
        (                                                                   \
            (                                                               \
                (_p_pl_)->split_list_offset[(_i_split_)+1] -                \
                (_p_pl_)->split_list_offset[(_i_split_)]                    \
            ) / 2                                                           \
        )


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_P_SL
 *
 *  Purpose   : Returns the pointer to first entry of a 'split list'
 *              or NULL if list is empty.
 *
 *  Input     : _p_pl_    - pointer to base of the 'periodic list'
 *              _i_split_ - index (0..4) which references a 'split list'
 *
 *  Return    : pointer to first entry of a 'split list'
 *              or NULL if list is empty
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_P_SL(_p_pl_, _i_split_)                    \
        (                                                                   \
            ((_p_pl_)->split_list_offset[(_i_split_)] ==                    \
             (_p_pl_)->split_list_offset[(_i_split_)+1])                    \
            ? (SV_MVB_ADMINISTRATOR_SL*)NULL                                \
            : (SV_MVB_ADMINISTRATOR_SL*)                                    \
              (                                                             \
                  (UNSIGNED32)(_p_pl_) +                                    \
                  (UNSIGNED32)(_p_pl_)->split_list_offset[(_i_split_)]      \
              )                                                             \
        )


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_ADMINISTRATOR_PL
 *
 *  Purpose           : Defines the 'periodic list'
 *                      of the MVB_Administrator object.
 * --------------------------------------------------------------------------
 */
/* periodic list */
#define TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_PL \
    ((SV_MVB_ADMINISTRATOR_CL_MAX + SV_MVB_ADMINISTRATOR_SL_MAX) * 2)
typedef struct
{
    UNSIGNED16  cycle_list_offset[SV_MVB_ADMINISTRATOR_CL_MAX];
    UNSIGNED16  split_list_offset[SV_MVB_ADMINISTRATOR_SL_MAX];
}   SV_MVB_ADMINISTRATOR_PL;

/* alternative type if cycle list offsets */
#define TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_PL_CLO 24
typedef struct
{
    UNSIGNED16  cycle_1_offset;
    UNSIGNED16  cycle_2_offset;
    UNSIGNED16  cycle_4_offset;
    UNSIGNED16  cycle_8_offset;
    UNSIGNED16  cycle_16_offset;
    UNSIGNED16  cycle_32_offset;
    UNSIGNED16  cycle_64_offset;
    UNSIGNED16  cycle_128_offset;
    UNSIGNED16  cycle_256_offset;
    UNSIGNED16  cycle_512_offset;
    UNSIGNED16  cycle_1024_offset;
    WORD16      reserved;
}   SV_MVB_ADMINISTRATOR_PL_CLO;

/* alternative type of split list offsets */
#define TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_PL_SLO 12
typedef struct
{
    UNSIGNED16  split_2_4_offset;
    UNSIGNED16  split_8_16_offset;
    UNSIGNED16  split_32_64_offset;
    UNSIGNED16  split_128_256_offset;
    UNSIGNED16  split_512_1024_offset;
    WORD16      reserved;
}   SV_MVB_ADMINISTRATOR_PL_SLO;

/* alternative type of split list */
#define TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_PL_SL 2728
typedef struct
{
    SV_MVB_ADMINISTRATOR_SL split_2_4[4];
    SV_MVB_ADMINISTRATOR_SL split_8_16[16];
    SV_MVB_ADMINISTRATOR_SL split_32_64[64];
    SV_MVB_ADMINISTRATOR_SL split_128_256[256];
    SV_MVB_ADMINISTRATOR_SL split_512_1024[1024];
}   SV_MVB_ADMINISTRATOR_PL_SL;


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_P_PL
 *
 *  Purpose   : Returns the pointer to first entry of the 'periodic list'
 *              or NULL if list is empty.
 *
 *  Input     : _p_base_ - pointer to base of MVB_Administrator object
 *
 *  Return    : pointer to base of the 'periodic list'
 *              or NULL if list is empty
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_P_PL(_p_base_)                             \
        (                                                                   \
            ((_p_base_)->periodic_list_offset ==                            \
             (_p_base_)->bus_administrators_list_offset)                    \
            ? (SV_MVB_ADMINISTRATOR_PL*)NULL                                \
            : (SV_MVB_ADMINISTRATOR_PL*)                                    \
              (                                                             \
                  (UNSIGNED32)(_p_base_) +                                  \
                  (UNSIGNED32)(_p_base_)->periodic_list_offset              \
              )                                                             \
        )


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_ADMINISTRATOR_BAL
 *
 *  Purpose           : Defines the 'bus administrator list entry'
 *                      of the MVB_Administrator object.
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_BAL 2
#ifdef TCN_LE
    typedef struct /* little-endian representation */
    {
        BITFIELD16  device_address  : 12;
        BITFIELD16  f_code          : 4;  /* fix: '1000'B */
    }   SV_MVB_ADMINISTRATOR_BAL;
#else /* #ifdef TCN_LE */
    typedef struct /* big-endian representation */
    {
        BITFIELD16  f_code          : 4;  /* fix: '1000'B */
        BITFIELD16  device_address  : 12;
    }   SV_MVB_ADMINISTRATOR_BAL;
#endif /* #else */


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_C_BAL
 *
 *  Purpose   : Returns the number of entries
 *              of the 'bus administrator list'.
 *
 *  Input     : _p_base_ - pointer to base of MVB_Administrator object
 *
 *  Return    : number of entires of the 'bus administrator list'
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_C_BAL(_p_base_)                            \
        (UNSIGNED16)                                                        \
        (                                                                   \
            (                                                               \
                (_p_base_)->devices_scan_list_offset -                      \
                (_p_base_)->bus_administrators_list_offset                  \
            ) / 2                                                           \
        )


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_P_BAL
 *
 *  Purpose   : Returns the pointer to first entry
 *              of the 'bus administrator list' or NULL if list is empty.
 *
 *  Input     : _p_base_ - pointer to base of MVB_Administrator object
 *
 *  Return    : pointer to first entry of the 'bus administrator list'
 *              or NULL if list is empty.
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_P_BAL(_p_base_)                            \
        (                                                                   \
            ((_p_base_)->bus_administrators_list_offset ==                  \
             (_p_base_)->devices_scan_list_offset)                          \
            ? (SV_MVB_ADMINISTRATOR_BAL*)NULL                               \
            : (SV_MVB_ADMINISTRATOR_BAL*)                                   \
              (                                                             \
                  (UNSIGNED32)(_p_base_) +                                  \
                  (UNSIGNED32)(_p_base_)->bus_administrators_list_offset    \
              )                                                             \
        )


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_C_DSL
 *
 *  Purpose   : Returns the number of entries of the 'devices scan list'.
 *
 *  Input     : _p_base_ - pointer to base of MVB_Administrator object
 *
 *  Return    : number of entires of the 'devices scan list'
 *
 *  Remarks   : - The number of entries is equal to the element
 *                'macro_cycles', which depends on the element 'basic_period'
 *                (e.g. basic_period=1000 => 1024 entries).
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_C_DSL(_p_base_)                            \
        (UNSIGNED16)                                                        \
        (                                                                   \
            (_p_base_)->end_list_offset -                                   \
            (_p_base_)->devices_scan_list_offset                            \
        )


/* --------------------------------------------------------------------------
 *  Macro     : SV_MVB_ADMINISTRATOR_GET_P_DSL
 *
 *  Purpose   : Returns the pointer to first entry
 *              of the 'devices scan list' or NULL if list is empty.
 *
 *  Input     : _p_base_ - pointer to base of MVB_Administrator object
 *
 *  Return    : pointer to first entry of the 'devices scan list'
 *              or NULL if list is empty
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ADMINISTRATOR_GET_P_DSL(_p_base_)                            \
        (                                                                   \
            ((_p_base_)->devices_scan_list_offset ==                        \
             (_p_base_)->end_list_offset)                                   \
            ? (UNSIGNED8*)NULL                                              \
            : (UNSIGNED8*)                                                  \
              (                                                             \
                  (UNSIGNED32)(_p_base_) +                                  \
                  (UNSIGNED32)(_p_base_)->devices_scan_list_offset          \
              )                                                             \
        )


/* ==========================================================================
 *
 *  Error Counters - Port specific MVB diagnostic information
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Bit Constants : SV_MVB_ERRCNT_CMD_BIT_xxx
 *
 *  Purpose       : Defines the commands for the Error Counters.
 *
 *  Remarks       : - Used for element 'command' of structured type
 *                    'SV_MVB_ERRCNT_GLOBAL'.
 *                  - Used for element 'command' of structured type
 *                    'SV_MVB_ERRCNT_DS_PORT'.
 *                  - Used for element 'command' of structured type
 *                    'SV_MVB_ERRCNT_PD_PORT'.
 * --------------------------------------------------------------------------
 */
#define SV_MVB_ERRCNT_CMD_BIT_RESET_SEL 0x0001 /* reset selected counters  */
                                               /*  after reading           */
#define SV_MVB_ERRCNT_CMD_BIT_RESET_ALL 0x0002 /* reset all counters after */
                                               /*  reading selected one    */


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_ERRCNT_GLOBAL
 *
 *  Purpose           : Global error counter object.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          WORD16      reserved0;
 *                          BITSET16    command;
 *                          WORD32      reserved1;
 *                          WORD32      reserved2;
 *                          UNSIGNED32  errors_lineA;
 *                          UNSIGNED32  errors_lineB;
 *                      }   SV_MVB_ERRCNT_GLOBAL;
 *
 *  Element           : reserved0     - reserved (=0)
 *                      command       - command to be performed;
 *                                      see bit constants
 *                                      'SV_MVB_ERRCNT_CMD_BIT_xxx'
 *                      reserved1     - reserved (=0)
 *                      reserved2     - reserved (=0)
 *                      errors_lineA  - 32-bit counter incremented each
 *                                      time a device status or process data
 *                                      frame was received over Line_B but
 *                                      not over Line_A;
 *                                      this counter does not wraparound
 *                                      when reaching its highest value
 *                      errors_lineB  - 32-bit counter incremented each
 *                                      time a device status or process data
 *                                      frame was received over Line_A but
 *                                      not over Line_B;
 *                                      this counter does not wraparound
 *                                      when reaching its highest value
 *
 *  Remarks           : - The global error counters can be read from
 *                        a connected bus controller link layer
 *                        by a supervision service handler procedure
 *                        (e.g. as_service_handler or ls_service_handler)
 *                        using SV_MVB_SERVICE_ERRCNT_GET_GLOBAL as service
 *                        identifier (parameter 'service').
 *                      - The user has to provide a memory buffer of this
 *                        type and has to specify the element 'command'
 *                        (parameter 'p_parameter').
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_ERRCNT_GLOBAL 20
typedef struct
{
    WORD16      reserved0;
    BITSET16    command;
    WORD32      reserved1;
    WORD32      reserved2;
    UNSIGNED32  errors_lineA;
    UNSIGNED32  errors_lineB;
}   SV_MVB_ERRCNT_GLOBAL;


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_ERRCNT_DS_PORT
 *
 *  Purpose           : Device status port specific error counter object.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          WORD16      device_address;
 *                          BITSET16    command;
 *                          WORD32      reserved1;
 *                          WORD32      reserved2;
 *                          UNSIGNED32  errors_lineA;
 *                          UNSIGNED32  errors_lineB;
 *                      }   SV_MVB_ERRCNT_DS_PORT;
 *
 *  Element           : device_address    - device address (0..4095)
 *                      command           - command to be performed;
 *                                          see bit constants
 *                                          'SV_MVB_ERRCNT_CMD_BIT_xxx'
 *                      reserved1         - reserved (=0)
 *                      reserved2         - reserved (=0)
 *                      errors_lineA      - 32-bit counter incremented each
 *                                          time a device status frame was
 *                                          received over Line_B but not over
 *                                          Line_A;
 *                                          this counter does not wraparound
 *                                          when reaching its highest value
 *                      errors_lineB      - 32-bit counter incremented each
 *                                          time a device status frame was
 *                                          received over Line_A but not over
 *                                          Line_B;
 *                                          this counter does not wraparound
 *                                          when reaching its highest value
 *
 *  Remarks           : - The device status port specific error counters can
 *                        be read from a connected bus controller link layer
 *                        by a supervision service handler procedure
 *                        (e.g. as_service_handler or ls_service_handler)
 *                        using SV_MVB_SERVICE_ERRCNT_GET_DS_PORT as service
 *                        identifier (parameter 'service').
 *                      - The user has to provide a memory buffer of this
 *                        type and has to specify the elements
 *                        'device_address' and 'command'
 *                        (parameter 'p_parameter').
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_ERRCNT_DS_PORT 20
typedef struct
{
    WORD16      device_address;
    BITSET16    command;
    WORD32      reserved1;
    WORD32      reserved2;
    UNSIGNED32  errors_lineA;
    UNSIGNED32  errors_lineB;
}   SV_MVB_ERRCNT_DS_PORT;


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_ERRCNT_PD_PORT
 *
 *  Purpose           : Process data port specific error counter object.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          WORD16      port_address;
 *                          BITSET16    command;
 *                          WORD32      reserved1;
 *                          WORD32      reserved2;
 *                          UNSIGNED32  errors_lineA;
 *                          UNSIGNED32  errors_lineB;
 *                      }   SV_MVB_ERRCNT_PD_PORT;
 *
 *  Element           : port_address  - port address (0..4095)
 *                      command       - command to be performed;
 *                                      see bit constants
 *                                      'SV_MVB_ERRCNT_CMD_BIT_xxx'
 *                      reserved1     - reserved (=0)
 *                      reserved2     - reserved (=0)
 *                      errors_lineA  - 32-bit counter incremented each
 *                                      time a process data frame was
 *                                      received over Line_B but not over
 *                                      Line_A;
 *                                      this counter does not wraparound
 *                                      when reaching its highest value
 *                      errors_lineB  - 32-bit counter incremented each
 *                                      time a process data frame was
 *                                      received over Line_A but not over
 *                                      Line_B;
 *                                      this counter does not wraparound
 *                                      when reaching its highest value
 *
 *  Remarks           : - The process data port specific error counters can
 *                        be read from a connected bus controller link layer
 *                        by a supervision service handler procedure
 *                        (e.g. as_service_handler or ls_service_handler)
 *                        using SV_MVB_SERVICE_ERRCNT_GET_PD_PORT as service
 *                        identifier (parameter 'service').
 *                      - The user has to provide a memory buffer of this
 *                        type and has to specify the elements 'port_address'
 *                        and 'command' (parameter 'p_parameter').
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_ERRCNT_PD_PORT 20
typedef struct
{
    WORD16      port_address;
    BITSET16    command;
    WORD32      reserved1;
    WORD32      reserved2;
    UNSIGNED32  errors_lineA;
    UNSIGNED32  errors_lineB;
}   SV_MVB_ERRCNT_PD_PORT;


/* ==========================================================================
 *
 *  MVB UART Emulation
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_MUE_SV_BA_CONFIG
 *
 *  Purpose           : MUE SV bus administrator configuration object.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          UNSIGNED16  nr_entries;
 *                          WORD16      ba_list[1]; (dynamic size)
 *                      }   SV_MVB_MUE_SV_BA_CONFIG;
 *
 *  Element           : nr_entries - number of entries in this list
 *                      ba_list    - list of bus administrator configuration
 *                                   entries
 *
 *  Remarks           : - The MUE bus administrator configuration object
 *                        can be written to a connected bus controller link
 *                        layer by a supervision service handler procedure
 *                        (e.g. as_service_handler or ls_service_handler)
 *                        using SV_MVB_SERVICE_MUE_SV_PUT_BA_CONFIG as
 *                        service identifier (parameter 'service').
 *                      - The user has to provide a memory buffer of this
 *                        type (parameter 'p_parameter').
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_MUE_SV_BA_CONFIG 4
typedef struct
{
    UNSIGNED16  nr_entries;
    WORD16      ba_list[1]; /* dynamic size */
}   SV_MVB_MUE_SV_BA_CONFIG;


/* --------------------------------------------------------------------------
 *  Constants : SV_MVB_MUE_PORT_STATUS_xxx
 *
 *  Purpose   : MUE status information (related to a MVB device status port
 *              or a MVB process data port).
 *
 *  Remarks   : - Used for element 'port_status' of structured type
 *                'SV_MVB_MUE_SV_DEVICE_STATUS'.
 *              - Used for element 'port_status' of structured type
 *                'SV_MVB_MUE_PD_PORT_DATA'.
 * --------------------------------------------------------------------------
 */
#define SV_MVB_MUE_PORT_STATUS_SINK_MASK    0xC0 /* mask for sink bits     */
#define SV_MVB_MUE_PORT_STATUS_SINK_A       0x80 /* received on MVB line A */
#define SV_MVB_MUE_PORT_STATUS_SINK_B       0x40 /* received on MVB line B */


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_MUE_SV_DEVICE_STATUS
 *
 *  Purpose           : MUE SV device status object.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          WORD16      device_address;
 *                          WORD8       reserved1;
 *                          BITSET8     port_status;
 *                          BITSET16    device_status;
 *                          UNSIGNED16  port_freshness;
 *                      }   SV_MVB_MUE_SV_DEVICE_STATUS;
 *
 *  Element           : device_address - device identifier
 *                                       (12-bit device address)
 *                      reserved1      - reserved (=0)
 *                      port_status    - status information releated to
 *                                       a MVB device status port;
 *                                       see constants 
 *                                       'SV_MVB_MUE_PORT_STATUS_xxx'
 *                      device_status  - data related to a MVB device
 *                                       status port;
 *                                       any combination of bit constants
 *                                       'SV_MVB_DEVICE_STATUS_BIT_xxx'
 *                      freshness      - freshness timer related to a
 *                                       MVB device status port
 *
 *  Remarks           : - The MUE device data object can be read from
 *                        a connected bus controller link layer by
 *                        a supervision service handler procedure
 *                        (e.g. as_service_handler or ls_service_handler)
 *                        using SV_MVB_SERVICE_MUE_SV_GET_DEVICE_STATUS as
 *                        service identifier (parameter 'service').
 *                      - The user has to provide a memory buffer of this
 *                        type and has to specify the element
 *                        'device_address' (parameter 'p_parameter').
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_MUE_SV_DEVICE_STATUS 8
typedef struct
{
    WORD16      device_address;
    WORD8       reserved1;
    BITSET8     port_status;
    BITSET16    device_status;
    UNSIGNED16  freshness;
}   SV_MVB_MUE_SV_DEVICE_STATUS;


/* --------------------------------------------------------------------------
 *  Structured Type   : SV_MVB_MUE_PD_PORT_DATA
 *
 *  Purpose           : MUE PD port data object.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          WORD16      port_address;
 *                          UNSIGNED8   port_size;
 *                          BITSET8     port_status;
 *                          WORD8       port_data[32];
 *                          UNSIGNED16  port_freshness;
 *                          WORD16      reserved1;
 *                      }   SV_MVB_MUE_PD_PORT_DATA;
 *
 *  Element           : port_address   - port identifier
 *                                       (12-bit logical adddress)
 *                      port_size      - size of a port (number of bytes)
 *                      port_status    - status information releated to
 *                                       a MVB process data port;
 *                                       see constants
 *                                       'SV_MVB_MUE_PORT_STATUS_xxx'
 *                      port_data      - data related to a MVB process
 *                                       data port; up to 32 bytes, which
 *                                       is the largest size of a port
 *                      port_freshness - freshness timer related to a
 *                                       MVB process data port
 *                      reserved1      - reserved (=0)
 *
 *  Remarks           : - The MUE port data object can be read from
 *                        a connected bus controller link layer by
 *                        a supervision service handler procedure
 *                        (e.g. as_service_handler or ls_service_handler)
 *                        using SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA as
 *                        service identifier (parameter 'service').
 *                      - The user has to provide a memory buffer of this
 *                        type and has to specify the element
 *                        'port_address' (parameter 'p_parameter').
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_SV_MVB_MUE_PD_PORT_DATA 40
typedef struct
{
    WORD16      port_address;
    UNSIGNED8   port_size;
    BITSET8     port_status;
    WORD8       port_data[32];
    UNSIGNED16  port_freshness;
    WORD16      reserved1;
}   SV_MVB_MUE_PD_PORT_DATA;


#endif /* #ifndef TCN_SV_M_H */
