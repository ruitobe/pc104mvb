#ifndef TCN_TNM_H
#define TCN_TNM_H

/* ==========================================================================
 *
 *  File      : TCN_TNM.H
 *
 *  Purpose   : Common Interface for Train Communication Network
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
#include <tcn_sv_m.h>
#include <tcn_am.h>


/* ==========================================================================
 *
 *  Constants and Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Enumerated Type   : MM_RESULT
 *
 *  Purpose           : Result of a procedure (see also AM_RESULT).
 * --------------------------------------------------------------------------
 */
typedef enum
{
    MM_OK                   = 0,    /* successful termination           */

    MM_SIF_NOT_SUPPORTED    = 33,   /* SIF_code not supported           */
    MM_RDONLY_ACCESS        = 34,   /* read only access permitted       */
    MM_CMD_NOT_EXECUTED     = 35,   /* service failed                   */
    MM_DNLD_NO_FLASH        = 36,   /* no non-volatile storage at that  */
                                    /* location                         */
    MM_DNLD_FLASH_HW_ERR    = 37,   /* hardware error during download   */
    MM_BAD_CHECKSUM         = 38,   /* domain has an incorrect checksum */
    MM_INT_ERROR            = 39,   /* internal error                   */
    MM_ER_VERS              = 40,   /* erroneous version                */
    MM_BUS_HW_BAD           = 41,   /* link damaged                     */
    MM_BUS_HW_NO_CONFIG     = 42,   /* unconfigured hardware            */
    MM_LP_ERROR             = 43,   /* failed access to Traffic_Store   */
    MM_VERSION_CONFLICT     = 44    /* version conflict                 */
}   MM_RESULT;


/* --------------------------------------------------------------------------
 *  Constants : TNM_KEY_DEFAULT_xxx
 *
 *  Purpose   : Default tnm_key pair.
 * --------------------------------------------------------------------------
 */
#define TNM_KEY_DEFAULT_CALL    0x02    /* call  message (request ) */
#define TNM_KEY_DEFAULT_REPLY   0x82    /* reply message (response) */


/* --------------------------------------------------------------------------
 *  Bit Constants : TNM_STATION_STATUS_BIT_xxx
 *
 *  Purpose       : Station status.
 * --------------------------------------------------------------------------
 */
/* capabilities */
#define TNM_STATION_STATUS_BIT_SP   0x8000  /* special device (class1=0)   */
#define TNM_STATION_STATUS_BIT_BA   0x4000  /* MVB bus administrator       */
#define TNM_STATION_STATUS_BIT_GW   0x2000  /* gateway or train bus node   */
#define TNM_STATION_STATUS_BIT_MD   0x1000  /* message data                */

/* class specific (general) */
#define TNM_STATION_STATUS_BIT_CS0  0x0800  /* first  bit ...  ...of       */
#define TNM_STATION_STATUS_BIT_CS1  0x0400  /* second bit ...    class     */
#define TNM_STATION_STATUS_BIT_CS2  0x0200  /* third  bit ...   specific   */
#define TNM_STATION_STATUS_BIT_CS3  0x0100  /* fourth bit ...    field     */

/* common flags */
#define TNM_STATION_STATUS_BIT_RES  0x0080  /* unused (=0)                 */
#define TNM_STATION_STATUS_BIT_LND  0x0040  /* link disturbed              */
#define TNM_STATION_STATUS_BIT_SSD  0x0020  /* some system disturbance     */
#define TNM_STATION_STATUS_BIT_SDD  0x0010  /* some device disturbance     */
#define TNM_STATION_STATUS_BIT_SCD  0x0008  /* some comm.  disturbance     */
#define TNM_STATION_STATUS_BIT_FRC  0x0004  /* forced station              */
#define TNM_STATION_STATUS_BIT_DNR  0x0002  /* station not ready           */
#define TNM_STATION_STATUS_BIT_SER  0x0001  /* station reserved by manager */


/* --------------------------------------------------------------------------
 *  Enumerated Type   : TNM_SIF_CODE
 *
 *  Purpose           : Identifies the requested service for the
 *                      default tnm_key pair (TNM_KEY_DEFAULT_xxx).
 * --------------------------------------------------------------------------
 */
