/* ==========================================================================
 *
 *  File      : MDFL_LS.C
 *
 *  Purpose   : Bus Controller Link Layer for MDFULL - 
 *                  Link Layer Bus Controller Interface for Supervision
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
#ifdef MUELL_MDFL


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
#include <tcn_sv.h>
#include <tcn_sv_m.h>
#include <tcn_ls.h>

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer - MUELL
 * --------------------------------------------------------------------------
 */
#include <muell_ls.h>

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer - MDFULL
 * --------------------------------------------------------------------------
 */
#include <mdfl_bc.h>
#include <mdfl_ls.h>
#include <mdfl_lm.h>

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>
#include <mue_sv.h>
#include <mue_pd.h>


/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Mutex for UART access
 * --------------------------------------------------------------------------
 */
#if (MDFULL_BC_MUTEX_UART == 1)

#define MDFULL_LS_MACRO_UART_MUTEX_LOCK(_p_osl_mutex_object_uart_)          \
    pthread_mutex_lock((_p_osl_mutex_object_uart_))

#define MDFULL_LS_MACRO_UART_MUTEX_UNLOCK(_p_osl_mutex_object_uart_)        \
    pthread_mutex_unlock((_p_osl_mutex_object_uart_))

#else /* #if (MDFULL_BC_MUTEX_UART == 1) */

#define MDFULL_LS_MACRO_UART_MUTEX_LOCK(_p_osl_mutex_object_uart_)
#define MDFULL_LS_MACRO_UART_MUTEX_UNLOCK(_p_osl_mutex_object_uart_)

#endif /* #else */


/* ==========================================================================
 *
 *  Local Procedures
 *
 * ==========================================================================
 */

#ifdef MDFULL_BC_ERRCNT_ALL_PD

/* --------------------------------------------------------------------------
 *  Procedure : mdfull_ls_service_errcnt_get_pd_port
 *
 *  Purpose   : Handles link layer service
 *              'SV_MVB_SERVICE_ERRCNT_GET_PD_PORT'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              mdfull_ls_service_errcnt_get_pd_port
 *              (
 *                  void                    *p_bus_ctrl,
 *                  SV_MVB_ERRCNT_PD_PORT   *p_errcnt_pd_port
 *              );
 *
 *  Input     : p_bus_ctrl        - pointer to bus controller specific values
 *
 *  In-/Output: p_errcnt_pd_port  - pointer to a memory buffer of structured
 *                                  type 'SV_MVB_ERRCNT_PD_PORT'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
mdfull_ls_service_errcnt_get_pd_port
(
    void                    *p_bus_ctrl,
    SV_MVB_ERRCNT_PD_PORT   *p_errcnt_pd_port
)
{
    LS_RESULT       ls_result          = LS_OK;
    MDFULL_BUS_CTRL *p_mdfull_bus_ctrl = (MDFULL_BUS_CTRL*)p_bus_ctrl;
    WORD16          port_address;
    UNSIGNED16      counter;

    MDFULL_LS_MACRO_UART_MUTEX_LOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    /* clear process data port specific error counter object */
    port_address = p_errcnt_pd_port->port_address;
    memset((void*)p_errcnt_pd_port, 0, sizeof(SV_MVB_ERRCNT_PD_PORT));
    p_errcnt_pd_port->port_address = port_address;

    /* check input parameters */
    if (port_address > mue_pd_port_config_la_mask)
    {
        ls_result = LS_CONFIG;
    } /* if (port_address > mue_pd_port_config_la_mask) */

    /* check, if port is defined */
    if (LS_OK == ls_result)
    {
        if (0 == p_mdfull_bus_ctrl->lp_size[port_address])
        {
            ls_result = LS_ERROR;
        } /* if (0 == p_mdfull_bus_ctrl->lp_size[port_address]) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        p_errcnt_pd_port->errors_lineA = \
            p_mdfull_bus_ctrl->errcnt_pd_port_lineA[port_address];
        p_errcnt_pd_port->errors_lineB = \
            p_mdfull_bus_ctrl->errcnt_pd_port_lineB[port_address];
        if (p_errcnt_pd_port->command & SV_MVB_ERRCNT_CMD_BIT_RESET_SEL)
        {
            p_mdfull_bus_ctrl->errcnt_pd_port_lineA[port_address] = 0;
            p_mdfull_bus_ctrl->errcnt_pd_port_lineB[port_address] = 0;
        } /* if (p_errcnt_pd_port->command & ...) */
        if (p_errcnt_pd_port->command & SV_MVB_ERRCNT_CMD_BIT_RESET_ALL)
        {
            for (counter=0; counter<mdfull_lp_port_number_max; counter++)
            {
                p_mdfull_bus_ctrl->errcnt_pd_port_lineA[counter] = 0;
                p_mdfull_bus_ctrl->errcnt_pd_port_lineB[counter] = 0;
            } /* for (counter=0; counter<mdfull_lp_port_number_max; ...) */
        } /* if (p_errcnt_pd_port->command & ...) */
    } /* if (LS_OK == ls_result) */

    MDFULL_LS_MACRO_UART_MUTEX_UNLOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    return(ls_result);

} /* mdfull_ls_service_errcnt_get_pd_port */

