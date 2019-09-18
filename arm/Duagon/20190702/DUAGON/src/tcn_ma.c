/* ==========================================================================
 *
 *  File      : TCN_MA.C
 *
 *  Purpose   : Agent Interface (Train Communication Network)
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
 *  - TCN_MA_PRINT            - debug printouts
 *  - TCN_MA_STACK_SIZE <val> - overrides default stack size of TNM agent
 *  - TCN_MA_FS_DISABLE       - disable file system services
 *  - TCN_MA_STATION_SIMPLE   - simple station mapping mapping,
 *                              i.e. station ID = device address & 0xFF
 *                              NOTE: range of device address 0x01..0xFE
 *  - TNM_FS_SIM              - simulate TNM agent using local file system
 *
 * ==========================================================================
 */
#define TCN_MA_STATION_SIMPLE


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
#if defined (TCN_MD) || defined (TNM_FS_SIM)


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
#include <tcn_tnm.h>
#include <tcn_ma.h>
#ifndef TNM_FS_SIM
#   include <tcn_sv.h>
#   include <tcn_sv_m.h>
#   include <tcn_as.h>
#   include <tcn_pd.h>
#   include <tcn_ap.h>
#endif /* #ifndef TNM_FS_SIM */


/* ==========================================================================
 *
 *  Constants
 *
 * ==========================================================================
 */
#ifndef TNM_FS_SIM

#ifndef TCN_MA_IN_MSG_SIZE_MAX
#   define TCN_MA_IN_MSG_SIZE_MAX   65535
#endif


/* ==========================================================================
 *
 *  Inventory Object
 *
 * ==========================================================================
 */
typedef struct
{
    /* identification */
    STRING32    manufacturer_name;  /* manufacturer                        */
    STRING32    device_type;        /* serial number                       */
    UNSIGNED8   station_id;         /* station identifier                  */
    STRING32    station_name;       /* station name                        */
    /* capabilities */
    STRING32    agent_version;      /* software version                    */
    BITSET256   service_set;        /* supported services                  */
}   TCN_MA_INVENTORY_OBJECT;

TCN_DECL_LOCAL  TCN_MA_INVENTORY_OBJECT tcn_ma_inventory_object;


/* ==========================================================================
 *
 *  Reservation Object
 *
 * ==========================================================================
 */
typedef struct
{
    UNSIGNED16  reserved;
    UNSIGNED16  reservation_time_out;
    AM_ADDRESS  manager;
    UNSIGNED32  manager_id;
    INTEGER32   fd;                   /* FS_open    */
    DIR         *dirp;                /* FS_opendir */
}   TCN_MA_RESERVATION_OBJECT;

TCN_DECL_LOCAL \
    TCN_MA_RESERVATION_OBJECT   tcn_ma_reservation_object;

#endif /* #ifndef TNM_FS_SIM */


/* ==========================================================================
 *
 *  Service List
 *
 * ==========================================================================
 */
#ifndef TCN_MA_SERVICE_DESC_SIZE_MAX
#   define TCN_MA_SERVICE_DESC_SIZE_MAX 65535
#endif

typedef struct
{
    MA_SERVICE_CALL     service_call;
    MA_SERVICE_CLOSE    service_close;
    CHARACTER8          service_desc; /* dynamic size; must include */
                                      /* terminating null byte      */
}   TCN_MA_SERVICE;

TCN_DECL_LOCAL  BOOLEAN1            tcn_ma_mutex_status_service = FALSE;
TCN_DECL_LOCAL  pthread_mutex_t     tcn_ma_mutex_object_service;
TCN_DECL_LOCAL  TCN_MA_SERVICE      *tcn_ma_service[256];


/* ==========================================================================
 *
 *  Station Restart
 *
 * ==========================================================================
 */
#ifndef TNM_FS_SIM

TCN_DECL_LOCAL  MA_STATION_RESTART  tcn_ma_station_restart_function;
TCN_DECL_LOCAL  BOOLEAN1            tcn_ma_station_restart_action;

#endif /* #ifndef TNM_FS_SIM */


/* ==========================================================================
 *
 *  Agent Control
 *
 * ==========================================================================
 */
#ifndef TNM_FS_SIM

typedef enum
{
    TCN_MA_CTRL_ACTION_NONE             = 0,
    TCN_MA_CTRL_ACTION_SERVICE_CALL     = 1,
    TCN_MA_CTRL_ACTION_REPLY_REQUEST    = 2,
    TCN_MA_CTRL_ACTION_SERVICE_CLOSE    = 3,
    TCN_MA_CTRL_ACTION_RECEIVE_REQUEST  = 4
}   TCN_MA_CTRL_ACTION;

typedef struct
{
    TCN_MA_CTRL_ACTION  action;
    AM_ADDRESS          manager_address;
    void                *call_msg_adr;
    UNSIGNED32          call_msg_size;
    MA_SERVICE_CLOSE    service_close;
}   TCN_MA_CTRL;

#endif /* #ifndef TNM_FS_SIM */

typedef struct
{
    void                *reply_msg_adr;
    UNSIGNED32          reply_msg_size;
}   TCN_MA_CTRL_DEFAULT;

#ifndef TNM_FS_SIM

TCN_DECL_LOCAL  UNSIGNED16              tcn_ma_replier_ref = 0;
TCN_DECL_LOCAL  BOOLEAN1                tcn_ma_sem_status_ctrl = FALSE;
TCN_DECL_LOCAL  sem_t                   tcn_ma_sem_object_ctrl;
TCN_DECL_LOCAL  BOOLEAN1                tcn_ma_pthread_status_ctrl = FALSE;
TCN_DECL_LOCAL  pthread_attr_t          tcn_ma_pthread_attr_t_ctrl;
TCN_DECL_LOCAL  pthread_t               tcn_ma_pthread_object_ctrl;
#ifdef __ECOS__
    TCN_DECL_LOCAL  struct sched_param  tcn_ma_pthread_sched_param_ctrl;
#endif /* __ECOS__ */
TCN_DECL_LOCAL  TCN_MA_CTRL             tcn_ma_ctrl;

#endif /* #ifndef TNM_FS_SIM */

TCN_DECL_LOCAL  TCN_MA_CTRL_DEFAULT     tcn_ma_ctrl_default;



/* ==========================================================================
 *
 *  Miscellaneous Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Check Station Reservation
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MM_RESULT
tcn_ma_check_station_reservation
(
    AM_ADDRESS  *manager
)
{
    MM_RESULT   mm_result = MM_OK;
    AM_ADDRESS  *m;

#ifdef TNM_FS_SIM

    /* avoid warnings */
    manager = manager;
    m       = NULL;

#else /* #ifdef TNM_FS_SIM */

    if (0 == (tcn_ma_reservation_object.reserved))
    {
        /* ------------------------------------------------------------------
         *  station is not reserved
         * ------------------------------------------------------------------
         */
        mm_result = MM_RDONLY_ACCESS;
    } /* if (0 == (tcn_ma_reservation_object.reserved)) */
    else
    {
        /* ------------------------------------------------------------------
         *  station is reserved
         * ------------------------------------------------------------------
         */
        m = &tcn_ma_reservation_object.manager;
        if ((m->sg_node      != manager->sg_node)       ||  \
            (m->func_or_stat != manager->func_or_stat)  ||  \
            (m->next_station != manager->next_station)  ||  \
            (m->topo_counter != manager->topo_counter))
        {
            mm_result = MM_RDONLY_ACCESS;
        } /* if ((...)) */
    } /* else */

#endif /* #else */

    return(mm_result);

} /* tcn_ma_check_station_reservation */


/* --------------------------------------------------------------------------
 *  Default Services - MA_SERVICE_CLOSE
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_close_default (void)
{

#ifndef TNM_FS_SIM
    if (NULL != tcn_ma_ctrl_default.reply_msg_adr)
    {
        free((void*)tcn_ma_ctrl_default.reply_msg_adr);
    } /* if (NULL != tcn_ma_ctrl_default.reply_msg_adr) */

    tcn_ma_ctrl_default.reply_msg_adr  = (void*)NULL;
    tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)0;
#endif /* #ifndef TNM_FS_SIM */

} /* tcn_ma_service_close_default */


/* ==========================================================================
 *
 *  Default Services
 *
 * ==========================================================================
 */

