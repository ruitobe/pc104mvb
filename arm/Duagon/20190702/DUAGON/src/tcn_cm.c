/* ==========================================================================
 *
 *  File      : TCN_CM.C
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
 *  Pre-processor Definitions:
 *  --------------------------
 *  - TCN_CM_PRINT    - debug printouts
 *  - TCN_MD          - support message data
 *  - TCN_CM_SIM      - simulate (bypass any TCN Driver API procedure)
 *  - TCN_CM_TEST     - test application (requires TCN_CM_SIM)
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
#include <tcn_osl.h>
#include <tcn_cm.h>
#include <tcn_sv.h>
#include <tcn_sv_m.h>
#include <tcn_as.h>
#include <tcn_pd.h>
#include <tcn_ap.h>
#ifdef TCN_MD
#   include <tcn_am.h>
#endif


/* ==========================================================================
 *
 *  Local Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : tcn_cm_fs_read
 *
 *  Purpose   : Read from a file.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              CM_RESULT
 *              tcn_cm_fs_read
 *              (
 *                  int         fd,
 *                  void        *buf,
 *                  INTEGER32   nbyte
 *              );
 *
 *  Input     : fd    - file descriptor
 *              buf   - pointer to memory buffer where to put the data bytes
 *                      read from the file
 *              nbyte - max. number of bytes to be read from file
 *
 *  Return    : any CM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
TCN_DECL_INLINE
CM_RESULT
tcn_cm_fs_read
(
    int         fd,
    void        *buf,
    INTEGER32   nbyte
)
{
    CM_RESULT   cm_result = CM_OK;

    if ((INTEGER32)read(fd, buf, (unsigned int)nbyte) != (INTEGER32)nbyte)
    {
        cm_result = CM_FILE_ACCESS;
    } /* if ((INTEGER32)read(fd, buf, (unsigned int)nbyte) != ...) */

    return(cm_result);

} /* tcn_cm_fs_read */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_cm_fs_lseek
 *
 *  Purpose   : Move the file offset.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              CM_RESULT
 *              tcn_cm_fs_lseek
 *              (
 *                  int         fd,
 *                  UNSIGNED32  offset
 *              );
 *
 *  Input     : fd     - file descriptor
 *              offset - the file offset shall be set to 'offset' bytes
 *
 *  Return    : any CM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
TCN_DECL_INLINE
CM_RESULT
tcn_cm_fs_lseek
(
    int         fd,
    UNSIGNED32  offset
)
{
    CM_RESULT   cm_result = CM_OK;

    if ((UNSIGNED32)lseek(fd, offset, SEEK_SET) != offset)
    {
        cm_result = CM_FILE_ACCESS;
    } /* if ((UNSIGNED32)lseek(fd, offset, SEEK_SET) != offset) */

    return(cm_result);

} /* tcn_cm_fs_lseek */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_cm_config_type_d2000_entry_md_control
 *
 *  Purpose   : Configures a device from a D2000 configuration structure
 *              - process entry 'MD_Control'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              CM_RESULT
 *              tcn_cm_config_type_d2000_entry_md_control
 *              (
 *                  int                     fd,
 *                  CM_D2000_ENTRIES_LIST   *p_entries_list
 *              );
 *
 *  Input     : fd             - file descriptor
 *              p_entries_list - pointer to list entry of D2000
 *                               configuration structure specific
 *                               parameter (entry to be processed)
 *
 *  Return    : any CM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
CM_RESULT
tcn_cm_config_type_d2000_entry_md_control
(
    int                     fd,
    CM_D2000_ENTRIES_LIST   *p_entries_list
)
{
    CM_RESULT   cm_result             = CM_OK;
    WORD8       value8;
    UNSIGNED16  max_call_number       = 0;
    UNSIGNED16  max_inst_number       = 0;
    UNSIGNED16  default_reply_timeout = 0;
    UNSIGNED8   my_credit             = 0;
#ifdef TCN_MD
#ifndef TCN_CM_SIM
    AM_RESULT   am_result;
#endif /* #ifndef TCN_CM_SIM */
#endif /* #ifdef TCN_MD */

#ifndef TCN_MD
    /* avoid warnings */
    p_entries_list = p_entries_list;
#endif /* #ifndef TCN_MD */

#ifdef TCN_CM_PRINT
    TCN_OSL_PRINTF("  - entry 'MD_Control'\n");
#endif /* #ifdef TCN_CM_PRINT */

    /* read entry value 'max_call_number' */
    cm_result = tcn_cm_fs_read(fd, &max_call_number, 2);
    if (CM_OK == cm_result)
    {
        TCN_MACRO_CONVERT_BE16_TO_CPU(&max_call_number);
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("    - max_call_number      =0x%04X\n", \
            max_call_number);
#endif /* #ifdef TCN_CM_PRINT */
    } /* if (CM_OK == cm_result) */

    /* read entry value 'max_inst_number' */
    if (CM_OK == cm_result)
    {
        cm_result = tcn_cm_fs_read(fd, &max_inst_number, 2);
    } /* if (CM_OK == cm_result) */
    if (CM_OK == cm_result)
    {
        TCN_MACRO_CONVERT_BE16_TO_CPU(&max_inst_number);
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("    - max_inst_number      =0x%04X\n", \
            max_inst_number);
#endif /* #ifdef TCN_CM_PRINT */
    } /* if (CM_OK == cm_result) */

    /* read entry value 'default_reply_timeout' */
    if (CM_OK == cm_result)
    {
        cm_result = tcn_cm_fs_read(fd, &default_reply_timeout, 2);
    } /* if (CM_OK == cm_result) */
    if (CM_OK == cm_result)
    {
        TCN_MACRO_CONVERT_BE16_TO_CPU(&default_reply_timeout);
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("    - default_reply_timeout=0x%04X\n", \
            default_reply_timeout);
#endif /* #ifdef TCN_CM_PRINT */
    } /* if (CM_OK == cm_result) */

    /* read entry value 'reserved1' */
    if (CM_OK == cm_result)
    {
        cm_result = tcn_cm_fs_read(fd, &value8, 1);
    } /* if (CM_OK == cm_result) */

    /* read entry value 'my_credit' */
    if (CM_OK == cm_result)
    {
        cm_result = tcn_cm_fs_read(fd, &my_credit, 1);
    } /* if (CM_OK == cm_result) */
#ifdef TCN_CM_PRINT
    if (CM_OK == cm_result)
    {
        TCN_OSL_PRINTF("    - my_credit            =0x%02X\n", \
            my_credit);
    } /* if (CM_OK == cm_result) */
#endif /* #ifdef TCN_CM_PRINT */

#ifdef TCN_MD
#ifndef TCN_CM_SIM
    if (CM_OK == cm_result)
    {
        am_result =                     \
            am_announce_device          \
            (                           \
                max_call_number,        \
                max_inst_number,        \
                default_reply_timeout,  \
                my_credit               \
            );
        if (AM_OK != am_result)
        {
            cm_result = CM_ERROR;
        } /* if (AM_OK != am_result) */
    } /* if (CM_OK == cm_result) */
#endif /* #ifndef TCN_CM_SIM */

    /* set processing status of list entry */
    if (CM_OK == cm_result)
    {
        p_entries_list->status = CM_D2000_STATUS_OK;
    } /* if (CM_OK == cm_result) */

#endif /* #ifdef TCN_MD */

    return(cm_result);

} /* tcn_cm_config_type_d2000_entry_md_control */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_cm_config_type_d2000_entry_function_directory
 *
 *  Purpose   : Configures a device from a D2000 configuration structure
 *              - process entry 'Function_Directory'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              CM_RESULT
 *              tcn_cm_config_type_d2000_entry_function_directory
 *              (
 *                  int                     fd,
 *                  CM_D2000_ENTRIES_LIST   *p_entries_list
 *              );
 *
 *  Input     : fd             - file descriptor
 *              p_entries_list - pointer to list entry of D2000
 *                               configuration structure specific
 *                               parameter (entry to be processed)
 *
 *  Return    : any CM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
