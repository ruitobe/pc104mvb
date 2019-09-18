/* ==========================================================================
 *
 *  File      : MD16_LP.C
 *
 *  Purpose   : Bus Controller Link Layer for MD16 - 
 *                  Link Layer Bus Controller Interface for Process Data
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
#ifdef MUELL_MD16


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
#include <tcn_pd.h>
#include <tcn_lp.h>

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer
 * --------------------------------------------------------------------------
 */
#include <md16_bc.h>
#include <md16_ls.h>
#include <md16_lp.h>

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>
#include <mue_sv.h>
#include <mue_pd.h>


/* ==========================================================================
 *
 *  Public Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
TCN_DECL_CONST \
    UNSIGNED16  md16_lp_port_number_max     = MD16_LP_PORT_NUMBER_MAX;
TCN_DECL_CONST \
    UNSIGNED16  md16_lp_port_size_number    = MD16_LP_PORT_SIZE_NUMBER;
TCN_DECL_CONST \
    UNSIGNED8   md16_lp_port_size_value[16] = MD16_LP_PORT_SIZE_VALUE;


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
#if (MD16_BC_MUTEX_UART == 1)

#define MD16_LP_MACRO_UART_MUTEX_LOCK(_p_osl_mutex_object_uart_)            \
    pthread_mutex_lock((_p_osl_mutex_object_uart_))

#define MD16_LP_MACRO_UART_MUTEX_UNLOCK(_p_osl_mutex_object_uart_)          \
    pthread_mutex_unlock((_p_osl_mutex_object_uart_))

#else /* #if (MD16_BC_MUTEX_UART == 1) */

#define MD16_LP_MACRO_UART_MUTEX_LOCK(_p_osl_mutex_object_uart_)
#define MD16_LP_MACRO_UART_MUTEX_UNLOCK(_p_osl_mutex_object_uart_)

#endif /* #else */


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : md16_lp_init (of procedure type 'LP_INIT')
 *
 *  Purpose   : Initialises a process data link layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              md16_lp_init
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  fsi
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              fsi        - freshness supervision interval (ignore)
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - This procedure has to be called at system
 *                initialisation before calling any other
 *                'md16_lp_xxx' procedure referencing the
 *                same traffic store (parameter 'p_bus_ctrl').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
md16_lp_init
(
    void        *p_bus_ctrl,
    UNSIGNED16  fsi
)
{
    LP_RESULT       lp_result        = LP_OK;
    MD16_BUS_CTRL   *p_md16_bus_ctrl = (MD16_BUS_CTRL*)p_bus_ctrl;
    MUE_RESULT      mue_result;
    UNSIGNED16      counter;

    MD16_LP_MACRO_UART_MUTEX_LOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    /* check input parameters           */
    /* - ignore input parameter 'fsi'   */

#ifdef MD16_BC_ERRCNT_ALL_PD
    /* clear error counters - all process data ports */
    for (counter=0; counter<md16_lp_port_number_max; counter++)
    {
        p_md16_bus_ctrl->errcnt_pd_port_lineA[counter] = 0;
        p_md16_bus_ctrl->errcnt_pd_port_lineB[counter] = 0;
    } /* for (counter=0; counter<md16_lp_port_number_max; counter++) */
#endif /* #ifdef MD16_BC_ERRCNT_ALL_PD */

    /* clear freshness supervision interval (fsi) */
    p_md16_bus_ctrl->lp_fsi = 0;

    /* clear port size configuration */
    for (counter=0; counter<md16_lp_port_number_max; counter++)
    {
        /* store port address configuration (0xFFFF=undefined) */
        p_md16_bus_ctrl->lp_address[counter] = 0xFFFF;
        /* store port size configuration (0=undefined) */
        p_md16_bus_ctrl->lp_size[counter]    = 0;
    } /* for (counter=0; counter<md16_lp_port_number_max; counter++) */

    /* define all ports as SINK with size 0 */
    counter = 0;
    while ((counter < md16_lp_port_number_max) && (LP_OK == lp_result))
    {
        mue_result =                    \
            mue_pd_16f_put_port_config  \
            (                           \
                p_bus_ctrl,             \
                (WORD8)counter,         \
                0x0000                  \
            );
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_md16_bus_ctrl->mue_result = mue_result;

            lp_result = LP_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
        counter++;
    } /* while ((counter < md16_lp_port_number_max) && (...)) */

    /* store freshness supervision interval (fsi) */
    if (LP_OK == lp_result)
    {
        p_md16_bus_ctrl->lp_fsi = fsi;
    } /* if (LP_OK == lp_result) */

    MD16_LP_MACRO_UART_MUTEX_UNLOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    return(lp_result);

} /* md16_lp_init */