#ifndef TNM_FS_SIM
/* --------------------------------------------------------------------------
 *  Station Service - Read_Station_Status
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Read_Station_Status
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                           mm_result  = MM_OK;
    TNM_MSG_Call_Read_Station_Status    *call_msg;
    TNM_MSG_Reply_Read_Station_Status   *reply_msg;
    UNSIGNED32                          msg_size   = 0;
    AS_RESULT                           as_result;
    ENUM8                               link_type  = SV_LINK_TYPE_UNKNOWN;
    STRING32                            link_name;
    SV_MVB_STATUS                       mvb_status;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_Read_Station_Status*)call_msg_adr;
    reply_msg = NULL;

    memset((void*)&mvb_status, 0, sizeof(mvb_status));

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_Read_Station_Status' */
    if (call_msg_size != sizeof(TNM_MSG_Call_Read_Station_Status))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    /* allocate and clear memory for 'Reply_Read_Station_Status' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_Read_Station_Status);
        reply_msg = (TNM_MSG_Reply_Read_Station_Status*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* check, if link_id=0 is of type SV_LINK_TYPE_MVB */
    if (MM_OK == mm_result)
    {
        as_result =                 \
            as_read_link_descriptor \
            (                       \
                (ENUM8)0,           \
                &link_type,         \
                &link_name          \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */
    if (MM_OK == mm_result)
    {
        if (SV_LINK_TYPE_MVB != link_type)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (SV_LINK_TYPE_MVB != link_type) */
    } /* if (MM_OK == mm_result) */

    /* read MVB_Status object */
    if (MM_OK == mm_result)
    {
        as_result =                         \
            as_service_handler              \
            (                               \
                (ENUM8)0,                   \
                SV_MVB_SERVICE_READ_STATUS, \
                (void*)&mvb_status          \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Read_Station_Status' */
        reply_msg->tnm_key        = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code       = (ENUM8)READ_STATION_STATUS;
        reply_msg->bus_id         = (UNSIGNED8)0;
        reply_msg->reserved1      = (WORD8)0;
        reply_msg->device_address = (UNSIGNED16)mvb_status.device_address;
        reply_msg->station_status = (BITSET16)          \
            (mvb_status.device_status &                 \
                (                                       \
                    SV_MVB_DEVICE_STATUS_BIT_SP     |   \
                    SV_MVB_DEVICE_STATUS_BIT_BA     |   \
                    SV_MVB_DEVICE_STATUS_BIT_GW     |   \
                    SV_MVB_DEVICE_STATUS_BIT_MD     |   \
                    SV_MVB_DEVICE_STATUS_BIT_RLD    |   \
                    SV_MVB_DEVICE_STATUS_BIT_SSD    |   \
                    SV_MVB_DEVICE_STATUS_BIT_SDD    |   \
                    SV_MVB_DEVICE_STATUS_BIT_FRC    |   \
                    SV_MVB_DEVICE_STATUS_BIT_DNR    |   \
                    SV_MVB_DEVICE_STATUS_BIT_SER        \
                )                                       \
            );
        if (tcn_ma_reservation_object.reserved)
        {
            reply_msg->station_status |= \
                (BITSET16)TNM_STATION_STATUS_BIT_SER;
        } /* if (tcn_ma_reservation_object.reserved) */

        /* swap bytes of 'Reply_Read_Station_Status' */
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->device_address);
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->station_status);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Read_Station_Status */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Read_Station_Status =        \
{                                                                           \
    tcn_ma_service_call_Read_Station_Status,                                \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  Station Service - Write_Station_Control
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Write_Station_Control
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                           mm_result  = MM_OK;
    TNM_MSG_Call_Write_Station_Control  *call_msg;
    TNM_MSG_Reply_Write_Station_Control *reply_msg;
    UNSIGNED32                          msg_size   = 0;
    AS_RESULT                           as_result;
    ENUM8                               link_type  = SV_LINK_TYPE_UNKNOWN;
    STRING32                            link_name;
    SV_MVB_STATUS                       mvb_status;

    call_msg  = (TNM_MSG_Call_Write_Station_Control*)call_msg_adr;
    reply_msg = NULL;

    memset((void*)&mvb_status, 0, sizeof(mvb_status));

    /* check station reservation */
    mm_result = tcn_ma_check_station_reservation(manager_address);

    if (MM_OK == mm_result)
    {
        *reply_msg_adr  = (void*)NULL;
        *reply_msg_size = (UNSIGNED32)0;
    } /* if (MM_OK == mm_result) */

    /* check size of 'Call_Write_Station_Control' */
    if (MM_OK == mm_result)
    {
        if (call_msg_size != sizeof(TNM_MSG_Call_Write_Station_Control))
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != ...) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_Write_Station_Control' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_Write_Station_Control);
        reply_msg = (TNM_MSG_Reply_Write_Station_Control*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* check, if link_id=0 is of type SV_LINK_TYPE_MVB */
    if (MM_OK == mm_result)
    {
        as_result =                 \
            as_read_link_descriptor \
            (                       \
                (ENUM8)0,           \
                &link_type,         \
                &link_name          \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */
    if (MM_OK == mm_result)
    {
        if (SV_LINK_TYPE_MVB != link_type)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (SV_LINK_TYPE_MVB != link_type) */
    } /* if (MM_OK == mm_result) */

    /* read MVB_Status object */
    if (MM_OK == mm_result)
    {
        as_result =                         \
            as_service_handler              \
            (                               \
                (ENUM8)0,                   \
                SV_MVB_SERVICE_READ_STATUS, \
                (void*)&mvb_status          \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */

#ifdef TCN_MA_STATION_SIMPLE
    if (MM_OK == mm_result)
    {
        /* modify inventory object                        */
        /* - simple station (station_id = device_address) */
        tcn_ma_inventory_object.station_id = \
            (UNSIGNED8)mvb_status.device_address;

        /* check element 'station_id' of 'Call_Write_Station_Control' */
        /* and modify inventory object                                */
        if (0 != call_msg->station_id)
        {
            if (call_msg->station_id != tcn_ma_inventory_object.station_id)
            {
                mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
            } /* if (call_msg->station_id != ...) */
        } /* if (0 != call_msg->station_id) */
    } /* if (MM_OK == mm_result) */
#else /* #ifdef TCN_MA_STATION_SIMPLE */
    if (MM_OK == mm_result)
    {
        /* check element 'station_id' of 'Call_Write_Station_Control' */
        /* and modify inventory object                                */
        if (0 != call_msg->station_id)
        {
            //todo
            ;
        } /* if (0 != call_msg->station_id) */
    } /* if (MM_OK == mm_result) */
#endif /* #else */

    if (MM_OK == mm_result)
    {
        /* check element 'station_name' of 'Call_Write_Station_Control' */
        /* and modify inventory object                                  */
        if (0 != call_msg->station_name.character[0])
        {
            memcpy                                              \
            (                                                   \
                (void*)&tcn_ma_inventory_object.station_name,   \
                (void*)&call_msg->station_name,                 \
                32                                              \
            );
        } /* if (0 != call_msg->station_name.character[0]) */

        /* check element 'command' of 'Call_Write_Station_Control' */
        if (0x80 == (call_msg->command & 0x80)) /* rst */
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (0x80 == (call_msg->command & 0x80)) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Write_Station_Control' */
        reply_msg->tnm_key        = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code       = (ENUM8)WRITE_STATION_CONTROL;
        reply_msg->bus_id         = (UNSIGNED8)0;
        reply_msg->reserved1      = (WORD8)0;
        reply_msg->device_address = (UNSIGNED16)mvb_status.device_address;

        /* swap bytes of 'Reply_Write_Station_Control' */
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->device_address);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Write_Station_Control */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Write_Station_Control =      \
{                                                                           \
    tcn_ma_service_call_Write_Station_Control,                              \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  Station Service - Read_Station_Inventory
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Read_Station_Inventory
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                               mm_result   = MM_OK;
    TNM_MSG_Call_Read_Station_Inventory     *call_msg;
    TNM_MSG_Reply_Read_Station_Inventory    *reply_msg;
    UNSIGNED32                              msg_size    = 0;
    AS_RESULT                               as_result;
    BITSET16                                link_set    = 0x0000;
    BITSET16                                pd_link_set;
    BITSET16                                md_link_set;
    ENUM8                                   link_type   = \
                                                SV_LINK_TYPE_UNKNOWN;
    STRING32                                link_name;
    SV_MVB_STATUS                           mvb_status;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_Read_Station_Inventory*)call_msg_adr;
    reply_msg = NULL;

    memset((void*)&mvb_status, 0, sizeof(mvb_status));

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_Read_Station_Inventory' */
    if (call_msg_size != sizeof(TNM_MSG_Call_Read_Station_Inventory))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    /* allocate and clear memory for 'Reply_Read_Station_Inventory' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_Read_Station_Inventory);
        reply_msg = (TNM_MSG_Reply_Read_Station_Inventory*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* read link set */
    if (MM_OK == mm_result)
    {
        as_result =             \
            as_read_link_set    \
            (                   \
                &link_set,      \
                &pd_link_set,   \
                &md_link_set    \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */

    /* check, if link_id=0 is of type SV_LINK_TYPE_MVB */
    if (MM_OK == mm_result)
    {
        as_result =                 \
            as_read_link_descriptor \
            (                       \
                (ENUM8)0,           \
                &link_type,         \
                &link_name          \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */
    if (MM_OK == mm_result)
    {
        if (SV_LINK_TYPE_MVB != link_type)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (SV_LINK_TYPE_MVB != link_type) */
    } /* if (MM_OK == mm_result) */

    /* read MVB_Status object */
    if (MM_OK == mm_result)
    {
        as_result =                         \
            as_service_handler              \
            (                               \
                (ENUM8)0,                   \
                SV_MVB_SERVICE_READ_STATUS, \
                (void*)&mvb_status          \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* modify inventory object                        */
        /* - simple station (station_id = device_address) */
        tcn_ma_inventory_object.station_id = \
            (UNSIGNED8)mvb_status.device_address;

        /* fill 'Reply_Read_Station_Inventory' */
        reply_msg->tnm_key           = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code          = (ENUM8)READ_STATION_INVENTORY;
        reply_msg->reserved1         = (WORD16)0;
        reply_msg->agent_version     = tcn_ma_inventory_object.agent_version;
        reply_msg->manufacturer_name = \
            tcn_ma_inventory_object.manufacturer_name;
        reply_msg->device_type       = tcn_ma_inventory_object.device_type;
        reply_msg->service_set       = tcn_ma_inventory_object.service_set;
        reply_msg->link_set          = (BITSET16)link_set;
        reply_msg->reserved2         = (WORD8)0;
        reply_msg->station_id        = tcn_ma_inventory_object.station_id;
        reply_msg->station_name      = tcn_ma_inventory_object.station_name;
        reply_msg->station_status    = (BITSET16)       \
            (mvb_status.device_status &                 \
                (                                       \
                    SV_MVB_DEVICE_STATUS_BIT_SP     |   \
                    SV_MVB_DEVICE_STATUS_BIT_BA     |   \
                    SV_MVB_DEVICE_STATUS_BIT_GW     |   \
                    SV_MVB_DEVICE_STATUS_BIT_MD     |   \
                    SV_MVB_DEVICE_STATUS_BIT_RLD    |   \
                    SV_MVB_DEVICE_STATUS_BIT_SSD    |   \
                    SV_MVB_DEVICE_STATUS_BIT_SDD    |   \
                    SV_MVB_DEVICE_STATUS_BIT_FRC    |   \
                    SV_MVB_DEVICE_STATUS_BIT_DNR    |   \
                    SV_MVB_DEVICE_STATUS_BIT_SER        \
                )                                       \
            );
        if (tcn_ma_reservation_object.reserved)
        {
            reply_msg->station_status |= \
                (BITSET16)TNM_STATION_STATUS_BIT_SER;
        } /* if (tcn_ma_reservation_object.reserved) */

        /* swap bytes of 'Reply_Read_Station_Inventory' */
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->link_set);
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->station_status);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Read_Station_Inventory */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Read_Station_Inventory =     \
{                                                                           \
    tcn_ma_service_call_Read_Station_Inventory,                             \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  Station Service - Write_Station_Reservation
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Write_Station_Reservation
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                               mm_result  = MM_OK;
    TNM_MSG_Call_Write_Station_Reservation  *call_msg;
    TNM_MSG_Reply_Write_Station_Reservation *reply_msg;
    AM_ADDRESS                              *m;
    UNSIGNED32                              msg_size   = 0;

    call_msg  = (TNM_MSG_Call_Write_Station_Reservation*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_Write_Station_Reservation' */
    if (call_msg_size != sizeof(TNM_MSG_Call_Write_Station_Reservation))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_access' */
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->command);
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->access_type);
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->reservation_time_out);
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->manager_id);

        /* check element 'command' of 'Call_Write_Station_Reservation' */
        if ((TNM_STATION_RESERVATION_COMMAND_RESERVE  !=    \
                call_msg->command )                      && \
            (TNM_STATION_RESERVATION_COMMAND_KEEPREL  !=    \
                call_msg->command )                      && \
            (TNM_STATION_RESERVATION_COMMAND_STARTREL !=    \
                call_msg->command))
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if ((...)) */
    } /* if (MM_OK == mm_result) */

    /* check element 'access_type' of 'Call_Write_Station_Reservation' */
    if (MM_OK == mm_result)
    {
        if ((TNM_STATION_RESERVATION_ACCESS_TYPE_WRITEREQ != \
                call_msg->access_type)   &&  \
            (TNM_STATION_RESERVATION_ACCESS_TYPE_OVERRIDE != \
                call_msg->access_type))
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if ((...)) */
    } /* if (MM_OK == mm_result) */

    /* check access rights */
    if (MM_OK == mm_result)
    {
        if (TNM_STATION_RESERVATION_ACCESS_TYPE_WRITEREQ == \
            call_msg->access_type)
        {
            if (tcn_ma_reservation_object.reserved)
            {
                m = &tcn_ma_reservation_object.manager;
                if ((manager_address->sg_node      == m->sg_node)      && \
                    (manager_address->func_or_stat == m->func_or_stat) && \
                    (manager_address->next_station == m->next_station) && \
                    (manager_address->topo_counter == m->topo_counter))
                {
                    if (TNM_STATION_RESERVATION_COMMAND_RESERVE == \
                        call_msg->command)
                    {
                        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
                    } /* if (...) */
                } /* if ((...)) */
                else
                {
                    if ((TNM_STATION_RESERVATION_COMMAND_KEEPREL  ==    \
                            call_msg->command)                       || \
                        (TNM_STATION_RESERVATION_COMMAND_STARTREL ==    \
                            call_msg->command))
                    {
                        mm_result = (MM_RESULT)MM_RDONLY_ACCESS;
                    } /* if ((...)) */
                } /* else */
            } /* if (tcn_ma_reservation_object.reserved) */
            else
            {
                if ((TNM_STATION_RESERVATION_COMMAND_KEEPREL ==    \
                        call_msg->command)                      || \
                    (TNM_STATION_RESERVATION_COMMAND_STARTREL ==   \
                        call_msg->command))
                {
                    mm_result = (MM_RESULT)MM_RDONLY_ACCESS;
                } /* if ((...)) */
            } /* else */
        } /* if (TNM_STATION_RESERVATION_ACCESS_TYPE_WRITEREQ == ...) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_Write_Station_Reservation' */
    if (MM_OK == mm_result)
    {
        msg_size  = \
            (UNSIGNED32)sizeof(TNM_MSG_Reply_Write_Station_Reservation);
        reply_msg = \
            (TNM_MSG_Reply_Write_Station_Reservation*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* close open file descriptor */
        if (0 < tcn_ma_reservation_object.fd)
        {
            /* call POSIX procedure 'close' */
            close(tcn_ma_reservation_object.fd);
        } /* if (0 < tcn_ma_reservation_object.fd) */

        /* close open directory stream */
        if (NULL != tcn_ma_reservation_object.dirp)
        {
            /* call POSIX procedure 'closedir' */
            closedir(tcn_ma_reservation_object.dirp);
        } /* if (NULL != tcn_ma_reservation_object.dirp) */

        if (TNM_STATION_RESERVATION_COMMAND_RESERVE == call_msg->command)
        {
            /* reserve station (set reservation object) */
            tcn_ma_reservation_object.reservation_time_out = \
                call_msg->reservation_time_out;
            tcn_ma_reservation_object.manager              = \
                *manager_address;
            tcn_ma_reservation_object.manager_id           = \
                call_msg->manager_id;
            tcn_ma_reservation_object.reserved             = 1;
            tcn_ma_reservation_object.fd                   = -1;
            tcn_ma_reservation_object.dirp                 = NULL;
        } /* if (TNM_STATION_RESERVATION_COMMAND_RESERVE == ...) */
        else
        {
            /* clear reservation object */
            memset                                  \
            (                                       \
                (void*)&tcn_ma_reservation_object,  \
                0,                                  \
                sizeof(TCN_MA_RESERVATION_OBJECT)   \
            );
            tcn_ma_reservation_object.fd   = -1;
            tcn_ma_reservation_object.dirp = NULL;

            if (TNM_STATION_RESERVATION_COMMAND_STARTREL == \
                call_msg->command)
            {
                tcn_ma_station_restart_action = (BOOLEAN1)TRUE;
            } /* if (TNM_STATION_RESERVATION_COMMAND_STARTREL == ...) */
        } /* else */

        /* fill 'Reply_Write_Station_Reservation' */
        reply_msg->tnm_key    = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code   = (ENUM8)WRITE_RESERVATION;
        reply_msg->reserved1  = (WORD8)0;
        reply_msg->manager_id = (UNSIGNED32)call_msg->manager_id;

        /* swap bytes of 'Reply_Write_Station_Reservation' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->manager_id);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Write_Station_Reservation */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Write_Station_Reservation =  \
{                                                                           \
    tcn_ma_service_call_Write_Station_Reservation,                          \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  Station Service - Read_Service_Descriptor
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Read_Service_Descriptor
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                               mm_result   = MM_OK;
    TNM_MSG_Call_Read_Service_Descriptor    *call_msg;
    TNM_MSG_Reply_Read_Service_Descriptor   *reply_msg;
    UNSIGNED32                              msg_size    = 0;
    UNSIGNED16                              string_size = 0;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_Read_Service_Descriptor*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_Read_Service_Descriptor' */
    if (call_msg_size != sizeof(TNM_MSG_Call_Read_Service_Descriptor))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    pthread_mutex_lock(&tcn_ma_mutex_object_service);

    if (MM_OK == mm_result)
    {
        string_size = (UNSIGNED16)strlen( \
            (char*)&tcn_ma_service[call_msg->get_sif_code]->service_desc);
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_Read_Service_Descriptor' */
    if (MM_OK == mm_result)
    {
        msg_size  = \
            (UNSIGNED32)sizeof(TNM_MSG_Reply_Read_Service_Descriptor);
        msg_size -= (UNSIGNED32)4; /* dynamic size */
        msg_size += (UNSIGNED32)string_size;
        msg_size += (UNSIGNED32)(string_size % 2); /* allign to 16-bit */
        reply_msg = (TNM_MSG_Reply_Read_Service_Descriptor*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Read_Service_Descriptor' */
        reply_msg->tnm_key      = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code     = (ENUM8)READ_SERVICE_DESCRIPTOR;
        reply_msg->reserved1    = (WORD8)0;
        reply_msg->get_sif_code = (ENUM8)call_msg->get_sif_code;
        reply_msg->reserved2    = (WORD16)0;
        reply_msg->string_size  = (UNSIGNED16)string_size;
        if (0 < string_size)
        {
            memcpy                                                          \
            (                                                               \
                (void*)reply_msg->service_description,                      \
                (void*)                                                     \
                    &tcn_ma_service[call_msg->get_sif_code]->service_desc,  \
                string_size                                                 \
            );
        } /* if (0 < string_size) */

        /* swap bytes of 'Reply_Read_Service_Descriptor' */
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->string_size);

    } /* if (MM_OK == mm_result) */

    pthread_mutex_unlock(&tcn_ma_mutex_object_service);

    if (MM_OK == mm_result)
    {
        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Read_Service_Descriptor */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Read_Service_Descriptor =    \
{                                                                           \
    tcn_ma_service_call_Read_Service_Descriptor,                            \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  Station Service - Read_Links_Descriptor
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Read_Links_Descriptor
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                           mm_result        = MM_OK;
    TNM_MSG_Call_Read_Links_Descriptor  *call_msg;
    TNM_MSG_Reply_Read_Links_Descriptor *reply_msg;
    UNSIGNED32                          msg_size         = 0;
    UNSIGNED16                          nr_links;
    ENUM8                               link_id;
    AS_RESULT                           as_result;
    ENUM8                               link_type;
    STRING32                            link_name;
    TNM_LINK_DESCRIPTOR                 *link_descriptor;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_Read_Links_Descriptor*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_Read_Links_Descriptor' */
    if (call_msg_size != sizeof(TNM_MSG_Call_Read_Links_Descriptor))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    /* allocate and clear memory for 'Reply_Read_Links_Descriptor' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_Read_Links_Descriptor);
        msg_size -= \
            (UNSIGNED32)sizeof(TNM_LINK_DESCRIPTOR); /* dynamic size */
        reply_msg = (TNM_MSG_Reply_Read_Links_Descriptor*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* fill links descriptor list (TNM_LINK_DESCRIPTOR) */
    /* of 'Reply_Read_Links_Descriptor'                 */
    nr_links = 0;
    link_id  = 0;
    while ((link_id < as_max_links) && (MM_OK == mm_result))
    {
        as_result =                 \
            as_read_link_descriptor \
            (                       \
                link_id,            \
                &link_type,         \
                &link_name          \
            );

        if (as_result == AS_OK)
        {
            /* re-allocate memory for 'Reply_Read_Links_Descriptor' */
            msg_size += (UNSIGNED32)sizeof(TNM_LINK_DESCRIPTOR);
            reply_msg = (TNM_MSG_Reply_Read_Links_Descriptor*) \
                malloc(msg_size);
            if (NULL == reply_msg)
            {
                mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
            } /* if (NULL == reply_msg) */

            if (MM_OK == mm_result)
            {
                link_descriptor = (TNM_LINK_DESCRIPTOR*) \
                    &reply_msg->links_descriptor[nr_links];
                link_descriptor->bus_id    = (UNSIGNED8)link_id;
                link_descriptor->link_type = (ENUM8)link_type;
                memcpy                                  \
                (                                       \
                    (void*)&link_descriptor->link_name, \
                    (void*)&link_name,                  \
                    32                                  \
                );

                nr_links++;

            } /* if (MM_OK == mm_result) */
        } /* if (as_result == AS_OK) */

        link_id++;
    } /* while ((link_id < as_max_links) && (MM_OK == mm_result)) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Read_Links_Descriptor' */
        reply_msg->tnm_key  = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code = (ENUM8)READ_LINKS_DESCRIPTOR;
        reply_msg->nr_links = (UNSIGNED16)nr_links;

        /* swap bytes of 'Reply_Read_Links_Descriptor' */
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->nr_links);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Read_Links_Descriptor */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Read_Links_Descriptor =      \
{                                                                           \
    tcn_ma_service_call_Read_Links_Descriptor,                              \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  MVB Link Service - Read_Mvb_Status
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Read_Mvb_Status
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                       mm_result  = MM_OK;
    TNM_MSG_Call_Read_Mvb_Status    *call_msg;
    TNM_MSG_Reply_Read_Mvb_Status   *reply_msg;
    UNSIGNED32                      msg_size   = 0;
    AS_RESULT                       as_result;
    ENUM8                           link_type  = SV_LINK_TYPE_UNKNOWN;
    STRING32                        link_name;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_Read_Mvb_Status*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_Read_Mvb_Status' */
    if (call_msg_size != sizeof(TNM_MSG_Call_Read_Mvb_Status))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    /* allocate and clear memory for 'Reply_Read_Mvb_Status' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_Read_Mvb_Status);
        reply_msg = (TNM_MSG_Reply_Read_Mvb_Status*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* check, if link is of type SV_LINK_TYPE_MVB                 */
    /* link reference: element 'bus_id' of 'Call_Read_Mvb_Status' */
    if (MM_OK == mm_result)
    {
        as_result =                         \
            as_read_link_descriptor         \
            (                               \
                (ENUM8)call_msg->bus_id,    \
                &link_type,                 \
                &link_name                  \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */
    if (MM_OK == mm_result)
    {
        if (SV_LINK_TYPE_MVB != link_type)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (SV_LINK_TYPE_MVB != link_type) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Read_Mvb_Status' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)READ_MVB_STATUS;
        reply_msg->bus_id    = (UNSIGNED8)call_msg->bus_id;
        reply_msg->reserved1 = (WORD8)0;

        /* read MVB_Status object and fill 'Reply_Read_Mvb_Status' */
        as_result =                             \
            as_service_handler                  \
            (                                   \
                (ENUM8)call_msg->bus_id,        \
                SV_MVB_SERVICE_READ_STATUS,     \
                (void*)&reply_msg->reserved2    \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Reply_Read_Mvb_Status' */
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->device_address);
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->device_status);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->lineA_errors);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->lineB_errors);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Read_Mvb_Status */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Read_Mvb_Status =            \
{                                                                           \
    tcn_ma_service_call_Read_Mvb_Status,                                    \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  MVB Link Service - Write_Mvb_Control
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Write_Mvb_Control
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                       mm_result  = MM_OK;
    TNM_MSG_Call_Write_Mvb_Control  *call_msg;
    TNM_MSG_Reply_Write_Mvb_Control *reply_msg;
    UNSIGNED32                      msg_size   = 0;
    AS_RESULT                       as_result;
    ENUM8                           link_type  = SV_LINK_TYPE_UNKNOWN;
    STRING32                        link_name;

    call_msg  = (TNM_MSG_Call_Write_Mvb_Control*)call_msg_adr;
    reply_msg = NULL;

    /* check station reservation */
    mm_result = tcn_ma_check_station_reservation(manager_address);

    if (MM_OK == mm_result)
    {
        *reply_msg_adr  = (void*)NULL;
        *reply_msg_size = (UNSIGNED32)0;

        /* check size of 'Call_Write_Mvb_Control' */
        if (call_msg_size != sizeof(TNM_MSG_Call_Write_Mvb_Control))
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != ...) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_Write_Mvb_Control' */
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->device_address);
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_Write_Mvb_Control' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_Write_Mvb_Control);
        reply_msg = (TNM_MSG_Reply_Write_Mvb_Control*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* check, if link is of type SV_LINK_TYPE_MVB                   */
    /* link reference: element 'bus_id' of 'Call_Write_Mvb_Control' */
    if (MM_OK == mm_result)
    {
        as_result =                         \
            as_read_link_descriptor         \
            (                               \
                (ENUM8)call_msg->bus_id,    \
                &link_type,                 \
                &link_name                  \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */
    if (MM_OK == mm_result)
    {
        if (SV_LINK_TYPE_MVB != link_type)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (SV_LINK_TYPE_MVB != link_type) */
    } /* if (MM_OK == mm_result) */

    /* write MVB_Status object (use 'Call_Write_Mvb_Control") */
    if (MM_OK == mm_result)
    {
        as_result =                                 \
            as_service_handler                      \
            (                                       \
                (ENUM8)call_msg->bus_id,            \
                SV_MVB_SERVICE_WRITE_CONTROL,       \
                (void*)&call_msg->device_address    \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Write_Mvb_Control' */
        reply_msg->tnm_key  = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code = (ENUM8)WRITE_MVB_CONTROL;

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Write_Mvb_Control */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Write_Mvb_Control =          \
{                                                                           \
    tcn_ma_service_call_Write_Mvb_Control,                                  \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  MVB Link Service - Read_Mvb_Devices
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Read_Mvb_Devices
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                       mm_result         = MM_OK;
    TNM_MSG_Call_Read_Mvb_Devices   *call_msg;
    TNM_MSG_Reply_Read_Mvb_Devices  *reply_msg;
    UNSIGNED32                      msg_size          = 0;
    AS_RESULT                       as_result;
    ENUM8                           link_type         = SV_LINK_TYPE_UNKNOWN;
    STRING32                        link_name;
    SV_MVB_STATUS                   mvb_status;
    SV_MVB_DEVICES                  *mvb_devices      = NULL;
    SV_MVB_DEVICES_LIST             *mvb_devices_list;
    SV_MVB_DEVICES_LIST             *msg_devices_list;
    UNSIGNED16                      nr_devices;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_Read_Mvb_Devices*)call_msg_adr;
    reply_msg = NULL;

    memset((void*)&mvb_status, 0, sizeof(mvb_status));

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_Read_Mvb_Devices' */
    if (call_msg_size != sizeof(TNM_MSG_Call_Read_Mvb_Devices))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    /* check, if link is of type SV_LINK_TYPE_MVB                  */
    /* link reference: element 'bus_id' of 'Call_Read_Mvb_Devices' */
    if (MM_OK == mm_result)
    {
        as_result =                         \
            as_read_link_descriptor         \
            (                               \
                (ENUM8)call_msg->bus_id,    \
                &link_type,                 \
                &link_name                  \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */
    if (MM_OK == mm_result)
    {
        if (SV_LINK_TYPE_MVB != link_type)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (SV_LINK_TYPE_MVB != link_type) */
    } /* if (MM_OK == mm_result) */

    /* read MVB_Status object */
    if (MM_OK == mm_result)
    {
        as_result =                         \
            as_service_handler              \
            (                               \
                (ENUM8)call_msg->bus_id,    \
                SV_MVB_SERVICE_READ_STATUS, \
                (void*)&mvb_status          \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */

    /* read MVB_Devices object */
    if (MM_OK == mm_result)
    {
        mvb_devices = (SV_MVB_DEVICES*)NULL;
        as_result =                             \
            as_service_handler                  \
            (                                   \
                (ENUM8)call_msg->bus_id,        \
                SV_MVB_SERVICE_READ_DEVICES,    \
                (void*)mvb_devices              \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_Read_Mvb_Devices' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32) \
            sizeof(TNM_MSG_Reply_Read_Mvb_Devices);
        msg_size -= (UNSIGNED32) \
            sizeof(SV_MVB_DEVICES_LIST); /* dynamic size */
        msg_size += (UNSIGNED32) \
            (sizeof(SV_MVB_DEVICES_LIST) * mvb_devices->nr_devices);
        reply_msg = (TNM_MSG_Reply_Read_Mvb_Devices*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Read_Mvb_Devices' */
        reply_msg->tnm_key        = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code       = (ENUM8)READ_MVB_DEVICES;
        reply_msg->bus_id         = (UNSIGNED8)call_msg->bus_id;
        reply_msg->reserved1      = (WORD8)0;
        reply_msg->device_address = (WORD16)mvb_status.device_address;
        reply_msg->nr_devices     = (UNSIGNED16)mvb_devices->nr_devices;

        /* swap bytes of 'Reply_Read_Mvb_Devices' */
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->device_address);
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->nr_devices);

        /* fill devices list list of devices (SV_MVB_DEVICES_LIST) */
        /* of 'Reply_Read_Mvb_Devices'                             */
        if (0 < mvb_devices->nr_devices)
        {
            mvb_devices_list = \
                (SV_MVB_DEVICES_LIST*)mvb_devices->devices_list;
            msg_devices_list = \
                (SV_MVB_DEVICES_LIST*)reply_msg->devices_list;
            for (nr_devices=0; nr_devices<mvb_devices->nr_devices; \
                 nr_devices++)
            {
                msg_devices_list = mvb_devices_list;

                /* swap bytes of 'SV_MVB_DEVICES_LIST' */
                TCN_MACRO_CONVERT_CPU_TO_BE16         \
                (                                     \
                    &msg_devices_list->device_address \
                );
                TCN_MACRO_CONVERT_CPU_TO_BE16         \
                (                                     \
                    &msg_devices_list->device_status  \
                );

                msg_devices_list++;
                mvb_devices_list++;

            } /* for (nr_devices=0; ...) */
        } /* if (0 < mvb_devices->nr_devices) */

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    /* release MVB_Devices object */
    if (NULL != mvb_devices)
    {
        free((void*)mvb_devices);
    } /* if (NULL != mvb_devices) */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Read_Mvb_Devices */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Read_Mvb_Devices =           \
{                                                                           \
    tcn_ma_service_call_Read_Mvb_Devices,                                   \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  MVB Link Service - Write_Mvb_Administrator
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Write_Mvb_Administrator
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                               mm_result    = MM_OK;
    TNM_MSG_Call_Write_Mvb_Administrator    *call_msg;
    TNM_MSG_Reply_Write_Mvb_Administrator   *reply_msg;
    UNSIGNED32                              size;
    WORD16                                  *p_ba_struct;
    UNSIGNED32                              counter;
    UNSIGNED32                              msg_size     = 0;
    AS_RESULT                               as_result;
    ENUM8                                   link_type    = \
                                                SV_LINK_TYPE_UNKNOWN;
    STRING32                                link_name;

    call_msg  = (TNM_MSG_Call_Write_Mvb_Administrator*)call_msg_adr;
    reply_msg = NULL;

    /* check station reservation */
    mm_result = tcn_ma_check_station_reservation(manager_address);

    if (MM_OK == mm_result)
    {
        *reply_msg_adr  = (void*)NULL;
        *reply_msg_size = (UNSIGNED32)0;

        /* check min. size of 'Call_Write_Mvb_Administrator' */
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_Write_Mvb_Administrator);
        msg_size -= (UNSIGNED32)2; /* dynamic size */
        msg_size += (UNSIGNED32)sizeof(SV_MVB_ADMINISTRATOR);
        msg_size -= (UNSIGNED32)2; /* dynamic size */
        if (call_msg_size < msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size < msg_size) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of MVB_Administrator object                  */
        /* (element 'ba_struct' of 'Call_Write_Mvb_Administrator") */
        size  = (UNSIGNED32)call_msg_size;
        size -= (UNSIGNED32)sizeof(WORD32); /* message header */
        size  = (UNSIGNED32)(size / 2);
        p_ba_struct = (WORD16*)call_msg->ba_struct;
        for (counter=0; counter<size; counter++)
        {
            TCN_MACRO_CONVERT_BE16_TO_CPU(p_ba_struct);
            p_ba_struct++;
        } /* for (counter=0; counter<size; counter++) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_Write_Mvb_Administrator' */
    if (MM_OK == mm_result)
    {
        msg_size  = \
            (UNSIGNED32)sizeof(TNM_MSG_Reply_Write_Mvb_Administrator);
        reply_msg = (TNM_MSG_Reply_Write_Mvb_Administrator*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* check, if link is of type SV_LINK_TYPE_MVB                         */
    /* link reference: element 'bus_id' of 'Call_Write_Mvb_Administrator' */
    if (MM_OK == mm_result)
    {
        as_result =                         \
            as_read_link_descriptor         \
            (                               \
                (ENUM8)call_msg->bus_id,    \
                &link_type,                 \
                &link_name                  \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */
    if (MM_OK == mm_result)
    {
        if (SV_LINK_TYPE_MVB != link_type)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (SV_LINK_TYPE_MVB != link_type) */
    } /* if (MM_OK == mm_result) */

    /* write MVB_Administrator object       */
    /* (use 'Call_Write_Mvb_Administrator") */
    if (MM_OK == mm_result)
    {
        as_result =                                 \
            as_service_handler                      \
            (                                       \
                (ENUM8)call_msg->bus_id,            \
                SV_MVB_SERVICE_WRITE_ADMINISTRATOR, \
                (void*)&call_msg->ba_struct         \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Write_Mvb_Administrator' */
        reply_msg->tnm_key  = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code = (ENUM8)WRITE_MVB_ADMINISTRATOR;

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Write_Mvb_Administrator */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Write_Mvb_Administrator =    \
{                                                                           \
    tcn_ma_service_call_Write_Mvb_Administrator,                            \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  Variables Service - Read_Ports_Configuration
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Read_Ports_Configuration
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                               mm_result    = MM_OK;
    TNM_MSG_Call_Read_Ports_Configuration   *call_msg;
    TNM_MSG_Reply_Read_Ports_Configuration  *reply_msg;
    AS_RESULT                               as_result;
    ENUM8                                   link_type    = \
                                                SV_LINK_TYPE_UNKNOWN;
    STRING32                                link_name;
    UNSIGNED32                              msg_size     = 0;
    UNSIGNED16                              nr_ports;
    WORD16                                  port_address;
    AP_RESULT                               ap_result;
    PD_PRT_ATTR                             prt_attr;
    TNM_PORT_OBJECT                         *ports_list;
    void                                    *temp;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_Read_Ports_Configuration*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_Read_Ports_Configuration' */
    if (call_msg_size != sizeof(TNM_MSG_Call_Read_Ports_Configuration))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    /* get type of link                                                    */
    /* link reference: element 'bus_id' of 'Call_Read_Ports_Configuration' */
    if (MM_OK == mm_result)
    {
        as_result =                         \
            as_read_link_descriptor         \
            (                               \
                (ENUM8)call_msg->bus_id,    \
                &link_type,                 \
                &link_name                  \
            );
        if (AS_OK != as_result)
        {
            mm_result = (MM_RESULT)MM_BUS_HW_BAD;
        } /* if (AS_OK != as_result) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_Read_Ports_Configuration' */
    if (MM_OK == mm_result)
    {
        msg_size  = \
            (UNSIGNED32)sizeof(TNM_MSG_Reply_Read_Ports_Configuration);
        msg_size -= (UNSIGNED32)sizeof(TNM_PORT_OBJECT); /* dynamic size */
        reply_msg = \
            (TNM_MSG_Reply_Read_Ports_Configuration*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* fill ports list (TNM_PORT_OBJECT)   */
    /* of 'Reply_Read_Ports_Configuration' */
    nr_ports     = 0;
    port_address = 0;
    while ((port_address < pd_prt_port_number_max) && (MM_OK == mm_result))
    {
        ap_result =                         \
            ap_port_manage                  \
            (                               \
                (ENUM8)call_msg->bus_id,    \
                (WORD16)port_address,       \
                (ENUM16)PD_PRT_CMD_STATUS,  \
                (PD_PRT_ATTR*)&prt_attr     \
            );

        if (AP_OK == ap_result)
        {
            /* re-allocate memory for 'Reply_Read_Ports_Configuration' */
            msg_size += (UNSIGNED32)sizeof(TNM_PORT_OBJECT);
            reply_msg = (TNM_MSG_Reply_Read_Ports_Configuration*) \
                malloc(msg_size);
            if (NULL == reply_msg)
            {
                mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
            } /* if (NULL == reply_msg) */

            if (MM_OK == mm_result)
            {
                ports_list = \
                    (TNM_PORT_OBJECT*)&reply_msg->ports_list[nr_ports];
                ports_list->bus_id       = call_msg->bus_id;
                ports_list->port_address = port_address;
                ports_list->f_code       = TNM_PORT_OBJECT_F_CODE_PWD;
                ports_list->port_config  = \
                    (BITSET16)(prt_attr.port_config >> 4);
                if (SV_LINK_TYPE_MVB == link_type)
                {
                    ports_list->port_size = 0;
                    switch (prt_attr.port_size)
                    {
                        case (2):
                            ports_list->f_code = \
                                TNM_PORT_OBJECT_F_CODE_PD16;
                            break;
                        case (4):
                            ports_list->f_code = \
                                TNM_PORT_OBJECT_F_CODE_PD32;
                            break;
                        case (8):
                            ports_list->f_code = \
                                TNM_PORT_OBJECT_F_CODE_PD64;
                            break;
                        case (16):
                            ports_list->f_code = \
                                TNM_PORT_OBJECT_F_CODE_PD128;
                            break;
                        case (32):
                            ports_list->f_code = \
                                TNM_PORT_OBJECT_F_CODE_PD256;
                            break;
                        default:
                            ports_list->port_size = prt_attr.port_size;
                    } /* switch (prt_attr.port_size) */
                } /* if (SV_LINK_TYPE_MVB == link_type) */
                else
                {
                    ports_list->port_size = prt_attr.port_size;
                } /* else */

                /* swap bytes of 'TNM_PORT_OBJECT'                         */
                /* NOTE:                                                   */
                /* Use temporary void pointer to avoid compiler warning    */
                /* 'dereferencing type-punned pointer will break strict-   */
                /* aliasing rules'.                                        */
                temp = (void*)&ports_list;
                TCN_MACRO_CONVERT_CPU_TO_BE32(temp);

                nr_ports++;

            } /* if (MM_OK == mm_result) */
        } /* if (AP_OK == ap_result) */

        port_address++;
    } /* while ((port_address < pd_prt_port_number_max) && (...)) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Read_Ports_Configuration' */
        reply_msg->tnm_key  = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code = (ENUM8)READ_PORTS_CONFIGURATION;
        reply_msg->nr_ports = (UNSIGNED16)nr_ports;

        /* swap bytes of 'Reply_Read_Ports_Configuration' */
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->nr_ports);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Read_Ports_Configuration */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Read_Ports_Configuration =   \
{                                                                           \
    tcn_ma_service_call_Read_Ports_Configuration,                           \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  Messages Service - Read_Messenger_Status
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Read_Messenger_Status
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                           mm_result  = MM_OK;
    TNM_MSG_Call_Read_Messenger_Status  *call_msg;
    TNM_MSG_Reply_Read_Messenger_Status *reply_msg;
    UNSIGNED32                          msg_size   = 0;
    AM_RESULT                           am_result;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_Read_Messenger_Status*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_Read_Messenger_Status' */
    if (call_msg_size != sizeof(TNM_MSG_Call_Read_Messenger_Status))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    /* allocate and clear memory for 'Reply_Read_Messenger_Status' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_Read_Messenger_Status);
        reply_msg = (TNM_MSG_Reply_Read_Messenger_Status*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Read_Messenger_Status' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)READ_MESSENGER_STATUS;
        reply_msg->reserved1 = (WORD16)0;

        /* read messenger status and fill 'Reply_Read_Messenger_Status' */
        am_result =                                                 \
            am_read_messenger_status                                \
            (                                                       \
                (AM_MESSENGER_STATUS*)&reply_msg->messenger_name    \
            );
        if (AM_OK != am_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AM_OK != am_result) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Reply_Read_Messenger_Status' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->messages_sent);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->messages_received);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->packets_sent);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->packet_retries);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->multicast_retries);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Read_Messenger_Status */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Read_Messenger_Status =      \
{                                                                           \
    tcn_ma_service_call_Read_Messenger_Status,                              \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  Messages Service - Write_Messenger_Control
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Write_Messenger_Control
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                               mm_result  = MM_OK;
    TNM_MSG_Call_Write_Messenger_Control    *call_msg;
    TNM_MSG_Reply_Write_Messenger_Control   *reply_msg;
    UNSIGNED32                              msg_size   = 0;
    AM_RESULT                               am_result;

    call_msg  = (TNM_MSG_Call_Write_Messenger_Control*)call_msg_adr;
    reply_msg = NULL;

    /* check station reservation */
    mm_result = tcn_ma_check_station_reservation(manager_address);

    if (MM_OK == mm_result)
    {
        *reply_msg_adr  = (void*)NULL;
        *reply_msg_size = (UNSIGNED32)0;

        /* check size of 'Call_Write_Messenger_Control' */
        if (call_msg_size != sizeof(TNM_MSG_Call_Write_Messenger_Control))
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != ...) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_Write_Messenger_Control' */
    if (MM_OK == mm_result)
    {
        msg_size  = \
            (UNSIGNED32)sizeof(TNM_MSG_Reply_Write_Messenger_Control);
        reply_msg = (TNM_MSG_Reply_Write_Messenger_Control*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* write messenger status (use 'Call_Write_Messenger_Control") */
    if (MM_OK == mm_result)
    {
        am_result =                                                 \
            am_write_messenger_control                              \
            (                                                       \
                *(AM_MESSENGER_CONTROL*)&call_msg->messenger_name   \
            );
        if (AM_OK != am_result)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (AM_OK != am_result) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Write_Messenger_Control' */
        reply_msg->tnm_key  = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code = (ENUM8)WRITE_MESSENGER_CONTROL;

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Write_Messenger_Control */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Write_Messenger_Control =    \
{                                                                           \
    tcn_ma_service_call_Write_Messenger_Control,                            \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  Messages Service - Read_Function_Directory
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Read_Function_Directory
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                               mm_result      = MM_OK;
    TNM_MSG_Call_Read_Function_Directory    *call_msg;
    TNM_MSG_Reply_Read_Function_Directory   *reply_msg;
    UNSIGNED32                              msg_size       = 0;
    UNSIGNED16                              nr_functions;
    UNSIGNED8                               function;
    UNSIGNED8                               station;
    AM_RESULT                               am_result;
    AM_DIR_ENTRY                            *function_list;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_Read_Function_Directory*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_Read_Function_Directory' */
    if (call_msg_size != sizeof(TNM_MSG_Call_Read_Function_Directory))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    /* allocate and clear memory for 'Reply_Read_Function_Directory' */
    if (MM_OK == mm_result)
    {
        msg_size  = \
            (UNSIGNED32)sizeof(TNM_MSG_Reply_Read_Function_Directory);
        msg_size -= (UNSIGNED32)sizeof(AM_DIR_ENTRY); /* dynamic size */
        reply_msg = (TNM_MSG_Reply_Read_Function_Directory*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* fill function directory entries (AM_DIR_ENTRY) */
    /* of 'Reply_Read_Function_Directory'             */
    nr_functions = 0;
    function     = 0;
    while ((255 > function) && (MM_OK == mm_result))
    {
        am_result =             \
            am_get_dir_entry    \
            (                   \
                function,       \
                &station        \
            );

        if ((AM_OK == am_result) && (AM_UNKNOWN != station))
        {
            /* re-allocate memory for 'Reply_Read_Function_Directory' */
            msg_size += (UNSIGNED32)sizeof(AM_DIR_ENTRY);
            reply_msg = (TNM_MSG_Reply_Read_Function_Directory*) \
                malloc(msg_size);
            if (NULL == reply_msg)
            {
                mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
            } /* if (NULL == reply_msg) */

            if (MM_OK == mm_result)
            {
                function_list = \
                    (AM_DIR_ENTRY*)&reply_msg->function_list[nr_functions];
                function_list->function = (UNSIGNED8)function;
                function_list->station  = (UNSIGNED8)station;

                nr_functions++;

            } /* if (MM_OK == mm_result) */
        } /* if ((AM_OK == am_result) && (AM_UNKNOWN != station)) */

        function++;
    } /* while ((255 > function) && (MM_OK == mm_result)) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Read_Function_Directory' */
        reply_msg->tnm_key      = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code     = (ENUM8)READ_FUNCTION_DIRECTORY;
        reply_msg->reserved1    = (WORD8)0;
        reply_msg->nr_functions = (UNSIGNED8)nr_functions;

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Read_Function_Directory */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Read_Function_Directory =    \
{                                                                           \
    tcn_ma_service_call_Read_Function_Directory,                            \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};


/* --------------------------------------------------------------------------
 *  Messages Service - Write_Function_Directory
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_Write_Function_Directory
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                               mm_result  = MM_OK;
    TNM_MSG_Call_Write_Function_Directory   *call_msg;
    TNM_MSG_Reply_Write_Function_Directory  *reply_msg;
    UNSIGNED32                              msg_size   = 0;
    AM_RESULT                               am_result;

    call_msg  = (TNM_MSG_Call_Write_Function_Directory*)call_msg_adr;
    reply_msg = NULL;

    /* check station reservation */
    mm_result = tcn_ma_check_station_reservation(manager_address);

    if (MM_OK == mm_result)
    {
        *reply_msg_adr  = (void*)NULL;
        *reply_msg_size = (UNSIGNED32)0;

        /* check min. size of 'Call_Write_Function_Directory' */
        msg_size  = \
            (UNSIGNED32)sizeof(TNM_MSG_Call_Write_Function_Directory);
        msg_size -= (UNSIGNED32)sizeof(AM_DIR_ENTRY); /* dynamic size */
        if (call_msg_size < msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size < msg_size) */
    } /* if (MM_OK == mm_result) */

    /* check size of 'Call_Write_Function_Directory' */
    if (MM_OK == mm_result)
    {
        msg_size += \
            (UNSIGNED32)(call_msg->nr_functions * sizeof(AM_DIR_ENTRY));
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_Write_Function_Directory' */
    if (MM_OK == mm_result)
    {
        msg_size  = \
            (UNSIGNED32)sizeof(TNM_MSG_Reply_Write_Function_Directory);
        reply_msg = \
            (TNM_MSG_Reply_Write_Function_Directory*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    /* check element 'clear_directory' of 'Call_Write_Function_Directory' */
    if (MM_OK == mm_result)
    {
        if (1 == call_msg->clear_directory)
        {
            /* clear function directory entries */
            am_result = am_clear_dir();
            if (AM_OK != am_result)
            {
                mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
            } /* if (AM_OK != am_result) */
        } /* if (1 == call_msg->clear_directory) */
    } /* if (MM_OK == mm_result) */

    /* put function directory entries (use function directory */
    /* entries of 'Call_Write_Function_Directory")            */
    if (MM_OK == mm_result)
    {
        if (0 < call_msg->nr_functions)
        {
            am_result =                                     \
                am_insert_dir_entries                       \
                (                                           \
                    (AM_DIR_ENTRY*)call_msg->function_list, \
                    (UNSIGNED8)call_msg->nr_functions       \
                );
            if (AM_OK != am_result)
            {
                mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
            } /* if (AM_OK != am_result) */
        } /* if (0 < call_msg->nr_functions) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_Write_Function_Directory' */
        reply_msg->tnm_key  = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code = (ENUM8)WRITE_FUNCTION_DIRECTORY;

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_Write_Function_Directory */

TCN_DECL_LOCAL  TCN_MA_SERVICE  tcn_ma_service_Write_Function_Directory =   \
{                                                                           \
    tcn_ma_service_call_Write_Function_Directory,                           \
    tcn_ma_service_close_default,                                           \
    0                                                                       \
};
#endif /* #ifndef TNM_FS_SIM */


#ifndef TCN_MA_FS_DISABLE
/* ==========================================================================
 *
 *  File System Services
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Local Variables
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL  BOOLEAN1    tcn_ma_FS_subscribed = FALSE;
TCN_DECL_LOCAL  UNSIGNED16  tcn_ma_FS_name_max   = 1024;


/* --------------------------------------------------------------------------
 *  File System Service - map 'errno' value from E* to TNM_FS_E*
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
INTEGER32
tcn_ma_FS_map_OS_to_TNM_errno (void)
{
    INTEGER32   result = TNM_FS_EIO;

    if (FALSE)
    {
        ;
        /* dummy if statement */
    } /* if (FALSE) */
#ifdef ENOERR       /* no error                                     */
    else if (ENOERR == errno)
    {
        result = TNM_FS_ENOERR;
    } /* else if (ENOERR == errno) */
#endif
#ifdef EPERM        /* operation not permitted                      */
    else if (EPERM == errno)
    {
        result = TNM_FS_EPERM;
    } /* else if (EPERM == errno) */
#endif
#ifdef ENOENT       /* no such file or directory                    */
    else if (ENOENT == errno)
    {
        result = TNM_FS_ENOENT;
    } /* else if (ENOENT == errno) */
#endif
#ifdef ESRCH        /* no such process                              */
    else if (ESRCH == errno)
    {
        result = TNM_FS_ESRCH;
    } /* else if (ESRCH == errno) */
#endif
#ifdef EINTR        /* interrupted function call                    */
    else if (EINTR == errno)
    {
        result = TNM_FS_EINTR;
    } /* else if (EINTR == errno) */
#endif
#ifdef EIO          /* input/output error                           */
    else if (EIO == errno)
    {
        result = TNM_FS_EIO;
    } /* else if (EIO == errno) */
#endif
#ifdef ENXIO        /* no such device or address                    */
    else if (ENXIO == errno)
    {
        result = TNM_FS_ENXIO;
    } /* else if (ENXIO == errno) */
#endif
#ifdef E2BIG        /* argument list too long                       */
    else if (E2BIG == errno)
    {
        result = TNM_FS_E2BIG;
    } /* else if (E2BIG == errno) */
#endif
#ifdef ENOEXEC      /* executable file format error                 */
    else if (ENOEXEC == errno)
    {
        result = TNM_FS_ENOEXEC;
    } /* else if (ENOEXEC == errno) */
#endif
#ifdef EBADF        /* bad file descriptor                          */
    else if (EBADF == errno)
    {
        result = TNM_FS_EBADF;
    } /* else if (EBADF == errno) */
#endif
#ifdef ECHILD       /* no child process                             */
    else if (ECHILD == errno)
    {
        result = TNM_FS_ECHILD;
    } /* else if (ECHILD == errno) */
#endif
#ifdef EAGAIN       /* resource temporarily unavailable             */
    else if (EAGAIN == errno)
    {
        result = TNM_FS_EAGAIN;
    } /* else if (EAGAIN == errno) */
#endif
#ifdef ENOMEM       /* not enough space                             */
    else if (ENOMEM == errno)
    {
        result = TNM_FS_ENOMEM;
    } /* else if (ENOMEM == errno) */
#endif
#ifdef EACCES       /* permission denied                            */
    else if (EACCES == errno)
    {
        result = TNM_FS_EACCES;
    } /* else if (EACCES == errno) */
#endif
#ifdef EFAULT       /* bad address                                  */
    else if (EFAULT == errno)
    {
        result = TNM_FS_EFAULT;
    } /* else if (EFAULT == errno) */
#endif
#ifdef EBUSY        /* resource busy                                */
    else if (EBUSY == errno)
    {
        result = TNM_FS_EBUSY;
    } /* else if (EBUSY == errno) */
#endif
#ifdef EEXIST       /* file exists                                  */
    else if (EEXIST == errno)
    {
        result = TNM_FS_EEXIST;
    } /* else if (EEXIST == errno) */
#endif
#ifdef EXDEV        /* improper link                                */
    else if (EXDEV == errno)
    {
        result = TNM_FS_EXDEV;
    } /* else if (EXDEV == errno) */
#endif
#ifdef ENODEV       /* no such device                               */
    else if (ENODEV == errno)
    {
        result = TNM_FS_ENODEV;
    } /* else if (ENODEV == errno) */
#endif
#ifdef ENOTDIR      /* not a directory                              */
    else if (ENOTDIR == errno)
    {
        result = TNM_FS_ENOTDIR;
    } /* else if (ENOTDIR == errno) */
#endif
#ifdef EISDIR       /* is a directory                               */
    else if (EISDIR == errno)
    {
        result = TNM_FS_EISDIR;
    } /* else if (EISDIR == errno) */
#endif
#ifdef EINVAL       /* invalid argument                             */
    else if (EINVAL == errno)
    {
        result = TNM_FS_EINVAL;
    } /* else if (EINVAL == errno) */
#endif
#ifdef ENFILE       /* too many files open in system                */
    else if (ENFILE == errno)
    {
        result = TNM_FS_ENFILE;
    } /* else if (ENFILE == errno) */
#endif
#ifdef EMFILE       /* too many open files                          */
    else if (EMFILE == errno)
    {
        result = TNM_FS_EMFILE;
    } /* else if (EMFILE == errno) */
#endif
#ifdef ENOTTY       /* inappropriate I/O control operation          */
    else if (ENOTTY == errno)
    {
        result = TNM_FS_ENOTTY;
    } /* else if (ENOTTY == errno) */
#endif
#ifdef ETXTBSY      /* text file busy                               */
    else if (ETXTBSY == errno)
    {
        result = TNM_FS_ETXTBSY;
    } /* else if (ETXTBSY == errno) */
#endif
#ifdef EFBIG        /* file too large                               */
    else if (EFBIG == errno)
    {
        result = TNM_FS_EFBIG;
    } /* else if (EFBIG == errno) */
#endif
#ifdef ENOSPC       /* no space left on a device                    */
    else if (ENOSPC == errno)
    {
        result = TNM_FS_ENOSPC;
    } /* else if (ENOSPC == errno) */
#endif
#ifdef ESPIPE       /* invalid seek                                 */
    else if (ESPIPE == errno)
    {
        result = TNM_FS_ESPIPE;
    } /* else if (ESPIPE == errno) */
#endif
#ifdef EROFS        /* read-only file system                        */
    else if (EROFS == errno)
    {
        result = TNM_FS_EROFS;
    } /* else if (EROFS == errno) */
#endif
#ifdef EMLINK       /* too many links                               */
    else if (EMLINK == errno)
    {
        result = TNM_FS_EMLINK;
    } /* else if (EMLINK == errno) */
#endif
#ifdef EPIPE        /* broken pipe                                  */
    else if (EPIPE == errno)
    {
        result = TNM_FS_EPIPE;
    } /* else if (EPIPE == errno) */
#endif
#ifdef ERANGE       /* result too large or too small                */
    else if (ERANGE == errno)
    {
        result = TNM_FS_ERANGE;
    } /* else if (ERANGE == errno) */
#endif
#ifdef ENOSYS       /* function not implemented                     */
    else if (ENOSYS == errno)
    {
        result = TNM_FS_ENOSYS;
    } /* else if (ENOSYS == errno) */
#endif
#ifdef ENOTEMPTY    /* directory not empty                          */
    else if (ENOTEMPTY == errno)
    {
        result = TNM_FS_ENOTEMPTY;
    } /* else if (ENOTEMPTY == errno) */
#endif
#ifdef ENAMETOOLONG /* filename too long                            */
    else if (ENAMETOOLONG == errno)
    {
        result = TNM_FS_ENAMETOOLONG;
    } /* else if (ENAMETOOLONG == errno) */
#endif
#ifdef ELOOP        /* too many levels of symbolic links            */
    else if (ELOOP == errno)
    {
        result = TNM_FS_ELOOP;
    } /* else if (ELOOP == errno) */
#endif
#ifdef ENOTSUP      /* not supported                                */
    else if (ENOTSUP == errno)
    {
        result = TNM_FS_ENOTSUP;
    } /* else if (ENOTSUP == errno) */
#endif
#ifdef EOVERFLOW    /* value too large to be stored in data type    */
    else if (EOVERFLOW == errno)
    {
        result = TNM_FS_EOVERFLOW;
    } /* else if (EOVERFLOW == errno) */
#endif

    return(result);

} /* tcn_ma_FS_map_OS_to_TNM_errno */


/* --------------------------------------------------------------------------
 *  File System Service - map 'oflag' bits from TNM_FS_O_xxx to O_xxx
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
INTEGER32
tcn_ma_FS_map_TNM_to_OS_oflag
(
    INTEGER32   in_oflag
)
{
    INTEGER32   out_oflag = 0;

    /* file access modes */
#ifdef O_RDONLY
    if (TNM_FS_O_RDONLY == (in_oflag & TNM_FS_O_RDONLY))
    {
        out_oflag |= O_RDONLY;
    } /* if (TNM_FS_O_RDONLY == (in_oflag & TNM_FS_O_RDONLY)) */
#endif
#ifdef O_WRONLY
    if (TNM_FS_O_WRONLY == (in_oflag & TNM_FS_O_WRONLY))
    {
        out_oflag |= O_WRONLY;
    } /* if (TNM_FS_O_WRONLY == (in_oflag & TNM_FS_O_WRONLY)) */
#endif
#ifdef O_RDWR
    if (TNM_FS_O_RDWR == (in_oflag & TNM_FS_O_RDWR))
    {
        out_oflag |= O_RDWR;
    } /* if (TNM_FS_O_RDWR == (in_oflag & TNM_FS_O_RDWR)) */
#endif

    /* file mode flags */
#ifdef O_CREAT
    if (TNM_FS_O_CREAT == (in_oflag & TNM_FS_O_CREAT))
    {
        out_oflag |= O_CREAT;
    } /* if (TNM_FS_O_CREAT == (in_oflag & TNM_FS_O_CREAT)) */
#endif
#ifdef O_EXCL
    if (TNM_FS_O_EXCL == (in_oflag & TNM_FS_O_EXCL))
    {
        out_oflag |= O_EXCL;
    } /* if (TNM_FS_O_EXCL == (in_oflag & TNM_FS_O_EXCL)) */
#endif
#ifdef O_NOCTTY
    if (TNM_FS_O_NOCTTY == (in_oflag & TNM_FS_O_NOCTTY))
    {
        out_oflag |= O_NOCTTY;
    } /* if (TNM_FS_O_NOCTTY == (in_oflag & TNM_FS_O_NOCTTY)) */
#endif
#ifdef O_TRUNC
    if (TNM_FS_O_TRUNC == (in_oflag & TNM_FS_O_TRUNC))
    {
        out_oflag |= O_TRUNC;
    } /* if (TNM_FS_O_TRUNC == (in_oflag & TNM_FS_O_TRUNC)) */
#endif

    /* file status flags */
#ifdef O_APPEND
    if (TNM_FS_O_APPEND == (in_oflag & TNM_FS_O_APPEND))
    {
        out_oflag |= O_APPEND;
    } /* if (TNM_FS_O_APPEND == (in_oflag & TNM_FS_O_APPEND)) */
#endif
#ifdef O_DSYNC
    if (TNM_FS_O_DSYNC == (in_oflag & TNM_FS_O_DSYNC))
    {
        out_oflag |= O_DSYNC;
    } /* if (TNM_FS_O_DSYNC == (in_oflag & TNM_FS_O_DSYNC)) */
#endif
#ifdef O_RSYNC
    if (TNM_FS_O_RSYNC == (in_oflag & TNM_FS_O_RSYNC))
    {
        out_oflag |= O_RSYNC;
    } /* if (TNM_FS_O_RSYNC == (in_oflag & TNM_FS_O_RSYNC)) */
#endif
#ifdef O_SYNC
    if (TNM_FS_O_SYNC == (in_oflag & TNM_FS_O_SYNC))
    {
        out_oflag |= O_SYNC;
    } /* if (TNM_FS_O_SYNC == (in_oflag & TNM_FS_O_SYNC)) */
#endif

    return(out_oflag);

} /* tcn_ma_FS_map_TNM_to_OS_oflag */


/* --------------------------------------------------------------------------
 *  File System Service - map type 'mode_t' bits from TNM_FS_SI* to S_I*
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
UNSIGNED32
tcn_ma_FS_map_TNM_to_OS_mode_bit
(
    UNSIGNED32  in_mode
)
{
    UNSIGNED32  out_mode = 0;

    /* file permissions by owner */
#ifdef S_IRUSR
    if (TNM_FS_S_IRUSR == (in_mode & TNM_FS_S_IRUSR))
    {
        out_mode |= S_IRUSR;
    } /* if (TNM_FS_S_IRUSR == (in_mode & TNM_FS_S_IRUSR)) */
#endif
#ifdef S_IWUSR
    if (TNM_FS_S_IWUSR == (in_mode & TNM_FS_S_IWUSR))
    {
        out_mode |= S_IWUSR;
    } /* if (TNM_FS_S_IWUSR == (in_mode & TNM_FS_S_IWUSR)) */
#endif
#ifdef S_IXUSR
    if (TNM_FS_S_IXUSR == (in_mode & TNM_FS_S_IXUSR))
    {
        out_mode |= S_IXUSR;
    } /* if (TNM_FS_S_IXUSR == (in_mode & TNM_FS_S_IXUSR)) */
#endif
#ifdef S_IRWXU
    if (TNM_FS_S_IRWXU == (in_mode & TNM_FS_S_IRWXU))
    {
        out_mode |= S_IRWXU;
    } /* if (TNM_FS_S_IRWXU == (in_mode & TNM_FS_S_IRWXU)) */
#endif
#ifdef S_IREAD
    if (TNM_FS_S_IREAD == (in_mode & TNM_FS_S_IREAD))
    {
        out_mode |= S_IREAD;
    } /* if (TNM_FS_S_IREAD == (in_mode & TNM_FS_S_IREAD)) */
#endif
#ifdef S_IWRITE
    if (TNM_FS_S_IWRITE == (in_mode & TNM_FS_S_IWRITE))
    {
        out_mode |= S_IWRITE;
    } /* if (TNM_FS_S_IWRITE == (in_mode & TNM_FS_S_IWRITE)) */
#endif
#ifdef S_IEXEC
    if (TNM_FS_S_IEXEC == (in_mode & TNM_FS_S_IEXEC))
    {
        out_mode |= S_IEXEC;
    } /* if (TNM_FS_S_IEXEC == (in_mode & TNM_FS_S_IEXEC)) */
#endif

    /* file permissions by group */
#ifdef S_IRGRP
    if (TNM_FS_S_IRGRP == (in_mode & TNM_FS_S_IRGRP))
    {
        out_mode |= S_IRGRP;
    } /* if (TNM_FS_S_IRGRP == (in_mode & TNM_FS_S_IRGRP)) */
#endif
#ifdef S_IWGRP
    if (TNM_FS_S_IWGRP == (in_mode & TNM_FS_S_IWGRP))
    {
        out_mode |= S_IWGRP;
    } /* if (TNM_FS_S_IWGRP == (in_mode & TNM_FS_S_IWGRP)) */
#endif
#ifdef S_IXGRP
    if (TNM_FS_S_IXGRP == (in_mode & TNM_FS_S_IXGRP))
    {
        out_mode |= S_IXGRP;
    } /* if (TNM_FS_S_IXGRP == (in_mode & TNM_FS_S_IXGRP)) */
#endif
#ifdef S_IRWXG
    if (TNM_FS_S_IRWXG == (in_mode & TNM_FS_S_IRWXG))
    {
        out_mode |= S_IRWXG;
    } /* if (TNM_FS_S_IRWXG == (in_mode & TNM_FS_S_IRWXG)) */
#endif

    /* file permissions by others */
#ifdef S_IROTH
    if (TNM_FS_S_IROTH == (in_mode & TNM_FS_S_IROTH))
    {
        out_mode |= S_IROTH;
    } /* if (TNM_FS_S_IROTH == (in_mode & TNM_FS_S_IROTH)) */
#endif
#ifdef S_IWOTH
    if (TNM_FS_S_IWOTH == (in_mode & TNM_FS_S_IWOTH))
    {
        out_mode |= S_IWOTH;
    } /* if (TNM_FS_S_IWOTH == (in_mode & TNM_FS_S_IWOTH)) */
#endif
#ifdef S_IXOTH
    if (TNM_FS_S_IXOTH == (in_mode & TNM_FS_S_IXOTH))
    {
        out_mode |= S_IXOTH;
    } /* if (TNM_FS_S_IXOTH == (in_mode & TNM_FS_S_IXOTH)) */
#endif
#ifdef S_IRWXO
    if (TNM_FS_S_IRWXO == (in_mode & TNM_FS_S_IRWXO))
    {
        out_mode |= S_IRWXO;
    } /* if (TNM_FS_S_IRWXO == (in_mode & TNM_FS_S_IRWXO)) */
#endif

    /* miscellaneous in_mode bits */
#ifdef S_ISUID
    if (TNM_FS_S_ISUID == (in_mode & TNM_FS_S_ISUID))
    {
        out_mode |= S_ISUID;
    } /* if (TNM_FS_S_ISUID == (in_mode & TNM_FS_S_ISUID)) */
#endif
#ifdef S_ISGID
    if (TNM_FS_S_ISGID == (in_mode & TNM_FS_S_ISGID))
    {
        out_mode |= S_ISGID;
    } /* if (TNM_FS_S_ISGID == (in_mode & TNM_FS_S_ISGID)) */
#endif
#ifdef S_ISVTX
    if (TNM_FS_S_ISVTX == (in_mode & TNM_FS_S_ISVTX))
    {
        out_mode |= S_ISVTX;
    } /* if (TNM_FS_S_ISVTX == (in_mode & TNM_FS_S_ISVTX)) */
#endif

    return(out_mode);

} /* tcn_ma_FS_map_TNM_to_OS_mode_bit */


/* --------------------------------------------------------------------------
 *  File System Service - map type 'mode_t' value from S_IF* to TNM_FS_S_IF*
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
UNSIGNED32
tcn_ma_FS_map_OS_to_TNM_mode_val
(
    UNSIGNED32  in_mode
)
{
    UNSIGNED32  result = 0;

#ifdef S_IFMT
    in_mode = (UNSIGNED32)(in_mode & S_IFMT);

    if (FALSE)
    {
        ;
        /* dummy if statement */
    } /* if (FALSE) */
#ifdef S_IFREG /* regular */
    else if (S_IFREG == in_mode)
    {
        result = TNM_FS_S_IFREG;
    } /* else if (S_IFREG == in_mode) */
#endif
#ifdef S_IFDIR /* directory */
    else if (S_IFDIR == in_mode)
    {
        result = TNM_FS_S_IFDIR;
    } /* else if (S_IFDIR == in_mode) */
#endif
#ifdef S_IFLNK /* symbolic link */
    else if (S_IFLNK == in_mode)
    {
        result = TNM_FS_S_IFLNK;
    } /* else if (S_IFLNK == in_mode) */
#endif
#ifdef S_IFBLK /* block special */
    else if (S_IFBLK == in_mode)
    {
        result = TNM_FS_S_IFBLK;
    } /* else if (S_IFBLK == in_mode) */
#endif
#ifdef S_IFCHR /* character special */
    else if (S_IFCHR == in_mode)
    {
        result = TNM_FS_S_IFCHR;
    } /* else if (S_IFCHR == in_mode) */
#endif
#ifdef S_IFIFO /* FIFO special */
    else if (S_IFIFO == in_mode)
    {
        result = TNM_FS_S_IFIFO;
    } /* else if (S_IFIFO == in_mode) */
#endif
#ifdef S_IFSOCK /* socket */
    else if (S_IFSOCK == in_mode)
    {
        result = TNM_FS_S_IFSOCK;
    } /* else if (S_IFSOCK == in_mode) */
#endif

#endif /* S_IFMT */


    if (0 == result)
    {
        if (FALSE)
        {
            ;
            /* dummy if statement */
        } /* if (FALSE) */
#ifdef S_ISREG /* regular */
        else if (S_ISREG(in_mode))
        {
            result = TNM_FS_S_IFREG;
        } /* else if (S_ISREG(in_mode)) */
#endif
#ifdef S_ISDIR /* directory */
        else if (S_ISDIR(in_mode))
        {
            result = TNM_FS_S_IFDIR;
        } /* else if (S_ISDIR(in_mode)) */
#endif
#ifdef S_ISLNK /* symbolic link */
        else if (S_ISLNK(in_mode))
        {
            result = TNM_FS_S_IFLNK;
        } /* else if (S_ISLNK(in_mode)) */
#endif
#ifdef S_ISBLK /* block special */
        else if (S_ISBLK(in_mode))
        {
            result = TNM_FS_S_IFBLK;
        } /* else if (S_ISBLK(in_mode)) */
#endif
#ifdef S_ISCHR /* character special */
        else if (S_ISCHR(in_mode))
        {
            result = TNM_FS_S_IFCHR;
        } /* else if (S_ISCHR(in_mode)) */
#endif
#ifdef S_ISFIFO /* FIFO special */
        else if (S_ISFIFO(in_mode))
        {
            result = TNM_FS_S_IFIFO;
        } /* else if (S_ISFIFO(in_mode)) */
#endif
#ifdef S_ISSOCK /* socket */
        else if (S_ISSOCK(in_mode))
        {
            result = TNM_FS_S_IFSOCK;
        } /* else if (S_ISSOCK(in_mode)) */
#endif
    } /* if (0 == result) */


    return(result);

} /* tcn_ma_FS_map_OS_to_TNM_mode_val */


/* --------------------------------------------------------------------------
 *  File System Service - map type 'mode_t' bits from S_I* to TNM_FS_S_I*
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
UNSIGNED32
tcn_ma_FS_map_OS_to_TNM_mode_bit
(
    UNSIGNED32  in_mode
)
{
    UNSIGNED32  out_mode = 0;

    /* file permissions by owner */
#ifdef S_IRUSR
    if (S_IRUSR == (in_mode & S_IRUSR))
    {
        out_mode |= TNM_FS_S_IRUSR;
    } /* if (S_IRUSR == (in_mode & S_IRUSR)) */
#endif
#ifdef S_IWUSR
    if (S_IWUSR == (in_mode & S_IWUSR))
    {
        out_mode |= TNM_FS_S_IWUSR;
    } /* if (S_IWUSR == (in_mode & S_IWUSR)) */
#endif
#ifdef S_IXUSR
    if (S_IXUSR == (in_mode & S_IXUSR))
    {
        out_mode |= TNM_FS_S_IXUSR;
    } /* if (S_IXUSR == (in_mode & S_IXUSR)) */
#endif
#ifdef S_IRWXU
    if (S_IRWXU == (in_mode & S_IRWXU))
    {
        out_mode |= TNM_FS_S_IRWXU;
    } /* if (S_IRWXU == (in_mode & S_IRWXU)) */
#endif
#ifdef S_IREAD
    if (S_IREAD == (in_mode & S_IREAD))
    {
        out_mode |= TNM_FS_S_IREAD;
    } /* if (S_IREAD == (in_mode & S_IREAD)) */
#endif
#ifdef S_IWRITE
    if (S_IWRITE == (in_mode & S_IWRITE))
    {
        out_mode |= TNM_FS_S_IWRITE;
    } /* if (S_IWRITE == (in_mode & S_IWRITE)) */
#endif
#ifdef S_IEXEC
    if (S_IEXEC == (in_mode & S_IEXEC))
    {
        out_mode |= TNM_FS_S_IEXEC;
    } /* if (S_IEXEC == (in_mode & S_IEXEC)) */
#endif

    /* file permissions by group */
#ifdef S_IRGRP
    if (S_IRGRP == (in_mode & S_IRGRP))
    {
        out_mode |= TNM_FS_S_IRGRP;
    } /* if (S_IRGRP == (in_mode & S_IRGRP)) */
#endif
#ifdef S_IWGRP
    if (S_IWGRP == (in_mode & S_IWGRP))
    {
        out_mode |= TNM_FS_S_IWGRP;
    } /* if (S_IWGRP == (in_mode & S_IWGRP)) */
#endif
#ifdef S_IXGRP
    if (S_IXGRP == (in_mode & S_IXGRP))
    {
        out_mode |= TNM_FS_S_IXGRP;
    } /* if (S_IXGRP == (in_mode & S_IXGRP)) */
#endif
#ifdef S_IRWXG
    if (S_IRWXG == (in_mode & S_IRWXG))
    {
        out_mode |= TNM_FS_S_IRWXG;
    } /* if (S_IRWXG == (in_mode & S_IRWXG)) */
#endif

    /* file permissions by others */
#ifdef S_IROTH
    if (S_IROTH == (in_mode & S_IROTH))
    {
        out_mode |= TNM_FS_S_IROTH;
    } /* if (S_IROTH == (in_mode & S_IROTH)) */
#endif
#ifdef S_IWOTH
    if (S_IWOTH == (in_mode & S_IWOTH))
    {
        out_mode |= TNM_FS_S_IWOTH;
    } /* if (S_IWOTH == (in_mode & S_IWOTH)) */
#endif
#ifdef S_IXOTH
    if (S_IXOTH == (in_mode & S_IXOTH))
    {
        out_mode |= TNM_FS_S_IXOTH;
    } /* if (S_IXOTH == (in_mode & S_IXOTH)) */
#endif
#ifdef S_IRWXO
    if (S_IRWXO == (in_mode & S_IRWXO))
    {
        out_mode |= TNM_FS_S_IRWXO;
    } /* if (S_IRWXO == (in_mode & S_IRWXO)) */
#endif

    /* miscellaneous in_mode bits */
#ifdef S_ISUID
    if (S_ISUID == (in_mode & S_ISUID))
    {
        out_mode |= TNM_FS_S_ISUID;
    } /* if (S_ISUID == (in_mode & S_ISUID)) */
#endif
#ifdef S_ISGID
    if (S_ISGID == (in_mode & S_ISGID))
    {
        out_mode |= TNM_FS_S_ISGID;
    } /* if (S_ISGID == (in_mode & S_ISGID)) */
#endif
#ifdef S_ISVTX
    if (S_ISVTX == (in_mode & S_ISVTX))
    {
        out_mode |= TNM_FS_S_ISVTX;
    } /* if (S_ISVTX == (in_mode & S_ISVTX)) */
#endif

    return(out_mode);

} /* tcn_ma_FS_map_OS_to_TNM_mode_bit */


/* --------------------------------------------------------------------------
 *  File System Service - map pathconf 'name' value from TNM_FS_PC_* to _PC_*
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
INTEGER32
tcn_ma_FS_map_TNM_to_OS_pathconf
(
    INTEGER32   name
)
{
    INTEGER32   result = 0;

    if (FALSE)
    {
        ;
        /* dummy if statement */
    } /* if (FALSE) */
#ifdef _PC_FILESIZEBITS
    else if (TNM_FS_PC_FILESIZEBITS == name)
    {
        result = _PC_FILESIZEBITS;
    } /* else if (TNM_FS_PC_FILESIZEBITS == name) */
#endif
#ifdef _PC_LINK_MAX
    else if (TNM_FS_PC_LINK_MAX == name)
    {
        result = _PC_LINK_MAX;
    } /* else if (TNM_FS_PC_LINK_MAX == name) */
#endif
#ifdef _PC_MAX_CANON
    else if (TNM_FS_PC_MAX_CANON == name)
    {
        result = _PC_MAX_CANON;
    } /* else if (TNM_FS_PC_MAX_CANON == name) */
#endif
#ifdef _PC_MAX_INPUT
    else if (TNM_FS_PC_MAX_INPUT == name)
    {
        result = _PC_MAX_INPUT;
    } /* else if (TNM_FS_PC_MAX_INPUT == name) */
#endif
#ifdef _PC_NAME_MAX
    else if (TNM_FS_PC_NAME_MAX == name)
    {
        result = _PC_NAME_MAX;
    } /* else if (TNM_FS_PC_NAME_MAX == name) */
#endif
#ifdef _PC_PATH_MAX
    else if (TNM_FS_PC_PATH_MAX == name)
    {
        result = _PC_PATH_MAX;
    } /* else if (TNM_FS_PC_PATH_MAX == name) */
#endif
#ifdef _PC_PIPE_BUF
    else if (TNM_FS_PC_PIPE_BUF == name)
    {
        result = _PC_PIPE_BUF;
    } /* else if (TNM_FS_PC_PIPE_BUF == name) */
#endif
#ifdef _PC_ALLOC_SIZE_MIN
    else if (TNM_FS_PC_ALLOC_SIZE_MIN == name)
    {
        result = _PC_ALLOC_SIZE_MIN;
    } /* else if (TNM_FS_PC_ALLOC_SIZE_MIN == name) */
#endif
#ifdef _PC_REC_INCR_XFER_SIZE
    else if (TNM_FS_PC_REC_INCR_XFER_SIZE == name)
    {
        result = _PC_REC_INCR_XFER_SIZE;
    } /* else if (TNM_FS_PC_REC_INCR_XFER_SIZE == name) */
#endif
#ifdef _PC_REC_MAX_XFER_SIZE
    else if (TNM_FS_PC_REC_MAX_XFER_SIZE == name)
    {
        result = _PC_REC_MAX_XFER_SIZE;
    } /* else if (TNM_FS_PC_REC_MAX_XFER_SIZE == name) */
#endif
#ifdef _PC_REC_MIN_XFER_SIZE
    else if (TNM_FS_PC_REC_MIN_XFER_SIZE == name)
    {
        result = _PC_REC_MIN_XFER_SIZE;
    } /* else if (TNM_FS_PC_REC_MIN_XFER_SIZE == name) */
#endif
#ifdef _PC_REC_XFER_ALIGN
    else if (TNM_FS_PC_REC_XFER_ALIGN == name)
    {
        result = _PC_REC_XFER_ALIGN;
    } /* else if (TNM_FS_PC_REC_XFER_ALIGN == name) */
#endif
#ifdef _PC_SYMLINK_MAX
    else if (TNM_FS_PC_SYMLINK_MAX == name)
    {
        result = _PC_SYMLINK_MAX;
    } /* else if (TNM_FS_PC_SYMLINK_MAX == name) */
#endif
#ifdef _PC_CHOWN_RESTRICTED
    else if (TNM_FS_PC_CHOWN_RESTRICTED == name)
    {
        result = _PC_CHOWN_RESTRICTED;
    } /* else if (TNM_FS_PC_CHOWN_RESTRICTED == name) */
#endif
#ifdef _PC_NO_TRUNC
    else if (TNM_FS_PC_NO_TRUNC == name)
    {
        result = _PC_NO_TRUNC;
    } /* else if (TNM_FS_PC_NO_TRUNC == name) */
#endif
#ifdef _PC_VDISABLE
    else if (TNM_FS_PC_VDISABLE == name)
    {
        result = _PC_VDISABLE;
    } /* else if (TNM_FS_PC_VDISABLE == name) */
#endif
#ifdef _PC_ASYNC_IO
    else if (TNM_FS_PC_ASYNC_IO == name)
    {
        result = _PC_ASYNC_IO;
    } /* else if (TNM_FS_PC_ASYNC_IO == name) */
#endif
#ifdef _PC_PRIO_IO
    else if (TNM_FS_PC_PRIO_IO == name)
    {
        result = _PC_PRIO_IO;
    } /* else if (TNM_FS_PC_PRIO_IO == name) */
#endif
#ifdef _PC_SYNC_IO
    else if (TNM_FS_PC_SYNC_IO == name)
    {
        result = _PC_SYNC_IO;
    } /* else if (TNM_FS_PC_SYNC_IO == name) */
#endif

    return(result);

} /* tcn_ma_FS_map_TNM_to_OS_pathconf */


/* --------------------------------------------------------------------------
 *  File System Service - FS_access
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_access
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_access  *call_msg;
    TNM_MSG_Reply_FS_access *reply_msg;
    UNSIGNED32              msg_size;
    INTEGER32               amode      = 0;
    INTEGER32               status     = -1;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_access*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check min. size of 'Call_FS_access' */
    msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_FS_access);
    msg_size -= (UNSIGNED32)4; /* dynamic size */
    if (call_msg_size < msg_size)
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size < msg_size) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_access' */
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->amode);
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->string_size);

        /* check size of 'Call_FS_access' */
        msg_size += (UNSIGNED32)call_msg->string_size;
        msg_size += \
            (UNSIGNED32)(call_msg->string_size % 2); /* allign to 16-bit */
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_access' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_access);
        reply_msg = (TNM_MSG_Reply_FS_access*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_access' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_ACCESS;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        /* parameter 'amode' */
#ifdef R_OK
        if (call_msg->amode & TNM_FS_R_OK)
        {
            amode |= R_OK;
        } /* if (call_msg->amode & TNM_FS_R_OK) */
#endif
#ifdef W_OK
        if (call_msg->amode & TNM_FS_W_OK)
        {
            amode |= W_OK;
        } /* if (call_msg->amode & TNM_FS_W_OK) */
#endif
#ifdef X_OK
        if (call_msg->amode & TNM_FS_X_OK)
        {
            amode |= X_OK;
        } /* if (call_msg->amode & TNM_FS_X_OK) */
#endif
#ifdef F_OK
        if (call_msg->amode & TNM_FS_F_OK)
        {
            amode |= F_OK;
        } /* if (call_msg->amode & TNM_FS_F_OK) */
#endif

        if (1 < call_msg->string_size)
        {
            /* call POSIX procedure 'access' */
            status = (INTEGER32)access(call_msg->path, amode);

            /* fill 'Reply_FS_access' */
            reply_msg->status = (INTEGER32)status;
            if (-1 == status)
            {
                /* NOTE:                                    */
                /* eCos will translate ENOENT into EACCES   */
                /* if the F_OK bit is set.                  */
                reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
            } /* if (-1 == status) */
        } /* if (1 < call_msg->string_size) */
        else
        {
            /* fill 'Reply_FS_access' */
            reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOENT;
            reply_msg->status    = (INTEGER32)-1;
        } /* else  */

        /* swap bytes of 'Reply_FS_access' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->status);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_access */


/* --------------------------------------------------------------------------
 *  File System Service - FS_open
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_open
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_open    *call_msg;
    TNM_MSG_Reply_FS_open   *reply_msg;
    UNSIGNED32              msg_size;
    CHARACTER8              *path      = NULL;
    INTEGER32               oflag      = 0;
    UNSIGNED32              mode       = 0;
    INTEGER32               fd         = -1;

    call_msg  = (TNM_MSG_Call_FS_open*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check min. size of 'Call_FS_open' */
    msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_FS_open);
    msg_size -= (UNSIGNED32)4; /*  dynamic size */
    if (call_msg_size < msg_size)
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size < msg_size) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_open' */
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->oflag);
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->mode);
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->file_size);
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->fs_type);
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->fs_attr);
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->string_size);
    } /* if (MM_OK == mm_result) */

    /* check station reservation */
    if (MM_OK == mm_result)
    {
        if (TNM_FS_O_RDONLY != (call_msg->oflag & TNM_FS_O_ACCMODE))
        {
            mm_result = tcn_ma_check_station_reservation(manager_address);
        } /* if (TNM_FS_O_RDONLY != (call_msg->oflag & TNM_FS_O_ACCMODE)) */
    } /* if (MM_OK == mm_result) */

    /* check size of 'Call_FS_open' */
    if (MM_OK == mm_result)
    {
        msg_size += (UNSIGNED32)call_msg->string_size;
        msg_size += \
            (UNSIGNED32)(call_msg->string_size % 2); /* allign to 16-bit */
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_open' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_open);
        reply_msg = (TNM_MSG_Reply_FS_open*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_open' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_OPEN;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        if (1 < call_msg->string_size)
        {
            /* allocate and fill memory block with:         */
            /* - parameter 'path'                           */
            /* - terminating null byte for parameter 'path' */
            /* - alignment to 32-bit boundary               */
            /* - parameter 'key[4]' (fix value 'OPEN')      */
            /* - parameter 'file_size'                      */
            /* - parameter 'fs_type'                        */
            /* - parameter 'fs_attr'                        */
            /* - parameter 'checksum1' and 'checksum2'      */
            {
                UNSIGNED32  malloc_size;
                UNSIGNED32  index;
                UNSIGNED8   *p8_begin;
                UNSIGNED8   *p8_end;
                UNSIGNED8   *p8;
                UNSIGNED16  checksum;

                /* parameter 'path' including terminating null byte */
                malloc_size = (UNSIGNED32)call_msg->string_size;

                /* allign next parameter to 32-bit boundary     */
                /* (use terminating null byte as as fill value) */
                if (0 != (call_msg->string_size % 4))
                {
                    malloc_size += \
                        (UNSIGNED32)(4 - (call_msg->string_size % 4));
                } /* if (0 != (call_msg->string_size % 4)) */

                index = malloc_size;

                /* parameter 'key' */
                malloc_size += (UNSIGNED32)4;

                /* parameter 'file_size' */
                malloc_size += (UNSIGNED32)4;

                /* parameter 'fs_type' */
                malloc_size += (UNSIGNED32)4;

                /* parameter 'fs_attr' */
                malloc_size += (UNSIGNED32)4;

                /* parameter 'checksum1' and 'checksum2' */
                malloc_size += (UNSIGNED32)4;

                /* allocate and clear memory block */
                path = (CHARACTER8*)malloc(malloc_size);
                if (NULL == path)
                {
                    mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
                } /* if (NULL == path) */
                else
                {
                    memset((void*)path, 0, malloc_size);
                } /* else */

                if (MM_OK == mm_result)
                {
                    /* set begin-pointer for checksum */
                    p8_begin = (UNSIGNED8*)path;

                    /* copy parameter 'path' into memory block */
                    memcpy                      \
                    (                           \
                        (void*)path,            \
                        (void*)call_msg->path,  \
                        call_msg->string_size   \
                    );

                    /* set parameter 'key[4]' into memory block */
                    /*  (fix value 'OPEN')                      */
                    memcpy((void*)&path[index], "OPEN", 4);
                    index += (UNSIGNED32)4;

                    /* copy parameter 'file_size' into memory block */
                    *(UNSIGNED32*)&path[index] = call_msg->file_size;
                    index += (UNSIGNED32)4;

                    /* copy parameter 'fs_type' into memory block */
                    *(ENUM32*)&path[index] = call_msg->fs_type;
                    index += (UNSIGNED32)4;

                    /* copy parameter 'fs_attr' into memory block */
                    *(BITSET32*)&path[index] = call_msg->fs_attr;
                    index += (UNSIGNED32)4;

                    /* set end-pointer for checksum */
                    p8_end = (UNSIGNED8*)&path[index];

                    /* set parameter 'checksum1' into memory block */
                    checksum = 0;
                    p8       = p8_begin;
                    while (p8 < p8_end)
                    {
                        checksum = (UNSIGNED16)(checksum + *p8++);
                    } /* while (p8 < p8_end) */
                    *(UNSIGNED16*)&path[index] = checksum;
                    index += (UNSIGNED32)2;

                    /* set value 'checksum2' into memory block */
                    *(UNSIGNED16*)&path[index] = \
                        (UNSIGNED16)(0xFFFF - checksum);
                } /* if (MM_OK == mm_result) */
            }

            if (MM_OK == mm_result)
            {
                /* parameter 'oflag' */
                oflag = tcn_ma_FS_map_TNM_to_OS_oflag(call_msg->oflag);

                /* parameter 'mode' */
                mode = tcn_ma_FS_map_TNM_to_OS_mode_bit(call_msg->mode);

                /* call POSIX procedure 'open' */
                fd = (INTEGER32)open(path, oflag, mode);

                /* fill 'Reply_FS_open' */
                reply_msg->fd = (INTEGER32)fd;
                if (-1 == fd)
                {
                    reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
                } /* if (-1 == fd) */
            } /* if (MM_OK == mm_result) */

            /* release buffer */
            if (NULL != path)
            {
                free((void*)path);
            } /* if (NULL != path) */
        } /* if (1 < call_msg->string_size) */
        else
        {
            /* fill 'Reply_FS_open' */
            reply_msg->sys_errno = (INTEGER32)ENOENT;
            reply_msg->fd        = (INTEGER32)-1;
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Reply_FS_open' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->fd);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_open */


/* --------------------------------------------------------------------------
 *  File System Service - FS_close
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_close
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_close   *call_msg;
    TNM_MSG_Reply_FS_close  *reply_msg;
    UNSIGNED32              msg_size   = 0;
    INTEGER32               status     = -1;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_close*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_FS_close' */
    if (call_msg_size != sizeof(TNM_MSG_Call_FS_close))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_close' */
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->fd);
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_close' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_close);
        reply_msg = (TNM_MSG_Reply_FS_close*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_close' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_CLOSE;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        /* call POSIX procedure 'close' */
        status = (INTEGER32)close(call_msg->fd);

        /* fill 'Reply_FS_close' */
        reply_msg->status = (INTEGER32)status;
        if (-1 == status)
        {
            reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
        } /* if (-1 == status) */

        /* swap bytes of 'Reply_FS_close' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->status);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_close */


/* --------------------------------------------------------------------------
 *  File System Service - FS_read
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_read
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_read    *call_msg;
    TNM_MSG_Reply_FS_read   *reply_msg;
    UNSIGNED32              msg_size;
    INTEGER32               nbyte      = -1;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_read*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_FS_read' */
    if (call_msg_size != sizeof(TNM_MSG_Call_FS_read))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_read' */
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->fd);
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->nbyte);
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_read' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_read);
        msg_size -= (UNSIGNED32)4; /* dynamic size */
        msg_size += (UNSIGNED32)call_msg->nbyte;
        msg_size += (UNSIGNED32)(call_msg->nbyte % 2); /* allign to 16-bit */
        reply_msg = (TNM_MSG_Reply_FS_read*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_read' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_READ;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        /* call POSIX procedure 'read' */
        nbyte = (INTEGER32)     \
            read                \
            (                   \
                call_msg->fd,   \
                reply_msg->buf, \
                call_msg->nbyte \
            );

        /* fill 'Reply_FS_read' */
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_read);
        msg_size -= (UNSIGNED32)4; /* dynamic size */
        reply_msg->nbyte = (INTEGER32)nbyte;
        if (-1 == nbyte)
        {
            reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
        } /* if (-1 == nbyte) */
        else
        {
            msg_size += (UNSIGNED32)nbyte;
            msg_size += (UNSIGNED32)(nbyte % 2); /* allign to 16-bit */
        } /* else */

        /* swap bytes of 'Reply_FS_read' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->nbyte);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_read */


/* --------------------------------------------------------------------------
 *  File System Service - FS_write
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_write
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_write   *call_msg;
    TNM_MSG_Reply_FS_write  *reply_msg;
    UNSIGNED32              msg_size   = 0;
    INTEGER32               nbyte      = -1;

    call_msg  = (TNM_MSG_Call_FS_write*)call_msg_adr;
    reply_msg = NULL;

    /* check station reservation */
    mm_result = tcn_ma_check_station_reservation(manager_address);

    if (MM_OK == mm_result)
    {
        *reply_msg_adr  = (void*)NULL;
        *reply_msg_size = (UNSIGNED32)0;

        /* check min. size of 'Call_FS_write' */
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_FS_write);
        msg_size -= (UNSIGNED32)4; /*  dynamic size */
        if (call_msg_size < msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size < msg_size) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_write' */
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->fd);
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->nbyte);

        /* check size of 'Call_FS_write' */
        msg_size += (UNSIGNED32)call_msg->nbyte;
        msg_size += (UNSIGNED32)(call_msg->nbyte % 2); /* allign to 16-bit */
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_write' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_write);
        reply_msg = (TNM_MSG_Reply_FS_write*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_write' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_WRITE;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        /* call POSIX procedure 'write' */
        nbyte = (INTEGER32)     \
            write               \
            (                   \
                call_msg->fd,   \
                call_msg->buf,  \
                call_msg->nbyte \
            );

        /* fill 'Reply_FS_write' */
        reply_msg->nbyte = (INTEGER32)nbyte;
        if (-1 == nbyte)
        {
            reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
        } /* if (-1 == nbyte) */

        /* swap bytes of 'Reply_FS_write' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->nbyte);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_write */


/* --------------------------------------------------------------------------
 *  File System Service - FS_lseek
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_lseek
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_lseek   *call_msg;
    TNM_MSG_Reply_FS_lseek  *reply_msg;
    UNSIGNED32              msg_size   = 0;
    UNSIGNED32              offset     = (UNSIGNED32)-1;
    INTEGER32               whence     = SEEK_SET;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_lseek*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_FS_lseek' */
    if (call_msg_size != sizeof(TNM_MSG_Call_FS_lseek))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_lseek' */
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->fd);
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->offset);
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->whence);
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_lseek' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_lseek);
        reply_msg = (TNM_MSG_Reply_FS_lseek*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_lseek' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_LSEEK;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        /* parameter 'whence' */
#ifdef SEEK_SET
        if (call_msg->whence & TNM_FS_SEEK_SET)
        {
            whence |= SEEK_SET;
        } /* if (call_msg->whence & TNM_FS_SEEK_SET) */
#endif
#ifdef SEEK_CUR
        if (call_msg->whence & TNM_FS_SEEK_CUR)
        {
            whence |= SEEK_CUR;
        } /* if (call_msg->whence & TNM_FS_SEEK_CUR) */
#endif
#ifdef SEEK_END
        if (call_msg->whence & TNM_FS_SEEK_END)
        {
            whence |= SEEK_END;
        } /* if (call_msg->whence & TNM_FS_SEEK_END) */
#endif

        /* call POSIX procedure 'lseek' */
        offset = (UNSIGNED32)       \
            lseek                   \
            (                       \
                call_msg->fd,       \
                call_msg->offset,   \
                whence              \
            );

        /* fill 'Reply_FS_lseek' */
        reply_msg->offset = (UNSIGNED32)offset;
        if ((UNSIGNED32)-1 == offset)
        {
            reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
        } /* if ((UNSIGNED32)-1 == offset) */

        /* swap bytes of 'Reply_FS_lseek' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->offset);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_lseek */


/* --------------------------------------------------------------------------
 *  File System Service - FS_unlink
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_unlink
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_unlink  *call_msg;
    TNM_MSG_Reply_FS_unlink *reply_msg;
    UNSIGNED32              msg_size   = 0;
    INTEGER32               status     = -1;

    call_msg  = (TNM_MSG_Call_FS_unlink*)call_msg_adr;
    reply_msg = NULL;

    /* check station reservation */
    mm_result = tcn_ma_check_station_reservation(manager_address);

    if (MM_OK == mm_result)
    {
        *reply_msg_adr  = (void*)NULL;
        *reply_msg_size = (UNSIGNED32)0;

        /* check min. size of 'Call_FS_unlink' */
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_FS_unlink);
        msg_size -= (UNSIGNED32)4; /* dynamic size */
        if (call_msg_size < msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size < msg_size) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_unlink' */
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->string_size);

        /* check size of 'Call_FS_unlink' */
        msg_size += (UNSIGNED32)call_msg->string_size;
        msg_size += \
            (UNSIGNED32)(call_msg->string_size % 2); /* allign to 16-bit */
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_unlink' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_unlink);
        reply_msg = (TNM_MSG_Reply_FS_unlink*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_unlink' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_UNLINK;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        if (1 < call_msg->string_size)
        {
            /* call POSIX procedure 'unlink' */
            status = (INTEGER32)unlink(call_msg->path);

            /* fill 'Reply_FS_unlink' */
            reply_msg->status = (INTEGER32)status;
            if (-1 == status)
            {
                reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
            } /* if (-1 == status) */
        } /* if (1 < call_msg->string_size) */
        else
        {
            /* fill 'Reply_FS_unlink' */
            reply_msg->sys_errno = (INTEGER32)ENOENT;
            reply_msg->status    = (INTEGER32)-1;
        } /* else */

        /* swap bytes of 'Reply_FS_unlink' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->status);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_unlink */


/* --------------------------------------------------------------------------
 *  File System Service - FS_stat
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_stat
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result   = MM_OK;
    TNM_MSG_Call_FS_stat    *call_msg;
    TNM_MSG_Reply_FS_stat   *reply_msg;
    UNSIGNED32              msg_size;
    struct stat             stat_buf[2]; /* stat_buf[0] = struct stat */
                                         /* stat_buf[0] = extension   */
    struct stat             *p_stat     = &stat_buf[0];
    INTEGER32               status      = -1;
    ENUM32                  fs_type;
    BITSET32                fs_attr;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_stat*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check min. size of 'Call_FS_stat' */
    msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_FS_stat);
    msg_size -= (UNSIGNED32)4; /* dynamic size */
    if (call_msg_size < msg_size)
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size < msg_size) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_stat' */
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->string_size);

        /* check size of 'Call_FS_stat' */
        msg_size += (UNSIGNED32)call_msg->string_size;
        msg_size += \
            (UNSIGNED32)(call_msg->string_size % 2); /* allign to 16-bit */
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_stat' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_stat);
        reply_msg = (TNM_MSG_Reply_FS_stat*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_stat' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_STAT;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        if (1 < call_msg->string_size)
        {
            /* allocate and fill memory block with:     */
            /* - struct stat                            */
            /* - alignment to 32-bit boundary           */
            /* - parameter 'key[4]' (fix value 'STAT')  */
            /* - parameter 'fs_type'                    */
            /* - parameter 'fs_attr'                    */
            /* - parameter 'checksum1' and 'checksum2'  */
            {
                UNSIGNED8   *buffer;
                UNSIGNED16  sizeof_struct_stat;
                UNSIGNED32  index;
                UNSIGNED8   *p8_begin;
                UNSIGNED8   *p8_end;
                UNSIGNED8   *p8;
                UNSIGNED16  checksum;

                buffer             = (UNSIGNED8*)p_stat;
                sizeof_struct_stat = (UNSIGNED16)sizeof(struct stat);

                /* clear memory block */
                memset((void*)stat_buf, 0, sizeof(stat_buf));

                /* set begin-pointer for checksum */
                p8_begin = (UNSIGNED8*)buffer;

                /* parameter 'struct stat' */
                index = (UNSIGNED32)sizeof_struct_stat;

                /* allign next parameter to 32-bit boundary */
                /* (use null byte as as fill value)         */
                if (0 != (sizeof_struct_stat % 4))
                {
                    index += (UNSIGNED32)(4 - (sizeof_struct_stat % 4));
                } /* if (0 != (sizeof_struct_stat % 4)) */

                /* set parameter 'key[4]' into memory block */
                /*  (fix value 'STAT')                      */
                memcpy((void*)&buffer[index], "STAT", 4);
                index += (UNSIGNED32)4;

                /* parameter 'fs_type' into memory block */
                index += (UNSIGNED32)4;

                /* parameter 'fs_attr' into memory block */
                index += (UNSIGNED32)4;

                /* set end-pointer for checksum */
                p8_end = (UNSIGNED8*)&buffer[index];

                /* set parameter 'checksum1' into memory block */
                checksum = 0;
                p8       = p8_begin;
                while (p8 < p8_end)
                {
                    checksum = (UNSIGNED16)(checksum + *p8++);
                } /* while (p8 < p8_end) */
                *(UNSIGNED16*)&buffer[index] = checksum;
                index += (UNSIGNED32)2;

                /* set value 'checksum2' into memory block */
                *(UNSIGNED16*)&buffer[index] = \
                    (UNSIGNED16)(0xFFFF - checksum);
            }

            /* call POSIX procedure 'stat' */
            status = (INTEGER32)stat(call_msg->path, p_stat);

            /* fill 'Reply_FS_stat' */
            reply_msg->status = (INTEGER32)status;
            if (-1 == status)
            {
                reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
            } /* if (-1 == status) */
            else
            {
                status = 0;

                /* handle memory block, which contains:     */
                /* - struct stat                            */
                /* - alignment to 32-bit boundary           */
                /* - parameter 'fs_type'                    */
                /* - parameter 'fs_attr'                    */
                /* - parameter 'checksum1' and 'checksum2'  */
                {
                    UNSIGNED8   *buffer;
                    UNSIGNED16  sizeof_struct_stat;
                    UNSIGNED32  index;
                    UNSIGNED16  checksum1, checksum2;
                    UNSIGNED8   *p8_begin;
                    UNSIGNED8   *p8_end;
                    UNSIGNED8   *p8;
                    UNSIGNED16  checksum;

                    buffer             = (UNSIGNED8*)p_stat;
                    sizeof_struct_stat = (UNSIGNED16)sizeof(struct stat);

                    /* set begin-pointer for checksum */
                    p8_begin = (UNSIGNED8*)buffer;

                    /* parameter 'struct stat' */
                    index = (UNSIGNED32)sizeof_struct_stat;

                    /* allign next parameter to 32-bit boundary */
                    /* (use null byte as as fill value)         */
                    if (0 != (sizeof_struct_stat % 4))
                    {
                        index += (UNSIGNED32)(4 - (sizeof_struct_stat % 4));
                    } /* if (0 != (sizeof_struct_stat % 4)) */

                    /* check parameter 'key[4]' (fix value 'stat') */
                    if (0 == memcmp((void*)&buffer[index], "stat", 4))
                    {
                        index += (UNSIGNED32)4;

                        /* parameter 'fs_type' */
                        fs_type = *(UNSIGNED32*)&buffer[index];
                        index += (UNSIGNED32)4;

                        /* parameter 'fs_attr' */
                        fs_attr = *(UNSIGNED32*)&buffer[index];
                        index += (UNSIGNED32)4;

                        /* set end-pointer for checksum */
                        p8_end = (UNSIGNED8*)&buffer[index];

                        /* parameter 'checksum1' and 'checksum2' */
                        checksum1 = *(UNSIGNED16*)&buffer[index];
                        index += (UNSIGNED32)2;
                        checksum2 = *(UNSIGNED16*)&buffer[index];

                        /* check parameter 'checksum1' and 'checksum2' */
                        checksum = 0;
                        p8       = p8_begin;
                        while (p8 < p8_end)
                        {
                            checksum = (UNSIGNED16)(checksum + *p8++);
                        } /* while (p8 < p8_end) */
                        if ((checksum1 != checksum) || \
                            (checksum2 != (UNSIGNED16)(0xFFFF - checksum)))
                        {
                            status = -1;
                        } /* if ((...)) */
                    } /* if (0 == memcmp((void*)&buffer[index], ...)) */
                    /* check parameter 'key[4]' (fix value 'STAT') */
                    else if (0 == memcmp((void*)&buffer[index], "STAT", 4))
                    {
                        fs_type = 0;
                        fs_attr = 0;

                        status  = 0;
                    } /* else if (0 == memcmp((void*)&buffer[index], ...)) */
                    else
                    {
                        fs_type = 0;
                        fs_attr = 0;

                        status  = -1;
                    } /* else */
                }

                if (-1 == status)
                {
                    reply_msg->sys_errno = (INTEGER32)TNM_FS_EINVAL;
                    reply_msg->status    = (INTEGER32)-1;
                } /* if (-1 == status) */
                else
                {
                    reply_msg->fs_type = fs_type;
                    reply_msg->fs_attr = fs_attr;

                    reply_msg->stat_dev = (UNSIGNED32)p_stat->st_dev;

                    reply_msg->stat_ino_high = (UNSIGNED32) \
                        (((UNSIGNED64)p_stat->st_ino >> 32) & 0xFFFFFFFF);
                    reply_msg->stat_ino_low  = (UNSIGNED32) \
                        (p_stat->st_ino & 0xFFFFFFFF);

                    /* file type and file mode bits */
                    reply_msg->stat_mode = (UNSIGNED32)         \
                        (                                       \
                            tcn_ma_FS_map_OS_to_TNM_mode_val(   \
                                p_stat->st_mode)              | \
                            tcn_ma_FS_map_OS_to_TNM_mode_bit(   \
                                p_stat->st_mode)                \
                        );

                    reply_msg->stat_nlink = (UNSIGNED32)p_stat->st_nlink;

                    reply_msg->stat_uid = (UNSIGNED32)p_stat->st_uid;
                    reply_msg->stat_gid = (UNSIGNED32)p_stat->st_gid;

                    reply_msg->stat_size_high = (UNSIGNED32) \
                        (((UNSIGNED64)p_stat->st_size >> 32) & 0xFFFFFFFF);
                    reply_msg->stat_size_low  = (UNSIGNED32) \
                        (p_stat->st_size & 0xFFFFFFFF);

                    reply_msg->stat_atime = (UNSIGNED32)p_stat->st_atime;
                    reply_msg->stat_mtime = (UNSIGNED32)p_stat->st_mtime;
                    reply_msg->stat_ctime = (UNSIGNED32)p_stat->st_ctime;
                } /* else */
            } /* else */
        } /* if (1 < call_msg->string_size) */
        else
        {
            /* fill 'Reply_FS_stat' */
            reply_msg->sys_errno = (INTEGER32)ENOENT;
            reply_msg->status    = (INTEGER32)-1;
        } /* else */

        /* swap bytes of 'Reply_FS_stat' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->status);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->fs_type);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->fs_attr);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_dev);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_ino_high);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_ino_low);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_mode);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_nlink);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_uid);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_gid);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_size_high);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_size_low);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_atime);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_mtime);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->stat_ctime);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_stat */


/* --------------------------------------------------------------------------
 *  File System Service - FS_pathconf
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_pathconf
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                   mm_result  = MM_OK;
    TNM_MSG_Call_FS_pathconf    *call_msg;
    TNM_MSG_Reply_FS_pathconf   *reply_msg;
    UNSIGNED32                  msg_size;
    INTEGER32                   name;
    INTEGER32                   value      = -1;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_pathconf*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check min. size of 'Call_FS_pathconf' */
    msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_FS_pathconf);
    msg_size -= (UNSIGNED32)4; /* dynamic size */
    if (call_msg_size < msg_size)
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size < msg_size) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_pathconf' */
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->string_size);
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->name);

        /* check size of 'Call_FS_pathconf' */
        msg_size += (UNSIGNED32)call_msg->string_size;
        msg_size += \
            (UNSIGNED32)(call_msg->string_size % 2); /* allign to 16-bit */
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_pathconf' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_pathconf);
        reply_msg = (TNM_MSG_Reply_FS_pathconf*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_pathconf' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_PATHCONF;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        if (1 < call_msg->string_size)
        {
            /* parameter 'name' */
            name = tcn_ma_FS_map_TNM_to_OS_pathconf(call_msg->name);

            /* call POSIX procedure 'pathconf' */
            errno = 0;
            value = (INTEGER32)pathconf(call_msg->path, name);

            /* fill 'Reply_FS_pathconf' */
            reply_msg->value = (INTEGER32)value;
            if ((-1 == value) && (0 != errno))
            {
                reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
            } /* if ((-1 == value) && (0 != errno)) */
        } /* if (1 < call_msg->string_size) */
        else
        {
            /* fill 'Reply_FS_pathconf' */
            reply_msg->sys_errno = (INTEGER32)ENOENT;
            reply_msg->value     = (INTEGER32)-1;
        } /* else */

        /* swap bytes of 'Reply_FS_pathconf' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->value);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_pathconf  */


/* --------------------------------------------------------------------------
 *  File System Service - FS_getcwd
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_getcwd
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_getcwd  *call_msg;
    TNM_MSG_Reply_FS_getcwd *reply_msg;
    UNSIGNED32              msg_size   = 0;
    CHARACTER8              *buf;
    UNSIGNED16              size;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_getcwd*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_FS_getcwd' */
    if (call_msg_size != sizeof(TNM_MSG_Call_FS_getcwd))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_getcwd' */
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->size);
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_getcwd' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_getcwd);
        msg_size -= (UNSIGNED32)4; /* dynamic size */
        msg_size += (UNSIGNED32)call_msg->size;
        msg_size += (UNSIGNED32)(call_msg->size % 2); /* allign to 16-bit */
        reply_msg = (TNM_MSG_Reply_FS_getcwd*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_getcwd' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_GETCWD;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        if (0 < call_msg->size)
        {
            /* call POSIX procedure 'getcwd' */
            buf = (CHARACTER8*)getcwd(reply_msg->buf, call_msg->size);

            /* fill 'Reply_FS_getcwd' */
            msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_getcwd);
            msg_size -= (UNSIGNED32)4; /* dynamic size */
            if (NULL == buf)
            {
                reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
                reply_msg->size      = (UNSIGNED16)0;
            } /* if (NULL == buf) */
            else
            {
                size = (UNSIGNED16)(strlen((char*)buf) + 1);
                msg_size += (UNSIGNED32)size;
                msg_size += (UNSIGNED32)(size % 2); /* allign to 16-bit */
                reply_msg->size = size;
            } /* else */
        } /* if (0 < call_msg->size) */
        else
        {
            reply_msg->sys_errno = (INTEGER32)EINVAL;
            reply_msg->size      = (UNSIGNED16)0;
        } /* else */

        /* swap bytes of 'Reply_FS_getcwd' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->size);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_getcwd  */


/* --------------------------------------------------------------------------
 *  File System Service - FS_chdir
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_chdir
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_chdir   *call_msg;
    TNM_MSG_Reply_FS_chdir  *reply_msg;
    UNSIGNED32              msg_size;
    INTEGER32               status     = -1;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_chdir*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check min. size of 'Call_FS_chdir' */
    msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_FS_chdir);
    msg_size -= (UNSIGNED32)4; /* dynamic size */
    if (call_msg_size < msg_size)
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size < msg_size) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_chdir' */
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->string_size);

        /* check size of 'Call_FS_chdir' */
        msg_size += (UNSIGNED32)call_msg->string_size;
        msg_size += \
            (UNSIGNED32)(call_msg->string_size % 2); /* allign to 16-bit */
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_chdir' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_chdir);
        reply_msg = (TNM_MSG_Reply_FS_chdir*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_chdir' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_CHDIR;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        if (1 < call_msg->string_size)
        {
            /* call POSIX procedure 'chdir' */
            status = (INTEGER32)chdir(call_msg->path);

            /* fill 'Reply_FS_chdir' */
            reply_msg->status = (INTEGER32)status;
            if (-1 == status)
            {
                reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
            } /* if (-1 == status) */
        } /* if (1 < call_msg->string_size) */
        else
        {
            /* fill 'Reply_FS_chdir' */
            reply_msg->sys_errno = (INTEGER32)ENOENT;
            reply_msg->status    = (INTEGER32)-1;
        } /* else */

        /* swap bytes of 'Reply_FS_chdir' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->status);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_chdir */


/* --------------------------------------------------------------------------
 *  File System Service - FS_mkdir
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_mkdir
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_mkdir   *call_msg;
    TNM_MSG_Reply_FS_mkdir  *reply_msg;
    UNSIGNED32              msg_size   = 0;
    UNSIGNED32              mode       = 0;
    INTEGER32               status     = -1;

    call_msg  = (TNM_MSG_Call_FS_mkdir*)call_msg_adr;
    reply_msg = NULL;

    /* check station reservation */
    mm_result = tcn_ma_check_station_reservation(manager_address);

    if (MM_OK == mm_result)
    {
        *reply_msg_adr  = (void*)NULL;
        *reply_msg_size = (UNSIGNED32)0;

        /* check min. size of 'Call_FS_mkdir' */
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_FS_mkdir);
        msg_size -= (UNSIGNED32)4; /* dynamic size */
        if (call_msg_size < msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size < msg_size) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_mkdir' */
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->mode);
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->string_size);

        /* check size of 'Call_FS_mkdir' */
        msg_size += (UNSIGNED32)call_msg->string_size;
        msg_size += \
            (UNSIGNED32)(call_msg->string_size % 2); /* allign to 16-bit */
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_mkdir' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_mkdir);
        reply_msg = (TNM_MSG_Reply_FS_mkdir*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_mkdir' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_MKDIR;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        if (1 < call_msg->string_size)
        {
            /* parameter 'mode' */
            mode = tcn_ma_FS_map_TNM_to_OS_mode_bit(call_msg->mode);

            /* call POSIX procedure 'mkdir' */
            status = (INTEGER32)mkdir(call_msg->path, mode);

            /* fill 'Reply_FS_mkdir' */
            reply_msg->status = (INTEGER32)status;
            if (-1 == status)
            {
                reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
            } /* if (-1 == status) */
        } /* if (1 < call_msg->string_size) */
        else
        {
            /* fill 'Reply_FS_mkdir' */
            reply_msg->sys_errno = (INTEGER32)ENOENT;
            reply_msg->status    = (INTEGER32)-1;
        } /* else */

        /* swap bytes of 'Reply_FS_mkdir' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->status);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_mkdir */


/* --------------------------------------------------------------------------
 *  File System Service - FS_rmdir
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_rmdir
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT               mm_result  = MM_OK;
    TNM_MSG_Call_FS_rmdir   *call_msg;
    TNM_MSG_Reply_FS_rmdir  *reply_msg;
    UNSIGNED32              msg_size   = 0;
    INTEGER32               status     = -1;

    call_msg  = (TNM_MSG_Call_FS_rmdir*)call_msg_adr;
    reply_msg = NULL;

    /* check station reservation */
    mm_result = tcn_ma_check_station_reservation(manager_address);

    if (MM_OK == mm_result)
    {
        *reply_msg_adr  = (void*)NULL;
        *reply_msg_size = (UNSIGNED32)0;

        /* check min. size of 'Call_FS_rmdir' */
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_FS_rmdir);
        msg_size -= (UNSIGNED32)4; /* dynamic size */
        if (call_msg_size < msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size < msg_size) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_rmdir' */
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->string_size);

        /* check size of 'Call_FS_rmdir' */
        msg_size += (UNSIGNED32)call_msg->string_size;
        msg_size += \
            (UNSIGNED32)(call_msg->string_size % 2); /* allign to 16-bit */
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_rmdir' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_rmdir);
        reply_msg = (TNM_MSG_Reply_FS_rmdir*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_rmdir' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_RMDIR;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        if (1 < call_msg->string_size)
        {
            /* call POSIX procedure 'rmdir' */
            status = (INTEGER32)rmdir(call_msg->path);

            /* fill 'Reply_FS_rmdir' */
            reply_msg->status = (INTEGER32)status;
            if (-1 == status)
            {
                reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
            } /* if (-1 == status) */
        } /* if (1 < call_msg->string_size) */
        else
        {
            /* fill 'Reply_FS_rmdir' */
            reply_msg->sys_errno = (INTEGER32)ENOENT;
            reply_msg->status    = (INTEGER32)-1;
        } /* else */

        /* swap bytes of 'Reply_FS_rmdir' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->status);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_rmdir */


/* --------------------------------------------------------------------------
 *  File System Service - FS_opendir
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_opendir
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                   mm_result  = MM_OK;
    TNM_MSG_Call_FS_opendir     *call_msg;
    TNM_MSG_Reply_FS_opendir    *reply_msg;
    UNSIGNED32                  msg_size;
    DIR                         *dirp;
    INTEGER32                   name_max;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_opendir*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check min. size of 'Call_FS_opendir' */
    msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Call_FS_opendir);
    msg_size -= (UNSIGNED32)4; /* dynamic size */
    if (call_msg_size < msg_size)
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size < msg_size) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_opendir' */
        TCN_MACRO_CONVERT_BE16_TO_CPU(&call_msg->string_size);

        /* check size of 'Call_FS_opendir' */
        msg_size += (UNSIGNED32)call_msg->string_size;
        msg_size += \
            (UNSIGNED32)(call_msg->string_size % 2); /* allign to 16-bit */
        if (call_msg_size != msg_size)
        {
            mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
        } /* if (call_msg_size != msg_size) */
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_opendir' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_opendir);
        reply_msg = (TNM_MSG_Reply_FS_opendir*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_opendir' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_OPENDIR;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        if (1 < call_msg->string_size)
        {
            /* call POSIX procedure 'opendir' */
            dirp = (DIR*)opendir(call_msg->dirname);

            /* fill 'Reply_FS_opendir' */
            reply_msg->dirp = (INTEGER32)dirp;
            if (NULL == dirp)
            {
                reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
            } /* if (NULL == dirp) */

            /* determine 'NAME_MAX' (used by 'FS_readdir') */
            name_max = (INTEGER32)              \
                pathconf                        \
                (                               \
                    (char*)call_msg->dirname,   \
                    (int)_PC_NAME_MAX           \
                );
            if (-1 == name_max)
            {
                tcn_ma_FS_name_max = (UNSIGNED16)1024;
            } /* if (-1 == name_max) */
            else
            {
                tcn_ma_FS_name_max = (UNSIGNED16)(name_max + 1);
            } /* else */
        } /* if (1 < call_msg->string_size) */
        else
        {
            /* fill 'Reply_FS_opendir' */
            reply_msg->sys_errno = (INTEGER32)ENOENT;
            reply_msg->dirp      = (INTEGER32)0;
        } /* else */

        /* swap bytes of 'Reply_FS_opendir' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->dirp);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_opendir */


/* --------------------------------------------------------------------------
 *  File System Service - FS_closedir
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_closedir
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                   mm_result  = MM_OK;
    TNM_MSG_Call_FS_closedir    *call_msg;
    TNM_MSG_Reply_FS_closedir   *reply_msg;
    UNSIGNED32                  msg_size   = 0;
    INTEGER32                   status;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_closedir*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_FS_closedir' */
    if (call_msg_size != sizeof(TNM_MSG_Call_FS_closedir))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_closedir' */
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->dirp);
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_closedir' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_closedir);
        reply_msg = (TNM_MSG_Reply_FS_closedir*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_closedir' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_CLOSEDIR;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        /* call POSIX procedure 'closedir' */
        status = (INTEGER32)closedir((DIR*)call_msg->dirp);

        /* fill 'Reply_FS_closedir' */
        reply_msg->status = (INTEGER32)status;
        if (-1 == status)
        {
            reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
        } /* if (-1 == status) */

        /* swap bytes of 'Reply_FS_closedir' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->status);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_closedir */


/* --------------------------------------------------------------------------
 *  File System Service - FS_readdir
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_readdir
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                   mm_result   = MM_OK;
    TNM_MSG_Call_FS_readdir     *call_msg;
    TNM_MSG_Reply_FS_readdir    *reply_msg;
    UNSIGNED32                  msg_size;
    struct dirent               *dir_entry;
    UNSIGNED16                  string_size;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_readdir*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_FS_readdir' */
    if (call_msg_size != sizeof(TNM_MSG_Call_FS_readdir))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_readdir' */
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->dirp);
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_readdir' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_readdir);
        msg_size -= (UNSIGNED32)4; /* dynamic size */
        msg_size += (UNSIGNED32)tcn_ma_FS_name_max;
        msg_size += \
            (UNSIGNED32)(tcn_ma_FS_name_max % 2); /* allign to 16-bit */
        reply_msg = (TNM_MSG_Reply_FS_readdir*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_readdir' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_READDIR;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        /* call POSIX procedure 'readdir' */
        errno     = 0;
        dir_entry = (struct dirent*)readdir((DIR*)call_msg->dirp);

        /* fill 'Reply_FS_readdir' */
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_readdir);
        msg_size -= (UNSIGNED32)4; /* dynamic size */
        if (NULL == dir_entry)
        {
            reply_msg->string_size = (UNSIGNED16)0;
            if (0 != errno)
            {
                reply_msg->sys_errno = tcn_ma_FS_map_OS_to_TNM_errno();
            } /* if (0 != errno) */
        } /* if (NULL == dir_entry) */
        else
        {
            string_size = (UNSIGNED16)(strlen((char*)dir_entry->d_name) + 1);
            msg_size += (UNSIGNED32)string_size;
            msg_size += (UNSIGNED32)(string_size % 2); /* allign to 16-bit */
            reply_msg->string_size = string_size;
            memcpy                          \
            (                               \
                (void*)reply_msg->d_name,   \
                (void*)dir_entry->d_name,   \
                string_size                 \
            );
        } /* else */

        /* swap bytes of 'Reply_FS_readdir' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE16(&reply_msg->string_size);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_readdir */


/* --------------------------------------------------------------------------
 *  File System Service - FS_rewinddir
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_service_call_FS_rewinddir
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
)
{
    MM_RESULT                   mm_result  = MM_OK;
    TNM_MSG_Call_FS_rewinddir   *call_msg;
    TNM_MSG_Reply_FS_rewinddir  *reply_msg;
    UNSIGNED32                  msg_size   = 0;

    /* avoid warnings */
    manager_address = manager_address;

    call_msg  = (TNM_MSG_Call_FS_rewinddir*)call_msg_adr;
    reply_msg = NULL;

    *reply_msg_adr  = (void*)NULL;
    *reply_msg_size = (UNSIGNED32)0;

    /* check size of 'Call_FS_rewinddir' */
    if (call_msg_size != sizeof(TNM_MSG_Call_FS_rewinddir))
    {
        mm_result = (MM_RESULT)MM_CMD_NOT_EXECUTED;
    } /* if (call_msg_size != ...) */

    if (MM_OK == mm_result)
    {
        /* swap bytes of 'Call_FS_rewinddir' */
        TCN_MACRO_CONVERT_BE32_TO_CPU(&call_msg->dirp);
    } /* if (MM_OK == mm_result) */

    /* allocate and clear memory for 'Reply_FS_rewinddir' */
    if (MM_OK == mm_result)
    {
        msg_size  = (UNSIGNED32)sizeof(TNM_MSG_Reply_FS_rewinddir);
        reply_msg = (TNM_MSG_Reply_FS_rewinddir*)malloc(msg_size);
        if (NULL == reply_msg)
        {
            mm_result = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == reply_msg) */
        else
        {
            memset((void*)reply_msg, 0, msg_size);
        } /* else */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        /* fill 'Reply_FS_rewinddir' */
        reply_msg->tnm_key   = (ENUM8)TNM_KEY_DEFAULT_REPLY;
        reply_msg->sif_code  = (ENUM8)FS_REWINDDIR;
        reply_msg->reserved1 = (WORD16)0;
        reply_msg->sys_errno = (INTEGER32)TNM_FS_ENOERR;

        /* call POSIX procedure 'rewinddir' */
        rewinddir((DIR*)call_msg->dirp);

        /* fill 'Reply_FS_rewinddir' */
        reply_msg->status = -1;

        /* swap bytes of 'Reply_FS_rewinddir' */
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->sys_errno);
        TCN_MACRO_CONVERT_CPU_TO_BE32(&reply_msg->status);

        /* set control structure for default services */
        tcn_ma_ctrl_default.reply_msg_adr  = (void*)reply_msg;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)msg_size;

        /* set output parameters */
        *reply_msg_adr  = (void*)reply_msg;
        *reply_msg_size = (UNSIGNED32)msg_size;
    } /* if (MM_OK == mm_result) */
    else
    {
        if (NULL != reply_msg)
        {
            free((void*)reply_msg);
        } /* if (NULL != reply_msg) */
    } /* else */

    *agent_status = mm_result;

} /* tcn_ma_service_call_FS_rewinddir */
#endif /* #ifndef TCN_MA_FS_DISABLE */


/* ==========================================================================
 *
 *  Agent
 *
 * ==========================================================================
 */

#ifdef TNM_FS_SIM

/* --------------------------------------------------------------------------
 *  tcn_ma_agent_fs_sim
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MM_RESULT
tcn_ma_agent_fs_sim
(
  AM_ADDRESS          *tcn_address,
  void                *call_msg_adr,
  UNSIGNED32          call_msg_size,
  void                **reply_msg_adr,
  UNSIGNED32          *reply_msg_size
)
{
    AM_ADDRESS          manager_address;
    TNM_MSG_HEADER      *msg_header;
    MA_SERVICE_CALL     service_call    = NULL;
    MA_SERVICE_CLOSE    service_close   = NULL;
    MM_RESULT           agent_status;

    /* avoid warninsg */
    tcn_address = tcn_address;

#ifdef TCN_MA_PRINT
    TCN_OSL_PRINTF("tcn_ma_agent_fs_sim\n");
    TCN_OSL_PRINTF("===================\n");
#endif /* TCN_MA_PRINT */

    manager_address.sg_node      = AM_SAME_NODE;
    manager_address.func_or_stat = AM_MANAGER_FCT;
    manager_address.next_station = AM_SAME_STATION;
    manager_address.topo_counter = AM_ANY_TOPO;

    /* ----------------------------------------------------------------------
     *  service call
     * ----------------------------------------------------------------------
     */
#ifdef TCN_MA_PRINT
    TCN_OSL_PRINTF("- service_call()\n");
#endif /* TCN_MA_PRINT */

    /* check 'call_msg_adr' */
    if (NULL == call_msg_adr)
    {
        agent_status = (MM_RESULT)AM_NO_REM_MEM_ERR;
    } /* if (NULL == call_msg_adr) */

    /* check message header entry 'tnm_key' */
    msg_header = (TNM_MSG_HEADER*)call_msg_adr;
#ifdef TCN_MA_PRINT
    TCN_OSL_PRINTF("- tnm_key =0x%02X\n", msg_header->tnm_key);
#endif /* TCN_MA_PRINT */
    if (TNM_KEY_DEFAULT_CALL != msg_header->tnm_key)
    {
        agent_status = (MM_RESULT)MM_INT_ERROR;
    } /* if (TNM_KEY_DEFAULT_CALL != msg_header->tnm_key) */

    pthread_mutex_lock(&tcn_ma_mutex_object_service);

    /* check message header entry 'sif_code' */
#ifdef TCN_MA_PRINT
    TCN_OSL_PRINTF("- sif_code=%d\n", msg_header->sif_code);
#endif /* TCN_MA_PRINT */
    if (NULL == tcn_ma_service[msg_header->sif_code])
    {
        agent_status = (MM_RESULT)MM_SIF_NOT_SUPPORTED;
    } /* if (NULL == tcn_ma_service[msg_header->sif_code]) */
    else
    {
#ifdef TCN_MA_PRINT
        TCN_OSL_PRINTF("- service_desc='%s'\n", \
            &tcn_ma_service[msg_header->sif_code]->service_desc);
#endif /* TCN_MA_PRINT */

        service_call  = tcn_ma_service[msg_header->sif_code]->service_call;
        service_close = tcn_ma_service[msg_header->sif_code]->service_close;
    } /* else */

    pthread_mutex_unlock(&tcn_ma_mutex_object_service);

    (service_call)                      \
    (                                   \
        (AM_ADDRESS*)&manager_address,  \
        (void*)call_msg_adr,            \
        (UNSIGNED32)call_msg_size,      \
        (void**)reply_msg_adr,          \
        (UNSIGNED32*)reply_msg_size,    \
        (MM_RESULT*)&agent_status       \
    );


    /* ----------------------------------------------------------------------
     *  service close
     * ----------------------------------------------------------------------
     */
    if (NULL != service_close)
    {
        (service_close)();
    } /* if (NULL != service_close) */


    return(MM_OK);

} /* tcn_ma_agent_fs_sim */

#else /* #ifdef TNM_FS_SIM */

/* --------------------------------------------------------------------------
 *  tcn_ma_receive_confirm
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_receive_confirm
(
    UNSIGNED8           replier_function,
    const AM_ADDRESS    *caller,
    void *              in_msg_adr,
    UNSIGNED32          in_msg_size,
    void *              replier_ref
)
{

    /* avoid warnings */
    replier_function = replier_function;
    replier_ref      = replier_ref;

#ifdef TCN_MA_PRINT
    TCN_OSL_PRINTF("tcn_ma_receive_confirm\n");
    TCN_OSL_PRINTF("======================\n");
    TCN_OSL_PRINTF("replier_function    =%d\n", replier_function);
    TCN_OSL_PRINTF("caller->sg_node     =%d\n", caller->sg_node);
    TCN_OSL_PRINTF("caller->func_or_stat=%d\n", caller->func_or_stat);
    TCN_OSL_PRINTF("caller->next_station=%d\n", caller->next_station);
    TCN_OSL_PRINTF("caller->topo_counter=%d\n", caller->topo_counter);
    TCN_OSL_PRINTF("in_msg_size         =%ld\n", in_msg_size);
    TCN_OSL_PRINTF("replier_ref         =0x%08lX\n", (WORD32)replier_ref);
#endif /* TCN_MA_PRINT */

    tcn_ma_ctrl.action          = TCN_MA_CTRL_ACTION_SERVICE_CALL;
    tcn_ma_ctrl.manager_address = *caller;
    tcn_ma_ctrl.call_msg_adr    = in_msg_adr;
    tcn_ma_ctrl.call_msg_size   = in_msg_size;
    tcn_ma_ctrl.service_close   = (MA_SERVICE_CLOSE)NULL;

    sem_post(&tcn_ma_sem_object_ctrl);

} /* tcn_ma_receive_confirm */


/* --------------------------------------------------------------------------
 *  tcn_ma_reply_confirm
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_reply_confirm
(
    UNSIGNED8   replier_function,
    void        *replier_ref
)
{

    /* avoid warnings */
    replier_function = replier_function;
    replier_ref      = replier_ref;

#ifdef TCN_MA_PRINT
    TCN_OSL_PRINTF("tcn_ma_reply_confirm\n");
    TCN_OSL_PRINTF("====================\n");
    TCN_OSL_PRINTF("replier_function=%d\n", replier_function);
    TCN_OSL_PRINTF("replier_ref     =0x%08lX\n", (WORD32)replier_ref);
#endif /* TCN_MA_PRINT */

    tcn_ma_ctrl.action = TCN_MA_CTRL_ACTION_SERVICE_CLOSE;

    sem_post(&tcn_ma_sem_object_ctrl);

} /* tcn_ma_reply_confirm */


/* --------------------------------------------------------------------------
 *  tcn_ma_agent
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
tcn_ma_agent (void)
{
    TCN_DECL_LOCAL  TNM_MSG_HEADER      *msg_header;
    TCN_DECL_LOCAL  MA_SERVICE_CALL     service_call;
    TCN_DECL_LOCAL  MA_SERVICE_CLOSE    service_close;
    TCN_DECL_LOCAL  void                *reply_msg_adr;
    TCN_DECL_LOCAL  UNSIGNED32          reply_msg_size;
    TCN_DECL_LOCAL  MM_RESULT           agent_status;
    TCN_DECL_LOCAL  AM_RESULT           am_result;

#ifdef TCN_MA_PRINT
    TCN_OSL_PRINTF("tcn_ma_agent\n");
    TCN_OSL_PRINTF("============\n");
#endif /* TCN_MA_PRINT */

    sem_wait(&tcn_ma_sem_object_ctrl);

    /* ----------------------------------------------------------------------
     *  service call
     * ----------------------------------------------------------------------
     */
    if (TCN_MA_CTRL_ACTION_SERVICE_CALL == tcn_ma_ctrl.action)
    {
#ifdef TCN_MA_PRINT
        TCN_OSL_PRINTF("- service_call()\n");
#endif /* TCN_MA_PRINT */

        agent_status = MM_OK;

        /* check 'tcn_ma_ctrl.call_msg_adr' */
        if (NULL == tcn_ma_ctrl.call_msg_adr)
        {
            tcn_ma_ctrl.action = TCN_MA_CTRL_ACTION_REPLY_REQUEST;
            agent_status       = (MM_RESULT)AM_NO_REM_MEM_ERR;
        } /* if (NULL == tcn_ma_ctrl.call_msg_adr) */

        /* check message header entry 'tnm_key' */
        if (MM_OK == agent_status)
        {
            msg_header = (TNM_MSG_HEADER*)tcn_ma_ctrl.call_msg_adr;
#ifdef TCN_MA_PRINT
            TCN_OSL_PRINTF("- tnm_key =0x%02X\n", msg_header->tnm_key);
#endif /* TCN_MA_PRINT */
            if (TNM_KEY_DEFAULT_CALL != msg_header->tnm_key)
            {
                tcn_ma_ctrl.action = TCN_MA_CTRL_ACTION_REPLY_REQUEST;
                agent_status       = (MM_RESULT)MM_INT_ERROR;
            } /* if (TNM_KEY_DEFAULT_CALL != msg_header->tnm_key) */
        } /* if (MM_OK == agent_status) */

        if (MM_OK == agent_status)
        {
            pthread_mutex_lock(&tcn_ma_mutex_object_service);
        } /* if (MM_OK == agent_status) */

        /* check message header entry 'sif_code' */
        if (MM_OK == agent_status)
        {
#ifdef TCN_MA_PRINT
            TCN_OSL_PRINTF("- sif_code=%d\n", msg_header->sif_code);
#endif /* TCN_MA_PRINT */
            if (NULL == tcn_ma_service[msg_header->sif_code])
            {
                tcn_ma_ctrl.action = TCN_MA_CTRL_ACTION_REPLY_REQUEST;
                agent_status       = (MM_RESULT)MM_SIF_NOT_SUPPORTED;
            } /* if (NULL == tcn_ma_service[msg_header->sif_code]) */
        } /* if (MM_OK == agent_status) */

#ifdef TCN_MA_PRINT
        if (MM_OK == agent_status)
        {
            TCN_OSL_PRINTF("- service_desc='%s'\n", \
                &tcn_ma_service[msg_header->sif_code]->service_desc);
        } /* if (MM_OK == agent_status) */
#endif /* TCN_MA_PRINT */

        if (MM_OK == agent_status)
        {
            service_call  = \
                tcn_ma_service[msg_header->sif_code]->service_call;
            service_close = \
                tcn_ma_service[msg_header->sif_code]->service_close;

            tcn_ma_ctrl.service_close = service_close;

            pthread_mutex_unlock(&tcn_ma_mutex_object_service);

            (service_call)                                  \
            (                                               \
                (AM_ADDRESS*)&tcn_ma_ctrl.manager_address,  \
                (void*)tcn_ma_ctrl.call_msg_adr,            \
                (UNSIGNED32)tcn_ma_ctrl.call_msg_size,      \
                (void**)&reply_msg_adr,                     \
                (UNSIGNED32*)&reply_msg_size,               \
                (MM_RESULT*)&agent_status                   \
            );

            tcn_ma_ctrl.action = TCN_MA_CTRL_ACTION_REPLY_REQUEST;

        } /* if (MM_OK == agent_status) */

    } /* if (TCN_MA_CTRL_ACTION_SERVICE_CALL == tcn_ma_ctrl.action) */


    /* ----------------------------------------------------------------------
     *  reply request
     * ----------------------------------------------------------------------
     */
    if (TCN_MA_CTRL_ACTION_REPLY_REQUEST == tcn_ma_ctrl.action)
    {
        tcn_ma_ctrl.action = TCN_MA_CTRL_ACTION_NONE;

        /* release memory of 'tcn_ma_ctrl.call_msg_adr' */
        if (NULL != tcn_ma_ctrl.call_msg_adr)
        {
            free((void*)tcn_ma_ctrl.call_msg_adr);
        } /* if (NULL != tcn_ma_ctrl.call_msg_adr) */
        tcn_ma_ctrl.call_msg_adr = (void*)NULL;

#ifdef TCN_MA_PRINT
        TCN_OSL_PRINTF("- am_reply_request()=");
#endif /* TCN_MA_PRINT */

        am_result =                         \
            am_reply_request                \
            (                               \
                AM_AGENT_FCT,               \
                reply_msg_adr,              \
                reply_msg_size,             \
                (void*)&tcn_ma_replier_ref, \
                (AM_RESULT)agent_status     \
            );

#ifdef TCN_MA_PRINT
        TCN_OSL_PRINTF("%d\n", am_result);
#endif /* TCN_MA_PRINT */

        if (AM_OK != am_result)
        {
            tcn_ma_ctrl.action = TCN_MA_CTRL_ACTION_REPLY_REQUEST;
            sched_yield();
        } /* if (AM_OK != am_result) */

    } /* if (TCN_MA_CTRL_ACTION_REPLY_REQUEST == tcn_ma_ctrl.action) */


    /* ----------------------------------------------------------------------
     *  service close
     * ----------------------------------------------------------------------
     */
    if (TCN_MA_CTRL_ACTION_SERVICE_CLOSE == tcn_ma_ctrl.action)
    {
        if (NULL != tcn_ma_ctrl.service_close)
        {
            (tcn_ma_ctrl.service_close)();
        } /* if (NULL != tcn_ma_ctrl.service_close) */

        if ((NULL != tcn_ma_station_restart_function) && \
            (TRUE == tcn_ma_station_restart_action))
        {
            tcn_ma_station_restart_action = (BOOLEAN1)FALSE;
            tcn_ma_station_restart_function();
        } /* if ((...)) */

        tcn_ma_ctrl.action = TCN_MA_CTRL_ACTION_RECEIVE_REQUEST;

    } /* if (TCN_MA_CTRL_ACTION_SERVICE_CLOSE == tcn_ma_ctrl.action) */


    /* ----------------------------------------------------------------------
     *  receive request
     * ----------------------------------------------------------------------
     */
    if (TCN_MA_CTRL_ACTION_RECEIVE_REQUEST == tcn_ma_ctrl.action)
    {
        tcn_ma_ctrl.action = TCN_MA_CTRL_ACTION_NONE;

#ifdef TCN_MA_PRINT
        TCN_OSL_PRINTF("- am_receive_request()=");
#endif /* TCN_MA_PRINT */

        am_result =                         \
            am_receive_request              \
            (                               \
                AM_AGENT_FCT,               \
                NULL,                       \
                TCN_MA_IN_MSG_SIZE_MAX,     \
                (void*)&tcn_ma_replier_ref  \
            );

#ifdef TCN_MA_PRINT
        TCN_OSL_PRINTF("%d\n", am_result);
#endif /* TCN_MA_PRINT */

        if (AM_OK != am_result)
        {
            tcn_ma_ctrl.action = TCN_MA_CTRL_ACTION_RECEIVE_REQUEST;
            sched_yield();
        } /* if (AM_OK != am_result) */

    } /* if (TCN_MA_CTRL_ACTION_RECEIVE_REQUEST == tcn_ma_ctrl.action) */

} /* tcn_ma_agent */


/* --------------------------------------------------------------------------
 *  tcn_ma_agent
 * --------------------------------------------------------------------------
 */
void*
tcn_ma_agent_routine
(
    void        *arg
)
{

    /* avoid warnings */
    arg = arg;

    for(;;)
    {
        tcn_ma_agent();
    } /* for(;;) */

    return((void*)NULL);

} /* tcn_ma_agent_routine */


#ifdef __ECOS_WITH_MICROMONITOR__
/* --------------------------------------------------------------------------
 *  tcn_ma_station_restart (eCos with MicroMonitor support)
 * --------------------------------------------------------------------------
 */

#include <cyg/kernel/kapi.h>

extern int mon_restart(int restart_value);

void tcn_ma_station_restart (void)
{

    cyg_scheduler_lock();

    as_init();

    cyg_interrupt_disable();

    /* see MicroMonitor file 'cpu.h': */
    /* - #define INITIALIZE  (3<<4)   */
    mon_restart(3<<4);

} /* tcn_ma_station_restart */
#endif /* __ECOS_WITH_MICROMONITOR__ */

#endif /* TNM_FS_SIM */


/* ==========================================================================
 *
 *  Initialisation
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ma_init
 *
 *  Purpose   : Initialises the Agent.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MM_RESULT
 *              ma_init (void);
 *
 *  Return    : any MM_RESULT
 *
 *  Remarks   : - This procedure has to be called at system initialisation
 *                before calling any other 'ma_xxx' procedure.
 *              - This procedure shall be called only one time.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MM_RESULT
ma_init (void)
{
    MM_RESULT                   mm_result                 = MM_OK;
#ifndef TNM_FS_SIM
    TCN_DECL_LOCAL  BOOLEAN1    status_am_bind_replier    = FALSE;
    TCN_DECL_LOCAL  BOOLEAN1    status_am_receive_request = FALSE;
#endif /* #ifndef TNM_FS_SIM */
    UNSIGNED16                  sif_code;
#ifndef TNM_FS_SIM
    BITSET8                     *p_byte;
    UNSIGNED16                  c_byte;
    UNSIGNED16                  c_bit;
    UNSIGNED16                  index;
    UNSIGNED16                  shift;
    AM_ADDRESS                  am_address;
    AM_RESULT                   am_result;
#endif /* #ifndef TNM_FS_SIM */

#ifndef TNM_FS_SIM
    /* ----------------------------------------------------------------------
     *  inventory object
     * ----------------------------------------------------------------------
     */
    /* clear inventory object */
    memset                                                  \
    (                                                       \
        (void*)&tcn_ma_inventory_object,                    \
        0,                                                  \
        sizeof(TCN_MA_INVENTORY_OBJECT)                     \
    );

    /* set STRING32-types of inventory object */
    strcpy                                                  \
    (                                                       \
        (char*)&tcn_ma_inventory_object.manufacturer_name,  \
    /*   STRING32 (should be null terminated)   */          \
    /*   <-- 10 --><-- 20 --><-- 30 -->32       */          \
    /*   12345678901234567890123456789012       */          \
        "duagon GmbH, CH-8953 Dietikon"                     \
    );
    strcpy                                                  \
    (                                                       \
        (char*)&tcn_ma_inventory_object.device_type,        \
    /*   STRING32 (should be null terminated)   */          \
    /*   <-- 10 --><-- 20 --><-- 30 -->32       */          \
    /*   12345678901234567890123456789012       */          \
        "MVB Interface"                                     \
    );
    strcpy                                                  \
    (                                                       \
        (char*)&tcn_ma_inventory_object.agent_version,      \
    /*   STRING32 (should be null terminated)   */          \
    /*   <-- 10 --><-- 20 --><-- 30 -->32       */          \
    /*   12345678901234567890123456789012       */          \
        "IEC 61375-1, TCN, Clause 5: TNM"                   \
    );

    /* ----------------------------------------------------------------------
     *  reservation object
     * ----------------------------------------------------------------------
     */
    /* clear reservation object */
    memset                                                  \
    (                                                       \
        (void*)&tcn_ma_reservation_object,                  \
        0,                                                  \
        sizeof(TCN_MA_RESERVATION_OBJECT)                   \
    );
    tcn_ma_reservation_object.fd   = -1;
    tcn_ma_reservation_object.dirp = NULL;
#endif /* #ifndef TNM_FS_SIM */


    /* ----------------------------------------------------------------------
     *  service list
     * ----------------------------------------------------------------------
     */
    if (FALSE == tcn_ma_mutex_status_service)
    {
        if (0 != pthread_mutex_init(&tcn_ma_mutex_object_service, NULL))
        {
            mm_result = AM_FAILURE;
        } /* if (0 != pthread_mutex_init(...)) */
        else
        {
            tcn_ma_mutex_status_service = TRUE;
        } /* else */
    } /* if (FALSE == tcn_ma_mutex_status_service) */
    if (TRUE == tcn_ma_mutex_status_service)
    {
        if (0 != pthread_mutex_lock(&tcn_ma_mutex_object_service))
        {
            mm_result = AM_FAILURE;
        } /* if (0 != pthread_mutex_lock(...)) */
        if (MM_OK == mm_result)
        {
            if (0 != pthread_mutex_unlock(&tcn_ma_mutex_object_service))
            {
                mm_result = AM_FAILURE;
            } /* if (0 != pthread_mutex_unlock(...)) */
        } /* if (MM_OK == mm_result) */
    } /* if (TRUE == tcn_ma_mutex_status_service) */

    if (MM_OK == mm_result)
    {
        pthread_mutex_lock(&tcn_ma_mutex_object_service);

        for (sif_code=0; sif_code<256; sif_code++)
        {
            tcn_ma_service[sif_code] = (TCN_MA_SERVICE*)NULL;
        } /* for (sif_code=0; sif_code<256; sif_code++) */

#ifndef TNM_FS_SIM
        /* default services - station services */
        tcn_ma_service[READ_STATION_STATUS]      = \
            &tcn_ma_service_Read_Station_Status;
        tcn_ma_service[WRITE_STATION_CONTROL]    = \
            &tcn_ma_service_Write_Station_Control;
        tcn_ma_service[READ_STATION_INVENTORY]    = \
            &tcn_ma_service_Read_Station_Inventory;
        tcn_ma_service[WRITE_RESERVATION]         = \
            &tcn_ma_service_Write_Station_Reservation;
        tcn_ma_service[READ_SERVICE_DESCRIPTOR]    = \
            &tcn_ma_service_Read_Service_Descriptor;
        tcn_ma_service[READ_LINKS_DESCRIPTOR]    = \
            &tcn_ma_service_Read_Links_Descriptor;
        /* default services - MVB link services */
        tcn_ma_service[READ_MVB_STATUS]          = \
            &tcn_ma_service_Read_Mvb_Status;
        tcn_ma_service[WRITE_MVB_CONTROL]        = \
            &tcn_ma_service_Write_Mvb_Control;
        tcn_ma_service[READ_MVB_DEVICES]         = \
            &tcn_ma_service_Read_Mvb_Devices;
        tcn_ma_service[WRITE_MVB_ADMINISTRATOR]  = \
            &tcn_ma_service_Write_Mvb_Administrator;
        /* default services - variables services */
        tcn_ma_service[READ_PORTS_CONFIGURATION] = \
            &tcn_ma_service_Read_Ports_Configuration;
        /* default services - messages services */
        tcn_ma_service[READ_MESSENGER_STATUS]    = \
            &tcn_ma_service_Read_Messenger_Status;
        tcn_ma_service[WRITE_MESSENGER_CONTROL]  = \
            &tcn_ma_service_Write_Messenger_Control;
        tcn_ma_service[READ_FUNCTION_DIRECTORY]  = \
            &tcn_ma_service_Read_Function_Directory;
        tcn_ma_service[WRITE_FUNCTION_DIRECTORY] = \
            &tcn_ma_service_Write_Function_Directory;

        /* calculate 'service_set' of inventory object */
        p_byte = (BITSET8*)&tcn_ma_inventory_object.service_set.byte;
        for (c_byte=0; c_byte<32; c_byte++)
        {
            *p_byte = 0;
            c_bit   = 0;
            for (c_bit=0; c_bit<8; c_bit++)
            {
                index = (UNSIGNED16)((c_byte * 8) + c_bit);
                shift = (UNSIGNED16)(7 - c_bit);
                if (NULL != tcn_ma_service[index])
                {
                    *p_byte |= (BITSET8)(1 << shift);
                } /* if (NULL != tcn_ma_service[index]) */
            } /* for (c_bit=0; c_bit<8; c_bit++) */
            p_byte++;
        } /* for (c_byte=0; c_byte<32; c_byte++) */
#endif /* #ifndef TNM_FS_SIM */

        pthread_mutex_unlock(&tcn_ma_mutex_object_service);

    } /* if (MM_OK == mm_result) */

#ifndef TCN_MA_FS_DISABLE
    if (MM_OK == mm_result)
    {
        /* set local variables for file system services */
        tcn_ma_FS_subscribed = FALSE;
        tcn_ma_FS_name_max   = 1024;

        if (FALSE == tcn_ma_FS_subscribed)
        {
            /* desubscribe all user defined services */
            for (sif_code=USER_SERVICE_0; sif_code<USER_SERVICE_127; \
                 sif_code++)
            {
                ma_subscribe                            \
                (                                       \
                    (ENUM16)1, /* desubscribe */        \
                    (ENUM8)sif_code,                    \
                    (MA_SERVICE_CALL)NULL,              \
                    (MA_SERVICE_CLOSE)NULL,             \
                    (void*)NULL                         \
                );
            } /* for (sif_code=USER_SERVICE_0; ...) */

            /* subscribe file system service - FS_access */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_ACCESS,                   \
                    tcn_ma_service_call_FS_access,      \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_access"                  \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_open */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_OPEN,                     \
                    tcn_ma_service_call_FS_open,        \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_open"                    \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_close */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_CLOSE,                    \
                    tcn_ma_service_call_FS_close,       \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_close"                   \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_read */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_READ,                     \
                    tcn_ma_service_call_FS_read,        \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_read"                    \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_write */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_WRITE,                    \
                    tcn_ma_service_call_FS_write,       \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_write"                   \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_lseek */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_LSEEK,                    \
                    tcn_ma_service_call_FS_lseek,       \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_lseek"                   \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_unlink */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_UNLINK,                   \
                    tcn_ma_service_call_FS_unlink,      \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_unlink"                  \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_stat */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_STAT,                     \
                    tcn_ma_service_call_FS_stat,        \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_stat"                    \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_pathconf */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_PATHCONF,                 \
                    tcn_ma_service_call_FS_pathconf,    \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_pathconf"                \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_getcwd */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_GETCWD,                   \
                    tcn_ma_service_call_FS_getcwd,      \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_getcwd"                  \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_chdir */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_CHDIR,                    \
                    tcn_ma_service_call_FS_chdir,       \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_chdir"                   \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_mkdir */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_MKDIR,                    \
                    tcn_ma_service_call_FS_mkdir,       \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_mkdir"                   \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_rmdir */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_RMDIR,                    \
                    tcn_ma_service_call_FS_rmdir,       \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_rmdir"                   \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_opendir */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_OPENDIR,                  \
                    tcn_ma_service_call_FS_opendir,     \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_opendir"                 \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_closedir */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_CLOSEDIR,                 \
                    tcn_ma_service_call_FS_closedir,    \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_closedir"                \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_readdir */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_READDIR,                  \
                    tcn_ma_service_call_FS_readdir,     \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_readdir"                 \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            /* subscribe file system service - FS_rewinddir */
            mm_result =                                 \
                ma_subscribe                            \
                (                                       \
                    (ENUM16)0, /* subscribe */          \
                    (ENUM8)FS_REWINDDIR,                \
                    tcn_ma_service_call_FS_rewinddir,   \
                    tcn_ma_service_close_default,       \
                    (void*)"FS_rewinddir"               \
                );
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            tcn_ma_FS_subscribed = TRUE;
        } /* if (MM_OK == mm_result) */

    } /* if (FALSE == tcn_ma_FS_subscribed) */
#endif /* #ifndef TCN_MA_FS_DISABLE */


#ifndef TNM_FS_SIM

    /* ----------------------------------------------------------------------
     *  station restart
     * ----------------------------------------------------------------------
     */
     if (MM_OK == mm_result)
     {
#ifdef __ECOS_WITH_MICROMONITOR__
        tcn_ma_station_restart_function = \
            (MA_STATION_RESTART)tcn_ma_station_restart;
#else /* #ifdef __ECOS_WITH_MICROMONITOR__ */
        tcn_ma_station_restart_function = (MA_STATION_RESTART)NULL;
#endif /* #else */
        tcn_ma_station_restart_action   = (BOOLEAN1)FALSE;
     } /* if (MM_OK == mm_result) */


    /* ----------------------------------------------------------------------
     *  agent control
     * ----------------------------------------------------------------------
     */
    if (MM_OK == mm_result)
    {
        if (FALSE == tcn_ma_sem_status_ctrl)
        {
            if (0 != sem_init(&tcn_ma_sem_object_ctrl, 0, 0))
            {
                mm_result = AM_FAILURE;
            } /* if (0 != sem_init(...)) */
            else
            {
                tcn_ma_sem_status_ctrl = TRUE;
            } /* else */
        } /* if (FALSE == tcn_ma_sem_status_ctrl) */
    } /* if (MM_OK == mm_result) */


    if (FALSE == tcn_ma_pthread_status_ctrl)
    {
        if (MM_OK == mm_result)
        {
            if (0 != pthread_attr_init(&tcn_ma_pthread_attr_t_ctrl))
            {
                mm_result = AM_FAILURE;
            } /* if (0 != pthread_attr_init(...)) */
        } /* if (MM_OK == mm_result) */

#ifdef TCN_MA_STACK_SIZE
        if (MM_OK == mm_result)
        {
            if (0 != pthread_attr_setstacksize        \
                     (                                \
                         &tcn_ma_pthread_attr_t_ctrl, \
                         TCN_MA_STACK_SIZE            \
                     ))
            {
                mm_result = AM_FAILURE;
            } /* if (0 != pthread_attr_setstacksize(...)) */
        } /* if (MM_OK == mm_result) */
#endif /* #ifdef TCN_MA_STACK_SIZE */

#ifdef __ECOS__
        if (MM_OK == mm_result)
        {
            if (0 != pthread_attr_setinheritsched     \
                     (                                \
                         &tcn_ma_pthread_attr_t_ctrl, \
                         PTHREAD_EXPLICIT_SCHED       \
                     ))
            {
                mm_result = AM_FAILURE;
            } /* if (0 != pthread_attr_setinheritsched(...)) */
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            if (0 != pthread_attr_setschedpolicy      \
                     (                                \
                         &tcn_ma_pthread_attr_t_ctrl, \
                         SCHED_RR                     \
                     ))
            {
                mm_result = AM_FAILURE;
            } /* if (0 != pthread_attr_setschedpolicy(...)) */
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            tcn_ma_pthread_sched_param_ctrl.sched_priority = 22;
            if (0 != pthread_attr_setschedparam           \
                     (                                    \
                         &tcn_ma_pthread_attr_t_ctrl,     \
                         &tcn_ma_pthread_sched_param_ctrl \
                     ))
            {
                mm_result = AM_FAILURE;
            } /* if (0 != pthread_attr_setschedparam(...)) */
        } /* if (MM_OK == mm_result) */
#endif /* __ECOS__ */

        if (MM_OK == mm_result)
        {
            if (0 != pthread_create
                     (                 \
                         &tcn_ma_pthread_object_ctrl, \
                         &tcn_ma_pthread_attr_t_ctrl, \
                         tcn_ma_agent_routine,        \
                         (void*)NULL
                     ))
            {
                mm_result = AM_FAILURE;
            } /* if (0 != pthread_create(...)) */
        } /* if (MM_OK == mm_result) */

        if (MM_OK == mm_result)
        {
            tcn_ma_pthread_status_ctrl = TRUE;
        } /* if (MM_OK == mm_result) */
    } /* if (FALSE == tcn_ma_pthread_status_ctrl) */

    if (MM_OK == mm_result)
    {
        memset((void*)&am_address, 0, sizeof(am_address));
        tcn_ma_ctrl.action          = TCN_MA_CTRL_ACTION_NONE;
        tcn_ma_ctrl.manager_address = am_address;
        tcn_ma_ctrl.call_msg_adr    = (void*)NULL;
        tcn_ma_ctrl.call_msg_size   = (UNSIGNED32)0;
        tcn_ma_ctrl.service_close   = (MA_SERVICE_CLOSE)NULL;

        tcn_ma_ctrl_default.reply_msg_adr  = (void*)NULL;
        tcn_ma_ctrl_default.reply_msg_size = (UNSIGNED32)0;

        if (FALSE == status_am_bind_replier)
        {
            am_result =                         \
                am_bind_replier                 \
                (                               \
                    AM_AGENT_FCT,               \
                    tcn_ma_receive_confirm,     \
                    tcn_ma_reply_confirm        \
                );
            if (AM_OK != am_result)
            {
                mm_result = (MM_RESULT)am_result;
            } /* if (AM_OK != am_result) */
            else
            {
                status_am_bind_replier = TRUE;
            } /* else */
        } /* if (FALSE == status_am_bind_replier) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        if (FALSE == status_am_receive_request)
        {
            am_result =                         \
                am_receive_request              \
                (                               \
                    AM_AGENT_FCT,               \
                    NULL,                       \
                    TCN_MA_IN_MSG_SIZE_MAX,     \
                    (void*)&tcn_ma_replier_ref  \
                );
            if (AM_OK != am_result)
            {
                mm_result = (MM_RESULT)am_result;
            } /* if (AM_OK != am_result) */
            else
            {
                status_am_receive_request = TRUE;
            } /* else */
        } /* if (FALSE == status_am_receive_request) */
    } /* if (MM_OK == mm_result) */
#endif /* #ifndef TNM_FS_SIM */


    return(mm_result);

} /* ma_init */


/* ==========================================================================
 *
 *  User Service Subscription
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ma_subscribe
 *
 *  Purpose   : Indicates which user procedure to call when a user-defined
 *              service call is received. A previously assigned SIF_code is
 *              overwritten without a warning.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MM_RESULT
 *              ma_subscribe
 *              (
 *                  ENUM16              command,
 *                  ENUM8               sif_code,
 *                  MA_SERVICE_CALL     service_call,
 *                  MA_SERVICE_CLOSE    service_close,
 *                  void                *service_desc
 *              );
 *
 *  Input     : command       - 0=subscribe, 1=desubscribe
 *              sif_code      - user SIF_code (>= 128); see also enumerated
 *                              type 'TCN_SIF_CODE'
 *              service_call  - procedure of type 'MA_SERVICE_CALL' which
 *                              will execute the service when called
 *              service_close - procedure of type 'MA_SERVICE_CLOSE' which
 *                              the Agent will call when the Reply_Message
 *                              has been completely sent (e.g. to free the
 *                              buffer)
 *              service_desc  - descriptor of the service, as a visible
 *                              string terminated by a '00'H character
 *
 *  Return    : any MM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MM_RESULT
ma_subscribe
(
    ENUM16              command,
    ENUM8               sif_code,
    MA_SERVICE_CALL     service_call,
    MA_SERVICE_CLOSE    service_close,
    void                *service_desc
)
{
    MM_RESULT       mm_result   = MM_OK;
    UNSIGNED32      desc_size   = 0;
    TCN_MA_SERVICE  *ma_service;
#ifndef TNM_FS_SIM
    UNSIGNED16      index;
    UNSIGNED16      shift;
#endif /* #ifndef TNM_FS_SIM */


    /* ----------------------------------------------------------------------
     *  check parameters
     * ----------------------------------------------------------------------
     */
    if (128 > sif_code)
    {
        mm_result = MM_SIF_NOT_SUPPORTED;
    } /* if (128 > sif_code) */
    if (MM_OK == mm_result)
    {
        if (NULL == service_call)
        {
            mm_result = (MM_RESULT)AM_FAILURE;
        } /* if (NULL == service_call) */
    } /* if (MM_OK == mm_result) */

    if (MM_OK == mm_result)
    {
        pthread_mutex_lock(&tcn_ma_mutex_object_service);

        switch (command)
        {
            /* --------------------------------------------------------------
             *  subscribe
             * --------------------------------------------------------------
             */
            case (0):
                if (NULL != tcn_ma_service[sif_code])
                {
                    pthread_mutex_unlock(&tcn_ma_mutex_object_service);
                    mm_result = (MM_RESULT)AM_FAILURE;
                } /* if (NULL != tcn_ma_service[sif_code]) */

                if (MM_OK == mm_result)
                {
                    if (NULL == service_desc)
                    {
                        desc_size = 0;
                    } /* if (NULL == service_desc) */

                    desc_size = (UNSIGNED32)strlen(service_desc);
                    if (TCN_MA_SERVICE_DESC_SIZE_MAX < desc_size)
                    {
                        pthread_mutex_unlock(&tcn_ma_mutex_object_service);
                        mm_result = (MM_RESULT)AM_FAILURE;
                    } /* if (TCN_MA_SERVICE_DESC_SIZE_MAX < desc_size) */
                } /* if (MM_OK == mm_result) */

                /* ----------------------------------------------------------
                 *  NOTE:
                 *  The structured type 'TCN_MA_SERVICE' contains one byte
                 *  of *  the string 'service_desc', which is reserved for
                 *  the terminating null byte.
                 *  The value of 'desc_size' contains the number of bytes
                 *  in the current string 'service_desc', not including
                 *  the terminating null byte.
                 *  The allocated memory for 'ma_service' contains space
                 *  for the current string 'service_desc' and its terminating
                 *  null byte.
                 *  Due to this, the function 'strcpy' can be used to copy
                 *  the current string 'service_desc'.
                 * ----------------------------------------------------------
                 */
                if (MM_OK == mm_result)
                {
                    ma_service = (TCN_MA_SERVICE*)malloc( \
                        sizeof(TCN_MA_SERVICE) + desc_size);
                    if (NULL == ma_service)
                    {
                        pthread_mutex_unlock(&tcn_ma_mutex_object_service);
                        mm_result = (MM_RESULT)AM_NO_LOC_MEM_ERR;
                    } /* if (NULL == ma_service) */
                    ma_service->service_call  = service_call;
                    ma_service->service_close = service_close;
                    if (NULL == service_desc)
                    {
                        ma_service->service_desc = (CHARACTER8)0;
                    } /* if (NULL == service_desc) */
                    else
                    {
                        strcpy((char*)&ma_service->service_desc, \
                            (char*)service_desc);
                    } /* else */

                    tcn_ma_service[sif_code] = ma_service;

#ifndef TNM_FS_SIM
                    /* modify 'service_set' of inventory object */
                    index = (UNSIGNED16)(sif_code / 8);
                    shift = (UNSIGNED16)(7 - (sif_code % 8));
                    tcn_ma_inventory_object.service_set.byte[index] |= \
                        (BITSET8)(1 << shift);
#endif /* #ifndef TNM_FS_SIM */
                } /* if (MM_OK == mm_result) */

                break; /* case (0): */

            /* --------------------------------------------------------------
             *  desubscribe
             * --------------------------------------------------------------
             */
            case (1):
                if (NULL == tcn_ma_service[sif_code])
                {
                    pthread_mutex_unlock(&tcn_ma_mutex_object_service);
                    mm_result = (MM_RESULT)AM_FAILURE;
                } /* if (NULL == tcn_ma_service[sif_code]) */

                if (MM_OK == mm_result)
                {
                    free((void*)tcn_ma_service[sif_code]);

                    tcn_ma_service[sif_code] = (TCN_MA_SERVICE*)NULL;

#ifndef TNM_FS_SIM
                    /* modify 'service_set' of inventory object */
                    index = (UNSIGNED16)(sif_code / 8);
                    shift = (UNSIGNED16)(7 - (sif_code % 8));
                    tcn_ma_inventory_object.service_set.byte[index] &= \
                        (BITSET8)~(1 << shift);
#endif /* #ifndef TNM_FS_SIM */
                } /* if (MM_OK == mm_result) */

                break; /* case (1): */

            default:
                mm_result = (MM_RESULT)AM_FAILURE;

        } /* switch (command) */

        pthread_mutex_unlock(&tcn_ma_mutex_object_service);

    } /* if (MM_OK == mm_result) */

    return(mm_result);

} /* ma_subscribe */


#ifndef TNM_FS_SIM

/* ==========================================================================
 *
 *  Restart Procedure Subscription
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ma_subscribe_restart
 *
 *  Purpose   : Indicates which user procedure to call when the Station is
 *              reset or a reservation time-out takes place.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MM_RESULT
 *              ma_subscribe_restart
 *              (
 *                  MA_STATION_RESTART  station_restart
 *              );
 *
 *  Input     : station_restart - procedure of type 'MA_STATION_RESTART',
 *                                which the Agent will call when the
 *                                reservation time-out elapses or when
 *                                a reset command has been received
 *
 *  Return    : any MM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MM_RESULT
ma_subscribe_restart
(
    MA_STATION_RESTART  station_restart
)
{

    tcn_ma_station_restart_function = station_restart;

    return(MM_OK);

} /* ma_subscribe_restart */

#endif /* #ifndef TNM_FS_SIM */


#endif /* #if defined (TCN_MD) || defined (TNM_FS_SIM) */
