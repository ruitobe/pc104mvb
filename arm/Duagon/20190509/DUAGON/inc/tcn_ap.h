#ifndef TCN_AP_H
#define TCN_AP_H

/* ==========================================================================
 *
 *  File      : TCN_AP.H
 *
 *  Purpose   : Application Layer Interface for Process Variables
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
#include <tcn_pd.h>


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
#define AP_MAX_TRAFFIC_STORES               16
TCN_DECL_PUBLIC TCN_DECL_CONST UNSIGNED16   ap_max_traffic_store;


/* --------------------------------------------------------------------------
 *  Enumerated Type   : AP_RESULT
 *
 *  Purpose           : Result of a procedure.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    AP_OK           = 0,    /* correct termination                         */
    AP_PRT_PASSIVE  = 1,    /* port (dataset) not active                   */
    AP_ERROR        = 2,    /* unspecified error                           */
    AP_CONFIG       = 3,    /* configuration error                         */
    AP_MEMORY       = 4,    /* not enough memory                           */
    AP_UNKNOW_TS    = 5,    /* unknown traffic store (e.g. not connected)  */
    AP_RANGE        = 6,    /* memory address error                        */
    AP_DATA_TYPE    = 7     /* unsupported data type                       */
}   AP_RESULT;


/* ==========================================================================
 *
 *  Initialisation
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ap_init
 *
 *  Purpose   : Initialises the process data related layers
 *              (incl. sub-layers).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_init (void);
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - This procedure has to be called at system initialisation
 *                before calling any other 'ap_xxx' procedure.
 *              - This procedure shall be called only one time.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_init (void);


/* --------------------------------------------------------------------------
 *  Procedure : ap_show_traffic_stores
 *
 *  Purpose   : Retrieves the number of link layers supporting process data
 *              connected to this station and lists their traffic store
 *              identifiers.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_show_traffic_stores
 *              (
 *                  UNSIGNED8   *nr_of_busses,
 *                  UNSIGNED8   *link_id_list
 *              );
 *
 *  Output    : nr_of_busses - number of connected link layers supporting
 *                             process data (NOTE: the max. number of
 *                             connected link layers is 16)
 *              link_id_list - list of connected link layers supporting
 *                             process data, with at least the 'ts_id' of
 *                             each (NOTE: range of ts_id=0..15)
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - The application has to provide a buffer for the link
 *                identifier list (parameter 'link_id_list'). The size of
 *                this buffer should be 16, which is the max. number of
 *                connected link layers.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_show_traffic_stores
(
    UNSIGNED8   *nr_of_busses,
    UNSIGNED8   *link_id_list
);


/* ==========================================================================
 *
 *  Ports Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ap_ts_config
 *
 *  Purpose   : Configures an empty traffic store and sets up the freshness
 *              supervision interval.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_ts_config
 *              (
 *                  ENUM8       ts_id,
 *                  UNSIGNED16  fsi
 *              );
 *
 *  Input     : ts_id - traffic store identifier (0..15)
 *              fsi   - freshness supervision interval
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 *              - This procedure has to be called at system initialisation
 *                before calling any other 'ap_xxx' procedure referencing
 *                the same traffic store (parameter 'ts_id').
 *              - This procedure shall be called only one time for each
 *                traffic store.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_ts_config
(
    ENUM8       ts_id,
    UNSIGNED16  fsi
);


/* --------------------------------------------------------------------------
 *  Procedure : ap_ts_info
 *
 *  Purpose   : Returns information about a traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_ts_info
 *              (
 *                  ENUM8       ts_id,
 *                  UNSIGNED16  *port_number_max,
 *                  UNSIGNED8   *port_size_max
 *              );
 *
 *  Input     : ts_id           - traffic store identifier (0..15)
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Output    : port_number_max - max. number of supported ports
 *              port_size_max   - max. size of a port (number of bytes)
 *
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_ts_info
(
    ENUM8       ts_id,
    UNSIGNED16  *port_number_max,
    UNSIGNED8   *port_size_max
);


/* --------------------------------------------------------------------------
 *  Procedure : ap_port_manage
 *
 *  Purpose   : Manages a port located in a traffic store (e.g. configure,
 *              delete, modify, retrieve status).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_port_manage
 *              (
 *                  ENUM8       ts_id,
 *                  WORD16      port_address,
 *                  ENUM16      command,
 *                  PD_PRT_ATTR *prt_attr
 *              );
 *
 *  Input     : ts_id        - traffic store identifier (0..15)
 *              port_address - port address (0..4095)
 *              command      - port manage command (any PD_PRT_CMD)
 *  In-/Output: prt_attr     - port attributes of structured type
 *                             'PD_PRT_ATTR'
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - If command PD_PRT_CMD_CONFIG is used, then the port value
 *                will be cleared (set all bytes to 0x00) and the freshness
 *                timer of the port will be set to the largest value (65535).
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_port_manage
(
    ENUM8       ts_id,
    WORD16      port_address,
    ENUM16      command,
    PD_PRT_ATTR *prt_attr
);


/* ==========================================================================
 *
 *  Datasets Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : DS_NAME
 *
 *  Purpose           : Identifies a dataset.
 *
 *  Syntax            : typedef struct (big-endian representation)
 *                      {
 *                          BITFIELD16  traffic_store_id    : 4;
 *                          BITFIELD16  port_address        : 12;
 *                      }   DS_NAME;
 *
 *  Element           : traffic_store_id - traffic store identifier (0..15)
 *                      port_address     - port address (0..4095)
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_DS_NAME 2
#ifdef TCN_LE
    typedef struct /* little-endian representation */
    {
        BITFIELD16  port_address        : 12;
        BITFIELD16  traffic_store_id    : 4;
    }   DS_NAME;