typedef enum
{
    READ_STATION_STATUS         = 0,
    WRITE_STATION_CONTROL       = 1,
    READ_STATION_INVENTORY      = 2,
    WRITE_RESERVATION           = 3,
    READ_SERVICE_DESCRIPTOR     = 4,
    READ_LINKS_DESCRIPTOR       = 6,
    WRITE_LINKS_DESCRIPTOR      = 7,
    READ_MVB_STATUS             = 10,
    WRITE_MVB_CONTROL           = 11,
    READ_MVB_DEVICES            = 12,
    WRITE_MVB_ADMINISTRATOR     = 13,
    READ_WTB_STATUS             = 20,
    WRITE_WTB_CONTROL           = 21,
    READ_WTB_NODES              = 22,
    READ_WTB_TOPOGRAPHY         = 24,
    WRITE_ATTACH_PORT           = 29,
    READ_PORTS_CONFIGURATION    = 30,
    WRITE_PORTS_CONFIGURATION   = 31,
    READ_VARIABLES              = 32,
    WRITE_FORCE_VARIABLES       = 33,
    WRITE_UNFORCE_VARIABLES     = 35,
    WRITE_UNFORCE_ALL           = 37,
    READ_VARIABLE_BINDINGS      = 38,
    WRITE_VARIABLE_BINDINGS     = 39,
    READ_MESSENGER_STATUS       = 40,
    WRITE_MESSENGER_CONTROL     = 41,
    READ_FUNCTION_DIRECTORY     = 42,
    WRITE_FUNCTION_DIRECTORY    = 43,
    READ_STATION_DIRECTORY      = 44,
    WRITE_STATION_DIRECTORY     = 45,
    READ_GROUP_DIRECTORY        = 46,
    WRITE_GROUP_DIRECTORY       = 47,
    READ_NODE_DIRECTORY         = 48,
    WRITE_NODE_DIRECTORY        = 49,
    READ_MEMORY                 = 50,
    WRITE_MEMORY                = 51,
    WRITE_DOWNLOAD_SETUP        = 53,
    WRITE_DOWNLOAD_SEGMENT      = 55,
    READ_TASKS_STATUS           = 60,
    WRITE_TASKS_CONTROL         = 61,
    READ_CLOCK                  = 70,
    WRITE_CLOCK                 = 71,
    READ_JOURNAL                = 80,
    READ_EQUIPMENT              = 82,

    /* user services */
    USER_SERVICE_0              = 128,
    USER_SERVICE_127            = 255,

    /* file system services */
    FS_ACCESS                   = 128,
    FS_OPEN                     = 129,
    FS_CLOSE                    = 130,
    FS_READ                     = 131,
    FS_WRITE                    = 132,
    FS_LSEEK                    = 133,
    FS_UNLINK                   = 134,
    FS_STAT                     = 135,
    FS_PATHCONF                 = 136,
    FS_GETCWD                   = 137,
    FS_CHDIR                    = 138,
    FS_MKDIR                    = 139,
    FS_RMDIR                    = 140,
    FS_OPENDIR                  = 141,
    FS_CLOSEDIR                 = 142,
    FS_READDIR                  = 143,
    FS_REWINDDIR                = 144

}   TNM_SIF_CODE;


/* --------------------------------------------------------------------------
 *  Structured Type   : TNM_MSG_HEADER
 *
 *  Purpose           : Common header of a management message.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          ENUM8       tnm_key;
 *                          ENUM8       sif_code;
 *                      }   TNM_MSG_HEADER;
 *
 *  Element           : tnm_key  - The default 'tnm_key' pair is '02'H/'82'H.
 *                                 The most significant bit of 'tnm_key'
 *                                 indicates a call ('00'H) or reply ('80'H)
 *                                 message.
 *                      sif_code - any TNM_SIF_CODE
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_HEADER 2
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
}   TNM_MSG_HEADER;


/* --------------------------------------------------------------------------
 *  Structured Types  : TNM_MSG_xxx
 *
 *  Purpose           : Types describing the bodies of the management
 *                      messages.
 * --------------------------------------------------------------------------
 */

/* --------------------------------------------------------------------------
 *  Station Service - Read_Station_Status
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Station_Status 2
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
}   TNM_MSG_Call_Read_Station_Status;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Station_Status 8
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED8   bus_id;
    WORD8       reserved1;
    WORD16      device_address;
    BITSET16    station_status;
}   TNM_MSG_Reply_Read_Station_Status;

/* --------------------------------------------------------------------------
 *  Station Service - Write_Station_Control
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Station_Control 36
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    BITSET8     command;
    UNSIGNED8   station_id;
    STRING32    station_name;
}   TNM_MSG_Call_Write_Station_Control;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Station_Control 6
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED8   bus_id;
    WORD8       reserved1;
    WORD16      device_address;
}   TNM_MSG_Reply_Write_Station_Control;

/* --------------------------------------------------------------------------
 *  Station Service - Read_Station_Inventory
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Station_Inventory 2
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
}   TNM_MSG_Call_Read_Station_Inventory;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Station_Inventory 170
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    STRING32    agent_version;
    STRING32    manufacturer_name;
    STRING32    device_type;
    BITSET256   service_set;
    BITSET16    link_set;
    WORD8       reserved2;
    UNSIGNED8   station_id;
    STRING32    station_name;
    BITSET16    station_status;
}   TNM_MSG_Reply_Read_Station_Inventory;

/* --------------------------------------------------------------------------
 *  Station Service - Write_Station_Reservation
 * --------------------------------------------------------------------------
 */
