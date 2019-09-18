/* ==========================================================================
 *
 *  File      : TCN_STR.C
 *
 *  Purpose   : Check size of TCN structured types
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
#ifdef TCN_STR


/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - common
 * --------------------------------------------------------------------------
 */
#include <tcn_def.h>
#include <tcn_osl.h>

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - supervision
 * --------------------------------------------------------------------------
 */
/* TCN supervision */
#include <tcn_sv.h>
#include <tcn_sv_m.h>
#include <tcn_as.h>

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - process data
 * --------------------------------------------------------------------------
 */
#include <tcn_pd.h>
#include <tcn_ap.h>

#ifdef TCN_MD
/* --------------------------------------------------------------------------
 *  TCN Software Architecture - message data
 * --------------------------------------------------------------------------
 */
#   include <tcn_md.h>
#   include <tcn_am.h>

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - extra (train network management)
 * --------------------------------------------------------------------------
 */
#   include <tcn_tnm.h>
#endif /* #ifdef TCN_MD */

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - extra (configuration manager)
 * --------------------------------------------------------------------------
 */
#include <tcn_cm.h>

/* --------------------------------------------------------------------------
 *  JDP MD-stack
 * --------------------------------------------------------------------------
 */
#ifdef TCN_MD
#   include <stack_am.h>
#endif