/* --------------------------------------------------------------------------
 *  Procedure : md16_lp_manage (of procedure type 'LP_MANAGE')
 *
 *  Purpose   : Manages a port located in a traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              md16_lp_manage
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      port_address,
 *                  ENUM16      command,
 *                  PD_PRT_ATTR *prt_attr
 *              );
 *
 *  Input     : p_bus_ctrl   - pointer to bus controller specific values
 *              port_address - port address (0..4096)
 *              command      - port manage command; any PD_PRT_CMD
 *
 *  In-/Output: prt_attr     - pointer to a memory buffer which contains
 *                             the port attributes (memory buffer of
 *                             structured type 'PD_PRT_ATTR')
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
md16_lp_manage
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    ENUM16      command,
    PD_PRT_ATTR *prt_attr
)
{
    LP_RESULT       lp_result            = LP_OK;
    MD16_BUS_CTRL   *p_md16_bus_ctrl     = (MD16_BUS_CTRL*)p_bus_ctrl;
    BOOLEAN1        repeat;
    WORD8           mue_port_handle      = 0;
    WORD8           mue_port_handle_free;
    WORD16          *p_lp_address;
    MUE_RESULT      mue_result           = MUE_RESULT_ERROR;
    BITSET16        mue_port_config;
    WORD8           mue_port_data[32];

    MD16_LP_MACRO_UART_MUTEX_LOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    /* check input parameters */
    if (port_address > mue_pd_port_config_la_mask)
    {
        lp_result = LP_CONFIG;
    } /* if (port_address > mue_pd_port_config_la_mask) */
    if (LP_OK == lp_result)
    {
        if ((PD_PRT_CMD_CONFIG != command) && \
            (PD_PRT_CMD_DELETE != command) && \
            (PD_PRT_CMD_MODIFY != command) && \
            (PD_PRT_CMD_STATUS != command))
        {
            /* unknown command */
            lp_result = LP_CONFIG;
        } /* if ((...)) */
    } /* if (LP_OK == lp_result) */

    /* check, if port is defined; get port handle */
    if (LP_OK == lp_result)
    {
        mue_port_handle      = 0;
        mue_port_handle_free = 0xFF;
        p_lp_address         = (WORD16*)&p_md16_bus_ctrl->lp_address[0];
        repeat               = TRUE;
        while (TRUE == repeat)
        {
            if (*p_lp_address == port_address)
            {
                if (PD_PRT_CMD_CONFIG == command)
                {
                    lp_result = LP_CONFIG;
                    repeat    = FALSE;
                } /* if (PD_PRT_CMD_CONFIG == command) */
                else
                {
                    repeat = FALSE;
                } /* else */
            } /* if (*p_lp_address == port_address) */

            if (TRUE == repeat)
            {
                if ((0xFFFF == *p_lp_address) && \
                    (0xFF == mue_port_handle_free))
                {
                    mue_port_handle_free = mue_port_handle;
                } /* if ((0xFFFF == *p_lp_address) && (...)) */

                mue_port_handle++;
                if (mue_port_handle == md16_lp_port_number_max)
                {
                    if (PD_PRT_CMD_CONFIG == command)
                    {
                        if (0xFF == mue_port_handle_free)
                        {
                            lp_result = LP_CONFIG;
                            repeat    = FALSE;
                        } /* if (0xFF == mue_port_handle_free) */
                        else
                        {
                            mue_port_handle = mue_port_handle_free;
                            repeat          = FALSE;
                        } /* else */
                    } /* if (PD_PRT_CMD_CONFIG == command) */
                    else
                    {
                        lp_result = LP_PRT_PASSIVE;
                        repeat    = FALSE;
                    } /* else */
                } /* if (mue_port_handle == md16_lp_port_number_max) */
                p_lp_address++;
            } /* if (TRUE == repeat) */
        } /* while (TRUE == repeat) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        /* ------------------------------------------------------------------
         *  PD_PRT_CMD_DELETE
         * ------------------------------------------------------------------
         */
        if (PD_PRT_CMD_DELETE == command)
        {
            prt_attr->port_size   = 0;
            prt_attr->port_config = 0x00;

            /* SINK, FC0 */
            mue_port_config = (BITSET16)0x0000;

            mue_result =                    \
                mue_pd_16f_put_port_config  \
                (                           \
                    p_bus_ctrl,             \
                    mue_port_handle,        \
                    mue_port_config         \
                );
            if (MUE_RESULT_OK != mue_result)
            {
                /* store last result of MUE */
                p_md16_bus_ctrl->mue_result = mue_result;

                lp_result = LP_ERROR;
            } /* if (MUE_RESULT_OK != mue_result) */

            if (LP_OK == lp_result)
            {
                /* store port address configuration (0xFFFF=undefined) */
                p_md16_bus_ctrl->lp_address[mue_port_handle] = 0xFFFF;
                /* store port size configuration (0=undefined) */
                p_md16_bus_ctrl->lp_size[mue_port_handle]    = 0;
            } /* if (LP_OK == lp_result) */
        } /* if (PD_PRT_CMD_DELETE == command) */

        /* ------------------------------------------------------------------
         *  PD_PRT_CMD_STATUS
         * ------------------------------------------------------------------
         */
        else if (PD_PRT_CMD_STATUS == command)
        {
            prt_attr->port_size   = 0;
            prt_attr->port_config = 0x00;

            mue_result = mue_pd_16f_get_port_config(p_bus_ctrl, \
                mue_port_handle, &mue_port_config);
            if (MUE_RESULT_OK != mue_result)
            {
                /* store last result of MUE */
                p_md16_bus_ctrl->mue_result = mue_result;

                lp_result = LP_ERROR;
            } /* if (MUE_RESULT_OK != mue_result) */

            if (LP_OK == lp_result)
            {
                /* port attributes -> port_size */
                prt_attr->port_size = (UNSIGNED8)(0x2 << \
                    ((mue_port_config & mue_pd_port_config_fc_mask) >> 12));

                /* port attributes -> port_config */
                if (mue_port_config & mue_pd_port_config_src)
                {
                    prt_attr->port_config = PD_PRT_CFG_BIT_SOURCE;
                } /* if (mue_port_config & mue_pd_port_config_src) */
                else
                {
                    prt_attr->port_config = PD_PRT_CFG_BIT_SINK;
                } /* else */

                /* port attributes -> p_bus_specific */
                prt_attr->p_bus_specific = NULL;
            } /* if (LP_OK == lp_result) */
        } /* else if (PD_PRT_CMD_STATUS == command) */

        /* ------------------------------------------------------------------
         *  PD_PRT_CMD_CONFIG, PD_PRT_CMD_MODIFY
         * ------------------------------------------------------------------
         */
        else
        {
            /* logical address */
            mue_port_config = \
                (BITSET16)(port_address & mue_pd_port_config_la_mask);

            /* f-code */
            switch (prt_attr->port_size)
            {
                case (2):
                    mue_port_config |= (BITSET16)mue_pd_port_config_fc0;
                    break;
                case (4):
                    mue_port_config |= (BITSET16)mue_pd_port_config_fc1;
                    break;
                case (8):
                    mue_port_config |= (BITSET16)mue_pd_port_config_fc2;
                    break;
                case (16):
                    mue_port_config |= (BITSET16)mue_pd_port_config_fc3;
                    break;
                case (32):
                    mue_port_config |= (BITSET16)mue_pd_port_config_fc4;
                    break;
                default:
                    lp_result = LP_CONFIG;
            } /* switch (prt_attr->port_size) */

            /* source bit */
            if (LP_OK == lp_result)
            {
                if (prt_attr->port_config & PD_PRT_CFG_BIT_SOURCE)
                {
                    mue_port_config |= (BITSET16)mue_pd_port_config_src;
                } /* if (prt_attr->port_config & PD_PRT_CFG_BIT_SOURCE) */
                else if (prt_attr->port_config & PD_PRT_CFG_BIT_SINK)
                {
                    ;
                } /* else if (prt_attr->port_config & PD_PRT_CFG_BIT_SINK) */
                else
                {
                    lp_result = LP_CONFIG;
                } /* else */
            } /* if (LP_OK == lp_result) */

            if (LP_OK == lp_result)
            {
                if (PD_PRT_CMD_CONFIG == command)
                {
                    /* clear port value */
                    memset((void*)mue_port_data, 0, 32);
                    mue_result =                    \
                        mue_pd_16f_put_port_data    \
                        (                           \
                            p_bus_ctrl,             \
                            mue_port_handle,        \
                            32,                     \
                            mue_port_data           \
                        );
                    if (MUE_RESULT_OK != mue_result)
                    {
                        /* store last result of MUE */
                        p_md16_bus_ctrl->mue_result = mue_result;

                        lp_result = LP_ERROR;
                    } /* if (MUE_RESULT_OK != mue_result) */
                } /* if (PD_PRT_CMD_CONFIG == command) */
            } /* if (LP_OK == lp_result) */

            if (LP_OK == lp_result)
            {
                mue_result =                        \
                    mue_pd_16f_put_port_config      \
                    (                               \
                        p_bus_ctrl,                 \
                        mue_port_handle,            \
                        mue_port_config             \
                    );
                if (MUE_RESULT_OK != mue_result)
                {
                    /* store last result of MUE */
                    p_md16_bus_ctrl->mue_result = mue_result;

                    lp_result = LP_ERROR;
                } /* if (MUE_RESULT_OK != mue_result) */
            } /* if (LP_OK == lp_result) */

            if (LP_OK == lp_result)
            {
#ifdef MD16_BC_ERRCNT_ALL_PD
                /* clear error counters - all process data ports */
                p_md16_bus_ctrl->errcnt_pd_port_lineA[mue_port_handle] = 0;
                p_md16_bus_ctrl->errcnt_pd_port_lineB[mue_port_handle] = 0;
#endif /* #ifdef MD16_BC_ERRCNT_ALL_PD */
                /* store port address configuration (0xFFFF=undefined) */
                p_md16_bus_ctrl->lp_address[mue_port_handle] = \
                    port_address;
                /* store port size configuration (0=undefined) */
                p_md16_bus_ctrl->lp_size[mue_port_handle]    = \
                    prt_attr->port_size;
            } /* if (LP_OK == lp_result) */
        } /* else */
    } /* if (LP_OK == lp_result) */

    MD16_LP_MACRO_UART_MUTEX_UNLOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    return(lp_result);

} /* md16_lp_manage */