CM_RESULT
tcn_cm_config_type_d2000_entry_function_directory
(
    int                     fd,
    CM_D2000_ENTRIES_LIST   *p_entries_list
)
{
    CM_RESULT       cm_result       = CM_OK;
    UNSIGNED32      counter;
    ENUM8           clear_directory;
    UNSIGNED8       nr_functions    = 0;
    UNSIGNED8       function_id;
    UNSIGNED8       station_id      = 0;
#ifdef TCN_MD
#ifndef TCN_CM_SIM
    AM_RESULT       am_result;
    AM_DIR_ENTRY    function_list;
#endif /* #ifndef TCN_CM_SIM */
#endif /* #ifdef TCN_MD */

#ifndef TCN_MD
    /* avoid warnings */
    p_entries_list = p_entries_list;
#endif /* #ifndef TCN_MD */

#ifdef TCN_CM_PRINT
    TCN_OSL_PRINTF("  - entry 'Function_Directory'\n");
#endif /* #ifdef TCN_CM_PRINT */

    /* read entry value 'clear_directory' */
    cm_result = tcn_cm_fs_read(fd, &clear_directory, 1);
#ifdef TCN_CM_PRINT
    if (CM_OK == cm_result)
    {
        TCN_OSL_PRINTF("    - clear_directory=0x%02X\n", clear_directory);
    } /* if (CM_OK == cm_result) */
#endif /* #ifdef TCN_CM_PRINT */

#ifdef TCN_MD
#ifndef TCN_CM_SIM
    if ((CM_OK == cm_result) && (1 == clear_directory))
    {
        am_result = am_clear_dir();
        if (AM_OK != am_result)
        {
            cm_result = CM_ERROR;
        } /* if (AM_OK != am_result) */
    } /* if ((CM_OK == cm_result) && (1 == clear_directory)) */
#endif /* #ifndef TCN_CM_SIM */
#endif /* #ifdef TCN_MD */

    /* read entry value 'nr_functions' */
    if (CM_OK == cm_result)
    {
        cm_result = tcn_cm_fs_read(fd, &nr_functions, 1);
    } /* if (CM_OK == cm_result) */
#ifdef TCN_CM_PRINT
    if (CM_OK == cm_result)
    {
        TCN_OSL_PRINTF("    - nr_functions   =0x%02X\n", nr_functions);
    } /* if (CM_OK == cm_result) */
#endif /* #ifdef TCN_CM_PRINT */

    if ((CM_OK == cm_result) && (0 < nr_functions))
    {
        counter = 0;
        while ((counter < nr_functions) && (CM_OK == cm_result))
        {
            /* read entry value 'function_id' */
            cm_result = tcn_cm_fs_read(fd, &function_id, 1);
#ifdef TCN_CM_PRINT
            if (CM_OK == cm_result)
            {
                TCN_OSL_PRINTF("      - function_id=0x%02X, ", \
                    function_id);
            } /* if (CM_OK == cm_result) */
#endif /* #ifdef TCN_CM_PRINT */

            /* read entry value 'station_id' */
            if (CM_OK == cm_result)
            {
                cm_result = tcn_cm_fs_read(fd, &station_id, 1);
            } /* if (CM_OK == cm_result) */
#ifdef TCN_CM_PRINT
            if (CM_OK == cm_result)
            {
                TCN_OSL_PRINTF("station_id=0x%02X\n", station_id);
            } /* if (CM_OK == cm_result) */
#endif /* #ifdef TCN_CM_PRINT */

#ifdef TCN_MD
#ifndef TCN_CM_SIM
            if (CM_OK == cm_result)
            {
                function_list.function = function_id;
                function_list.station  = station_id;
                am_result = am_insert_dir_entries(&function_list, 1);
                if (AM_OK != am_result)
                {
                    cm_result = CM_ERROR;
                } /* if (AM_OK != am_result) */
            } /* if (CM_OK == cm_result) */
#endif /* #ifndef TCN_CM_SIM */
#endif /* #ifdef TCN_MD */

            counter++;
        } /* while ((counter < nr_functions) && (CM_OK == cm_result)) */
    } /* if ((CM_OK == cm_result) && (0 < nr_functions)) */

#ifdef TCN_MD
    /* set processing status of list entry */
    if (CM_OK == cm_result)
    {
        p_entries_list->status = CM_D2000_STATUS_OK;
    } /* if (CM_OK == cm_result) */
#endif /* #ifdef TCN_MD */

    return(cm_result);

} /* tcn_cm_config_type_d2000_entry_function_directory */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_cm_config_type_d2000_entry_mvb_control
 *
 *  Purpose   : Configures a device from a D2000 configuration structure
 *              - process entry 'MVB_Control'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              CM_RESULT
 *              tcn_cm_config_type_d2000_entry_mvb_control
 *              (
 *                  int                     fd,
 *                  CM_D2000_ENTRIES_LIST   *p_entries_list
 *              );
 *
 *  Input     : fd             - file descriptor
 *              p_entries_list - pointer to list entry of D2000
 *                               configuration structure specific
 *                               parameter (entry to be processed)
 *
 *  Return    : any CM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