#define TNM_STATION_RESERVATION_COMMAND_RESERVE         1
#define TNM_STATION_RESERVATION_COMMAND_KEEPREL         2
#define TNM_STATION_RESERVATION_COMMAND_STARTREL        3

#define TNM_STATION_RESERVATION_ACCESS_TYPE_WRITEREQ    0
#define TNM_STATION_RESERVATION_ACCESS_TYPE_OVERRIDE    1

#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Station_Reservation 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    ENUM16      command;
    ENUM16      access_type;
    UNSIGNED16  reservation_time_out;
    UNSIGNED32  manager_id;
}   TNM_MSG_Call_Write_Station_Reservation;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Station_Reservation 8
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    UNSIGNED32  manager_id;
}   TNM_MSG_Reply_Write_Station_Reservation;

/* --------------------------------------------------------------------------
 *  Station Service - Read_Service_Descriptor
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Service_Descriptor 4
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD8       reserved1;
    ENUM8       get_sif_code;
}   TNM_MSG_Call_Read_Service_Descriptor;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Service_Descriptor 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD8       reserved1;
    ENUM8       get_sif_code;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  service_description[4]; /* dynamic size */
}   TNM_MSG_Reply_Read_Service_Descriptor;

/* --------------------------------------------------------------------------
 *  Station Service - link descriptor
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_LINK_DESCRIPTOR 34
typedef struct
{
    UNSIGNED8   bus_id;
    ENUM8       link_type;
    STRING32    link_name;
}   TNM_LINK_DESCRIPTOR;

/* --------------------------------------------------------------------------
 *  Station Service - Read_Links_Descriptor
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Links_Descriptor 2
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
}   TNM_MSG_Call_Read_Links_Descriptor;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Links_Descriptor \
    (4 + TCN_SIZEOF_STRUCT_TNM_LINK_DESCRIPTOR)
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED16  nr_links;
    TNM_LINK_DESCRIPTOR \
                links_descriptor[1]; /* dynamic size */
}   TNM_MSG_Reply_Read_Links_Descriptor;

/* --------------------------------------------------------------------------
 *  MVB Link Service - Read_Mvb_Status
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Mvb_Status 4
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED8   bus_id;
    WORD8       reserved1;
}   TNM_MSG_Call_Read_Mvb_Status;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Mvb_Status 84
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED8   bus_id;
    WORD8       reserved1;
    /* see SV_MVB_STATUS */
    WORD16      reserved2;
    WORD16      device_address;
    STRING32    mvb_hardware_name;
    STRING32    mvb_software_name;
    BITSET16    device_status;
    WORD8       reserved3;
    UNSIGNED8   t_ignore;
    UNSIGNED32  lineA_errors;
    UNSIGNED32  lineB_errors;
}   TNM_MSG_Reply_Read_Mvb_Status;

/* --------------------------------------------------------------------------
 *  MVB Link Service - Write_Mvb_Control
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Mvb_Control 10
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED8   bus_id;
    WORD8       reserved1;
    /* see SV_MVB_CONTROL */
    WORD16      device_address;
    WORD8       reserved2;
    WORD8       t_ignore;
    WORD8       reserved3;
    BITSET8     command;
}   TNM_MSG_Call_Write_Mvb_Control;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Mvb_Control 2
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
}   TNM_MSG_Reply_Write_Mvb_Control;

/* --------------------------------------------------------------------------
 *  MVB Link Service - Read_Mvb_Devices
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Mvb_Devices 4
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED8   bus_id;
    WORD8       reserved1;
}   TNM_MSG_Call_Read_Mvb_Devices;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Mvb_Devices \
    (8 + TCN_SIZEOF_STRUCT_SV_MVB_DEVICES_LIST)
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED8   bus_id;
    WORD8       reserved1;
    WORD16      device_address;
    /* see SV_MVB_DEVICES */
    UNSIGNED16  nr_devices;
    SV_MVB_DEVICES_LIST \
                devices_list[1]; /* dynamic size */
}   TNM_MSG_Reply_Read_Mvb_Devices;

/* --------------------------------------------------------------------------
 *  MVB Link Service - Write_Mvb_Administrator
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Mvb_Administrator 8
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED8   bus_id;
    WORD8       reserved1;
    WORD16      ba_struct[2]; /* dynamic size */
}   TNM_MSG_Call_Write_Mvb_Administrator;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Mvb_Administrator 2
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
}   TNM_MSG_Reply_Write_Mvb_Administrator;

/* --------------------------------------------------------------------------
 *  Variables Service - port object
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_PORT_OBJECT 4
#ifdef TCN_LE
    typedef struct /* little-endian representation */
    {
        BITFIELD16  port_size       : 8;
        BITFIELD16  port_config     : 4;
        BITFIELD16  f_code          : 4;
        BITFIELD16  port_address    : 12;
        BITFIELD16  bus_id          : 4;
    }   TNM_PORT_OBJECT;