#else /* #ifdef TCN_LE */
    typedef struct /* big-endian representation */
    {
        BITFIELD16  traffic_store_id    : 4;
        BITFIELD16  port_address        : 12;
    }   DS_NAME;
#endif /* #else */


/* --------------------------------------------------------------------------
 *  Procedure : ap_put_dataset
 *
 *  Purpose   : Copies a dataset from the application to a port in the
 *              traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_put_dataset
 *              (
 *                  DS_NAME     *dataset,
 *                  void        *p_value
 *              );
 *
 *  Input     : dataset - DS_NAME of the dataset to be published
 *              p_value - pointer to a memory buffer of the application
 *                        where the dataset value is copied from
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - The previous value of the dataset in the port is
 *                overwritten.
 *              - A port in a traffic store hold its dataset as it will be
 *                transmitted over the bus.
 *              - A dataset (parameter 'p_value') is handled as an array of
 *                octets (e.g. WORD8).
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_put_dataset
(
    DS_NAME     *dataset,
    void        *p_value
);


/* --------------------------------------------------------------------------
 *  Procedure : ap_get_dataset
 *
 *  Purpose   : Copies a dataset and its freshness timer from a port in the
 *              traffic store to the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_get_dataset
 *              (
 *                  DS_NAME     *dataset,
 *                  void        *p_value,
 *                  void        *p_fresh
 *              );
 *
 *  Input     : dataset - DS_NAME of the dataset to be received
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Input     : p_value - pointer to a memory buffer of the application
 *                        where the dataset value is copied to
 *              p_fresh - pointer to a memory buffer of the application
 *                        where the freshness timer is copied to
 *
 *  Remarks   : - A port in a traffic store hold its dataset as it will be
 *                transmitted over the bus.
 *              - A dataset (parameter 'p_value') is handled as an array of
 *                octets (e.g. WORD8).
 *              - A freshness timer (parameter 'p_fresh') is handled as
 *                type UNSIGNED16.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_get_dataset
(
    DS_NAME     *dataset,
    void        *p_value,
    void        *p_fresh
);


/* ==========================================================================
 *
 *  Variables Interface (individual access)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : PV_NAME
 *
 *  Purpose           : Identifies a process variable.
 *
 *  Syntax            : typedef struct (big-endian representation)
 *                      {
 *                          BITFIELD16  traffic_store_id    : 4;
 *                          BITFIELD16  port_address        : 12;
 *                          BITFIELD16  var_size            : 6;
 *                          BITFIELD16  var_octet_offset    : 7;
 *                          BITFIELD16  var_bit_number      : 3;
 *                          BITFIELD16  var_type            : 6;
 *                          BITFIELD16  chk_octet_offset    : 7;
 *                          BITFIELD16  chk_bit_number      : 3;
 *                      }   PV_NAME;
 *
 *  Element           : traffic_store_id - traffic store identifier (0..15)
 *                                         NOTE: first part of DS_NAME
 *                      port_address     - port address (0..4095)
 *                                         NOTE: second part of DS_NAME
 *                      var_size         - any AP_VAR_SIZE
 *                      var_octet_offset - NOTE: first octet has offset 0
 *                      var_bit_number   - NOTE: counted from the right
 *                      var_type         - any AP_VAR_TYPE
 *                      chk_octet_offset - NOTE: first octet has offset 0
 *                      chk_bit_number   - NOTE: counted from the right
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_PV_NAME 6
#ifdef TCN_LE
    typedef struct /* little-endian representation */
    {
        BITFIELD16  chk_bit_number      : 3;
        BITFIELD16  chk_octet_offset    : 7;
        BITFIELD16  var_type            : 6;
        BITFIELD16  var_bit_number      : 3;
        BITFIELD16  var_octet_offset    : 7;
        BITFIELD16  var_size            : 6;
        BITFIELD16  port_address        : 12;
        BITFIELD16  traffic_store_id    : 4;
    }   PV_NAME;