CM_RESULT
tcn_cm_config_type_d2000_entry_mvb_control
(
    int                     fd,
    CM_D2000_ENTRIES_LIST   *p_entries_list
)
{
    CM_RESULT       cm_result          = CM_OK;
    WORD8           value8;
    ENUM8           bus_id;
    SV_MVB_CONTROL  mvb_control_object;
#ifndef TCN_CM_SIM
    AS_RESULT       as_result;
    ENUM8           link_type;
    STRING32        link_name;
#endif /* #ifndef TCN_CM_SIM */

    memset((void*)&mvb_control_object, 0, sizeof(mvb_control_object));

    /* read entry value 'bus_id' */
    cm_result = tcn_cm_fs_read(fd, &bus_id, 1);

    /* check max. range of 'bus_id' */
    if ((CM_OK == cm_result) && (bus_id > as_max_links))
    {
        cm_result = CM_UNKNOW_LINK;
    } /* if ((CM_OK == cm_result) && (bus_id > as_max_links)) */

    /* check, if 'bus_id' match */
    if ((CM_OK == cm_result) && (bus_id == p_entries_list->bus_id))
    {
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("  - entry 'MVB_Control'\n");
#endif /* #ifdef TCN_CM_PRINT */

#ifndef TCN_CM_SIM
        as_result =                     \
            as_read_link_descriptor     \
            (                           \
                (ENUM8)bus_id,          \
                (ENUM8*)&link_type,     \
                (STRING32*)&link_name   \
            );
        if (AS_OK != as_result)
        {
            cm_result = CM_ERROR;
        } /* if (AS_OK != as_result) */

        if ((CM_OK == cm_result) && (SV_LINK_TYPE_MVB != link_type))
        {
            cm_result = CM_CONFIG;
        } /* if ((CM_OK == cm_result) && (SV_LINK_TYPE_MVB != link_type)) */
#endif /* #ifndef TCN_CM_SIM */

        /* read entry value 'reserved1' */
        if (CM_OK == cm_result)
        {
            cm_result = tcn_cm_fs_read(fd, &value8, 1);
        } /* if (CM_OK == cm_result) */

        /* read entry values of MVB control object */
        /* (ignore element 'reserved3')            */
        if (CM_OK == cm_result)
        {
            cm_result =                             \
                tcn_cm_fs_read                      \
                (                                   \
                    fd,                             \
                    &mvb_control_object,            \
                    (sizeof(SV_MVB_CONTROL) - 2)    \
                );
        } /* if (CM_OK == cm_result) */
        if (CM_OK == cm_result)
        {
            TCN_MACRO_CONVERT_BE16_TO_CPU           \
            (                                       \
                &mvb_control_object.device_address  \
            );
        } /* if (CM_OK == cm_result) */
#ifdef TCN_CM_PRINT
        if (CM_OK == cm_result)
        {
            TCN_OSL_PRINTF("    - device_address=0x%04X, ", \
                mvb_control_object.device_address);
            TCN_OSL_PRINTF("t_ignore=0x%02X, ", \
                mvb_control_object.t_ignore);
            TCN_OSL_PRINTF("command=0x%02X\n", \
                mvb_control_object.command);
        } /* if (CM_OK == cm_result) */
#endif /* #ifdef TCN_CM_PRINT */

#ifndef TCN_CM_SIM
        if (CM_OK == cm_result)
        {
            as_result =                                     \
                as_service_handler                          \
                (                                           \
                    (ENUM8)bus_id,                          \
                    (ENUM8)SV_MVB_SERVICE_WRITE_CONTROL,    \
                    (void*)&mvb_control_object              \
                );
            if (AS_OK != as_result)
            {
                cm_result = CM_ERROR;
            } /* if (AS_OK != as_result) */
        } /* if (CM_OK == cm_result) */
#endif /* #ifndef TCN_CM_SIM */

        /* set processing status of list entry */
        if (CM_OK == cm_result)
        {
            p_entries_list->status = CM_D2000_STATUS_OK;
        } /* if (CM_OK == cm_result) */

    } /* if ((CM_OK == cm_result) && (bus_id == p_entries_list->bus_id)) */

    return(cm_result);

} /* tcn_cm_config_type_d2000_entry_mvb_control */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_cm_config_type_d2000_entry_mvb_administrator
 *
 *  Purpose   : Configures a device from a D2000 configuration structure
 *              - process entry 'MVB_Administrator'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              CM_RESULT
 *              tcn_cm_config_type_d2000_entry_mvb_administrator
 *              (
 *                  int                     fd,
 *                  CM_D2000_ENTRIES_LIST   *p_entries_list,
 *                  UNSIGNED32              offset
 *              );
 *
 *  Input     : fd             - file descriptor
 *              p_entries_list - pointer to list entry of D2000
 *                               configuration structure specific
 *                               parameter (entry to be processed)
 *              offset         - offset from begin of file to first byte
 *                               of entry 'MVB_Administrator'.
 *
 *  Return    : any CM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
CM_RESULT
tcn_cm_config_type_d2000_entry_mvb_administrator
(
    int                     fd,
    CM_D2000_ENTRIES_LIST   *p_entries_list,
    UNSIGNED32              offset
)
{
    CM_RESULT               cm_result                 = CM_OK;
    UNSIGNED32              counter;
    WORD8                   value8;
    WORD16                  value16                   = 0;
    WORD16                  *p16;
    ENUM8                   bus_id                    = 0;
    SV_MVB_ADMINISTRATOR    *mvb_administrator_object = NULL;
#ifndef TCN_CM_SIM
    AS_RESULT               as_result;
    ENUM8                   link_type;
    STRING32                link_name;
#endif /* #ifndef TCN_CM_SIM */

    /* read entry value 'bus_id' */
    cm_result = tcn_cm_fs_read(fd, &bus_id, 1);

    /* check max. range of 'bus_id' */
    if ((CM_OK == cm_result) && (bus_id > as_max_links))
    {
        cm_result = CM_UNKNOW_LINK;
    } /* if ((CM_OK == cm_result) && (bus_id > as_max_links)) */

    /* check, if 'bus_id' match */
    if ((CM_OK == cm_result) && (bus_id == p_entries_list->bus_id))
    {
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("  - entry 'MVB_Administrator'\n");
#endif /* #ifdef TCN_CM_PRINT */

#ifndef TCN_CM_SIM
        as_result =                     \
            as_read_link_descriptor     \
            (                           \
                (ENUM8)bus_id,          \
                (ENUM8*)&link_type,     \
                (STRING32*)&link_name   \
            );
        if (AS_OK != as_result)
        {
            cm_result = CM_ERROR;
        } /* if (AS_OK != as_result) */
        if ((CM_OK == cm_result) && (SV_LINK_TYPE_MVB != link_type))
        {
            cm_result = CM_CONFIG;
        } /* if ((CM_OK == cm_result) && (SV_LINK_TYPE_MVB != link_type)) */
#endif /* #ifndef TCN_CM_SIM */

        /* read entry value 'reserved1' */
        if (CM_OK == cm_result)
        {
            cm_result = tcn_cm_fs_read(fd, &value8, 1);
        } /* if (CM_OK == cm_result) */

        /* jump to entry value 'end_list_offset' */
        if (CM_OK == cm_result)
        {
            cm_result = tcn_cm_fs_lseek(fd, (offset + 38));
        } /* if (CM_OK == cm_result) */

        /* read entry value 'end_list_offset' */
        if (CM_OK == cm_result)
        {
            cm_result = tcn_cm_fs_read(fd, &value16, 2);
        } /* if (CM_OK == cm_result) */
        if (CM_OK == cm_result)
        {
            TCN_MACRO_CONVERT_BE16_TO_CPU(&value16);
        } /* if (CM_OK == cm_result) */

        /* check max. size of MVB_Administrator object */
        if ((CM_OK == cm_result) && (28396 < value16))
        {
            cm_result = CM_CONFIG;
        } /* if ((CM_OK == cm_result) && (28396 < value16)) */

        /* jump to entry value 'checkword' */
        if (CM_OK == cm_result)
        {
            cm_result = tcn_cm_fs_lseek(fd, (offset + 4));
        } /* if (CM_OK == cm_result) */

        /* allocate memory for MVB administrator object */
        if (CM_OK == cm_result)
        {
            mvb_administrator_object = (SV_MVB_ADMINISTRATOR*)malloc(value16);
            if (NULL == mvb_administrator_object)
            {
                cm_result = CM_MEMORY;
            } /* if (NULL == mvb_administrator_object) */
        } /* if (CM_OK == cm_result) */

        /* read entry values of MVB administrator object */
        if (CM_OK == cm_result)
        {
            if (read(fd, mvb_administrator_object, value16) != \
                (INTEGER32)value16)
            {
                cm_result = CM_FILE_ACCESS;
            } /* if (read(fd, mvb_administrator_object, value16) != ...) */
        } /* if (CM_OK == cm_result) */

        /* swap bytes of MVB administrator object */
        if (CM_OK == cm_result)
        {
            p16 = (WORD16*)mvb_administrator_object;
            for (counter=0; counter<(UNSIGNED32)(value16 / 2); counter++)
            {
                TCN_MACRO_CONVERT_BE16_TO_CPU(p16);
                p16++;
            } /* for (counter=0; counter<(UNSIGNED32)(value16 / 2); counter++) */

#ifndef TCN_CM_SIM
            as_result =                                         \
                as_service_handler                              \
                (                                               \
                    (ENUM8)bus_id,                              \
                    (ENUM8)SV_MVB_SERVICE_WRITE_ADMINISTRATOR,  \
                    (void*)mvb_administrator_object             \
                );
            if (AS_OK != as_result)
            {
                cm_result = CM_ERROR;
            } /* if (AS_OK != as_result) */
#endif /* #ifndef TCN_CM_SIM */
        } /* if (CM_OK == cm_result) */

        /* release memory (MVB administrator object) */
        if (NULL != mvb_administrator_object)
        {
            free((void*)mvb_administrator_object);
        } /* if (NULL != mvb_administrator_object) */

        /* set processing status of list entry */
        if (CM_OK == cm_result)
        {
            p_entries_list->status = CM_D2000_STATUS_OK;
        } /* if (CM_OK == cm_result) */

    } /* if ((CM_OK == cm_result) && (bus_id == p_entries_list->bus_id)) */

    return(cm_result);

} /* tcn_cm_config_type_d2000_entry_mvb_administrator */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_cm_config_type_d2000_entry_ports_configuration
 *
 *  Purpose   : Configures a device from a D2000 configuration structure
 *              - process entry 'Ports_Configuration'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              CM_RESULT
 *              tcn_cm_config_type_d2000_entry_ports_configuration
 *              (
 *                  int                     fd,
 *                  CM_D2000_ENTRIES_LIST   *p_entries_list
 *              );
 *
 *  Input     : fd             - file descriptor
 *              p_entries_list - pointer to list entry of D2000
 *                               configuration structure specific
 *                               parameter (entry to be processed)
 *
 *  Return    : any CM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