#endif /* #ifdef MDFULL_BC_ERRCNT_ALL_PD */


/* --------------------------------------------------------------------------
 *  Procedure : mdfull_ls_service_mue_pd_get_port_data
 *
 *  Purpose   : Handles link layer service
 *              'SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              mdfull_ls_service_mue_pd_get_port_data
 *              (
 *                  void                    *p_bus_ctrl,
 *                  SV_MVB_MUE_PD_PORT_DATA *p_mue_pd_port_data
 *              );
 *
 *  Input     : p_bus_ctrl            - pointer to bus controller specific
 *                                      values
 *
 *  In-/Output: p_mue_pd_port_data    - pointer to a memory buffer of
 *                                      structured type
 *                                      'SV_MVB_MUE_PD_PORT_DATA'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
mdfull_ls_service_mue_pd_get_port_data
(
    void                    *p_bus_ctrl,
    SV_MVB_MUE_PD_PORT_DATA *p_mue_pd_port_data
)
{
    LS_RESULT       ls_result          = LS_OK;
    MDFULL_BUS_CTRL *p_mdfull_bus_ctrl = (MDFULL_BUS_CTRL*)p_bus_ctrl;
    WORD16          port_address;
    UNSIGNED8       port_size          = 0;
    MUE_RESULT      mue_result;

    MDFULL_LS_MACRO_UART_MUTEX_LOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    /* clear MUE PD port data object */
    port_address = p_mue_pd_port_data->port_address;
    memset((void*)p_mue_pd_port_data, 0, sizeof(SV_MVB_MUE_PD_PORT_DATA));
    p_mue_pd_port_data->port_address = port_address;

    /* check input parameters */
    if (port_address > mue_pd_port_config_la_mask)
    {
        ls_result = LS_CONFIG;
    } /* if (port_address > mue_pd_port_config_la_mask) */

    if (LS_OK == ls_result)
    {
        /* get port size configuration */
        port_size = p_mdfull_bus_ctrl->lp_size[port_address];

        /* check, if port is defined */
        if (0 == port_size)
        {
            ls_result = LS_ERROR;
        } /* if (0 == port_size) */
        else
        {
            p_mue_pd_port_data->port_size = port_size;
        } /* else */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        mue_result =                                \
            mue_pd_full_get_port_data               \
            (                                       \
                p_bus_ctrl,                         \
                port_address,                       \
                (BOOLEAN1)FALSE,                    \
                port_size,                          \
                &p_mue_pd_port_data->port_status,   \
                p_mue_pd_port_data->port_data,      \
                &p_mue_pd_port_data->port_freshness \
            );
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_mdfull_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        /* handle Error Counters */
        mdfull_ls_errcnt_pd                 \
        (                                   \
            p_mdfull_bus_ctrl,              \
            port_address,                   \
            p_mue_pd_port_data->port_status \
        );
    } /* if (LS_OK == ls_result) */

    MDFULL_LS_MACRO_UART_MUTEX_UNLOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    return(ls_result);

} /* mdfull_ls_service_mue_pd_get_port_data */


