/* ==========================================================================
 *
 *  File      : PD16_LS.C
 *
 *  Purpose   : Bus Controller Link Layer for PD16 - 
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
//#define MUELL_PD16  1
 
#ifdef MUELL_PD16


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
 *  TCN Bus Controller Link Layer - PD16
 * --------------------------------------------------------------------------
 */
#include <pd16_bc.h>
#include <pd16_ls.h>

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
#if (PD16_BC_MUTEX_UART == 1)

#define PD16_LS_MACRO_UART_MUTEX_LOCK(_p_osl_mutex_object_uart_)            \
    pthread_mutex_lock((_p_osl_mutex_object_uart_))

#define PD16_LS_MACRO_UART_MUTEX_UNLOCK(_p_osl_mutex_object_uart_)          \
    pthread_mutex_unlock((_p_osl_mutex_object_uart_))

#else /* #if (PD16_BC_MUTEX_UART == 1) */

#define PD16_LS_MACRO_UART_MUTEX_LOCK(_p_osl_mutex_object_uart_)
#define PD16_LS_MACRO_UART_MUTEX_UNLOCK(_p_osl_mutex_object_uart_)

#endif /* #else */


/* ==========================================================================
 *
 *  Local Procedures
 *
 * ==========================================================================
 */

#ifdef PD16_BC_ERRCNT_ALL_PD

/* --------------------------------------------------------------------------
 *  Procedure : pd16_ls_service_errcnt_get_pd_port
 *
 *  Purpose   : Handles link layer service
 *              'SV_MVB_SERVICE_ERRCNT_GET_PD_PORT'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              pd16_ls_service_errcnt_get_pd_port
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
pd16_ls_service_errcnt_get_pd_port
(
    void                    *p_bus_ctrl,
    SV_MVB_ERRCNT_PD_PORT   *p_errcnt_pd_port
)
{
    LS_RESULT       ls_result        = LS_OK;
    PD16_BUS_CTRL   *p_pd16_bus_ctrl = (PD16_BUS_CTRL*)p_bus_ctrl;
    WORD16          port_address;
    WORD8           mue_port_handle  = 0;
    WORD16          *p_lp_address;
    UNSIGNED16      counter;

    PD16_LS_MACRO_UART_MUTEX_LOCK(&p_pd16_bus_ctrl->osl_mutex_object_uart);

    /* clear process data port specific error counter object */
    port_address = p_errcnt_pd_port->port_address;
    memset((void*)p_errcnt_pd_port, 0, sizeof(SV_MVB_ERRCNT_PD_PORT));
    p_errcnt_pd_port->port_address = port_address;

    /* check input parameters */
    if (port_address > mue_pd_port_config_la_mask)
    {
        ls_result = LS_CONFIG;
    } /* if (port_address > mue_pd_port_config_la_mask) */

    /* check, if port is defined; get port handle */
    if (LS_OK == ls_result)
    {
        ls_result       = LS_ERROR;
        mue_port_handle = 0;
        p_lp_address    = (WORD16*)&p_pd16_bus_ctrl->lp_address[0];
        while ((mue_port_handle < pd16_lp_port_number_max) && \
               (LS_OK != ls_result))
        {
            if (*p_lp_address == port_address)
            {
                ls_result = LS_OK;
            } /* if (*p_lp_address == port_address) */
            else
            {
                mue_port_handle++;
                p_lp_address++;
            } /* else */
        } /* while ((mue_port_handle < pd16_lp_port_number_max) && (...)) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        p_errcnt_pd_port->errors_lineA = \
            p_pd16_bus_ctrl->errcnt_pd_port_lineA[mue_port_handle];
        p_errcnt_pd_port->errors_lineB = \
            p_pd16_bus_ctrl->errcnt_pd_port_lineB[mue_port_handle];
        if (p_errcnt_pd_port->command & SV_MVB_ERRCNT_CMD_BIT_RESET_SEL)
        {
            p_pd16_bus_ctrl->errcnt_pd_port_lineA[mue_port_handle] = 0;
            p_pd16_bus_ctrl->errcnt_pd_port_lineB[mue_port_handle] = 0;
        } /* if (p_errcnt_pd_port->command & ...) */
        if (p_errcnt_pd_port->command & SV_MVB_ERRCNT_CMD_BIT_RESET_ALL)
        {
            for (counter=0; counter<pd16_lp_port_number_max; counter++)
            {
                p_pd16_bus_ctrl->errcnt_pd_port_lineA[counter] = 0;
                p_pd16_bus_ctrl->errcnt_pd_port_lineB[counter] = 0;
            } /* for (counter=0; counter<pd16_lp_port_number_max; ...) */
        } /* if (p_errcnt_pd_port->command & ...) */
    } /* if (LS_OK == ls_result) */

    PD16_LS_MACRO_UART_MUTEX_UNLOCK(&p_pd16_bus_ctrl->osl_mutex_object_uart);

    return(ls_result);

} /* pd16_ls_service_errcnt_get_pd_port */