#else /* #ifdef TCN_LE */
    typedef struct /* big-endian representation */
    {
        BITFIELD16  traffic_store_id    : 4;
        BITFIELD16  port_address        : 12;
        BITFIELD16  var_size            : 6;
        BITFIELD16  var_octet_offset    : 7;
        BITFIELD16  var_bit_number      : 3;
        BITFIELD16  var_type            : 6;
        BITFIELD16  chk_octet_offset    : 7;
        BITFIELD16  chk_bit_number      : 3;
    }   PV_NAME;
#endif /* #else */


/* --------------------------------------------------------------------------
 *  Enumerated Type   : AP_VAR_TYPE
 *
 *  Purpose           : Type of a process variable (6-bit value).
 * --------------------------------------------------------------------------
 */
typedef enum
{
    /* primitive variable types with  */
    /* size less than one 16-bit word */
    AP_VAR_TYPE_BOOLEAN1                = 0x00, /*  0 */
    AP_VAR_TYPE_ANTIVALENT2             = 0x01, /*  1 */
    AP_VAR_TYPE_BCD4                    = 0x02, /*  2 */
    AP_VAR_TYPE_ENUM4                   = 0x02, /*  2 */
    AP_VAR_TYPE_BITSET8                 = 0x04, /*  4 */
    AP_VAR_TYPE_WORD8                   = 0x05, /*  5 */
    AP_VAR_TYPE_ENUM8                   = 0x05, /*  5 */
    AP_VAR_TYPE_UNSIGNED8               = 0x05, /*  5 */
    AP_VAR_TYPE_INTEGER8                = 0x06, /*  6 */
    AP_VAR_TYPE_CHARACTER8              = 0x07, /*  7 */

    /* primitive variable types with */
    /* size of one 16-bit word       */
    AP_VAR_TYPE_BITSET16                = 0x04, /*  4 */
    AP_VAR_TYPE_WORD16                  = 0x05, /*  5 */
    AP_VAR_TYPE_ENUM16                  = 0x05, /*  5 */
    AP_VAR_TYPE_UNSIGNED16              = 0x05, /*  5 */
    AP_VAR_TYPE_INTEGER16               = 0x06, /*  6 */
    AP_VAR_TYPE_BIPOLAR2_16             = 0x08, /*  8 */
    AP_VAR_TYPE_UNIPOLAR2_16            = 0x09, /*  9 */
    AP_VAR_TYPE_BIPOLAR4_16             = 0x0A, /* 10 */

    /* primitive variable types with */
    /* size of two 16-bit word       */
    AP_VAR_TYPE_REAL32                  = 0x03, /*  3 */
    AP_VAR_TYPE_BITSET32                = 0x04, /*  4 */
    AP_VAR_TYPE_WORD32                  = 0x05, /*  5 */
    AP_VAR_TYPE_ENUM32                  = 0x05, /*  5 */
    AP_VAR_TYPE_UNSIGNED32              = 0x05, /*  5 */
    AP_VAR_TYPE_INTEGER32               = 0x06, /*  6 */

    /* primitive variable types with */
    /* size of three 16-bit word     */
    AP_VAR_TYPE_TIMEDATE48              = 0x02, /*  2 */

    /* primitive variable types with */
    /* size of four 16-bit word      */
    AP_VAR_TYPE_BITSET64                = 0x04, /*  4 */
    AP_VAR_TYPE_WORD64                  = 0x05, /*  5 */
    AP_VAR_TYPE_ENUM64                  = 0x05, /*  5 */
    AP_VAR_TYPE_UNSIGNED64              = 0x05, /*  5 */
    AP_VAR_TYPE_INTEGER64               = 0x06, /*  6 */

    /* structured variable types */
    AP_VAR_TYPE_ARRAY_WORD8_ODD         = 0x07, /*  7 */
    AP_VAR_TYPE_ARRAY_WORD8_EVEN        = 0x0F, /* 15 */
    AP_VAR_TYPE_ARRAY_UNSIGNED16        = 0x0D, /* 13 */
    AP_VAR_TYPE_ARRAY_INTEGER16         = 0x0E, /* 14 */
    AP_VAR_TYPE_ARRAY_UNSIGNED32        = 0x0B, /* 11 */
    AP_VAR_TYPE_ARRAY_INTEGER32         = 0x0C, /* 12 */

    /* flag for little-endian representation of data types (MSB of 6-bits) */
    AP_VAR_TYPE_FLAG_LE                 = 0x20  /* 100000b */
}   AP_VAR_TYPE;