/* ==========================================================================
 *
 *  Miscellaneous Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mdfull_ls_errcnt_pd
 *
 *  Purpose   : Handles error counters of a process data port.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              void
 *              mdfull_ls_errcnt_pd
 *              (
 *                  MDFULL_BUS_CTRL *p_mdfull_bus_ctrl,
 *                  WORD16          port_address,
 *                  BITSET8         mue_port_status
 *              );
 *
 *  Input     : p_mdfull_bus_ctrl - pointer to MDFULL bus controller
 *                                  specific values
 *              port_address      - port identifier (12-bit logical adddress)
 *              mue_port_status   - status information releated to a MVB
 *                                  process data port;
 *                                  any combination of constants
 *                                  'MUE_PD_PORT_STATUS_xxx' or constants
 *                                  'mue_pd_port_status_xxx'
 *
 *  Remarks   : - A link layer is identified by 'p_mdfull_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
void
mdfull_ls_errcnt_pd
(
    MDFULL_BUS_CTRL *p_mdfull_bus_ctrl,
    WORD16          port_address,
    BITSET8         mue_port_status
)
{
    BITSET8     mue_status_sink;
    UNSIGNED32  *p_errcnt_lineA;
    UNSIGNED32  *p_errcnt_lineB;

    if ((p_mdfull_bus_ctrl->mue_sv_device_config & \
        mue_sv_device_config_line_mask) == mue_sv_device_config_line_mask)
    {
        mue_status_sink = \
            (BITSET8)(mue_port_status & mue_pd_port_status_sink_mask);

        p_errcnt_lineA = (UNSIGNED32*) \
            &p_mdfull_bus_ctrl->errcnt_global_lineA;
        p_errcnt_lineB = (UNSIGNED32*) \
            &p_mdfull_bus_ctrl->errcnt_global_lineB;

        if (mue_status_sink == mue_pd_port_status_sink_b)
        {
            if (0xFFFFFFFFUL > *p_errcnt_lineA)
            {
                (*p_errcnt_lineA)++;
            } /* if (0xFFFFFFFFUL > *p_errcnt_lineA) */
        } /* if (mue_status_sink == mue_pd_port_status_sink_b) */
        if (mue_status_sink == mue_pd_port_status_sink_a)
        {
            if (0xFFFFFFFFUL > *p_errcnt_lineB)
            {
                (*p_errcnt_lineB)++;
            } /* if (0xFFFFFFFFUL > *p_errcnt_lineB) */
        } /* if (mue_status_sink == mue_pd_port_status_sink_a) */

#ifdef MDFULL_BC_ERRCNT_ALL_PD
        p_errcnt_lineA = (UNSIGNED32*) \
            &p_mdfull_bus_ctrl->errcnt_pd_port_lineA[port_address];
        p_errcnt_lineB = (UNSIGNED32*) \
            &p_mdfull_bus_ctrl->errcnt_pd_port_lineB[port_address];

        if (mue_status_sink == mue_pd_port_status_sink_b)
        {
            if (0xFFFFFFFFUL > *p_errcnt_lineA)
            {
                (*p_errcnt_lineA)++;
            } /* if (0xFFFFFFFFUL > *p_errcnt_lineA) */
        } /* if (mue_status_sink == mue_pd_port_status_sink_b) */
        if (mue_status_sink == mue_pd_port_status_sink_a)
        {
            if (0xFFFFFFFFUL > *p_errcnt_lineB)
            {
                (*p_errcnt_lineB)++;
            } /* if (0xFFFFFFFFUL > *p_errcnt_lineB) */
        } /* if (mue_status_sink == mue_pd_port_status_sink_a) */
#else /* #ifdef MDFULL_BC_ERRCNT_ALL_PD */
        /* avoid warnings */
        port_address = port_address;
