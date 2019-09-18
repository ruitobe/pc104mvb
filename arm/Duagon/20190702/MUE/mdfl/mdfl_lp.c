/* ==========================================================================
 *
 *  File      : MDFL_LP.C
 *
 *  Purpose   : Bus Controller Link Layer for MDFULL - 
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
#include <tcn_pd.h>
#include <tcn_lp.h>

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer
 * --------------------------------------------------------------------------
 */
#include <mdfl_bc.h>
#include <mdfl_ls.h>
#include <mdfl_lp.h>

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
    UNSIGNED16  mdfull_lp_port_number_max     = MDFULL_LP_PORT_NUMBER_MAX;
TCN_DECL_CONST \
    UNSIGNED16  mdfull_lp_port_size_number    = MDFULL_LP_PORT_SIZE_NUMBER;
TCN_DECL_CONST \
    UNSIGNED8   mdfull_lp_port_size_value[16] = MDFULL_LP_PORT_SIZE_VALUE;


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

#define MDFULL_LP_MACRO_UART_MUTEX_LOCK(_p_osl_mutex_object_uart_)          \
    pthread_mutex_lock((_p_osl_mutex_object_uart_))

#define MDFULL_LP_MACRO_UART_MUTEX_UNLOCK(_p_osl_mutex_object_uart_)        \
    pthread_mutex_unlock((_p_osl_mutex_object_uart_))

#else /* #if (MDFULL_BC_MUTEX_UART == 1) */

#define MDFULL_LP_MACRO_UART_MUTEX_LOCK(_p_osl_mutex_object_uart_)
#define MDFULL_LP_MACRO_UART_MUTEX_UNLOCK(_p_osl_mutex_object_uart_)

#endif /* #else */


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mdfull_lp_init (of procedure type 'LP_INIT')
 *
 *  Purpose   : Initialises a process data link layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              mdfull_lp_init
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
 *                'mdfull_lp_xxx' procedure referencing the
 *                same traffic store (parameter 'p_bus_ctrl').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
mdfull_lp_init
(
    void        *p_bus_ctrl,
    UNSIGNED16  fsi
)
{
    LP_RESULT       lp_result          = LP_OK;
    MDFULL_BUS_CTRL *p_mdfull_bus_ctrl = (MDFULL_BUS_CTRL*)p_bus_ctrl;
    MUE_RESULT      mue_result;
    UNSIGNED16      counter;

    MDFULL_LP_MACRO_UART_MUTEX_LOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    /* check input parameters           */
    /* - ignore input parameter 'fsi'   */

#ifdef MDFULL_BC_ERRCNT_ALL_PD
    /* clear error counters - all process data ports */
    for (counter=0; counter<mdfull_lp_port_number_max; counter++)
    {
        p_mdfull_bus_ctrl->errcnt_pd_port_lineA[counter] = 0;
        p_mdfull_bus_ctrl->errcnt_pd_port_lineB[counter] = 0;
    } /* for (counter=0; counter<mdfull_lp_port_number_max; counter++) */
#endif /* #ifdef MDFULL_BC_ERRCNT_ALL_PD */

    /* clear freshness supervision interval (fsi) */
    p_mdfull_bus_ctrl->lp_fsi = 0;

    /* clear port size configuration */
    for (counter=0; counter<mdfull_lp_port_number_max; counter++)
    {
        /* store port size configuration (0=undefined) */
        p_mdfull_bus_ctrl->lp_size[counter] = 0;
    } /* for (counter=0; counter<mdfull_lp_port_number_max; counter++) */

    /* define all ports as SINK with size 0 */
    counter = 0;
    while ((counter < mdfull_lp_port_number_max) && (LP_OK == lp_result))
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

            lp_result = LP_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
        counter++;
    } /* while ((counter < mdfull_lp_port_number_max) && (...)) */

    /* store freshness supervision interval (fsi) */
    if (LP_OK == lp_result)
    {
        p_mdfull_bus_ctrl->lp_fsi = fsi;
    } /* if (LP_OK == lp_result) */

    MDFULL_LP_MACRO_UART_MUTEX_UNLOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    return(lp_result);

} /* mdfull_lp_init */