/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */
#define TCN_STR_CHECK_SIZEOF_STRUCT(_file_, _type_, _size_)                 \
    TCN_MACRO_START                                                         \
        TCN_OSL_PRINTF("%-10s -", #_file_);                                 \
        TCN_OSL_PRINTF(" %-40s =", #_type_);                                \
        TCN_OSL_PRINTF(" %3d", (WORD16)sizeof(_type_));                     \
        TCN_OSL_PRINTF(" %3d", (WORD16)(_size_));                           \
        if (sizeof(_type_) % 4)                                             \
            TCN_OSL_PRINTF(" [%1d]",(WORD16)(sizeof(_type_) % 4));          \
        else                                                                \
            TCN_OSL_PRINTF("    ");                                         \
        if (sizeof(_type_) != (_size_))                                     \
        {                                                                   \
            TCN_OSL_PRINTF(" - WARNING");                                   \
        } /* if (sizeof((_type_)) != (_size_)) */                           \
        TCN_OSL_PRINTF("\n");                                               \
    TCN_MACRO_END


/* ==========================================================================
 *
 *  Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : tcn_str
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
UNSIGNED16
tcn_str (void)
{

    TCN_OSL_PRINTF("\n");
    /*              1234567890 -                                           */
    TCN_OSL_PRINTF("            ");
    /*               1234567890123456789012345678901234567890 =            */
    TCN_OSL_PRINTF("                                           ");
    /*               123                                                   */
    TCN_OSL_PRINTF("    ");
    /*               123                                                   */
    TCN_OSL_PRINTF(" exp");
    /*               123                                                   */
    TCN_OSL_PRINTF(" MOD");
    TCN_OSL_PRINTF("\n");

    /*              1234567890 -                                           */
    TCN_OSL_PRINTF("file name   ");
    /*               1234567890123456789012345678901234567890 =            */
    TCN_OSL_PRINTF(" name of structured type                   ");
    /*               123                                                   */
    TCN_OSL_PRINTF(" num");
    /*               123                                                   */
    TCN_OSL_PRINTF(" num");
    /*               123                                                   */
    TCN_OSL_PRINTF(" (4)");
    TCN_OSL_PRINTF("\n");

    /*              1234567890 -                                           */
    TCN_OSL_PRINTF("------------");
    /*               1234567890123456789012345678901234567890 =            */
    TCN_OSL_PRINTF("-------------------------------------------");
    /*               123                                                   */
    TCN_OSL_PRINTF("----");
    /*               123                                                   */
    TCN_OSL_PRINTF("----");
    /*               123                                                   */
    TCN_OSL_PRINTF("----");
    TCN_OSL_PRINTF("\n");


    /* ======================================================================
     *
     *  Supervision
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  tcn_def.h
     * ----------------------------------------------------------------------
     */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_def.h,                                                          \
        BITSET256,                                                          \
        TCN_SIZEOF_STRUCT_BITSET256                                         \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_def.h,                                                          \
        TIMEDATE48,                                                         \
        TCN_SIZEOF_STRUCT_TIMEDATE48                                        \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_def.h,                                                          \
        STRING32,                                                           \
        TCN_SIZEOF_STRUCT_STRING32                                          \
    );

    /* ----------------------------------------------------------------------
     *  tcn_sv.h
     * ----------------------------------------------------------------------
     */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv.h,                                                           \
        SV_LL_INFO,                                                         \
        TCN_SIZEOF_STRUCT_SV_LL_INFO                                        \
    );

    /* ----------------------------------------------------------------------
     *  tcn_sv_m.h
     * ----------------------------------------------------------------------
     */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_DEVICE_STATUS,                                               \
        TCN_SIZEOF_STRUCT_SV_MVB_DEVICE_STATUS                              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_STATUS,                                                      \
        TCN_SIZEOF_STRUCT_SV_MVB_STATUS                                     \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_CONTROL,                                                     \
        TCN_SIZEOF_STRUCT_SV_MVB_CONTROL                                    \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_DEVICES_LIST,                                                \
        TCN_SIZEOF_STRUCT_SV_MVB_DEVICES_LIST                               \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_DEVICES,                                                     \
        TCN_SIZEOF_STRUCT_SV_MVB_DEVICES                                    \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_ADMINISTRATOR,                                               \
        TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR                              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_ADMINISTRATOR_PL,                                            \
        TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_PL                           \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_ADMINISTRATOR_PL_CLO,                                        \
        TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_PL_CLO                       \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_ADMINISTRATOR_PL_SLO,                                        \
        TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_PL_SLO                       \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_ADMINISTRATOR_CL,                                            \
        TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_CL                           \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_ADMINISTRATOR_SL,                                            \
        TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_SL                           \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_ADMINISTRATOR_BAL,                                           \
        TCN_SIZEOF_STRUCT_SV_MVB_ADMINISTRATOR_BAL                          \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_ERRCNT_GLOBAL,                                               \
        TCN_SIZEOF_STRUCT_SV_MVB_ERRCNT_GLOBAL                              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_ERRCNT_DS_PORT,                                              \
        TCN_SIZEOF_STRUCT_SV_MVB_ERRCNT_DS_PORT                             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_ERRCNT_PD_PORT,                                              \
        TCN_SIZEOF_STRUCT_SV_MVB_ERRCNT_PD_PORT                             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_MUE_SV_BA_CONFIG,                                            \
        TCN_SIZEOF_STRUCT_SV_MVB_MUE_SV_BA_CONFIG                           \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_MUE_SV_DEVICE_STATUS,                                        \
        TCN_SIZEOF_STRUCT_SV_MVB_MUE_SV_DEVICE_STATUS                       \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_sv_m.h,                                                         \
        SV_MVB_MUE_PD_PORT_DATA,                                            \
        TCN_SIZEOF_STRUCT_SV_MVB_MUE_PD_PORT_DATA                           \
    );


    /* ======================================================================
     *
     *  Process Data
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  tcn_pd.h
     * ----------------------------------------------------------------------
     */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_pd.h,                                                           \
        PD_PRT_ATTR,                                                        \
        TCN_SIZEOF_STRUCT_PD_PRT_ATTR                                       \
    );

    /* ----------------------------------------------------------------------
     *  tcn_ap.h
     * ----------------------------------------------------------------------
     */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_ap.h,                                                           \
        DS_NAME,                                                            \
        TCN_SIZEOF_STRUCT_DS_NAME                                           \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_ap.h,                                                           \
        PV_NAME,                                                            \
        TCN_SIZEOF_STRUCT_PV_NAME                                           \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_ap.h,                                                           \
        PV_LIST,                                                            \
        TCN_SIZEOF_STRUCT_PV_LIST                                           \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_ap.h,                                                           \
        PV_SET,                                                             \
        TCN_SIZEOF_STRUCT_PV_SET                                            \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_ap.h,                                                           \
        PV_CLUSTER,                                                         \
        TCN_SIZEOF_STRUCT_PV_CLUSTER                                        \
    );