#endif /* #ifdef PD16_BC_ERRCNT_ALL_PD */


/* --------------------------------------------------------------------------
 *  Procedure : pd16_ls_service_mue_pd_get_port_data
 *
 *  Purpose   : Handles link layer service
 *              'SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              pd16_ls_service_mue_pd_get_port_data
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
pd16_ls_service_mue_pd_get_port_data
(
    void                    *p_bus_ctrl,
    SV_MVB_MUE_PD_PORT_DATA *p_mue_pd_port_data
)
{
    LS_RESULT       ls_result        = LS_OK;
    PD16_BUS_CTRL   *p_pd16_bus_ctrl = (PD16_BUS_CTRL*)p_bus_ctrl;
    WORD16          port_address;
    WORD8           mue_port_handle  = 0;
    WORD16          *p_lp_address;
    UNSIGNED8       port_size;
    MUE_RESULT      mue_result;

    PD16_LS_MACRO_UART_MUTEX_LOCK(&p_pd16_bus_ctrl->osl_mutex_object_uart);

    /* clear MUE PD port data object */
    port_address = p_mue_pd_port_data->port_address;
    memset((void*)p_mue_pd_port_data, 0, sizeof(SV_MVB_MUE_PD_PORT_DATA));
    p_mue_pd_port_data->port_address = port_address;

    /* check input parameters */
    if (port_address > mue_pd_port_config_la_mask)
    {
        ls_result = LS_CONFIG;
    } /* if (port_address > mue_pd_port_config_la_mask) */

    /* check, if port is defined; get port handle */
    if (LS_OK == ls_result)
    {
        ls_result       = LS_ERROR;
        mue_port_handle = 0;
        p_lp_address    = (WORD16*)&p_pd16_bus_ctrl->lp_address[0];
        while ((mue_port_handle < pd16_lp_port_number_max) && \
               (LS_OK != ls_result))
        {
            if (*p_lp_address == port_address)
            {
                ls_result = LS_OK;
            } /* if (*p_lp_address == port_address) */
            else
            {
                mue_port_handle++;
                p_lp_address++;
            } /* else */
        } /* while ((mue_port_handle < pd16_lp_port_number_max) && (...)) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        /* get port size configuration */
        port_size = p_pd16_bus_ctrl->lp_size[mue_port_handle];
        p_mue_pd_port_data->port_size = port_size;

        mue_result =                                \
            mue_pd_16_get_port_data                 \
            (                                       \
                p_bus_ctrl,                         \
                mue_port_handle,                    \
                (BOOLEAN1)FALSE,                    \
                port_size,                          \
                &p_mue_pd_port_data->port_status,   \
                p_mue_pd_port_data->port_data       \
            );
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_pd16_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        /* handle Error Counters */
        pd16_ls_errcnt_pd                   \
        (                                   \
            p_pd16_bus_ctrl,                \
            mue_port_handle,                \
            p_mue_pd_port_data->port_status \
        );
    } /* if (LS_OK == ls_result) */

    PD16_LS_MACRO_UART_MUTEX_UNLOCK(&p_pd16_bus_ctrl->osl_mutex_object_uart);

    return(ls_result);

} /* pd16_ls_service_mue_pd_get_port_data */