CM_RESULT
tcn_cm_config_type_d2000_entry_ports_configuration
(
    int                     fd,
    CM_D2000_ENTRIES_LIST   *p_entries_list
)
{
    CM_RESULT   cm_result    = CM_OK;
    UNSIGNED32  counter;
    WORD16      value16;
    ENUM8       bus_id       = 0;
    UNSIGNED16  nr_ports;
    UNSIGNED16  port_count;
    WORD16      port_address = 0;
    PD_PRT_ATTR prt_attr;
#ifndef TCN_CM_SIM
    AS_RESULT   as_result;
    AP_RESULT   ap_result;
    ENUM8       link_type;
    STRING32    link_name;
#endif /* #ifndef TCN_CM_SIM */

    /* read entry value 'nr_ports' */
    cm_result = tcn_cm_fs_read(fd, &nr_ports, 2);
    if (CM_OK == cm_result)
    {
        TCN_MACRO_CONVERT_BE16_TO_CPU(&nr_ports);
    } /* if (CM_OK == cm_result) */

    if ((CM_OK == cm_result) && (0 < nr_ports))
    {
        port_count = 0;
        counter    = 0;
        while ((counter < nr_ports) && (CM_OK == cm_result))
        {
            /* read entry values 'bus_id' & 'port_address' */
            cm_result = tcn_cm_fs_read(fd, &value16, 2);
            if (CM_OK == cm_result)
            {
                TCN_MACRO_CONVERT_BE16_TO_CPU(&value16);
                bus_id       = (ENUM8)((value16 & 0xF000) >> 12);
                port_address = (WORD16)(value16 & 0x0FFF);
            } /* if (CM_OK == cm_result) */

            /* read entry values 'f_code', 'config' */
            /* and 'port_size'                      */
            if (CM_OK == cm_result)
            {
                cm_result = tcn_cm_fs_read(fd, &value16, 2);
            } /* if (CM_OK == cm_result) */
            if (CM_OK == cm_result)
            {
                TCN_MACRO_CONVERT_BE16_TO_CPU(&value16);
            } /* if (CM_OK == cm_result) */

            /* check max. range of 'bus_id' */
            if ((CM_OK == cm_result) && (bus_id > as_max_links))
            {
                cm_result = CM_UNKNOW_LINK;
            } /* if ((CM_OK == cm_result) && (bus_id > as_max_links)) */

            /* check, if 'bus_id' match */
            if ((CM_OK == cm_result) && (bus_id == p_entries_list->bus_id))
            {
#ifdef TCN_CM_PRINT
                if (0 == port_count)
                {
                    TCN_OSL_PRINTF("  - entry 'Ports_Configuration', ");
                    TCN_OSL_PRINTF("nr_ports=0x%04X\n", nr_ports);
                } /* if (0 == port_count) */
#endif /* #ifdef TCN_CM_PRINT */

#ifndef TCN_CM_SIM
                as_result =                     \
                    as_read_link_descriptor     \
                    (                           \
                        (ENUM8)bus_id,          \
                        (ENUM8*)&link_type,     \
                        (STRING32*)&link_name   \
                    );
                if (AS_OK != as_result)
                {
                    cm_result = CM_ERROR;
                } /* if (AS_OK != as_result) */
                if ((CM_OK == cm_result) && (SV_LINK_TYPE_MVB != link_type))
                {
                    cm_result = CM_CONFIG;
                } /* if ((CM_OK == cm_result) && (...)) */
#endif /* #ifndef TCN_CM_SIM */

#ifdef TCN_CM_PRINT
                if (CM_OK == cm_result)
                {
                    TCN_OSL_PRINTF("    - address=0x%04X, ",    \
                        port_address);
                    TCN_OSL_PRINTF("f_code=0x%02X, ",           \
                        (UNSIGNED8)((value16 & 0xF000) >> 12));
                    TCN_OSL_PRINTF("config=0x%02X, ",           \
                        (UNSIGNED8)((value16 & 0x0F00) >> 8));
                    TCN_OSL_PRINTF("size=0x%02X\n",             \
                        (UNSIGNED8)(value16 & 0x000F));
                } /* if (CM_OK == cm_result) */
#endif /* #ifdef TCN_CM_PRINT */

                /* set 'prt_attr.port_size' */
                if (CM_OK == cm_result)
                {
                    switch ((value16 & 0xF000) >> 12)
                    {
                        case (0):
                            prt_attr.port_size = 2;
                            break;
                        case (1):
                            prt_attr.port_size = 4;
                            break;
                        case (2):
                            prt_attr.port_size = 8;
                            break;
                        case (3):
                            prt_attr.port_size = 16;
                            break;
                        case (4):
                            prt_attr.port_size = 32;
                            break;
                        default:
                            cm_result = CM_CONFIG;
                    } /* switch ((value16 & 0xF000) >> 12) */
                } /* if (CM_OK == cm_result) */

                if (CM_OK == cm_result)
                {
                    /* set 'prt_attr.port_config' */
                    if (0x0800 == (value16 & 0x0800))
                    {
                        /* bit 'src' */
                        prt_attr.port_config = PD_SOURCE_PORT;
                    } /* if (0x0800 == (value16 & 0x0800)) */
                    else if (0x0400 == (value16 & 0x0400))
                    {
                        /* bit 'snk' */
                        prt_attr.port_config = PD_SINK_PORT;
                    } /* else if (0x0400 == (value16 & 0x0400)) */
                    else
                    {
                        cm_result = CM_CONFIG;
                    } /* else */
                } /* if (CM_OK == cm_result) */

                if (CM_OK == cm_result)
                {
                    /* set 'prt_attr.reserved' */
                    prt_attr.reserved = 0;

                    /* set 'prt_attr.p_bus_specific' */
                    prt_attr.p_bus_specific = NULL;

#ifndef TCN_CM_SIM
                    ap_result =                         \
                        ap_port_manage                  \
                        (                               \
                            (ENUM8)bus_id,              \
                            (WORD16)port_address,       \
                            (ENUM16)PD_PRT_CMD_CONFIG,  \
                            (PD_PRT_ATTR*)&prt_attr     \
                        );
                    if (AP_OK != ap_result)
                    {
                        cm_result = CM_ERROR;
                    } /* if (AP_OK != ap_result) */
#endif /* #ifndef TCN_CM_SIM */
                } /* if (CM_OK == cm_result) */

                if (CM_OK == cm_result)
                {
                    port_count++;
                } /* if (CM_OK == cm_result) */

            } /* if ((CM_OK == cm_result) && (...)) */

            counter++;
        } /* while ((counter < nr_ports) && (CM_OK == cm_result)) */

        if ((CM_OK == cm_result) && (0 < port_count))
        {
            /* set processing status of list entry */
            p_entries_list->status = CM_D2000_STATUS_OK;
        } /* if ((CM_OK == cm_result) && (0 < port_count)) */

    } /* if ((CM_OK == cm_result) && (0 < nr_ports)) */

    return(cm_result);

} /* tcn_cm_config_type_d2000_entry_ports_configuration */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_cm_config_type_d2000
 *
 *  Purpose   : Configures a device from a D2000 configuration structure.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              CM_RESULT
 *              tcn_cm_config_type_d2000
 *              (
 *                  ENUM16              config_type,
 *                  CHARACTER8          *path,
 *                  CM_D2000_PARAMETER  *p_parameter
 *              );
 *
 *  Input     : config_type - any CM_CONFIG_TYPE
                path        - points to a pathname naming the file (includes
 *                            the terminating null byte)
 *              p_parameter - pointer to D2000 configuration structure
 *                            specific parameter (structured type);
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
TCN_DECL_LOCAL
CM_RESULT
tcn_cm_config_type_d2000
(
    ENUM16              config_type,
    CHARACTER8          *path,
    CM_D2000_PARAMETER  *p_parameter
)
{
    CM_RESULT               cm_result       = CM_OK;
    CM_D2000_ENTRIES_LIST   *p_entries_list = NULL;
    CM_D2000_ENTRIES_LIST   entries_list;   /* dummy for default list */
    BOOLEAN1                default_list_ba_enbale;
    BOOLEAN1                r_list_entry;
    UNSIGNED16              c_list_entry;
    WORD8                   value8;
    int                     fd              = -1;
    INTEGER32               nbyte;
    UNSIGNED16              checksum_read   = 0;
    UNSIGNED16              checksum_calc   = 0;
    UNSIGNED32              size_read       = 0;
    UNSIGNED32              size_calc       = 0;
    UNSIGNED16              nr_entries      = 0;
    BOOLEAN1                r_entry;
    UNSIGNED32              c_entry;
    UNSIGNED32              offset          = 0;
    ENUM8                   tnm_key         = 0;
    ENUM8                   sif_code        = 0;


    entries_list.entry  = 0;
    entries_list.bus_id = 0;
    entries_list.status = 0;

    default_list_ba_enbale = FALSE;

    if (NULL != p_parameter)
    {
        if (0 == p_parameter->nr_entries)
        {
            cm_result = CM_CONFIG;
        } /* if (0 == p_parameter->nr_entries) */

        /* set processing status of list entry */
        if (CM_OK == cm_result)
        {
            p_entries_list = \
                (CM_D2000_ENTRIES_LIST*)p_parameter->entries_list;
            for (c_list_entry=0; c_list_entry<p_parameter->nr_entries; \
                 c_list_entry++)
            {
                p_entries_list->status = CM_D2000_STATUS_UNDEFINED;
                p_entries_list++;
            } /* for (c_list_entry=0; ...) */
        } /* if (CM_OK == cm_result) */
    } /* if (NULL != p_parameter) */

    if (CM_OK == cm_result)
    {
        fd = open(path, O_RDONLY);
        if (-1 == fd)
        {
            cm_result = CM_FILE_ACCESS;
        } /* if (-1 == fd) */
    } /* if (CM_OK == cm_result) */

    /* read header value 'checksum' */
    if (CM_OK == cm_result)
    {
        cm_result = tcn_cm_fs_read(fd, &checksum_read, 2);
    } /* if (CM_OK == cm_result) */
    if (CM_OK == cm_result)
    {
        TCN_MACRO_CONVERT_BE16_TO_CPU(&checksum_read);
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("- checksum_read=0x%04X, ", checksum_read);
#endif /* #ifdef TCN_CM_PRINT */
    } /* if (CM_OK == cm_result) */

    /* read header value 'size' */
    if (CM_OK == cm_result)
    {
        if (CM_CONFIG_TYPE_D2000_SERVER == config_type)
        {
            cm_result = tcn_cm_fs_lseek(fd, 2);
        } /* if (CM_CONFIG_TYPE_D2000_SERVER == config_type) */
        else
        {
            cm_result = tcn_cm_fs_lseek(fd, 4);
        } /* else */
        cm_result = tcn_cm_fs_read(fd, &size_read, 4);
    } /* if (CM_OK == cm_result) */
    if (CM_OK == cm_result)
    {
        TCN_MACRO_CONVERT_BE32_TO_CPU(&size_read);
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("size_read=0x%08lX (%ld)\n", size_read, size_read);
#endif /* #ifdef TCN_CM_PRINT */
    } /* if (CM_OK == cm_result) */

    /* ----------------------------------------------------------------------
     *  calculate checksum
     * ----------------------------------------------------------------------
     */
    /* jump to first byte after header value 'checksum' */
    if (CM_OK == cm_result)
    {
        cm_result = tcn_cm_fs_lseek(fd, 2);
    } /* if (CM_OK == cm_result) */
    if (CM_OK == cm_result)
    {
        checksum_calc = 0;
        size_calc     = 2;
        nbyte         = read(fd, &value8, 1);
        while ((1 == nbyte) && (size_calc != size_read))
        {
            checksum_calc = (UNSIGNED16)(checksum_calc + value8);
            size_calc++;
            nbyte = read(fd, &value8, 1);
            /* NOTE: nbyte=0 -> end-of-file (EOF) */
        } /* while ((1 == nbyte) && (size_calc != size_read)) */
        if (0 > nbyte)
        {
            cm_result = CM_FILE_ACCESS;
        } /* if (0 > nbyte) */
    } /* if (CM_OK == cm_result) */

#ifdef TCN_CM_PRINT
    if (CM_OK == cm_result)
    {
        TCN_OSL_PRINTF("- checksum_calc=0x%04X, ", checksum_calc);
        TCN_OSL_PRINTF("size_calc=0x%08lX (%ld)\n", size_calc, size_calc);
    } /* if (CM_OK == cm_result) */
#endif /* #ifdef TCN_CM_PRINT */

    /* ----------------------------------------------------------------------
     *  check header value 'checksum'
     * ----------------------------------------------------------------------
     */
    if (CM_OK == cm_result)
    {
        if (checksum_read != checksum_calc)
        {
#ifdef TCN_CM_PRINT
            TCN_OSL_PRINTF("CM ERROR:\n");
            TCN_OSL_PRINTF("- checksum_read != checksum_calc\n");
#endif /* #ifdef TCN_CM_PRINT */
            cm_result = CM_CONFIG;
        } /* if (checksum_read != checksum_calc) */
    } /* if (CM_OK == cm_result) */

    /* ----------------------------------------------------------------------
     *  check header value 'size'
     * ----------------------------------------------------------------------
     */
    if (CM_OK == cm_result)
    {
        if (size_calc != size_read)
        {
#ifdef TCN_CM_PRINT
            TCN_OSL_PRINTF("CM ERROR:\n");
            TCN_OSL_PRINTF("- size_calc != size_read\n");
#endif /* #ifdef TCN_CM_PRINT */
            cm_result = CM_CONFIG;
        } /* if (size_calc != size_read) */
    } /* if (CM_OK == cm_result) */

    /* ----------------------------------------------------------------------
     *  check header value 'nr_entries'
     * ----------------------------------------------------------------------
     */
    if (CM_OK == cm_result)
    {
        if (CM_CONFIG_TYPE_D2000_SERVER == config_type)
        {
            cm_result = tcn_cm_fs_lseek(fd, 54);
        } /* if (CM_CONFIG_TYPE_D2000_SERVER == config_type) */
        else
        {
            cm_result = tcn_cm_fs_lseek(fd, 64);
        } /* else */
    } /* if (CM_OK == cm_result) */
    if (CM_OK == cm_result)
    {
        cm_result = tcn_cm_fs_read(fd, &nr_entries, 2);
    } /* if (CM_OK == cm_result) */
    if (CM_OK == cm_result)
    {
        TCN_MACRO_CONVERT_BE16_TO_CPU(&nr_entries);
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("- nr_entries=0x%04X (%d)\n", nr_entries, nr_entries);
#endif /* #ifdef TCN_CM_PRINT */
        if (0 == nr_entries)
        {
#ifdef TCN_CM_PRINT
            TCN_OSL_PRINTF("CM ERROR (entry 'Header'):\n");
            TCN_OSL_PRINTF("- nr_entries=%d\n", nr_entries);
#endif /* #ifdef TCN_CM_PRINT */
            cm_result = CM_CONFIG;
        } /* if (0 == nr_entries) */
    } /* if (CM_OK == cm_result) */

    /* ----------------------------------------------------------------------
     *  process list entries (default or user list)
     * ----------------------------------------------------------------------
     */
    if (CM_OK == cm_result)
    {
        if (NULL == p_parameter)
        {
            if (CM_CONFIG_TYPE_D2000_SERVER == config_type)
            {
                entries_list.entry  = CM_D2000_ENTRY_FUNCTION_DIRECTORY;
            } /* if (CM_CONFIG_TYPE_D2000_SERVER == config_type) */
            else
            {
                entries_list.entry  = CM_D2000_ENTRY_MD_CONTROL;
            } /* else */
            entries_list.bus_id = 0;
            p_entries_list      = &entries_list;
        } /* if (NULL == p_parameter) */
        else
        {
            p_entries_list = p_parameter->entries_list;
        } /* else */
    } /* if (CM_OK == cm_result) */

    r_list_entry = TRUE;
    c_list_entry = 0;
    while ((CM_OK == cm_result) && (TRUE == r_list_entry))
    {
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("- entry=0x%02X, bus_id=0x%02X\n", \
            p_entries_list->entry, p_entries_list->bus_id);
#endif /* #ifdef TCN_CM_PRINT */

        /* ------------------------------------------------------------------
         *  search first entry which match and process it
         *  (ignore all other entries)
         * ------------------------------------------------------------------
         */
        r_entry = TRUE;
        c_entry = 0;
        while ((CM_OK == cm_result) && (TRUE == r_entry) && \
               (c_entry < nr_entries))
        {
            /* --------------------------------------------------------------
             *  Header
             * --------------------------------------------------------------
             */

            /* jump to current array element of 'entry_list' */
            if (CM_CONFIG_TYPE_D2000_SERVER == config_type)
            {
                cm_result = tcn_cm_fs_lseek(fd, (56 + (c_entry * 4)));
            } /* if (CM_CONFIG_TYPE_D2000_SERVER == config_type) */
            else
            {
                cm_result = tcn_cm_fs_lseek(fd, (66 + (c_entry * 4)));
            } /* else */

            /* read header value 'offset' */
            if (CM_OK == cm_result)
            {
                cm_result = tcn_cm_fs_read(fd, &offset, 4);
            } /* if (CM_OK == cm_result) */
            if (CM_OK == cm_result)
            {
                TCN_MACRO_CONVERT_BE32_TO_CPU(&offset);
            } /* if (CM_OK == cm_result) */

            /* jump to current entry */
            if (CM_OK == cm_result)
            {
                cm_result = tcn_cm_fs_lseek(fd, offset);
            } /* if (CM_OK == cm_result) */

            /* set processing status of list entry */
            if (CM_OK == cm_result)
            {
                p_entries_list->status = CM_D2000_STATUS_FAILED;
            } /* if (CM_OK == cm_result) */


            /* --------------------------------------------------------------
             *  common to all entries
             * --------------------------------------------------------------
             */

            /* read entry value 'tnm_key' */
            if (CM_OK == cm_result)
            {
                cm_result = tcn_cm_fs_read(fd, &tnm_key, 1);
            } /* if (CM_OK == cm_result) */

            /* read entry value 'sif_code' */
            if (CM_OK == cm_result)
            {
                cm_result = tcn_cm_fs_read(fd, &sif_code, 1);
            } /* if (CM_OK == cm_result) */

            /* --------------------------------------------------------------
             *  check entry value 'tnm_key'
             * --------------------------------------------------------------
             */
            if ((CM_OK == cm_result) && (0x02 != tnm_key))
            {
#ifdef TCN_CM_PRINT
                TCN_OSL_PRINTF("CM ERROR:\n");
                TCN_OSL_PRINTF("- tnm_key=0x%02X\n", tnm_key);
#endif /* #ifdef TCN_CM_PRINT */
                cm_result = CM_CONFIG;
            } /* if ((CM_OK == cm_result) && (0x02 != tnm_key)) */

            /* --------------------------------------------------------------
             *  check entry value 'sif_code'
             * --------------------------------------------------------------
             */
            if ((CM_OK == cm_result) && (sif_code == p_entries_list->entry))
            {
                switch (sif_code)
                {
                    /* ------------------------------------------------------
                     *  entry 'MD_Control'
                     * ------------------------------------------------------
                     */
                    case (CM_D2000_ENTRY_MD_CONTROL):
                        cm_result =                                         \
                        tcn_cm_config_type_d2000_entry_md_control           \
                        (                                                   \
                            fd,                                             \
                            p_entries_list                                  \
                        );
                        r_entry = FALSE;
                        break;
                    /* ------------------------------------------------------
                     *  entry 'Function_Directory'
                     * ------------------------------------------------------
                     */
                    case (CM_D2000_ENTRY_FUNCTION_DIRECTORY):
                        cm_result =                                         \
                        tcn_cm_config_type_d2000_entry_function_directory   \
                        (                                                   \
                            fd,                                             \
                            p_entries_list                                  \
                        );
                        r_entry = FALSE;
                        break;
                    /* ------------------------------------------------------
                     *  entry 'MVB_Control'
                     * ------------------------------------------------------
                     */
                    case (CM_D2000_ENTRY_MVB_CONTROL):
                        cm_result =                                         \
                        tcn_cm_config_type_d2000_entry_mvb_control          \
                        (                                                   \
                            fd,                                             \
                            p_entries_list                                  \
                        );
                        r_entry = FALSE;
                        break;
                    /* ------------------------------------------------------
                     *  entry 'MVB_Administrator'
                     * ------------------------------------------------------
                     */
                    case (CM_D2000_ENTRY_MVB_ADMINISTRATOR):
                        cm_result =                                         \
                        tcn_cm_config_type_d2000_entry_mvb_administrator    \
                        (                                                   \
                            fd,                                             \
                            p_entries_list,                                 \
                            offset                                          \
                        );
                        r_entry = FALSE;
                        break;
                    /* ------------------------------------------------------
                     *  entry 'Ports_Configuration'
                     * ------------------------------------------------------
                     */
                    case (CM_D2000_ENTRY_PORTS_CONFIGURATION):
                        cm_result =                                         \
                        tcn_cm_config_type_d2000_entry_ports_configuration  \
                        (                                                   \
                            fd,                                             \
                            p_entries_list                                  \
                        );
                        r_entry = FALSE;
                        break;
                    /* ------------------------------------------------------
                     *  default
                     * ------------------------------------------------------
                     */
                    default:
#ifdef TCN_CM_PRINT
                        TCN_OSL_PRINTF("- sif_code=0x%02X\n", sif_code);
#endif /* #ifdef TCN_CM_PRINT */
                        r_entry = TRUE;
                } /* switch (sif_code) */
            } /* if ((CM_OK == cm_result) && (...)) */

            c_entry++;
        } /* while ((CM_OK == cm_result) && (TRUE == r_entry) && (...)) */

        if (CM_OK == cm_result)
        {
            /* set processing status of list entry */
            if (CM_D2000_STATUS_FAILED == p_entries_list->status)
            {
                p_entries_list->status = CM_D2000_STATUS_UNDEFINED;
            } /* if (CM_D2000_STATUS_FAILED == p_entries_list->status) */

            if (NULL == p_parameter)
            {
                /* ----------------------------------------------------------
                 *  default list
                 * ----------------------------------------------------------
                 */
                switch (entries_list.entry)
                {
                    case (CM_D2000_ENTRY_MD_CONTROL):
                        entries_list.entry  = \
                            CM_D2000_ENTRY_FUNCTION_DIRECTORY;
                        entries_list.bus_id = 0;
                        break;
                    case (CM_D2000_ENTRY_FUNCTION_DIRECTORY):
                        entries_list.entry  = \
                            CM_D2000_ENTRY_PORTS_CONFIGURATION;
                        entries_list.bus_id = 0;
                        break;
                    case (CM_D2000_ENTRY_PORTS_CONFIGURATION):
                        entries_list.bus_id++;
                        if (entries_list.bus_id >= as_max_links)
                        {
                            entries_list.entry  = CM_D2000_ENTRY_MVB_CONTROL;
                            entries_list.bus_id = 0;
                            default_list_ba_enbale = FALSE;
                        } /* if (entries_list.bus_id >= as_max_links) */
                        break;
                    case (CM_D2000_ENTRY_MVB_CONTROL):
                        if (FALSE == default_list_ba_enbale)
                        {
                            entries_list.entry = \
                                CM_D2000_ENTRY_MVB_ADMINISTRATOR;
                        } /* if (FALSE == default_list_ba_enbale) */
                        else
                        {
                            entries_list.entry = CM_D2000_ENTRY_MVB_CONTROL;
                            entries_list.bus_id++;
                            if (entries_list.bus_id >= as_max_links)
                            {
                                r_list_entry = FALSE;
                            } /* if (entries_list.bus_id >= as_max_links) */
                            default_list_ba_enbale = FALSE;
                        } /* else */
                        break;
                    case (CM_D2000_ENTRY_MVB_ADMINISTRATOR):
                        if (CM_D2000_STATUS_OK == entries_list.status)
                        {
                            entries_list.entry = CM_D2000_ENTRY_MVB_CONTROL;
                            default_list_ba_enbale = TRUE;
                        } /* if (CM_D2000_STATUS_OK == ...) */
                        else
                        {
                            entries_list.entry = CM_D2000_ENTRY_MVB_CONTROL;
                            entries_list.bus_id++;
                            if (entries_list.bus_id >= as_max_links)
                            {
                                r_list_entry = FALSE;
                            } /* if (entries_list.bus_id >= as_max_links) */
                            default_list_ba_enbale = FALSE;
                        } /* else */
                        break;
                    default:
                        ;
                } /* switch (entries_list.entry) */
            } /* if (NULL == p_parameter) */
            else
            {
                /* ----------------------------------------------------------
                 *  user list
                 * ----------------------------------------------------------
                 */
                c_list_entry++;
                if (c_list_entry >= p_parameter->nr_entries)
                {
                    r_list_entry = FALSE;
                } /* if (c_list_entry >= p_parameter->nr_entries) */
                else
                {
                    p_entries_list++;
                } /* else */
            } /* else */
        } /* if (CM_OK == cm_result) */

    } /* while ((CM_OK == cm_result) && (TRUE == r_list_entry)) */

#ifdef TCN_CM_PRINT
    TCN_OSL_PRINTF("- close file\n");
#endif /* #ifdef TCN_CM_PRINT */
    if (-1 != fd)
    {
        if (-1 == close(fd))
        {
            cm_result = CM_FILE_ACCESS;
        } /* if (-1 == close(fd)) */
    } /* if (-1 != fd) */

    return(cm_result);

} /* tcn_cm_config_type_d2000 */