#else /* #ifdef TCN_LE */
    typedef struct /* big-endian representation */
    {
        BITFIELD16  bus_id          : 4;
        BITFIELD16  port_address    : 12;
        BITFIELD16  f_code          : 4;
        BITFIELD16  port_config     : 4;
        BITFIELD16  port_size       : 8;
    }   TNM_PORT_OBJECT;
#endif /* #else */

#define TNM_PORT_OBJECT_F_CODE_PD16     0 /*   16-bit port                 */
#define TNM_PORT_OBJECT_F_CODE_PD32     1 /*   32-bit port                 */
#define TNM_PORT_OBJECT_F_CODE_PD64     2 /*   64-bit port                 */
#define TNM_PORT_OBJECT_F_CODE_PD128    3 /*  128-bit port                 */
#define TNM_PORT_OBJECT_F_CODE_PD256    4 /*  256-bit port                 */
#define TNM_PORT_OBJECT_F_CODE_PD512    5 /*  512-bit port                 */
#define TNM_PORT_OBJECT_F_CODE_PD1024   6 /* 1024-bit port                 */
#define TNM_PORT_OBJECT_F_CODE_PWD      7 /* variable size port (e.g. WTB) */

#define TNM_PORT_OBJECT_PORT_CONFIG_SRC 0x8 /* port enabled as publisher   */
                                            /* (source)                    */
#define TNM_PORT_OBJECT_PORT_CONFIG_SNK 0x4 /* port enabled as subscriber  */
                                            /* (sink)                      */
#define TNM_PORT_OBJECT_PORT_CONFIG_TWC 0x2 /* port transfer frames with   */
                                            /* checksum                    */
#define TNM_PORT_OBJECT_PORT_CONFIG_FRC 0x8 /* port forced to force value  */

/* --------------------------------------------------------------------------
 *  Variables Service - Read_Ports_Configuration
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Ports_Configuration 4
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED8   bus_id;
    WORD8       reserved1;
}   TNM_MSG_Call_Read_Ports_Configuration;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Ports_Configuration \
    (4 + TCN_SIZEOF_STRUCT_TNM_PORT_OBJECT)
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    UNSIGNED16  nr_ports;
    TNM_PORT_OBJECT \
                ports_list[1]; /* dynamic size */
}   TNM_MSG_Reply_Read_Ports_Configuration;

/* --------------------------------------------------------------------------
 *  Messages Service - Read_Messenger_Status
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Messenger_Status 2
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
}   TNM_MSG_Call_Read_Messenger_Status;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Messenger_Status 64
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    /* see AM_MESSENGER_STATUS */
    STRING32    messenger_name;
    UNSIGNED8   send_time_out;
    UNSIGNED8   alive_time_out;
    UNSIGNED8   ack_time_out;
    UNSIGNED8   credit;
    WORD8       reserved2;
    UNSIGNED8   packet_size;
    UNSIGNED8   instances;
    UNSIGNED8   multicast_window;
    UNSIGNED32  messages_sent;
    UNSIGNED32  messages_received;
    UNSIGNED32  packets_sent;
    UNSIGNED32  packet_retries;
    UNSIGNED32  multicast_retries;
}   TNM_MSG_Reply_Read_Messenger_Status;

/* --------------------------------------------------------------------------
 *  Messages Service - Write_Messenger_Control
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Messenger_Control 44
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    /* see AM_MESSENGER_CONTROL */
    STRING32    messenger_name;
    UNSIGNED8   send_time_out;
    UNSIGNED8   alive_time_out;
    UNSIGNED8   ack_time_out;
    UNSIGNED8   credit;
    WORD8       reserved2;
    UNSIGNED8   packet_size;
    BITSET8     clear_counter;
    UNSIGNED8   multicast_window;
}   TNM_MSG_Call_Write_Messenger_Control;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Messenger_Control 2
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
}   TNM_MSG_Reply_Write_Messenger_Control;

/* --------------------------------------------------------------------------
 *  Messages Service - Read_Function_Directory
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Function_Directory 2
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
}   TNM_MSG_Call_Read_Function_Directory;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Function_Directory \
    (4 + TCN_SIZEOF_STRUCT_AM_DIR_ENTRY)
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD8       reserved1;
    UNSIGNED8   nr_functions;
    AM_DIR_ENTRY \
                function_list[1]; /* dynamic size */
}   TNM_MSG_Reply_Read_Function_Directory;

/* --------------------------------------------------------------------------
 *  Messages Service - Write_Function_Directory
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Function_Directory \
    (4 + TCN_SIZEOF_STRUCT_AM_DIR_ENTRY)
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    ENUM8       clear_directory;
    UNSIGNED8   nr_functions;
    AM_DIR_ENTRY \
                function_list[1]; /* dynamic size */
}   TNM_MSG_Call_Write_Function_Directory;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Function_Directory 2
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
}   TNM_MSG_Reply_Write_Function_Directory;

/* --------------------------------------------------------------------------
 *  File System Service - System Error Numbers
 *  NOTE: See also POSIX header file 'errno.h'.
 * --------------------------------------------------------------------------
 */