/* ==========================================================================
 *
 *  Miscellaneous Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : pd16_ls_errcnt_pd
 *
 *  Purpose   : Handles error counters of a process data port.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              void
 *              pd16_ls_errcnt_pd
 *              (
 *                  PD16_BUS_CTRL   *p_pd16_bus_ctrl,
 *                  WORD8           mue_port_handle,
 *                  BITSET8         mue_port_status
 *              );
 *
 *  Input     : p_pd16_bus_ctrl   - pointer to PD16 bus controller
 *                                  specific values
 *              mue_port_handle   - port identifier (0..15)
 *              mue_port_status   - status information releated to a MVB
 *                                  process data port;
 *                                  any combination of constants
 *                                  'MUE_PD_PORT_STATUS_xxx' or constants
 *                                  'mue_pd_port_status_xxx'
 *
 *  Remarks   : - A link layer is identified by 'p_pd16_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
void
pd16_ls_errcnt_pd
(
    PD16_BUS_CTRL   *p_pd16_bus_ctrl,
    WORD8           mue_port_handle,
    BITSET8         mue_port_status
)
{
    BITSET8     mue_status_sink;
    UNSIGNED32  *p_errcnt_lineA;
    UNSIGNED32  *p_errcnt_lineB;

    if ((p_pd16_bus_ctrl->mue_sv_device_config & \
        mue_sv_device_config_line_mask) == mue_sv_device_config_line_mask)
    {
        mue_status_sink = \
            (BITSET8)(mue_port_status & mue_pd_port_status_sink_mask);

        p_errcnt_lineA = (UNSIGNED32*)&p_pd16_bus_ctrl->errcnt_global_lineA;
        p_errcnt_lineB = (UNSIGNED32*)&p_pd16_bus_ctrl->errcnt_global_lineB;

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

#ifdef PD16_BC_ERRCNT_ALL_PD
        p_errcnt_lineA = (UNSIGNED32*) \
            &p_pd16_bus_ctrl->errcnt_pd_port_lineA[mue_port_handle];
        p_errcnt_lineB = (UNSIGNED32*) \
            &p_pd16_bus_ctrl->errcnt_pd_port_lineB[mue_port_handle];

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
#else /* #ifdef PD16_BC_ERRCNT_ALL_PD */
        /* avoid warnings */
        mue_port_handle = mue_port_handle;
#endif /* #else */

    } /* if ((p_pd16_bus_ctrl->mue_sv_device_config & ...) == ...) */

} /* pd16_ls_errcnt_pd */


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : pd16_ls_init (of procedure type 'LS_INIT')
 *
 *  Purpose   : Initialises a link layer as a whole.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              pd16_ls_init
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
 *                initialisation before calling any other 'pd16_ls_xxx' or
 *                'pd16_lp_xxx' procedure referencing the same link layer
 *                (parameter 'p_bus_ctrl').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
pd16_ls_init
(
    void        *p_bus_ctrl
)
{
    LS_RESULT       ls_result        = LS_OK;
    PD16_BUS_CTRL   *p_pd16_bus_ctrl = (PD16_BUS_CTRL*)p_bus_ctrl;
    UNSIGNED16      counter;
    MUE_RESULT      mue_result;


    /* ======================================================================
     *
     *  bus controller structure (dynamic values)
     *
     * ======================================================================
     */

#if (PD16_BC_MUTEX_UART == 1)
    /* ----------------------------------------------------------------------
     *  OSL
     * ----------------------------------------------------------------------
     */
    /* initialise mutex for UART */
    if (FALSE == p_pd16_bus_ctrl->osl_mutex_status_uart)
    {
        if (0 != pthread_mutex_init( \
            &p_pd16_bus_ctrl->osl_mutex_object_uart, NULL))
        {
            ls_result = LS_ERROR;
        } /* if (0 != pthread_mutex_init(...)) */
        else
        {
            p_pd16_bus_ctrl->osl_mutex_status_uart = TRUE;
        } /* else */
    } /* if (FALSE == p_pd16_bus_ctrl->osl_mutex_status_uart) */
    if (TRUE == p_pd16_bus_ctrl->osl_mutex_status_uart)
    {
        if (0 != pthread_mutex_lock( \
            &p_pd16_bus_ctrl->osl_mutex_object_uart))
        {
            ls_result = LS_ERROR;
        } /* if (0 != pthread_mutex_lock(...)) */
        if (LS_OK == ls_result)
        {
            if (0 != pthread_mutex_unlock( \
                &p_pd16_bus_ctrl->osl_mutex_object_uart))
            {
                ls_result = LS_ERROR;
            } /* if (0 != pthread_mutex_unlock(...)) */
        } /* if (LS_OK == ls_result) */
    } /* if (TRUE == p_pd16_bus_ctrl->osl_mutex_status_uart) */
#endif /* #if (PD16_BC_MUTEX_UART == 1) */


    PD16_LS_MACRO_UART_MUTEX_LOCK(&p_pd16_bus_ctrl->osl_mutex_object_uart);


    if (LS_OK == ls_result)
    {
        ls_result = muell_ls_init(p_bus_ctrl);
    } /* if (LS_OK == ls_result) */


    /* ----------------------------------------------------------------------
     *  Clear Error Counters
     * ----------------------------------------------------------------------
     */
#ifdef PD16_BC_ERRCNT_ALL_PD
    /* error counters - all process data ports */
    for (counter=0; counter<pd16_lp_port_number_max; counter++)
    {
        p_pd16_bus_ctrl->errcnt_pd_port_lineA[counter] = 0;
        p_pd16_bus_ctrl->errcnt_pd_port_lineB[counter] = 0;
    } /* for (counter=0; counter<pd16_lp_port_number_max; counter++) */
#endif /* #ifdef PD16_BC_ERRCNT_ALL_PD */


    /* ----------------------------------------------------------------------
     *  LP
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        /* clear freshness supervision interval */
        p_pd16_bus_ctrl->lp_fsi = 0;
        for (counter=0; counter<pd16_lp_port_number_max; counter++)
        {
            /* store port address configuration (0xFFFF=undefined) */
            p_pd16_bus_ctrl->lp_address[counter] = 0xFFFF;
            /* store port size configuration (0=undefined) */
            p_pd16_bus_ctrl->lp_size[counter]    = 0;
            /* store last port freshness timer (default=0xFFFF) */
            p_pd16_bus_ctrl->lp_fresh[counter]   = 0xFFFF;
        } /* for (counter=0; counter<pd16_lp_port_number_max; counter++) */
    } /* if (LS_OK == ls_result) */


    /* ----------------------------------------------------------------------
     *  clear process data port configuration
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        /* define all ports as SINK with address 0x0000 and size 0 */
        counter = 0;
        while ((counter < pd16_lp_port_number_max) && (LS_OK == ls_result))
        {
            mue_result =                    \
                mue_pd_16_put_port_config   \
                (                           \
                    p_bus_ctrl,             \
                    (WORD8)counter,         \
                    0x0000                  \
                );
					  TCN_OSL_PRINTF("Mue_result: %u\n", mue_result);
            if (MUE_RESULT_OK != mue_result)
            {
                /* store last result of MUE */
                p_pd16_bus_ctrl->mue_result = mue_result;

                ls_result = LS_ERROR;
            } /* if (MUE_RESULT_OK != mue_result) */
            counter++;
        } /* while ((counter < pd16_lp_port_number_max) && (...)) */
    } /* if (LS_OK == ls_result) */


    PD16_LS_MACRO_UART_MUTEX_UNLOCK(&p_pd16_bus_ctrl->osl_mutex_object_uart);


    return(ls_result);

} /* pd16_ls_init */


