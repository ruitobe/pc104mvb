#ifndef TCN_CM_H
#define TCN_CM_H

/* ==========================================================================
 *
 *  File      : TCN_CM.H
 *
 *  Purpose   : Configuration Manager
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
 *  Constants and Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Enumerated Type   : CM_RESULT
 *
 *  Purpose           : Result of a procedure.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    CM_OK           = 0,  /* correct termination                           */
    CM_PRT_PASSIVE  = 1,  /* port (dataset) not active                     */
    CM_ERROR        = 2,  /* unspecified error                             */
    CM_CONFIG       = 3,  /* configuration error                           */
    CM_MEMORY       = 4,  /* not enough memory                             */
    CM_UNKNOW_LINK  = 5,  /* unknown link layer (e.g. not connected)       */
    CM_RANGE        = 6,  /* memory address error                          */
    CM_DATA_TYPE    = 7,  /* unsupported data type                         */
    CM_FILE_ACCESS  = 8,  /* file access error (see value of 'errno')      */
    CM_MAX_ERR      = 15  /* highest system code; configuration structure  */
                          /* specific codes are higher than 15             */
}   CM_RESULT;


/* --------------------------------------------------------------------------
 *  Enumerated Type   : CM_CONFIG_TYPE
 *
 *  Purpose           : Type of a configuration structure.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    CM_CONFIG_TYPE_D2000        = 1,    /* D2000 configuration tool        */
    CM_CONFIG_TYPE_D2000_SERVER = 2     /* D2000 configuration tool;       */
                                        /*  compatible with mode 'SERVER'  */
}   CM_CONFIG_TYPE;


/* ==========================================================================
 *
 *  Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : cm_init
 *
 *  Purpose   : Initialises the configuration manager.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              CM_RESULT
 *              cm_init (void);
 *
 *  Return    : any CM_RESULT
 *
 *  Remarks   : - This procedure has to be called at system initialisation
 *                before calling any other 'cm_xxx' procedure.
 *              - This procedure shall be called only one time.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
CM_RESULT
cm_init (void);


/* --------------------------------------------------------------------------
 *  Procedure : cm_config_device
 *
 *  Purpose   : Configures a device from a configuration structure.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              CM_RESULT
 *              cm_config_device
 *              (
 *                  ENUM16      config_type,
 *                  CHARACTER8  *path,
 *                  void        *p_parameter
 *              );
 *
 *  Input     : config_type - any CM_CONFIG_TYPE
 *              path        - points to a pathname naming the file (includes
 *                            the terminating null byte)
 *              p_parameter - pointer to configuration structure specific
 *                            parameter (structured type);
 *                            NULL -> a configuration structure specific
 *                                    default processing algorithm is used
 *
 *  Return    : any CM_RESULT
 *
 *  Remarks   : - The file will be opened by POSIX library procedure 'open'
 *                with oflag=O_RDONLY (see also 'Operating System
 *                Libraries').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
CM_RESULT
cm_config_device
(
    ENUM16      config_type,
    CHARACTER8  *path,
    void        *p_parameter
);


/* ==========================================================================
 *
 *  Configuration Structure 'D2000'
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Enumerated Type   : CM_D2000_ENTRY
 *
 *  Purpose           : Identifier of a D2000 configuration structure entry.
 * --------------------------------------------------------------------------
 */
typedef enum                                    /* NOTES:                  */
{                                               /* ------                  */
    CM_D2000_ENTRY_MD_CONTROL           = 0x28, /* am_announce_device      */
    CM_D2000_ENTRY_FUNCTION_DIRECTORY   = 0x2B, /* am_insert_dir_entries   */
    CM_D2000_ENTRY_MVB_CONTROL          = 0x0B, /* MVB control object      */
    CM_D2000_ENTRY_MVB_ADMINISTRATOR    = 0x0D, /* MVB administrator obj.  */
    CM_D2000_ENTRY_PORTS_CONFIGURATION  = 0x1F  /* ap_port_manage          */
}   CM_D2000_ENTRY;


/* --------------------------------------------------------------------------
 *  Enumerated Type   : CM_D2000_STATUS
 *
 *  Purpose           : Processing status of a D2000 configuration structure
 *                      entry.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    CM_D2000_STATUS_OK                  = 0x00,
    CM_D2000_STATUS_FAILED              = 0x01,
    CM_D2000_STATUS_UNDEFINED           = 0xFF
}   CM_D2000_STATUS;


/* --------------------------------------------------------------------------
 *  Structured Type   : CM_D2000_PARAMETER (CM_D2000_ENTRIES_LIST)
 *
 *  Purpose           : D2000 configuration structure specific parameter
 *                      (identifies a list of entries to be processed).
 *
 *  Syntax            : typedef struct
 *                      {
 *                          ENUM8       entry;
 *                          ENUM8       bus_id;
 *                          ENUM16      status;
 *                      }   CM_D2000_ENTRIES_LIST;
 *
 *                      typedef struct
 *                      {
 *                          UNSIGNED16              nr_entries;
 *                          WORD16                  reserved;
 *                          CM_D2000_ENTRIES_LIST   entries_list[1];
 *                                                  (dynamic size)
 *                      }   CM_D2000_PARAMETER;
 *
 *  Element           : entry         - entry identifier (any CM_D2000_ENTRY)
 *                      bus_id        - bus identifier (0..15)
 *                      status        - entry processing status
 *                                      (any CM_D2000_STATUS)
 *
 *                      nr_entries    - number of entries in this list
 *                      reserved      - reserved (=0)
 *                      entries_list  - list of configuration structure
 *                                      entries
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_CM_D2000_ENTRIES_LIST 4
typedef struct
{
    ENUM8       entry;
    ENUM8       bus_id;
    ENUM8       status;
    WORD8       reserved;
}   CM_D2000_ENTRIES_LIST;

#define TCN_SIZEOF_STRUCT_CM_D2000_PARAMETER \
    (4 + TCN_SIZEOF_STRUCT_CM_D2000_ENTRIES_LIST)
typedef struct
{
    UNSIGNED16              nr_entries;
    WORD16                  reserved;
    CM_D2000_ENTRIES_LIST   entries_list[1]; /* dynamic size */
}   CM_D2000_PARAMETER;


#endif /* #ifndef TCN_CM_H */