/* --------------------------------------------------------------------------
 *  Enumerated Type   : AP_VAR_SIZE
 *
 *  Purpose           : Size of a process variable (6-bit value).
 * --------------------------------------------------------------------------
 */
typedef enum
{
    /* variable size is less than one 16-bit word */
    AP_VAR_SIZE_8                       = 0,
    AP_VAR_SIZE_BOOLEAN1                = 0,
    AP_VAR_SIZE_ANTIVALENT2             = 0,
    AP_VAR_SIZE_BCD4                    = 0,
    AP_VAR_SIZE_ENUM4                   = 0,
    AP_VAR_SIZE_BITSET8                 = 0,
    AP_VAR_SIZE_WORD8                   = 0,
    AP_VAR_SIZE_ENUM8                   = 0,
    AP_VAR_SIZE_UNSIGNED8               = 0,
    AP_VAR_SIZE_INTEGER8                = 0,
    AP_VAR_SIZE_CHARACTER8              = 0,

    /* variable size is one 16-bit word */
    AP_VAR_SIZE_16                      = 1,
    AP_VAR_SIZE_BITSET16                = 1,
    AP_VAR_SIZE_WORD16                  = 1,
    AP_VAR_SIZE_ENUM16                  = 1,
    AP_VAR_SIZE_UNSIGNED16              = 1,
    AP_VAR_SIZE_INTEGER16               = 1,
    AP_VAR_SIZE_BIPOLAR2_16             = 1,
    AP_VAR_SIZE_UNIPOLAR2_16            = 1,
    AP_VAR_SIZE_BIPOLAR4_16             = 1,

    /* variable size is two 16-bit words */
    AP_VAR_SIZE_32                      = 2,
    AP_VAR_SIZE_REAL32                  = 2,
    AP_VAR_SIZE_BITSET32                = 2,
    AP_VAR_SIZE_WORD32                  = 2,
    AP_VAR_SIZE_ENUM32                  = 2,
    AP_VAR_SIZE_UNSIGNED32              = 2,
    AP_VAR_SIZE_INTEGER32               = 2,

    /* variable size is three 16-bit words */
    AP_VAR_SIZE_48                      = 3,
    AP_VAR_SIZE_TIMEDATE48              = 3,

    /* variable size is four 16-bit words */
    AP_VAR_SIZE_64                      = 4,
    AP_VAR_SIZE_BITSET64                = 4,
    AP_VAR_SIZE_WORD64                  = 4,
    AP_VAR_SIZE_ENUM64                  = 4,
    AP_VAR_SIZE_UNSIGNED64              = 4,
    AP_VAR_SIZE_INTEGER64               = 4
}   AP_VAR_SIZE;