#define TNM_FS_ENOERR           0   /* no error                            */
#define TNM_FS_EPERM            1   /* operation not permitted             */
#define TNM_FS_ENOENT           2   /* no such file or directory           */
#define TNM_FS_ESRCH            3   /* no such process                     */
#define TNM_FS_EINTR            4   /* interrupted function call           */
#define TNM_FS_EIO              5   /* input/output error                  */
#define TNM_FS_ENXIO            6   /* no such device or address           */
#define TNM_FS_E2BIG            7   /* argument list too long              */
#define TNM_FS_ENOEXEC          8   /* executable file format error        */
#define TNM_FS_EBADF            9   /* bad file descriptor                 */
#define TNM_FS_ECHILD           10  /* no child process                    */
#define TNM_FS_EAGAIN           11  /* resource temporarily unavailable    */
#define TNM_FS_ENOMEM           12  /* not enough space                    */
#define TNM_FS_EACCES           13  /* permission denied                   */
#define TNM_FS_EFAULT           14  /* bad address                         */
#define TNM_FS_EBUSY            16  /* resource busy                       */
#define TNM_FS_EEXIST           17  /* file exists                         */
#define TNM_FS_EXDEV            18  /* improper link                       */
#define TNM_FS_ENODEV           19  /* no such device                      */
#define TNM_FS_ENOTDIR          20  /* not a directory                     */
#define TNM_FS_EISDIR           21  /* is a directory                      */
#define TNM_FS_EINVAL           22  /* invalid argument                    */
#define TNM_FS_ENFILE           23  /* too many files open in system       */
#define TNM_FS_EMFILE           24  /* too many open files                 */
#define TNM_FS_ENOTTY           25  /* inappropriate I/O control operation */
#define TNM_FS_ETXTBSY          26  /* text file busy                      */
#define TNM_FS_EFBIG            27  /* file too large                      */
#define TNM_FS_ENOSPC           28  /* no space left on a device           */
#define TNM_FS_ESPIPE           29  /* invalid seek                        */
#define TNM_FS_EROFS            30  /* read-only file system               */
#define TNM_FS_EMLINK           31  /* too many links                      */
#define TNM_FS_EPIPE            32  /* broken pipe                         */
#define TNM_FS_ERANGE           34  /* result too large or too small       */
#define TNM_FS_ENOSYS           88  /* function not implemented            */
#define TNM_FS_ENOTEMPTY        90  /* directory not empty                 */
#define TNM_FS_ENAMETOOLONG     91  /* filename too long                   */
#define TNM_FS_ELOOP            92  /* too many levels of symbolic links   */
#define TNM_FS_ENOTSUP          134 /* not supported                       */
#define TNM_FS_EOVERFLOW        139 /* value too large to be stored in     */
                                    /*  data type                          */

/* --------------------------------------------------------------------------
 *  File System Service - File Mode Bits
 *  NOTE: See also POSIX header file 'sys/stat.h'.
 *  Used for:
 *  - element 'mode'      of 'TNM_MSG_Call_FS_open'
 *  - element 'mode'      of 'TNM_MSG_Call_FS_mkdir'
 *  - element 'stat_mode' of 'TNM_MSG_Reply_FS_stat'
 * --------------------------------------------------------------------------
 */
/* file permissions by owner */
#define TNM_FS_S_IRUSR  0000400 /* read permission by owner             */
#define TNM_FS_S_IWUSR  0000200 /* write permission by owner            */
#define TNM_FS_S_IXUSR  0000100 /* execute/search permission by owner   */
#define TNM_FS_S_IRWXU  0000700 /* read, write, execute/search          */
                                /* permissions by owner                 */
#define TNM_FS_S_IREAD  0000400 /* read permission by owner             */
#define TNM_FS_S_IWRITE 0000200 /* write permission by owner            */
#define TNM_FS_S_IEXEC  0000100 /* execute/search permission by owner   */
/* file permissions by group */
#define TNM_FS_S_IRGRP  0000040 /* read permission by group             */
#define TNM_FS_S_IWGRP  0000020 /* write permission by group            */
#define TNM_FS_S_IXGRP  0000010 /* execute/search permission by group   */
#define TNM_FS_S_IRWXG  0000070 /* read, write, execute/search          */
                                /* permission by group                  */
/* file permissions by others */
#define TNM_FS_S_IROTH  0000004 /* read permission by others            */
#define TNM_FS_S_IWOTH  0000002 /* write permission by others           */
#define TNM_FS_S_IXOTH  0000001 /* execute/search permission by others  */
#define TNM_FS_S_IRWXO  0000007 /* read, write, execute/search          */
                                /* permission by others                 */
/* miscellaneous mode bits */
#define TNM_FS_S_ISUID  0004000 /* set user  ID on execution            */
#define TNM_FS_S_ISGID  0002000 /* set group ID on execution            */
#define TNM_FS_S_ISVTX  0001000 /* on directories, restricted deletion  */
                                /* flag                                 */