/* ==========================================================================
 *
 *  Public Procedures
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
cm_init (void)
{

    return(CM_OK);

} /* cm_init */


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
)
{
    CM_RESULT   cm_result;

    switch (config_type)
    {
        case (CM_CONFIG_TYPE_D2000):
        case (CM_CONFIG_TYPE_D2000_SERVER):
            cm_result =                                 \
                tcn_cm_config_type_d2000                \
                (                                       \
                    config_type,                        \
                    path,                               \
                    (CM_D2000_PARAMETER*)p_parameter    \
                );
            break;
        default:
            cm_result = CM_CONFIG;
    } /* switch (config_type) */

    return(cm_result);

} /* cm_config_device */


/* ==========================================================================
 *
 *  Test of configuration manager
 *
 * ==========================================================================
 */
#ifdef TCN_CM_TEST

/* --------------------------------------------------------------------------
 *  Procedure : cm_test
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
CM_RESULT
cm_test (void)
{
    CM_RESULT               cm_result             = CM_OK;
    CM_D2000_PARAMETER      *p_cm_d2000_parameter;
    CM_D2000_ENTRIES_LIST   *p_entries_list;
    CM_D2000_ENTRIES_LIST   entries_list[6];


    /* ----------------------------------------------------------------------
     *  config_type=CM_CONFIG_TYPE_D2000, path=d2000.bin
     * ----------------------------------------------------------------------
     */