/* --------------------------------------------------------------------------
 *  Procedure : md16_lp_put_dataset (of procedure type 'LP_PUT_DATASET')
 *
 *  Purpose   : Copies a dataset from the application to a port.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              md16_lp_put_dataset
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      port_address,
 *                  void        *p_value
 *              );
 *
 *  Input     : p_bus_ctrl   - pointer to bus controller specific values
 *              port_address - port address (0..4096)
 *              p_value      - pointer to a memory buffer of
 *                             the application where the dataset
 *                             is copied from
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
md16_lp_put_dataset
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    void        *p_value
)
{
    LP_RESULT       lp_result        = LP_OK;
    MD16_BUS_CTRL   *p_md16_bus_ctrl = (MD16_BUS_CTRL*)p_bus_ctrl;
    WORD8           mue_port_handle  = 0;
    WORD16          *p_lp_address;
    UNSIGNED8       port_size;
    MUE_RESULT      mue_result;

    MD16_LP_MACRO_UART_MUTEX_LOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    /* check input parameters */
    if (port_address > mue_pd_port_config_la_mask)
    {
        lp_result = LP_CONFIG;
    } /* if (port_address > mue_pd_port_config_la_mask) */

    /* check, if port is defined; get port handle */
    if (LP_OK == lp_result)
    {
        lp_result       = LP_PRT_PASSIVE;
        mue_port_handle = 0;
        p_lp_address    = (WORD16*)&p_md16_bus_ctrl->lp_address[0];
        while ((mue_port_handle < md16_lp_port_number_max) && \
               (LP_OK != lp_result))
        {
            if (*p_lp_address == port_address)
            {
                lp_result = LP_OK;
            } /* if (*p_lp_address == port_address) */
            else
            {
                mue_port_handle++;
                p_lp_address++;
            } /* else */
        } /* while ((mue_port_handle < md16_lp_port_number_max) && (...)) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        /* get port size configuration */
        port_size = p_md16_bus_ctrl->lp_size[mue_port_handle];

        mue_result =                    \
            mue_pd_16f_put_port_data    \
            (                           \
                p_bus_ctrl,             \
                mue_port_handle,        \
                port_size,              \
                p_value                 \
            );
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_md16_bus_ctrl->mue_result = mue_result;

            lp_result = LP_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LP_OK == lp_result) */

    MD16_LP_MACRO_UART_MUTEX_UNLOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    return(lp_result);

} /* md16_lp_put_dataset */