/* --------------------------------------------------------------------------
 *  File System Service - File System Types
 *  Used for:
 *  - element 'fs_type' of 'TNM_MSG_Call_FS_open'
 *  - element 'fs_type' of 'TNM_MSG_Reply_FS_stat'
 * --------------------------------------------------------------------------
 */
#define TNM_FS_TYPE_ANY         0x00000000 /* any file system (default)    */
#define TNM_FS_TYPE_TFS         0x00000001 /* TFS file system              */

/* --------------------------------------------------------------------------
 *  File System Service - File System Specific File Attributes
 *  (see also file 'tfs.h' of MicroMonitor)
 *  Used for:
 *  - element 'fs_attr' of 'TNM_MSG_Call_FS_open'
 *  - element 'fs_attr' of 'TNM_MSG_Reply_FS_stat'
 * --------------------------------------------------------------------------
 */
#define TNM_FS_ATTR_TFS_EXEC    0x00000001 /* executable script file       */
#define TNM_FS_ATTR_TFS_BRUN    0x00000002 /* file is to be automatically  */
                                           /*  run at boot time            */
#define TNM_FS_ATTR_TFS_QRYBRUN 0x00000004 /* file is to be automatically  */
                                           /*  run at boot time, after     */
                                           /*  querying user               */
#define TNM_FS_ATTR_TFS_SYMLINK 0x00000008 /* symbolic link file           */
#define TNM_FS_ATTR_TFS_EBIN    0x00000010 /* executable binary file,      */
                                           /*  i.e. ELF file format        */
#define TNM_FS_ATTR_TFS_CPRS    0x00000040 /* file is compressed           */
#define TNM_FS_ATTR_TFS_IPMOD   0x00000080 /* file is in-place-modifiable  */
#define TNM_FS_ATTR_TFS_UNREAD  0x00000100 /* file is not readable when    */
                                           /*  monitor is below required   */
                                           /*  user level                  */
#define TNM_FS_ATTR_TFS_ULVLMSK 0x00000600 /* user level mask defines      */
                                           /* 4 access levels:             */
#define TNM_FS_ATTR_TFS_ULVL0   0x00000000 /* - user level 0               */
#define TNM_FS_ATTR_TFS_ULVL1   0x00000200 /* - user level 1               */
#define TNM_FS_ATTR_TFS_ULVL2   0x00000400 /* - user level 2               */
#define TNM_FS_ATTR_TFS_ULVL3   0x00000600 /* - user level 3               */

/* --------------------------------------------------------------------------
 *  File System Service - FS_access
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_access 16
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   amode;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  path[4]; /* dynamic size */
}   TNM_MSG_Call_FS_access;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_access 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   status;
}   TNM_MSG_Reply_FS_access;

/* Symbolic constants for element 'amode' of 'TNM_MSG_Call_FS_access'.     */
/* NOTE: See also file 'unistd.h'.                                         */
#define TNM_FS_R_OK 1 /* test for read permission             */
#define TNM_FS_W_OK 2 /* test for write permission            */
#define TNM_FS_X_OK 4 /* test for execute (search) permission */
#define TNM_FS_F_OK 8 /* test for existence of file           */

/* --------------------------------------------------------------------------
 *  File System Service - FS_open
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_open 32
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   oflag;
    UNSIGNED32  mode;
    UNSIGNED32  file_size;
    ENUM32      fs_type;
    BITSET32    fs_attr;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  path[4]; /* dynamic size */
}   TNM_MSG_Call_FS_open;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_open 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   fd;
}   TNM_MSG_Reply_FS_open;

/* Symbolic constants for element 'oflag' of 'TNM_MSG_Call_FS_open'.       */
/* NOTE: See also POSIX header file 'fnctl.h'.                             */
/* file access modes */
#define TNM_FS_O_ACCMODE  0x00000003 /* mask for file access modes         */
#define TNM_FS_O_RDONLY   0x00000001 /* open for reading only              */
#define TNM_FS_O_WRONLY   0x00000002 /* open for writing only              */
#define TNM_FS_O_RDWR     0x00000003 /* open for reading and writing       */
/* file mode flags */
#define TNM_FS_O_CREAT    0x00000008 /* create file if it does not exist   */
#define TNM_FS_O_EXCL     0x00000010 /* exclusive use flag                 */
#define TNM_FS_O_NOCTTY   0x00000020 /* do not assign controlling terminal */
#define TNM_FS_O_TRUNC    0x00000040 /* truncate flag (i.e. truncate       */
                                     /*  length of existing file to 0)     */
/* file status flags */
#define TNM_FS_O_APPEND   0x00000080 /* set append mode                    */
#define TNM_FS_O_DSYNC    0x00000100 /* write according to synchronised    */
                                     /*  I/O data integrity completion     */