/* --------------------------------------------------------------------------
 *  Procedure : pd16_ls_service_handler
 *              (of procedure type 'LS_SERVICE_HANDLER')
 *
 *  Purpose   : Handles link layer specific services.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              pd16_ls_service_handler
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
pd16_ls_service_handler
(
    void        *p_bus_ctrl,
    ENUM8       service,
    void        *p_parameter
)
{
    LS_RESULT       ls_result        = LS_OK;
    PD16_BUS_CTRL   *p_pd16_bus_ctrl = (PD16_BUS_CTRL*)p_bus_ctrl;
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
            ll_info->pd_port_number_max  = pd16_lp_port_number_max;
            ll_info->pd_port_size_number = pd16_lp_port_size_number;
            memcpy                                  \
            (                                       \
                (void*)ll_info->pd_port_size_value, \
                (void*)pd16_lp_port_size_value,     \
                16                                  \
            );

            /* message data */
            ll_info->md_frame_size       = 0;

            break;

#ifdef PD16_BC_ERRCNT_ALL_PD
        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_ERRCNT_GET_PD_PORT
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_ERRCNT_GET_PD_PORT):
            ls_result = pd16_ls_service_errcnt_get_pd_port(p_bus_ctrl, \
                (SV_MVB_ERRCNT_PD_PORT*)p_parameter);
            break;
#endif /* #ifdef PD16_BC_ERRCNT_ALL_PD */

        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA):
            ls_result = pd16_ls_service_mue_pd_get_port_data(p_bus_ctrl, \
                (SV_MVB_MUE_PD_PORT_DATA*)p_parameter);
            break;

        /* ------------------------------------------------------------------
         *  default
         * ------------------------------------------------------------------
         */
        default:
            PD16_LS_MACRO_UART_MUTEX_LOCK( \
                &p_pd16_bus_ctrl->osl_mutex_object_uart);

            ls_result = \
                muell_ls_service_handler(p_bus_ctrl, service, p_parameter);

            PD16_LS_MACRO_UART_MUTEX_UNLOCK( \
                &p_pd16_bus_ctrl->osl_mutex_object_uart);

    } /* switch (service) */

    return(ls_result);

} /* pd16_ls_service_handler */


#endif /* #ifdef MUELL_PD16 */