/* --------------------------------------------------------------------------
 *  Procedure : md16_lp_get_dataset (of procedure type 'LP_GET_DATASET')
 *
 *  Purpose   : Copies a dataset from a port to the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              md16_lp_get_dataset
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      port_address,
 *                  void        *p_value,
 *                  void        *p_fresh
 *              );
 *
 *  Input     : p_bus_ctrl   - pointer to bus controller specific values
 *              port_address - port address (0..4096)
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Output    : p_value      - pointer to a memory buffer of
 *                             the application where the dataset
 *                             is copied to
 *              p_fresh      - pointer to a memory buffer of
 *                             the application where the freshness timer
 *                             is copied to
 *
 *  Remarks   : - A traffic store is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
md16_lp_get_dataset
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    void        *p_value,
    void        *p_fresh
)
{
    LP_RESULT       lp_result        = LP_OK;
    MD16_BUS_CTRL   *p_md16_bus_ctrl = (MD16_BUS_CTRL*)p_bus_ctrl;
    WORD8           mue_port_handle  = 0;
    WORD16          *p_lp_address;
    UNSIGNED8       port_size;
    MUE_RESULT      mue_result;
    BITSET8         mue_status       = 0x00;

    MD16_LP_MACRO_UART_MUTEX_LOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    /* check input parameters */
    if (port_address > mue_pd_port_config_la_mask)
    {
        lp_result = LP_CONFIG;
    } /* if (port_address > mue_pd_port_config_la_mask) */

    /* check, if port is defined; get port handle */
    if (LP_OK == lp_result)
    {
        lp_result       = LP_PRT_PASSIVE;
        mue_port_handle = 0;
        p_lp_address    = (WORD16*)&p_md16_bus_ctrl->lp_address[0];
        while ((mue_port_handle < md16_lp_port_number_max) && \
               (LP_OK != lp_result))
        {
            if (*p_lp_address == port_address)
            {
                lp_result = LP_OK;
            } /* if (*p_lp_address == port_address) */
            else
            {
                mue_port_handle++;
                p_lp_address++;
            } /* else */
        } /* while ((mue_port_handle < md16_lp_port_number_max) && (...)) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        /* get port size configuration */
        port_size = p_md16_bus_ctrl->lp_size[mue_port_handle];

        mue_result =                    \
            mue_pd_16f_get_port_data    \
            (                           \
                p_bus_ctrl,             \
                mue_port_handle,        \
                (BOOLEAN1)FALSE,        \
                port_size,              \
                &mue_status,            \
                p_value,                \
                (UNSIGNED16*)p_fresh    \
            );
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_md16_bus_ctrl->mue_result = mue_result;

            lp_result = LP_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        /* handle Error Counters */
        md16_ls_errcnt_pd       \
        (                       \
            p_md16_bus_ctrl,    \
            mue_port_handle,    \
            mue_status          \
        );
    } /* if (LP_OK == lp_result) */

    MD16_LP_MACRO_UART_MUTEX_UNLOCK(&p_md16_bus_ctrl->osl_mutex_object_uart);

    return(lp_result);

} /* md16_lp_get_dataset */


#endif /* #ifdef MUELL_MD16 */