#ifdef TCN_CM_PRINT
    TCN_OSL_PRINTF("\n");
    TCN_OSL_PRINTF("config_type=CM_CONFIG_TYPE_D2000, path=d2000.bin\n");
    TCN_OSL_PRINTF("================================================\n");
#endif /* #ifdef TCN_CM_PRINT */

    memset                                  \
    (                                       \
        (void*)entries_list,                \
        0,                                  \
        (6 * sizeof(CM_D2000_ENTRIES_LIST)) \
    );

    p_cm_d2000_parameter = (CM_D2000_PARAMETER*)&entries_list[0];

    p_entries_list           = p_cm_d2000_parameter->entries_list;
    p_entries_list[0].entry  = CM_D2000_ENTRY_PORTS_CONFIGURATION;
    p_entries_list[0].bus_id = 0;
    p_entries_list[1].entry  = CM_D2000_ENTRY_PORTS_CONFIGURATION;
    p_entries_list[1].bus_id = 20;
    p_entries_list[2].entry  = CM_D2000_ENTRY_FUNCTION_DIRECTORY;
    p_entries_list[2].bus_id = 0;
    p_entries_list[3].entry  = CM_D2000_ENTRY_MVB_CONTROL;
    p_entries_list[3].bus_id = 0;
    p_entries_list[4].entry  = CM_D2000_ENTRY_MVB_ADMINISTRATOR;
    p_entries_list[4].bus_id = 0;

    p_cm_d2000_parameter->nr_entries = 5;