#define TNM_FS_O_NONBLOCK 0x00000200 /* non-blocking mode                  */
#define TNM_FS_O_RSYNC    0x00000400 /* synchronised read I/O operations   */
#define TNM_FS_O_SYNC     0x00000800 /* write according to synchronised    */
                                     /*  I/O file integrity completion     */

/* --------------------------------------------------------------------------
 *  File System Service - FS_close
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_close 8
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   fd;
}   TNM_MSG_Call_FS_close;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_close 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   status;
}   TNM_MSG_Reply_FS_close;

/* --------------------------------------------------------------------------
 *  File System Service - FS_read
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_read 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   fd;
    INTEGER32   nbyte;
}   TNM_MSG_Call_FS_read;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_read 16
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   nbyte;
    WORD8       buf[4]; /* dynamic size */
}   TNM_MSG_Reply_FS_read;

/* --------------------------------------------------------------------------
 *  File System Service - FS_write
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_write 16
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   fd;
    INTEGER32   nbyte;
    WORD8       buf[4]; /* dynamic size */
}   TNM_MSG_Call_FS_write;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_write 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   nbyte;
}   TNM_MSG_Reply_FS_write;

/* --------------------------------------------------------------------------
 *  File System Service - FS_lseek
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_lseek 16
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   fd;
    UNSIGNED32  offset;
    INTEGER32   whence;
}   TNM_MSG_Call_FS_lseek;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_lseek 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    UNSIGNED32  offset;
}   TNM_MSG_Reply_FS_lseek;

/* Symbolic constants for element 'whence' of 'TNM_MSG_Call_FS_lseek'.     */
/* NOTE: See also POSIX header file 'unistd.h'.                            */
#define TNM_FS_SEEK_SET 0 /* set file offset to 'offset' bytes             */
#define TNM_FS_SEEK_CUR 1 /* set file offset to its current location plus  */
                          /*  'offset'                                     */
#define TNM_FS_SEEK_END 2 /* set file offset to the size of the file plus  */
                          /*  'offset'                                     */

/* --------------------------------------------------------------------------
 *  File System Service - FS_unlink
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_unlink 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  path[4]; /* dynamic size */
}   TNM_MSG_Call_FS_unlink;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_unlink 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   status;
}   TNM_MSG_Reply_FS_unlink;

/* --------------------------------------------------------------------------
 *  File System Service - FS_stat
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_stat 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  path[4]; /* dynamic size */
}   TNM_MSG_Call_FS_stat;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_stat 84
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   status;
    ENUM32      fs_type;
    BITSET32    fs_attr;
    UNSIGNED32  stat_dev;
    UNSIGNED32  stat_ino_high;
    UNSIGNED32  stat_ino_low;
    UNSIGNED32  stat_mode;
    UNSIGNED32  stat_nlink;
    UNSIGNED32  stat_uid;
    UNSIGNED32  stat_gid;
    WORD32      stat_reserved1;
    UNSIGNED32  stat_size_high;
    UNSIGNED32  stat_size_low;
    UNSIGNED32  stat_atime;
    UNSIGNED32  stat_mtime;
    UNSIGNED32  stat_ctime;
    WORD32      stat_reserved2;
    WORD32      stat_reserved3;
    WORD32      stat_reserved4;
}   TNM_MSG_Reply_FS_stat;

/* Symbolic constants for element 'stat_mode' of 'TNM_MSG_Call_FS_stat'.   */
/* NOTE: See also POSIX header file 'sys/stat.h'.                          */
#define TNM_FS_S_IFMT   0170000 /* file type mask    */
#define TNM_FS_S_IFREG  0100000 /* regular           */
#define TNM_FS_S_IFDIR  0040000 /* directory         */
#define TNM_FS_S_IFLNK  0120000 /* symbolic link     */
#define TNM_FS_S_IFBLK  0060000 /* block special     */
#define TNM_FS_S_IFCHR  0020000 /* character special */
#define TNM_FS_S_IFIFO  0010000 /* FIFO special      */
#define TNM_FS_S_IFSOCK 0140000 /* socket            */

/* Macros to test wether a file is of the specified type.                  */
/* Used for element 'stat_mode' of TNM_MSG_Reply_FS_stat.                  */
/* NOTE: See also POSIX header file 'sys/stat.h'.                          */
/* test for a regular file */
#define TNM_FS_S_ISREG(m)   (((m) & TNM_FS_S_IFMT) == TNM_FS_S_IFREG)
/* test for a directory */
#define TNM_FS_S_ISDIR(m)   (((m) & TNM_FS_S_IFMT) == TNM_FS_S_IFDIR)
/* test for a symbolic link */
#define TNM_FS_S_ISLNK(m)   (((m) & TNM_FS_S_IFMT) == TNM_FS_S_IFLNK)
/* test for a character special file */
#define TNM_FS_S_ISBLK(m)   (((m) & TNM_FS_S_IFMT) == TNM_FS_S_IFBLK)
/* test for a pipe of FIFO special file */
#define TNM_FS_S_ISCHR(m)   (((m) & TNM_FS_S_IFMT) == TNM_FS_S_IFCHR)
/* test for a block special file */
#define TNM_FS_S_ISFIFO(m)  (((m) & TNM_FS_S_IFMT) == TNM_FS_S_IFIFO)
/* test for a socket */
#define TNM_FS_S_ISSOCK(m)  (((m) & TNM_FS_S_IFMT) == TNM_FS_S_IFSOCK)