/* --------------------------------------------------------------------------
 *  Procedure : ap_put_variable
 *
 *  Purpose   : Copies a process variable from the application to
 *              a traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_put_variable
 *              (
 *                  PV_NAME     *ts_variable,
 *                  void        *p_value,
 *                  void        *p_check
 *              );
 *
 *  Input     : ts_variable - PV_NAME of the process variable to be published
 *              p_value     - pointer to a memory buffer of the application
 *                            where the process variable value is copied from
 *              p_check     - pointer to a memory buffer of the application
 *                            where the check variable value is copied from
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - The previous value of the process variable dataset is
 *                overwritten.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_put_variable
(
    PV_NAME     *ts_variable,
    void        *p_value,
    void        *p_check
);


/* --------------------------------------------------------------------------
 *  Procedure : ap_get_variable
 *
 *  Purpose   : Copies a process variable from a traffic store to
 *              the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_get_variable
 *              (
 *                  PV_NAME     *ts_variable,
 *                  void        *p_value,
 *                  void        *p_check,
 *                  void        *p_fresh
 *              );
 *
 *  Input     : ts_variable - PV_NAME of the process variable to be received
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Output    : p_value     - pointer to a memory buffer of the application
 *                            where the process variable value is copied to
 *              p_check     - pointer to a memory buffer of the application
 *                            where the check variable value is copied to
 *              p_fresh     - pointer to a memory buffer of the application
 *                            where the freshness timer is copied to
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_get_variable
(
    PV_NAME     *ts_variable,
    void        *p_value,
    void        *p_check,
    void        *p_fresh
);


/* ==========================================================================
 *
 *  Variables Interface (set access)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : PV_SET (PV_LIST)
 *
 *  Purpose           : Identifies a group of process variables belonging to
 *                      the same dataset.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          void        *p_variable;
 *                          UNSIGNED8   derived_type;
 *                          UNSIGNED8   array_count;
 *                          UNSIGNED8   octet_offset;
 *                          UNSIGNED8   bit_number;
 *                      }   PV_LIST;
 *
 *                      typedef struct
 *                      {
 *                          PV_LIST     *p_pv_list;
 *                          UNSIGNED16  c_pv_list;
 *                          UNSIGNED16  *p_freshtime;
 *                          DS_NAME     dataset;
 *                      }   PV_SET;
 *
 *  Element           : p_variable   - pointer to a memory buffer of the
 *                                     application where the process
 *                                     variable value is copied to/from
 *                      derived_type - any AP_DERIVED_TYPE
 *                      array_count  - number of elements in the array
 *                      octet_offset - offset in number of octets of
 *                                     a process variable
 *                      bit_number   - bit number of a process variable
 *                                     smaller than one octet
 *
 *                      p_pv_list    - pointer to a memory buffer of the
 *                                     application where a list of type
 *                                     PV_LIST is stored
 *                      c_pv_list    - number of process variables in the
 *                                     PV_LIST
 *                      p_freshtime  - pointer to a memory buffer of the
 *                                     application where the freshness timer
 *                                     is copied to (not used for ap_put_set)
 *                      dataset      - DS_NAME (holds for the whole set)
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_PV_LIST 8
typedef struct
{
    void        *p_variable;
    UNSIGNED8   derived_type;
    UNSIGNED8   array_count;
    UNSIGNED8   octet_offset;
    UNSIGNED8   bit_number;
}   PV_LIST;

#define TCN_SIZEOF_STRUCT_PV_SET 16
typedef struct
{
    PV_LIST     *p_pv_list;
    UNSIGNED16  c_pv_list;
    UNSIGNED16  *p_freshtime;
    DS_NAME     dataset;
}   PV_SET;


/* --------------------------------------------------------------------------
 *  Enumerated Type   : AP_DERIVED_TYPE
 *
 *  Purpose           : Data type of a process variable (8-bit value).
 * --------------------------------------------------------------------------
 */