#ifdef TCN_MD
    /* ======================================================================
     *
     *  Message Data
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  stack_am.h
     * ----------------------------------------------------------------------
     */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        stack_am.h,                                                         \
        STACK_AM_ADDRESS,                                                   \
        TCN_SIZEOF_STRUCT_STACK_AM_ADDRESS                                  \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        stack_am.h,                                                         \
        STACK_AM_DIR_ENTRY,                                                 \
        TCN_SIZEOF_STRUCT_STACK_AM_DIR_ENTRY                                \
    );

    /* ----------------------------------------------------------------------
     *  tcn_md.h
     * ----------------------------------------------------------------------
     */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_md.h,                                                           \
        MD_PACKET_NETWORK_HEADER,                                           \
        TCN_SIZEOF_STRUCT_MD_PACKET_NETWORK_HEADER                          \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_md.h,                                                           \
        MD_PACKET_NETWORK_DATA,                                             \
        TCN_SIZEOF_STRUCT_MD_PACKET_NETWORK_DATA                            \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_md.h,                                                           \
        MD_PACKET_LINK_HEADER_MVB,                                          \
        TCN_SIZEOF_STRUCT_MD_PACKET_LINK_HEADER_MVB                         \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_md.h,                                                           \
        MD_PACKET_LINK_HEADER_WTB,                                          \
        TCN_SIZEOF_STRUCT_MD_PACKET_LINK_HEADER_WTB                         \
    );

    /* ----------------------------------------------------------------------
     *  tcn_am.h
     * ----------------------------------------------------------------------
     */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_am.h,                                                           \
        AM_ADDRESS,                                                         \
        TCN_SIZEOF_STRUCT_AM_ADDRESS                                        \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_am.h,                                                           \
        AM_MESSENGER_STATUS,                                                \
        TCN_SIZEOF_STRUCT_AM_MESSENGER_STATUS                               \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_am.h,                                                           \
        AM_MESSENGER_CONTROL,                                               \
        TCN_SIZEOF_STRUCT_AM_MESSENGER_CONTROL                              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_am.h,                                                           \
        AM_STADI_ENTRY,                                                     \
        TCN_SIZEOF_STRUCT_AM_STADI_ENTRY                                    \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_am.h,                                                           \
        AM_DIR_ENTRY,                                                       \
        TCN_SIZEOF_STRUCT_AM_DIR_ENTRY                                      \
    );

    /* ----------------------------------------------------------------------
     *  tcn_tnm.h
     * ----------------------------------------------------------------------
     */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_HEADER,                                                     \
        TCN_SIZEOF_STRUCT_TNM_MSG_HEADER                                    \
    );
    /* Station Service - Read_Station_Status */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Read_Station_Status,                                   \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Station_Status                  \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Read_Station_Status,                                  \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Station_Status                 \
    );
    /* Station Service - Write_Station_Control */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Write_Station_Control,                                 \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Station_Control                \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Write_Station_Control,                                \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Station_Control               \
    );
    /* Station Service - Read_Station_Inventory */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Read_Station_Inventory,                                \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Station_Inventory               \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Read_Station_Inventory,                               \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Station_Inventory              \
    );
    /* Station Service - Write_Station_Reservation */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Write_Station_Reservation,                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Station_Reservation            \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Write_Station_Reservation,                            \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Station_Reservation           \
    );
    /* Station Service - Read_Service_Descriptor */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Read_Service_Descriptor,                               \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Service_Descriptor              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Read_Service_Descriptor,                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Service_Descriptor             \
    );
    /* Station Service - link descriptor */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_LINK_DESCRIPTOR,                                                \
        TCN_SIZEOF_STRUCT_TNM_LINK_DESCRIPTOR                               \
    );
    /* Station Service - Read_Links_Descriptor */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Read_Links_Descriptor,                                 \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Links_Descriptor                \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Read_Links_Descriptor,                                \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Links_Descriptor               \
    );
    /* MVB Link Service - Read_Mvb_Status */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Read_Mvb_Status,                                       \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Mvb_Status                      \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Read_Mvb_Status,                                      \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Mvb_Status                     \
    );
    /* MVB Link Service - Write_Mvb_Control */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Write_Mvb_Control,                                     \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Mvb_Control                    \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Write_Mvb_Control,                                    \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Mvb_Control                   \
    );
    /* MVB Link Service - Read_Mvb_Devices */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Read_Mvb_Devices,                                      \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Mvb_Devices                     \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Read_Mvb_Devices,                                     \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Mvb_Devices                    \
    );
    /* MVB Link Service - Write_Mvb_Administrator */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Write_Mvb_Administrator,                               \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Mvb_Administrator              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Write_Mvb_Administrator,                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Mvb_Administrator             \
    );
    /* Variables Service - port object */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_PORT_OBJECT,                                                    \
        TCN_SIZEOF_STRUCT_TNM_PORT_OBJECT                                   \
    );
    /* Variables Service - Read_Ports_Configuration */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Read_Ports_Configuration,                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Ports_Configuration             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Read_Ports_Configuration,                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Ports_Configuration            \
    );
    /* Messages Service - Read_Messenger_Status */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Read_Messenger_Status,                                 \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Messenger_Status                \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Read_Messenger_Status,                                \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Messenger_Status               \
    );
    /* Messages Service - Write_Messenger_Control */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Write_Messenger_Control,                               \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Messenger_Control              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Write_Messenger_Control,                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Messenger_Control             \
    );
    /* Messages Service - Read_Function_Directory */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Read_Function_Directory,                               \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Read_Function_Directory              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Read_Function_Directory,                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Read_Function_Directory             \
    );
    /* Messages Service - Write_Function_Directory */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_Write_Function_Directory,                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_Write_Function_Directory             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_Write_Function_Directory,                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_Write_Function_Directory            \
    );
    /* File System Service - FS_access */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_access,                                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_access                            \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_access,                                            \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_access                           \
    );
    /* File System Service - FS_open */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_open,                                               \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_open                              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_open,                                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_open                             \
    );
    /* File System Service - FS_close */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_close,                                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_close                             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_close,                                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_close                            \
    );
    /* File System Service - FS_read */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_read,                                               \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_read                              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_read,                                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_read                             \
    );
    /* File System Service - FS_write */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_write,                                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_write                             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_write,                                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_write                            \
    );
    /* File System Service - FS_lseek */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_lseek,                                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_lseek                             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_lseek,                                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_lseek                            \
    );
    /* File System Service - FS_unlink */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_unlink,                                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_unlink                            \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_unlink,                                            \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_unlink                           \
    );
    /* File System Service - FS_stat */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_stat,                                               \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_stat                              \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_stat,                                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_stat                             \
    );
    /* File System Service - FS_pathconf */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_pathconf,                                           \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_pathconf                          \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_pathconf,                                          \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_pathconf                         \
    );
    /* File System Service - FS_getcwd */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_getcwd,                                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_getcwd                            \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_getcwd,                                            \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_getcwd                           \
    );
    /* File System Service - FS_chdir */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_chdir,                                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_chdir                             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_chdir,                                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_chdir                            \
    );
    /* File System Service - FS_mkdir */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_mkdir,                                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_mkdir                             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_mkdir,                                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_mkdir                            \
    );
    /* File System Service - FS_rmdir */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_rmdir,                                              \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_rmdir                             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_rmdir,                                             \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_rmdir                            \
    );
    /* File System Service - FS_opendir */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_opendir,                                            \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_opendir                           \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_opendir,                                           \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_opendir                          \
    );
    /* File System Service - FS_closedir */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_closedir,                                           \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_closedir                          \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_closedir,                                          \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_closedir                         \
    );
    /* File System Service - FS_readdir */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Call_FS_readdir,                                            \
        TCN_SIZEOF_STRUCT_TNM_MSG_Call_FS_readdir                           \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_readdir,                                           \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_readdir                          \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_tnm.h,                                                          \
        TNM_MSG_Reply_FS_rewinddir,                                         \
        TCN_SIZEOF_STRUCT_TNM_MSG_Reply_FS_rewinddir                        \
    );