/* --------------------------------------------------------------------------
 *  File System Service - FS_pathconf
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_pathconf 16
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   name;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  path[4]; /* dynamic size */
}   TNM_MSG_Call_FS_pathconf;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_pathconf 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   value;
}   TNM_MSG_Reply_FS_pathconf;

/* Symbolic constants for element 'name' of 'TNM_MSG_Call_FS_pathconf'.    */
/* NOTE: See also POSIX header file 'unistd.h' and 'limits.h'.             */
#define TNM_FS_PC_LINK_MAX          0  /* value of LINK_MAX                */
#define TNM_FS_PC_MAX_CANON         1  /* value of MAX_CANON               */
#define TNM_FS_PC_MAX_INPUT         2  /* value of MAX_INPUT               */
#define TNM_FS_PC_NAME_MAX          3  /* value of NAME_MAX                */
#define TNM_FS_PC_PATH_MAX          4  /* value of PATH_MAX                */
#define TNM_FS_PC_PIPE_BUF          5  /* value of PIPE_BUF                */
#define TNM_FS_PC_CHOWN_RESTRICTED  6  /* value of _POSIX_CHOWN_RESTRICTED */
#define TNM_FS_PC_NO_TRUNC          7  /* value of _POSIX_NO_TRUNC         */
#define TNM_FS_PC_VDISABLE          8  /* value of _POSIX_VDISABLE         */
#define TNM_FS_PC_ASYNC_IO          9  /* value of _POSIX_ASYNC_IO         */
#define TNM_FS_PC_PRIO_IO           10 /* value of _POSIX_PRIO_IO          */
#define TNM_FS_PC_SYNC_IO           11 /* value of _POSIX_SYNC_IO          */

/* --------------------------------------------------------------------------
 *  File System Service - FS_getcwd
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_getcwd 8
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    WORD16      reserved2;
    UNSIGNED16  size;
}   TNM_MSG_Call_FS_getcwd;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_getcwd 16
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    WORD16      reserved2;
    UNSIGNED16  size;
    CHARACTER8  buf[4]; /* dynamic size */
}   TNM_MSG_Reply_FS_getcwd;

/* --------------------------------------------------------------------------
 *  File System Service - FS_chdir
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_chdir 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  path[4]; /* dynamic size */
}   TNM_MSG_Call_FS_chdir;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_chdir 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   status;
}   TNM_MSG_Reply_FS_chdir;

/* --------------------------------------------------------------------------
 *  File System Service - FS_mkdir
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_mkdir 16
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    UNSIGNED32  mode;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  path[4]; /* dynamic size */
}   TNM_MSG_Call_FS_mkdir;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_mkdir 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   status;
}   TNM_MSG_Reply_FS_mkdir;

/* --------------------------------------------------------------------------
 *  File System Service - FS_rmdir
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_rmdir 16
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   mode;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  path[4]; /* dynamic size */
}   TNM_MSG_Call_FS_rmdir;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_rmdir 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   status;
}   TNM_MSG_Reply_FS_rmdir;

/* --------------------------------------------------------------------------
 *  File System Service - FS_opendir
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_opendir 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  dirname[4]; /* dynamic size */
}   TNM_MSG_Call_FS_opendir;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_opendir 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   dirp;
}   TNM_MSG_Reply_FS_opendir;

/* --------------------------------------------------------------------------
 *  File System Service - FS_closedir
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_closedir 8
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   dirp;
}   TNM_MSG_Call_FS_closedir;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_closedir 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   status;
}   TNM_MSG_Reply_FS_closedir;

/* --------------------------------------------------------------------------
 *  File System Service - FS_readdir
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_readdir 8
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   dirp;
}   TNM_MSG_Call_FS_readdir;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_readdir 16
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    WORD16      reserved2;
    UNSIGNED16  string_size;
    CHARACTER8  d_name[4]; /* dynamic size */
}   TNM_MSG_Reply_FS_readdir;

/* --------------------------------------------------------------------------
 *  File System Service - FS_rewinddir
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_rewinddir 8
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   dirp;
}   TNM_MSG_Call_FS_rewinddir;

#define TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_rewinddir 12
typedef struct
{
    ENUM8       tnm_key;
    ENUM8       sif_code;
    WORD16      reserved1;
    INTEGER32   sys_errno;
    INTEGER32   status;
}   TNM_MSG_Reply_FS_rewinddir;


#endif /* #ifndef TCN_TNM_H */