typedef enum
{
    /* primitive variable types with        */
    /* size less than one 16-bit word       */
    AP_DERIVED_TYPE_BOOLEAN1            = 0x00,
    AP_DERIVED_TYPE_ANTIVALENT2         = 0x01,
    AP_DERIVED_TYPE_BCD4                = 0x02,
    AP_DERIVED_TYPE_ENUM4               = 0x02,
    AP_DERIVED_TYPE_BITSET8             = 0x04,
    AP_DERIVED_TYPE_WORD8               = 0x05,
    AP_DERIVED_TYPE_ENUM8               = 0x05,
    AP_DERIVED_TYPE_UNSIGNED8           = 0x05,
    AP_DERIVED_TYPE_INTEGER8            = 0x06,
    AP_DERIVED_TYPE_CHARACTER8          = 0x07,


    /* primitive variable types with        */
    /* size of one 16-bit word              */
    AP_DERIVED_TYPE_BITSET16            = 0x14,
    AP_DERIVED_TYPE_WORD16              = 0x15,
    AP_DERIVED_TYPE_ENUM16              = 0x15,
    AP_DERIVED_TYPE_UNSIGNED16          = 0x15,
    AP_DERIVED_TYPE_INTEGER16           = 0x16,
    AP_DERIVED_TYPE_BIPOLAR2_16         = 0x18,
    AP_DERIVED_TYPE_UNIPOLAR2_16        = 0x19,
    AP_DERIVED_TYPE_BIPOLAR4_16         = 0x1A,

    /* primitive variable types with        */
    /* size of two 16-bit word              */
    AP_DERIVED_TYPE_REAL32              = 0x23,
    AP_DERIVED_TYPE_BITSET32            = 0x24,
    AP_DERIVED_TYPE_WORD32              = 0x25,
    AP_DERIVED_TYPE_ENUM32              = 0x25,
    AP_DERIVED_TYPE_UNSIGNED32          = 0x25,
    AP_DERIVED_TYPE_INTEGER32           = 0x26,

    /* primitive variable types with        */
    /* size of three 16-bit word            */
    AP_DERIVED_TYPE_TIMEDATE48          = 0x32,

    /* primitive variable types with        */
    /* size of four 16-bit word             */
    AP_DERIVED_TYPE_BITSET64            = 0x44,
    AP_DERIVED_TYPE_WORD64              = 0x45,
    AP_DERIVED_TYPE_ENUM64              = 0x45,
    AP_DERIVED_TYPE_UNSIGNED64          = 0x45,
    AP_DERIVED_TYPE_INTEGER64           = 0x46,

    /* structured variable types (compatibility) */
    AP_DERIVED_TYPE_ARRAY_WORD8_ODD     = 0x05,
    AP_DERIVED_TYPE_ARRAY_WORD8_EVEN    = 0x05,
    AP_DERIVED_TYPE_ARRAY_UNSIGNED16    = 0x15,
    AP_DERIVED_TYPE_ARRAY_INTEGER16     = 0x16,
    AP_DERIVED_TYPE_ARRAY_UNSIGNED32    = 0x25,
    AP_DERIVED_TYPE_ARRAY_INTEGER32     = 0x26,

    /* size coding */
    AP_DERIVED_TYPE_SIZE_MASK           = 0x70,
    AP_DERIVED_TYPE_SIZE_WORD16_0       = 0x00,
    AP_DERIVED_TYPE_SIZE_WORD16_1       = 0x10,
    AP_DERIVED_TYPE_SIZE_WORD16_2       = 0x20,
    AP_DERIVED_TYPE_SIZE_WORD16_3       = 0x30,
    AP_DERIVED_TYPE_SIZE_WORD16_4       = 0x40,

    /* flag for little-endian representation of data types (MSB of 8-bits) */
    AP_DERIVED_TYPE_FLAG_LE             = 0x80  /* 10000000b */
}   AP_DERIVED_TYPE;