#endif /* #else */

    } /* if ((p_mdfull_bus_ctrl->mue_sv_device_config & ...) == ...) */

} /* mdfull_ls_errcnt_pd */


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mdfull_ls_init (of procedure type 'LS_INIT')
 *
 *  Purpose   : Initialises a link layer as a whole.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              mdfull_ls_init
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 *              - This procedure has to be called at system
 *                initialisation before calling any other 'mdfull_ls_xxx',
 *                'mdfull_lp_xxx' or 'mdfull_lm_xxx' procedure referencing
 *                the same link layer (parameter 'p_bus_ctrl').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
mdfull_ls_init
(
    void        *p_bus_ctrl
)
{
    LS_RESULT       ls_result          = LS_OK;
    MDFULL_BUS_CTRL *p_mdfull_bus_ctrl = (MDFULL_BUS_CTRL*)p_bus_ctrl;
    UNSIGNED16      counter;
    MUE_RESULT      mue_result;


    /* ======================================================================
     *
     *  bus controller structure (dynamic values)
     *
     * ======================================================================
     */

#if (MDFULL_BC_MUTEX_UART == 1)
    /* ----------------------------------------------------------------------
     *  OSL
     * ----------------------------------------------------------------------
     */
    /* initialise mutex for UART */
    if (FALSE == p_mdfull_bus_ctrl->osl_mutex_status_uart)
    {
        if (0 != pthread_mutex_init( \
            &p_mdfull_bus_ctrl->osl_mutex_object_uart, NULL))
        {
            ls_result = LS_ERROR;
        } /* if (0 != pthread_mutex_init(...)) */
        else
        {
            p_mdfull_bus_ctrl->osl_mutex_status_uart = TRUE;
        } /* else */
    } /* if (FALSE == p_mdfull_bus_ctrl->osl_mutex_status_uart) */
    if (TRUE == p_mdfull_bus_ctrl->osl_mutex_status_uart)
    {
        if (0 != pthread_mutex_lock( \
            &p_mdfull_bus_ctrl->osl_mutex_object_uart))
        {
            ls_result = LS_ERROR;
        } /* if (0 != pthread_mutex_lock(...)) */
        if (LS_OK == ls_result)
        {
            if (0 != pthread_mutex_unlock( \
                &p_mdfull_bus_ctrl->osl_mutex_object_uart))
            {
                ls_result = LS_ERROR;
            } /* if (0 != pthread_mutex_unlock(...)) */
        } /* if (LS_OK == ls_result) */
    } /* if (TRUE == p_mdfull_bus_ctrl->osl_mutex_status_uart) */
#endif /* #if (MDFULL_BC_MUTEX_UART == 1) */


    MDFULL_LS_MACRO_UART_MUTEX_LOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);


    if (LS_OK == ls_result)
    {
        ls_result = muell_ls_init(p_bus_ctrl);
    } /* if (LS_OK == ls_result) */


    /* ----------------------------------------------------------------------
     *  Clear Error Counters
     * ----------------------------------------------------------------------
     */
#ifdef MDFULL_BC_ERRCNT_ALL_PD
    /* error counters - all process data ports */
    for (counter=0; counter<mdfull_lp_port_number_max; counter++)
    {
        p_mdfull_bus_ctrl->errcnt_pd_port_lineA[counter] = 0;
        p_mdfull_bus_ctrl->errcnt_pd_port_lineB[counter] = 0;
    } /* for (counter=0; counter<mdfull_lp_port_number_max; counter++) */