/* --------------------------------------------------------------------------
 *  Procedure : mdfull_lp_manage (of procedure type 'LP_MANAGE')
 *
 *  Purpose   : Manages a port located in a traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              mdfull_lp_manage
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
mdfull_lp_manage
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    ENUM16      command,
    PD_PRT_ATTR *prt_attr
)
{
    LP_RESULT       lp_result          = LP_OK;
    MDFULL_BUS_CTRL *p_mdfull_bus_ctrl = (MDFULL_BUS_CTRL*)p_bus_ctrl;
    UNSIGNED8       port_size;
    MUE_RESULT      mue_result         = MUE_RESULT_ERROR;
    BITSET16        mue_port_config;
    WORD8           mue_port_data[32];

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

    MDFULL_LP_MACRO_UART_MUTEX_LOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    if (LP_OK == lp_result)
    {
        /* get port size configuration */
        port_size = p_mdfull_bus_ctrl->lp_size[port_address];

        /* check, if port is defined */
        if (PD_PRT_CMD_CONFIG == command)
        {
            if (0 != port_size)
            {
                lp_result = LP_CONFIG;
            } /* if (0 != port_size) */
        } /* if (PD_PRT_CMD_CONFIG == command) */
        else
        {
            if (0 == port_size)
            {
                lp_result = LP_PRT_PASSIVE;
            } /* if (0 == port_size) */
        } /* else */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        /* ------------------------------------------------------------------
         *  PD_PRT_CMD_DELETE
         * ------------------------------------------------------------------
         */
        if (PD_PRT_CMD_DELETE == command)
        {
            /* SINK, FC0, logical address */
            mue_port_config = \
                (BITSET16)(port_address & mue_pd_port_config_la_mask);

            mue_result = mue_pd_full_put_port_config(p_bus_ctrl, \
                mue_port_config);
            if (MUE_RESULT_OK != mue_result)
            {
                /* store last result of MUE */
                p_mdfull_bus_ctrl->mue_result = mue_result;

                lp_result = LP_ERROR;
            } /* if (MUE_RESULT_OK != mue_result) */

            if (LP_OK == lp_result)
            {
                /* store port size configuration (0=undefined) */
                p_mdfull_bus_ctrl->lp_size[port_address] = 0;
            } /* if (LP_OK == lp_result) */
        } /* if (PD_PRT_CMD_DELETE == command) */

        /* ------------------------------------------------------------------
         *  PD_PRT_CMD_STATUS
         * ------------------------------------------------------------------
         */
        else if (command == PD_PRT_CMD_STATUS)
        {
            prt_attr->port_size   = 0;
            prt_attr->port_config = 0x00;

            mue_result = mue_pd_full_get_port_config(p_bus_ctrl, port_address, \
                &mue_port_config);
            if (MUE_RESULT_OK != mue_result)
            {
                /* store last result of MUE */
                p_mdfull_bus_ctrl->mue_result = mue_result;

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
        } /* else if (command == PD_PRT_CMD_STATUS) */

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
                        mue_pd_full_put_port_data   \
                        (                           \
                            p_bus_ctrl,             \
                            port_address,           \
                            32,                     \
                            mue_port_data           \
                        );
                    if (MUE_RESULT_OK != mue_result)
                    {
                        /* store last result of MUE */
                        p_mdfull_bus_ctrl->mue_result = mue_result;

                        lp_result = LP_ERROR;
                    } /* if (MUE_RESULT_OK != mue_result) */
                } /* if (PD_PRT_CMD_CONFIG == command) */
            } /* if (LP_OK == lp_result) */

            if (LP_OK == lp_result)
            {
                mue_result = \
                    mue_pd_full_put_port_config(p_bus_ctrl, mue_port_config);
                if (MUE_RESULT_OK != mue_result)
                {
                    /* store last result of MUE */
                    p_mdfull_bus_ctrl->mue_result = mue_result;

                    lp_result = LP_ERROR;
                } /* if (MUE_RESULT_OK != mue_result) */
            } /* if (LP_OK == lp_result) */

            if (LP_OK == lp_result)
            {
#ifdef MDFULL_BC_ERRCNT_ALL_PD
                /* clear error counters - all process data ports */
                p_mdfull_bus_ctrl->errcnt_pd_port_lineA[port_address] = 0;
                p_mdfull_bus_ctrl->errcnt_pd_port_lineB[port_address] = 0;
#endif /* #ifdef MDFULL_BC_ERRCNT_ALL_PD */
                /* store port size configuration (0=undefined) */
                p_mdfull_bus_ctrl->lp_size[port_address] = \
                    prt_attr->port_size;
            } /* if (LP_OK == lp_result) */
        } /* else */
    } /* if (LP_OK == lp_result) */

    MDFULL_LP_MACRO_UART_MUTEX_UNLOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    return(lp_result);

} /* mdfull_lp_manage */