#ifdef TCN_CM_PRINT
    TCN_OSL_PRINTF("\n");
    TCN_OSL_PRINTF("cm_test() - user list\n");
#endif /* #ifdef TCN_CM_PRINT */
    cm_result =                     \
        cm_config_device            \
        (                           \
            CM_CONFIG_TYPE_D2000,   \
            "d2000.bin",            \
            p_cm_d2000_parameter    \
        );
#ifdef TCN_CM_PRINT
    TCN_OSL_PRINTF("cm_result=%d\n", cm_result);
#endif /* #ifdef TCN_CM_PRINT */

    if (CM_OK == cm_result)
    {
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("cm_test() - default list\n");
#endif /* #ifdef TCN_CM_PRINT */
        cm_result =                     \
            cm_config_device            \
            (                           \
                CM_CONFIG_TYPE_D2000,   \
                "d2000.bin",            \
                NULL                    \
            );
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("cm_result=%d\n", cm_result);
#endif /* #ifdef TCN_CM_PRINT */
    } /* if (CM_OK == cm_result) */


    /* ----------------------------------------------------------------------
     *  config_type=CM_CONFIG_TYPE_D2000_SERVER, path=d2000srv.bin
     * ----------------------------------------------------------------------
     */
    if (CM_OK == cm_result)
    {
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("\n\n");
        TCN_OSL_PRINTF("config_type=CM_CONFIG_TYPE_D2000_SERVER,");
        TCN_OSL_PRINTF(" path=d2000srv.bin\n");
        TCN_OSL_PRINTF("========================================");
        TCN_OSL_PRINTF("==================\n");
#endif /* #ifdef TCN_CM_PRINT */

        memset                                  \
        (                                       \
            (void*)entries_list,                \
            0,                                  \
            (6 * sizeof(CM_D2000_ENTRIES_LIST)) \
        );

        p_cm_d2000_parameter = (CM_D2000_PARAMETER*)&entries_list[0];

        p_entries_list           = p_cm_d2000_parameter->entries_list;
        p_entries_list[0].entry  = CM_D2000_ENTRY_PORTS_CONFIGURATION;
        p_entries_list[0].bus_id = 0;
        p_entries_list[1].entry  = CM_D2000_ENTRY_PORTS_CONFIGURATION;
        p_entries_list[1].bus_id = 20;
        p_entries_list[2].entry  = CM_D2000_ENTRY_FUNCTION_DIRECTORY;
        p_entries_list[2].bus_id = 0;
        p_entries_list[3].entry  = CM_D2000_ENTRY_MVB_CONTROL;
        p_entries_list[3].bus_id = 0;
        p_entries_list[4].entry  = CM_D2000_ENTRY_MVB_ADMINISTRATOR;
        p_entries_list[4].bus_id = 0;

        p_cm_d2000_parameter->nr_entries = 5;

#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("cm_test() - user list\n");
#endif /* #ifdef TCN_CM_PRINT */
        cm_result =                             \
            cm_config_device                    \
            (                                   \
                CM_CONFIG_TYPE_D2000_SERVER,    \
                "d2000SRV.bin",                 \
                p_cm_d2000_parameter            \
            );
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("cm_result=%d\n", cm_result);
#endif /* #ifdef TCN_CM_PRINT */
    } /* if (CM_OK == cm_result) */

    if (CM_OK == cm_result)
    {
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("cm_test() - default list\n");
#endif /* #ifdef TCN_CM_PRINT */
        cm_result =                             \
            cm_config_device                    \
            (                                   \
                CM_CONFIG_TYPE_D2000_SERVER,    \
                "d2000SRV.bin",                 \
                NULL                            \
            );
#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("cm_result=%d\n", cm_result);
#endif /* #ifdef TCN_CM_PRINT */
    } /* if (CM_OK == cm_result) */


    return(cm_result);

} /* cm_test */


/* --------------------------------------------------------------------------
 *  Procedure : main
 * --------------------------------------------------------------------------
 */
/* main */

#endif /* #ifdef TCN_CM_TEST */