#endif /* #ifdef MDFULL_BC_ERRCNT_ALL_PD */


    /* ----------------------------------------------------------------------
     *  LP
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        /* clear freshness supervision interval */
        p_mdfull_bus_ctrl->lp_fsi = 0;
        for (counter=0; counter<mdfull_lp_port_number_max; counter++)
        {
            /* store port size configuration (0=undefined) */
            p_mdfull_bus_ctrl->lp_size[counter] = 0;
        } /* for (counter=0; counter<mdfull_lp_port_number_max; counter++) */
    } /* if (LS_OK == ls_result) */


    /* ----------------------------------------------------------------------
     *  clear process data port configuration
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        /* define all ports as SINK with address 0x0000 and size 0 */
        counter = 0;
        while ((counter < mdfull_lp_port_number_max) && (LS_OK == ls_result))
        {
            mue_result =                    \
                mue_pd_full_put_port_config \
                (                           \
                    p_bus_ctrl,             \
                    (BITSET16)counter       \
                );
            if (MUE_RESULT_OK != mue_result)
            {
                /* store last result of MUE */
                p_mdfull_bus_ctrl->mue_result = mue_result;

                ls_result = LS_ERROR;
            } /* if (MUE_RESULT_OK != mue_result) */
            counter++;
        } /* while ((counter < mdfull_lp_port_number_max) && (...)) */
    } /* if (LS_OK == ls_result) */


    MDFULL_LS_MACRO_UART_MUTEX_UNLOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);


    return(ls_result);

} /* mdfull_ls_init */


/* --------------------------------------------------------------------------
 *  Procedure : mdfull_ls_service_handler
 *              (of procedure type 'LS_SERVICE_HANDLER')
 *
 *  Purpose   : Handles link layer specific services.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              mdfull_ls_service_handler
 *              (
 *                  void        *p_bus_ctrl,
 *                  ENUM8       service,
 *                  void        *p_parameter
 *              );
 *
 *  Input     : p_bus_ctrl  - pointer to bus controller specific values
 *              service     - link layer service
 *
 *  In-/Output: p_parameter - pointer to a memory buffer which contains
 *                            service specific values
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
mdfull_ls_service_handler
(
    void        *p_bus_ctrl,
    ENUM8       service,
    void        *p_parameter
)
{
    LS_RESULT       ls_result          = LS_OK;
    MDFULL_BUS_CTRL *p_mdfull_bus_ctrl = (MDFULL_BUS_CTRL*)p_bus_ctrl;
    SV_LL_INFO      *ll_info;

    switch (service)
    {
        /* ------------------------------------------------------------------
         *  SV_LL_SERVICE_READ_INFO
         * ------------------------------------------------------------------
         */
        case (SV_LL_SERVICE_READ_INFO):
            /* set Link Layer Information Object */
            ll_info = (SV_LL_INFO*)p_parameter;

            /* process data */
            ll_info->pd_port_number_max  = mdfull_lp_port_number_max;
            ll_info->pd_port_size_number = mdfull_lp_port_size_number;
            memcpy                                  \
            (                                       \
                (void*)ll_info->pd_port_size_value, \
                (void*)mdfull_lp_port_size_value,   \
                16                                  \
            );

            /* message data */
#if defined(TCN_MD) && defined(MUELL_MDFL)
            ll_info->md_frame_size       = mdfull_lm_frame_size;
#else
            ll_info->md_frame_size       = 0;
#endif

            break;

#ifdef MDFULL_BC_ERRCNT_ALL_PD
        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_ERRCNT_GET_PD_PORT
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_ERRCNT_GET_PD_PORT):
            ls_result = mdfull_ls_service_errcnt_get_pd_port(p_bus_ctrl, \
                (SV_MVB_ERRCNT_PD_PORT*)p_parameter);
            break;
#endif /* #ifdef MDFULL_BC_ERRCNT_ALL_PD */

        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA):
            ls_result = mdfull_ls_service_mue_pd_get_port_data(p_bus_ctrl, \
                (SV_MVB_MUE_PD_PORT_DATA*)p_parameter);
            break;

        /* ------------------------------------------------------------------
         *  default
         * ------------------------------------------------------------------
         */
        default:
            MDFULL_LS_MACRO_UART_MUTEX_LOCK( \
                &p_mdfull_bus_ctrl->osl_mutex_object_uart);

            ls_result = \
                muell_ls_service_handler(p_bus_ctrl, service, p_parameter);

            MDFULL_LS_MACRO_UART_MUTEX_UNLOCK( \
                &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    } /* switch (service) */

    return(ls_result);

} /* mdfull_ls_service_handler */


#endif /* #ifdef MUELL_MDFL */