/* --------------------------------------------------------------------------
 *  Procedure : mdfull_lp_put_dataset (of procedure type 'LP_PUT_DATASET')
 *
 *  Purpose   : Copies a dataset from the application to a port.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              mdfull_lp_put_dataset
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
mdfull_lp_put_dataset
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    void        *p_value
)
{
    LP_RESULT       lp_result          = LP_OK;
    MDFULL_BUS_CTRL *p_mdfull_bus_ctrl = (MDFULL_BUS_CTRL*)p_bus_ctrl;
    UNSIGNED8       port_size          = 0;
    MUE_RESULT      mue_result;

    MDFULL_LP_MACRO_UART_MUTEX_LOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    /* check input parameters */
    if (port_address > mue_pd_port_config_la_mask)
    {
        lp_result = LP_CONFIG;
    } /* if (port_address > mue_pd_port_config_la_mask) */

    if (LP_OK == lp_result)
    {
        /* get port size configuration */
        port_size = p_mdfull_bus_ctrl->lp_size[port_address];

        /* check, if port is defined */
        if (0 == port_size)
        {
            lp_result = LP_PRT_PASSIVE;
        } /* if (0 == port_size) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        mue_result =                    \
            mue_pd_full_put_port_data   \
            (                           \
                p_bus_ctrl,             \
                port_address,           \
                port_size,              \
                p_value                 \
            );
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_mdfull_bus_ctrl->mue_result = mue_result;

            lp_result = LP_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LP_OK == lp_result) */

    MDFULL_LP_MACRO_UART_MUTEX_UNLOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    return(lp_result);

} /* mdfull_lp_put_dataset */


/* --------------------------------------------------------------------------
 *  Procedure : mdfull_lp_get_dataset (of procedure type 'LP_GET_DATASET')
 *
 *  Purpose   : Copies a dataset from a port to the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              mdfull_lp_get_dataset
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
mdfull_lp_get_dataset
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    void        *p_value,
    void        *p_fresh
)
{
    LP_RESULT       lp_result          = LP_OK;
    MDFULL_BUS_CTRL *p_mdfull_bus_ctrl = (MDFULL_BUS_CTRL*)p_bus_ctrl;
    UNSIGNED8       port_size          = 0;
    MUE_RESULT      mue_result;
    BITSET8         mue_status         = 0x00;

    MDFULL_LP_MACRO_UART_MUTEX_LOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    /* check input parameters */
    if (port_address > mue_pd_port_config_la_mask)
    {
        lp_result = LP_CONFIG;
    } /* if (port_address > mue_pd_port_config_la_mask) */

    if (LP_OK == lp_result)
    {
        /* get port size configuration */
        port_size = p_mdfull_bus_ctrl->lp_size[port_address];

        /* check, if port is defined */
        if (0 == port_size)
        {
            lp_result = LP_PRT_PASSIVE;
        } /* if (0 == port_size) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        mue_result =                    \
            mue_pd_full_get_port_data   \
            (                           \
                p_bus_ctrl,             \
                port_address,           \
                (BOOLEAN1)FALSE,        \
                port_size,              \
                &mue_status,            \
                p_value,                \
                (UNSIGNED16*)p_fresh    \
            );
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_mdfull_bus_ctrl->mue_result = mue_result;

            lp_result = LP_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        /* handle Error Counters */
        mdfull_ls_errcnt_pd     \
        (                       \
            p_mdfull_bus_ctrl,  \
            port_address,       \
            mue_status          \
        );
    } /* if (LP_OK == lp_result) */

    MDFULL_LP_MACRO_UART_MUTEX_UNLOCK( \
        &p_mdfull_bus_ctrl->osl_mutex_object_uart);

    return(lp_result);

} /* mdfull_lp_get_dataset */


#endif /* #ifdef MUELL_MDFL */