#endif /* #ifdef TCN_MD */


    /* ======================================================================
     *
     *  Configuration Manager
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  tcn_cm.h
     * ----------------------------------------------------------------------
     */
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_cm.h,                                                           \
        CM_D2000_ENTRIES_LIST,                                              \
        TCN_SIZEOF_STRUCT_CM_D2000_ENTRIES_LIST                             \
    );
    TCN_STR_CHECK_SIZEOF_STRUCT                                             \
    (                                                                       \
        tcn_cm.h,                                                           \
        CM_D2000_PARAMETER,                                                 \
        TCN_SIZEOF_STRUCT_CM_D2000_PARAMETER                                \
    );


    TCN_OSL_PRINTF("\n");
    TCN_OSL_PRINTF \
        ("***********************************************************\n");
    TCN_OSL_PRINTF \
        ("* Check size of structured types terminates successfully! *\n");
    TCN_OSL_PRINTF \
        ("***********************************************************\n");

    return(0);

} /* tcn_str */


/* --------------------------------------------------------------------------
 *  Procedure : main
 * --------------------------------------------------------------------------
 */
int
main (void)
{
    UNSIGNED16  result;

    result = tcn_str();

    TCN_MACRO_TERMINATE_MAIN(result);

    return((int)result);

} /* main */


#endif /* #ifdef TCN_STR */