/* --------------------------------------------------------------------------
 *  Procedure : ap_put_set
 *
 *  Purpose   : Copies a set of variables from the application to
 *              a traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_put_set
 *              (
 *                  PV_SET      *pv_set
 *              );
 *
 *  Input     : pv_set - pointer to a memory buffer of the application where
 *                       a PV_SET is stored
 *
 *  Return    : result code; any AP_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_put_set
(
    PV_SET      *pv_set
);


/* --------------------------------------------------------------------------
 *  Procedure : ap_get_set
 *
 *  Purpose   : Copies a set of variables from a traffic store to
 *              the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_get_set
 *              (
 *                  PV_SET      *pv_set
 *              );
 *
 *  Input     : pv_set - pointer to a memory buffer of the application where
 *                       a PV_SET is stored
 *
 *  Return    : result code; any AP_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_get_set
(
    PV_SET      *pv_set
);


/* ==========================================================================
 *
 *  Variables Interface (cluster access)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : PV_CLUSTER
 *
 *  Purpose           : Identifies a group of PV_SETs ordered
 *                      by traffic stores.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          UNSIGNED8   ts_id;
 *                          UNSIGNED8   c_pv_set;
 *                          PV_SET      *p_pv_set[1]; (dynamic size)
 *                      }   PV_CLUSTER;
 *
 *  Element           : ts_id    - traffic store identifier (0..15)
 *                      c_pv_set - number of PV_SETs in the cluster
 *                      p_pv_set - array of pointers to PV_SET
 *
 *  Remarks           : - The value of element 'ts_id' is ignored, since it
 *                        is available in the PV_SET element 'dataset'
 *                        (which is of structured type 'DS_NAME').
 *                      - The memory required for a variable of structured
 *                        type 'PV_CLUSTER' must be allocated.
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_PV_CLUSTER 8
typedef struct
{
    UNSIGNED8   ts_id;
    UNSIGNED8   c_pv_set;
    PV_SET      *p_pv_set[1]; /* dynamic size */
}   PV_CLUSTER;


/* --------------------------------------------------------------------------
 *  Procedure : ap_put_cluster
 *
 *  Purpose   : Copies a cluster of variables from the application to
 *              the traffic store(s).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_put_cluster
 *              (
 *                  PV_CLUSTER  *pv_cluster
 *              );
 *
 *  Input     : pv_cluster - pointer to a memory buffer of the application
 *                           where a PV_CLUSTER is stored
 *
 *  Return    : result code; any AP_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_put_cluster
(
    PV_CLUSTER  *pv_cluster
);


/* --------------------------------------------------------------------------
 *  Procedure : ap_get_cluster
 *
 *  Purpose   : Copies a cluster of variables from the traffic store(s) to
 *              the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_get_cluster
 *              (
 *                  PV_CLUSTER  *pv_cluster
 *              );
 *
 *  Input     : pv_cluster - pointer to a memory buffer of the application
 *                           where a PV_CLUSTER is stored
 *
 *  Return    : result code; any AP_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_get_cluster
(
    PV_CLUSTER  *pv_cluster
);


#endif /* #ifndef TCN_AP_H */
