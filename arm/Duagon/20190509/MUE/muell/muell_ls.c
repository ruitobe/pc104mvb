/* ==========================================================================
 *
 *  File      : MUELL_LS.C
 *
 *  Purpose   : Bus Controller Link Layer for MUE (common) - 
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
 *  Pre-processor Definitions:
 *  --------------------------
 *  - MUELL_LS_PRINT          - debug printouts
 *  - MUELL_LS_SIM_BA         - simulate MVB bus administrator configuration
 *                              (bypass any TCN Driver API procedure)
 *  - MUELL_LS_BA_PD_DUMMY    - add dummy PD frame at end of BA list
 *                              (port address 0x000)
 *                              NOTE: Only required for old MVB controller,
 *                              e.g. PLD d-000374-001996 for D113LF.
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
 *  Include Files
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Version Information
 * --------------------------------------------------------------------------
 */
//#include <version.h>

/* --------------------------------------------------------------------------
 *  TCN Software Architecture
 * --------------------------------------------------------------------------
 */
#include <tcn_def.h>
#include <tcn_osl.h>
#include <tcn_bls.h>
#include <tcn_sv.h>
#include <tcn_sv_m.h>
#include <tcn_ls.h>

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer
 * --------------------------------------------------------------------------
 */
#include <muell_bc.h>
#include <muell_ls.h>

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>
#include <mue_acc.h>
#include <mue_sv.h>


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
    UNSIGNED16  muell_ls_device_number_max = MUELL_LS_DEVICE_NUMBER_MAX;


/* ==========================================================================
 *
 *  Local Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL  TCN_DECL_CONST \
    UNSIGNED16  muell_ls_ds_freshness_max = 40000;


/* ==========================================================================
 *
 *  Local Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_errcnt_ds
 *
 *  Purpose   : Handles error counters of a device status port.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              void
 *              muell_ls_errcnt_ds
 *              (
 *                  MUELL_BUS_CTRL  *p_muell_bus_ctrl,
 *                  WORD16          device_address,
 *                  BITSET8         mue_port_status
 *              );
 *
 *  Input     : p_muell_bus_ctrl  - pointer to MUELL bus controller
 *                                  specific values
 *              device_address    - device identifier (12-bit device address)
 *              mue_port_status   - status information releated to a MVB
 *                                  device status port;
 *                                  any combination of constants
 *                                  'MUE_SV_PORT_STATUS_xxx' or constants
 *                                  'mue_sv_port_status_xxx'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_muell_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
muell_ls_errcnt_ds
(
    MUELL_BUS_CTRL  *p_muell_bus_ctrl,
    WORD16          device_address,
    BITSET8         mue_port_status
)
{
    BITSET8     mue_status_sink;
    UNSIGNED32  *p_errcnt_lineA;
    UNSIGNED32  *p_errcnt_lineB;

    if ((p_muell_bus_ctrl->mue_sv_device_config & \
        mue_sv_device_config_line_mask) == mue_sv_device_config_line_mask)
    {
        mue_status_sink = \
            (BITSET8)(mue_port_status & mue_sv_port_status_sink_mask);

        p_errcnt_lineA = (UNSIGNED32*)&p_muell_bus_ctrl->errcnt_global_lineA;
        p_errcnt_lineB = (UNSIGNED32*)&p_muell_bus_ctrl->errcnt_global_lineB;

        if (mue_status_sink == mue_sv_port_status_sink_b)
        {
            if (0xFFFFFFFFUL > *p_errcnt_lineA)
            {
                (*p_errcnt_lineA)++;
            } /* if (0xFFFFFFFFUL > *p_errcnt_lineA) */
        } /* if (mue_status_sink == mue_sv_port_status_sink_b) */
        if (mue_status_sink == mue_sv_port_status_sink_a)
        {
            if (0xFFFFFFFFUL > *p_errcnt_lineB)
            {
                (*p_errcnt_lineB)++;
            } /* if (0xFFFFFFFFUL > *p_errcnt_lineB) */
        } /* if (mue_status_sink == mue_sv_port_status_sink_a) */

#ifdef MUELL_BC_ERRCNT_ALL_DS
        p_errcnt_lineA = (UNSIGNED32*) \
            &p_muell_bus_ctrl->errcnt_ds_port_lineA[device_address];
        p_errcnt_lineB = (UNSIGNED32*) \
            &p_muell_bus_ctrl->errcnt_ds_port_lineB[device_address];

        if (mue_status_sink == mue_sv_port_status_sink_b)
        {
            if (0xFFFFFFFFUL > *p_errcnt_lineA)
            {
                (*p_errcnt_lineA)++;
            } /* if (0xFFFFFFFFUL > *p_errcnt_lineA) */
        } /* if (mue_status_sink == mue_sv_port_status_sink_b) */
        if (mue_status_sink == mue_sv_port_status_sink_a)
        {
            if (0xFFFFFFFFUL > *p_errcnt_lineB)
            {
                (*p_errcnt_lineB)++;
            } /* if (0xFFFFFFFFUL > *p_errcnt_lineB) */
        } /* if (mue_status_sink == mue_sv_port_status_sink_a) */
#else /* #ifdef MUELL_BC_ERRCNT_ALL_DS */
        /* avoid warnings */
        device_address = device_address;
#endif /* #else */

    } /* if ((p_muell_bus_ctrl->mue_sv_device_config & ...) == ...) */

} /* muell_ls_errcnt_ds */


/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_service_read_status
 *
 *  Purpose   : Handles link layer service 'SV_MVB_SERVICE_READ_STATUS'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              muell_ls_service_read_status
 *              (
 *                  void            *p_bus_ctrl,
 *                  SV_MVB_STATUS   *p_mvb_status
 *              );
 *
 *  Input     : p_bus_ctrl   - pointer to bus controller specific values
 *
 *  Output    : p_mvb_status - pointer to a memory buffer
 *                             of structured type 'SV_MVB_STATUS'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
muell_ls_service_read_status
(
    void            *p_bus_ctrl,
    SV_MVB_STATUS   *p_mvb_status
)
{
    LS_RESULT       ls_result         = LS_OK;
    MUELL_BUS_CTRL  *p_muell_bus_ctrl = (MUELL_BUS_CTRL*)p_bus_ctrl;
    MUE_RESULT      mue_result;
    /* device config */
    BITSET16        mue_device_config;
    /* device status */
    WORD16          device_address    = 0;
    BITSET8         mue_port_status;
    BITSET16        device_status     = 0x0000;
    UNSIGNED16      freshness;
    BLS_DESC        *p_bls_desc;
    ENUM8           link_id;
    UNSIGNED16      found;

    /* ----------------------------------------------------------------------
     *  clear MVB_Status object
     * ----------------------------------------------------------------------
     */
    p_mvb_status->reserved1         = 0;
    p_mvb_status->device_address    = 0;
    memset(&p_mvb_status->mvb_hardware_name, 0, 32);
    memset(&p_mvb_status->mvb_software_name, 0, 32);
    p_mvb_status->device_status     = 0;
    p_mvb_status->reserved2         = 0;
    p_mvb_status->t_ignore          = 0;
    p_mvb_status->lineA_errors      = 0;
    p_mvb_status->lineB_errors      = 0;

    /* ----------------------------------------------------------------------
     *  get device config
     * ----------------------------------------------------------------------
     */
    mue_result = mue_sv_get_device_config(p_bus_ctrl, &mue_device_config);
    if (MUE_RESULT_OK != mue_result)
    {
        /* store last result of MUE */
        p_muell_bus_ctrl->mue_result = mue_result;

        ls_result = LS_ERROR;
    } /* if (MUE_RESULT_OK != mue_result) */
    if (LS_OK == ls_result)
    {
        device_address = (WORD16) \
            (mue_device_config & mue_sv_device_config_da_mask);
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  get device status
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if (p_muell_bus_ctrl->ls_ds_capability)
        {
            mue_result =                    \
                mue_sv_get_device_status    \
                (                           \
                    p_bus_ctrl,             \
                    device_address,         \
                    (BOOLEAN1)FALSE,        \
                    &mue_port_status,       \
                    &device_status,         \
                    &freshness              \
                );
            if (MUE_RESULT_OK != mue_result)
            {
                /* store last result of MUE */
                p_muell_bus_ctrl->mue_result = mue_result;

                ls_result = LS_ERROR;
            } /* if (MUE_RESULT_OK != mue_result) */

            /* --------------------------------------------------------------
             *  handle Error Counters
             * --------------------------------------------------------------
             */
            if (LS_OK == ls_result)
            {
                muell_ls_errcnt_ds      \
                (                       \
                    p_muell_bus_ctrl,   \
                    device_address,     \
                    mue_port_status     \
                );
            } /* if (LS_OK == ls_result) */

            /* ------------------------------------------------------------------
             *  check freshness timeout (40sec)
             * ------------------------------------------------------------------
             */
            if (LS_OK == ls_result)
            {
                if (muell_ls_ds_freshness_max <= freshness)
                {
                    device_status = 0x0000;
                } /* if (muell_ls_ds_freshness_max <= freshness) */
            } /* if (LS_OK == ls_result) */
        } /* if (p_muell_bus_ctrl->ls_ds_capability) */
        else
        {
            device_status = 0x0000;
        } /* else */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  set MVB_Status object
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        p_mvb_status->reserved1      = 0;
        p_mvb_status->device_address = device_address;
        p_mvb_status->device_status  = device_status;
        p_mvb_status->reserved2      = 0;
        p_mvb_status->t_ignore       = 0; /* only T_reply_def=42.7us */
        p_mvb_status->lineA_errors   = p_muell_bus_ctrl->errcnt_global_lineA;
        p_mvb_status->lineB_errors   = p_muell_bus_ctrl->errcnt_global_lineB;
        p_bls_desc = (BLS_DESC*)bls_desc_table[0];
        link_id    = 0;
        found      = 0;
        while ((BLS_MAX_LINKS > link_id) && (0 == found))
        {
            if (NULL != p_bls_desc)
            {
                if (p_bls_desc->p_bus_ctrl == p_bus_ctrl)
                {
                    strcpy                                          \
                    (                                               \
                        (char*)&p_mvb_status->mvb_hardware_name,    \
                        (char*)&p_bls_desc->link_name               \
                    );
                    found = 1;
                } /* if (p_bls_desc->p_bus_ctrl == p_bus_ctrl) */
            } /* if (NULL != p_bls_desc) */
            link_id++;
        } /* while ((BLS_MAX_LINKS > link_id) && (0 == found)) */
        if (0 == found)
        {
            strcpy                                          \
            (                                               \
                (char*)&p_mvb_status->mvb_hardware_name,    \
            /*   STRING32 (should be null terminated)   */  \
            /*   <-- 10 --><-- 20 --><-- 30 -->32       */  \
            /*   12345678901234567890123456789012       */  \
                "duagon MVB Controller MUE"                 \
            );
        } /* if (0 == found) */
        /*sprintf                                         \
        (                                               \
            (char*)&p_mvb_status->mvb_software_name,    \
            "%s %s",                                    \
            VERSION_STR_NAME, VERSION_STR_IDENTNR       \
        );*/
    } /* if (LS_OK == ls_result) */

    return(ls_result);

} /* muell_ls_service_read_status */


/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_service_write_control
 *
 *  Purpose   : Handles link layer service 'SV_MVB_SERVICE_WRITE_CONTROL'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              muell_ls_service_write_control
 *              (
 *                  void            *p_bus_ctrl,
 *                  SV_MVB_CONTROL  *p_mvb_control
 *              );
 *
 *  Input     : p_bus_ctrl    - pointer to bus controller specific values
 *              p_mvb_control - pointer to a memory buffer
 *                              of structured type 'SV_MVB_CONTROL'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
muell_ls_service_write_control
(
    void            *p_bus_ctrl,
    SV_MVB_CONTROL  *p_mvb_control
)
{
    LS_RESULT       ls_result         = LS_OK;
    MUELL_BUS_CTRL  *p_muell_bus_ctrl = (MUELL_BUS_CTRL*)p_bus_ctrl;
    MUE_RESULT      mue_result;
    BITSET16        mue_device_config = 0x0000;

    /* ----------------------------------------------------------------------
     *  check MVB_Control object
     * ----------------------------------------------------------------------
     */
    /* element 'device_address' */
    if (p_mvb_control->device_address > mue_sv_device_config_da_mask)
    {
        ls_result = LS_CONFIG;
    } /* if (p_mvb_control->device_address > mue_sv_device_config_da_mask) */
    if (LS_OK == ls_result)
    {
        /* element 't_ignore' */
        if (0 != p_mvb_control->t_ignore)
        {
            ls_result = LS_CONFIG;
        } /* if (0 != p_mvb_control->t_ignore) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  get device config
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        mue_result = mue_sv_get_device_config(p_bus_ctrl, \
            &mue_device_config);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  put device config
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        /* device address */
        mue_device_config &= ~mue_sv_device_config_da_mask;
        mue_device_config |= p_mvb_control->device_address;
        if (p_muell_bus_ctrl->ls_ba_capability)
        {
            /* bit 'BA_EN' */
            if (p_mvb_control->command & SV_MVB_CTRL_CMD_BIT_AON)
            {
                mue_device_config |= mue_sv_device_config_ba_en;
            } /* if (p_mvb_control->command & SV_MVB_CTRL_CMD_BIT_AON) */
            if (p_mvb_control->command & SV_MVB_CTRL_CMD_BIT_AOF)
            {
                mue_device_config &= ~mue_sv_device_config_ba_en;
            } /* if (p_mvb_control->command & SV_MVB_CTRL_CMD_BIT_AOF) */
            if ((0 == (p_mvb_control->command &    \
                    SV_MVB_CTRL_CMD_BIT_SLA))   && \
                (0 == (p_mvb_control->command &    \
                    SV_MVB_CTRL_CMD_BIT_SLB)))
            {
                mue_device_config &= ~mue_sv_device_config_ba_en;
            } /* if ((...)) */
            if ((0 == (mue_device_config & mue_sv_device_config_line_a)) && \
                (0 == (mue_device_config & mue_sv_device_config_line_b)))
            {
                mue_device_config &= ~mue_sv_device_config_ba_en;
            } /* if ((...)) */
        } /* if (p_muell_bus_ctrl->ls_ba_capability) */
        /* bit 'LINE_A' */
        if (p_mvb_control->command & SV_MVB_CTRL_CMD_BIT_SLA)
        {
            mue_device_config |= mue_sv_device_config_line_a;
        } /* if (p_mvb_control->command & SV_MVB_CTRL_CMD_BIT_SLA) */
        else
        {
            mue_device_config &= ~mue_sv_device_config_line_a;
        } /* else */
        /* bit 'LINE_B' */
        if (p_mvb_control->command & SV_MVB_CTRL_CMD_BIT_SLB)
        {
            mue_device_config |= mue_sv_device_config_line_b;
        } /* if (p_mvb_control->command & SV_MVB_CTRL_CMD_BIT_SLB) */
        else
        {
            mue_device_config &= ~mue_sv_device_config_line_b;
        } /* else */
        /* bit 'LED_DNR' */
        if ((mue_device_config & mue_sv_device_config_line_a) || \
            (mue_device_config & mue_sv_device_config_line_b))
        {
            mue_device_config |= mue_sv_device_config_led_dnr;
        } /* if ((mue_device_config & ...) || (...)) */
        else
        {
            mue_device_config &= ~mue_sv_device_config_led_dnr;
        } /* else */
        mue_result = mue_sv_put_device_config(p_bus_ctrl, mue_device_config);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  get device config
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        mue_result = mue_sv_get_device_config(p_bus_ctrl, \
            &mue_device_config);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        /* ------------------------------------------------------------------
         *  store MUE SV device configuration
         * ------------------------------------------------------------------
         */
        p_muell_bus_ctrl->mue_sv_device_config = mue_device_config;

        /* ------------------------------------------------------------------
         *  reset frame error counter of Line_A
         * ------------------------------------------------------------------
         */
        if (SV_MVB_CTRL_CMD_BIT_CLA == \
            (p_mvb_control->command & SV_MVB_CTRL_CMD_BIT_CLA))
        {
            p_muell_bus_ctrl->errcnt_global_lineA = 0;
        } /* if (SV_MVB_CTRL_CMD_BIT_CLA == ...) */

        /* ------------------------------------------------------------------
         *  reset frame error counter of Line_B
         * ------------------------------------------------------------------
         */
        if (SV_MVB_CTRL_CMD_BIT_CLB == \
            (p_mvb_control->command & SV_MVB_CTRL_CMD_BIT_CLB))
        {
            p_muell_bus_ctrl->errcnt_global_lineB = 0;
        } /* if (SV_MVB_CTRL_CMD_BIT_CLB == ...) */
    } /* if (LS_OK == ls_result) */

    return(ls_result);

} /* muell_ls_service_write_control */


/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_service_read_devices
 *
 *  Purpose   : Handles link layer service 'SV_MVB_SERVICE_READ_DEVICES'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              muell_ls_service_read_devices
 *              (
 *                  void            *p_bus_ctrl,
 *                  SV_MVB_DEVICES  **p_mvb_devices
 *              );
 *
 *  Input     : p_bus_ctrl    - pointer to bus controller specific values
 *              p_mvb_devices - pointer of structured type 'SV_MVB_DEVICES'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 *              - The user has to provide a pointer of this type for the
 *                MVB devices object (output parameter 'p_parameter').
 *                The memory required for the MVB devices object will be
 *                allocated by the bus controller link layer. It must be
 *                released by the user.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
muell_ls_service_read_devices
(
    void            *p_bus_ctrl,
    SV_MVB_DEVICES  **p_mvb_devices
)
{
    LS_RESULT           ls_result           = LS_OK;
    MUELL_BUS_CTRL      *p_muell_bus_ctrl   = (MUELL_BUS_CTRL*)p_bus_ctrl;
    MUE_RESULT          mue_result;
    UNSIGNED16          nr_devices;
    SV_MVB_DEVICES_LIST *p_mvb_devices_list;
    /* device status */
    WORD16              device_address;
    BITSET8             mue_port_status;
    BITSET16            device_status;
    UNSIGNED16          freshness;

    /* ----------------------------------------------------------------------
     *  allocate memory for MVB_Devices object
     * ----------------------------------------------------------------------
     */
    *p_mvb_devices = (SV_MVB_DEVICES*)malloc(sizeof(SV_MVB_DEVICES_LIST));
    if (NULL == *p_mvb_devices)
    {
        ls_result = LS_MEMORY;
    } /* if (NULL == *p_mvb_devices) */

    /* ----------------------------------------------------------------------
     *  clear MVB_Devices object
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        (*p_mvb_devices)->nr_devices = 0;
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  fill MVB_Devices object
     * ----------------------------------------------------------------------
     */
    nr_devices     = 0;
    device_address = 0;
    while ((device_address < mue_sv_device_number_max) && \
           (LP_OK == ls_result))
    {
        /* ------------------------------------------------------------------
         *  get device status
         * ------------------------------------------------------------------
         */
        mue_result =                    \
            mue_sv_get_device_status    \
            (                           \
                p_bus_ctrl,             \
                device_address,         \
                (BOOLEAN1)FALSE,        \
                &mue_port_status,       \
                &device_status,         \
                &freshness              \
            );
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */

        if (LS_OK == ls_result)
        {
            /* --------------------------------------------------------------
             *  handle Error Counters
             * --------------------------------------------------------------
             */
            muell_ls_errcnt_ds      \
            (                       \
                p_muell_bus_ctrl,   \
                device_address,     \
                mue_port_status     \
            );

            /* --------------------------------------------------------------
             *  check freshness timeout (40sec)
             * --------------------------------------------------------------
             */
            if (muell_ls_ds_freshness_max > freshness)
            {
                /* ----------------------------------------------------------
                 *  reallocate memory for MVB_Devices object
                 * ----------------------------------------------------------
                 */
                *p_mvb_devices = (SV_MVB_DEVICES*)                      \
                    realloc                                             \
                    (                                                   \
                        *p_mvb_devices,                                 \
                        (                                               \
                            sizeof(SV_MVB_DEVICES) +                    \
                            (sizeof(SV_MVB_DEVICES_LIST) * nr_devices)  \
                        )                                               \
                    );
                if (NULL == *p_mvb_devices)
                {
                    ls_result = LS_MEMORY;
                } /* if (NULL == *p_mvb_devices) */
                else
                {
                    /* ------------------------------------------------------
                     *  add device to MVB_Devices object
                     * ------------------------------------------------------
                     */
                    p_mvb_devices_list                 = \
                        &(*p_mvb_devices)->devices_list[nr_devices];
                    p_mvb_devices_list->device_address = device_address;
                    p_mvb_devices_list->device_status  = device_status;
                    (*p_mvb_devices)->nr_devices++;
                    nr_devices++;
                } /* else */
            } /* if (muell_ls_ds_freshness_max > freshness) */
        } /* if (LS_OK == ls_result) */

        device_address++;
    } /* while ((device_address < mue_sv_device_number_max) && (...)) */

    return(ls_result);

} /* muell_ls_service_read_devices */


/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_check_mvb_administrator_object
 *
 *  Purpose   : Checks a MVB_Administrator object.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              muell_ls_check_mvb_administrator_object
 *              (
 *                  SV_MVB_ADMINISTRATOR    *p
 *              );
 *
 *  Input     : p - pointer to a memory buffer of structured type
 *                  'SV_MVB_ADMINISTRATOR'
 *
 *  Return    : result code; any LS_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
muell_ls_check_mvb_administrator_object
(
    SV_MVB_ADMINISTRATOR    *p
)
{
    LS_RESULT               ls_result   = LS_OK;
    UNSIGNED16              size;
    SV_MVB_ADMINISTRATOR_PL *p_pl       = NULL;
    UNSIGNED16              i_cycle;
    UNSIGNED16              c_cycle;
    UNSIGNED16              i_split;
    UNSIGNED16              c_split;
    UNSIGNED16              c_split_max;
    SV_MVB_ADMINISTRATOR_SL *p_split;
    UNSIGNED16              sum;
    UNSIGNED16              value;

#ifdef MUELL_LS_PRINT
    TCN_OSL_PRINTF("muell_ls_check_mvb_administrator_object()\n");
#endif /* #ifdef MUELL_LS_PRINT */

    /* ======================================================================
     *
     *  basic checks
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  checkword0
     * ----------------------------------------------------------------------
     */
    ;

    /* ----------------------------------------------------------------------
     *  actualisation_index
     * ----------------------------------------------------------------------
     */
    ;

    /* ----------------------------------------------------------------------
     *  t_reply_max (default=0=42.7us)
     * ----------------------------------------------------------------------
     */
    if ((0x0000 != p->t_reply_max) && (0x002A != p->t_reply_max))
    {
#ifdef MUELL_LS_PRINT
        TCN_OSL_PRINTF("ERROR: t_reply_max=0x%04X\n", p->t_reply_max);
#endif /* #ifdef MUELL_LS_PRINT */
        ls_result = LS_CONFIG;
    } /* if ((0x0000 != p->t_reply_max) && (0x002A != p->t_reply_max)) */

    /* ----------------------------------------------------------------------
     *  macro_cycles (1024 for a 1ms T_bp)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if (0x0400 != p->macro_cycles)
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: macro_cycles=0x%04X\n", p->macro_cycles);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if (0x0400 != p->macro_cycles) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  event_poll_strategy:
     *  - 0x0000 (no event pooling)
     *  - 0x4000 (high priority event pooling only)
     *  - 0x8000 (low  priority event pooling only)
     *  - 0xC000 (high and low priority event pooling)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if ((0x0000 != p->event_poll_strategy) && \
            (0x4000 != p->event_poll_strategy) && \
            (0x8000 != p->event_poll_strategy) && \
            (0xC000 != p->event_poll_strategy))
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: event_poll_strategy=0x%04X\n", \
                p->event_poll_strategy);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if ((...)) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  basic_period (1000us)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if (0x03E8 != p->basic_period)
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: basic_period=0x%04X\n", p->basic_period);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if (0x03E8 != p->basic_period) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  macrocycles_per_turn
     * ----------------------------------------------------------------------
     */
    ;

    /* ----------------------------------------------------------------------
     *  devices_scan_strategy:
     *  - 0 (scan all known device addresses)
     *  - 1 (scan all       device addresses)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if ((0x0000 != p->devices_scan_strategy) && \
            (0x0001 != p->devices_scan_strategy))
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: devices_scan_strategy=0x%04X\n", \
                p->devices_scan_strategy);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if ((0x0000 != p->devices_scan_strategy) && (...)) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  known_devices_list_offset (even offset, first offset)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if ((0      != (p->known_devices_list_offset % 2)) || \
            (0x0024 != p->known_devices_list_offset))
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: known_devices_list_offset=0x%04X\n", \
                p->known_devices_list_offset);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if ((0 != (p->known_devices_list_offset % 2)) || (...)) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  reserved_list_offset (even offset, first offset)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if ((0 != (p->reserved_list_offset % 2)) || \
            (p->reserved_list_offset < p->known_devices_list_offset))
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: reserved_list_offset=0x%04X\n", \
                p->reserved_list_offset);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if ((0 != (p->reserved_list_offset % 2)) || (...)) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  periodic_list_offset (even offset, first offset)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if ((0 != (p->periodic_list_offset % 2)) || \
            (p->periodic_list_offset != p->reserved_list_offset))
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: periodic_list_offset=0x%04X\n", \
                p->periodic_list_offset);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if ((0 != (p->periodic_list_offset % 2)) || (...)) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  bus_administrators_list_offset (even offset, first offset)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if ((0 != (p->bus_administrators_list_offset % 2)) || \
            (p->bus_administrators_list_offset < p->periodic_list_offset))
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF                                         \
                ("ERROR: bus_administrators_list_offset=0x%04X\n", \
                    p->bus_administrators_list_offset);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if ((0 != (p->bus_administrators_list_offset % 2)) || (...)) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  devices_scan_list_offset (even offset, first offset)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if ((0 != (p->devices_scan_list_offset % 2)) || \
            (p->devices_scan_list_offset < p->bus_administrators_list_offset))
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: devices_scan_list_offset=0x%04X\n", \
                p->devices_scan_list_offset);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if ((0 != (p->devices_scan_list_offset % 2)) || (...)) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  end_list_offset
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if ((0 != (p->end_list_offset % 2)) || \
            (p->end_list_offset < p->devices_scan_list_offset))
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: end_list_offset=0x%04X\n", \
                p->end_list_offset);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if ((0 != (p->end_list_offset % 2)) || (...)) */
    } /* if (LS_OK == ls_result) */


    /* ======================================================================
     *
     *  advanced checks
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  check size of 'known devices list':
     *  - max. 4096 entries
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        size = SV_MVB_ADMINISTRATOR_GET_C_KDL(p);
        if (4096 < size)
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: C_KDL=%d\n", size);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if (4096 < size) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  check size of 'periodic list':
     *  - min. size =  2760 = 22 + 10 +    0 + 2728
     *  - max. size = 10952 = 22 + 10 + 8192 + 2728
     *  NOTE:
     *  - size of cycle_list_offsets = 22   (  10*2)
     *  - size of split_list_offsets = 10   (   5*2)
     *  - min. size of cycle_list    = 0
     *  - max. size of cycle_list    = 8192 (4096*2)
     *  - size of split_list         = 2728
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        size = (UNSIGNED16)(p->bus_administrators_list_offset - \
            p->periodic_list_offset);
        if ((2760 > size) || (10952 < size))
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: PL=%d\n", size);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if ((2760 > size) || (10952 < size)) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  get pointer to first entry of the 'periodic list'
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        p_pl = SV_MVB_ADMINISTRATOR_GET_P_PL(p);
    } /* if (LS_OK == ls_result) */
    /* ----------------------------------------------------------------------
     *  check size of the 'split lists'
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        i_split = 0;
        while (((5-1) > i_split) && (LS_OK == ls_result))
        {
            c_split = SV_MVB_ADMINISTRATOR_GET_C_SL(p_pl, i_split);
            if (c_split != (UNSIGNED16)(4 << (i_split * 2)))
            {
#ifdef MUELL_LS_PRINT
                TCN_OSL_PRINTF("ERROR: C_SL=%d\n", c_split);
#endif /* #ifdef MUELL_LS_PRINT */
                ls_result = LS_CONFIG;
            } /* if (c_split != (UNSIGNED16)(4 << (i_split * 2))) */
            i_split++;
        } /* while (((5-1) > i_split) && (LS_OK == ls_result)) */
    } /* if (LS_OK == ls_result) */
    /* ----------------------------------------------------------------------
     *  check size of the 'cycle lists' against sum
     *  of the corresponding 'split lists' counts
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        i_cycle = 1;
        while ((11 > i_cycle) && (LS_OK == ls_result))
        {
            c_cycle     = SV_MVB_ADMINISTRATOR_GET_C_CL(p_pl, i_cycle);
            i_split     = (UNSIGNED16)((i_cycle / 2) + (i_cycle % 2) - 1);
            c_split_max = (UNSIGNED16)(1 << i_cycle);
            p_split     = SV_MVB_ADMINISTRATOR_GET_P_SL(p_pl, i_split);
            sum         = 0;
            for (c_split=0; c_split<c_split_max; c_split++)
            {
                if (0 != (i_cycle % 2))
                {
                    value = p_split->lower_count;
                } /* if (0 != (i_cycle % 2)) */
                else
                {
                    value = p_split->upper_count;
                } /* else */
                sum = (UNSIGNED16)(sum + value);
                p_split++;
            } /* for (c_split=0; c_split<c_split_max; c_split++) */
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF                                              \
            (                                                           \
                "- i_cycle=%2d, i_split=%d, c_cycle=%3d, sum=%3d\n",    \
                i_cycle, i_split, c_cycle, sum                          \
            );
#endif /* #ifdef MUELL_LS_PRINT */
            if (c_cycle != sum)
            {
#ifdef MUELL_LS_PRINT
                TCN_OSL_PRINTF("ERROR: c_cycle=%d, sum=%d\n", c_cycle, sum);
#endif /* #ifdef MUELL_LS_PRINT */
                ls_result = LS_CONFIG;
            } /* if (c_cycle != sum) */
            i_cycle++;
        } /* while ((11 > i_cycle) && (LS_OK == ls_result)) */
    } /* if (LS_OK == ls_result) */
    /* ----------------------------------------------------------------------
     *  check range of F-code of each 'cycle list' entry
     * ----------------------------------------------------------------------
     */
    ;

    /* ----------------------------------------------------------------------
     *  check size of 'bus administrators list':
     *  - max. 4096 entries
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        size = SV_MVB_ADMINISTRATOR_GET_C_BAL(p);
        if (4096 < size)
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: C_BAL=%d\n", size);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if (4096 < size) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  check size of 'devices scan list':
     *  - 1024 entries
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        size = SV_MVB_ADMINISTRATOR_GET_C_DSL(p);
        if (1024 < size)
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("ERROR: C_DSL=%d\n", size);
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if (1024 < size) */
    } /* if (LS_OK == ls_result) */

    return(ls_result);

} /* muell_ls_check_mvb_administrator_object */


/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_service_write_administrator
 *
 *  Purpose   : Handles link layer service
 *              'SV_MVB_SERVICE_WRITE_ADMINISTRATOR'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              muell_ls_service_write_administrator
 *              (
 *                  void                    *p_bus_ctrl,
 *                  SV_MVB_ADMINISTRATOR    *p_mvb_administrator
 *              );
 *
 *  Input     : p_bus_ctrl          - pointer to bus controller specific
 *                                    values
 *              p_mvb_administrator - pointer to a memory buffer
 *                                    of structured type
 *                                    'SV_MVB_ADMINISTRATOR'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
muell_ls_service_write_administrator
(
    void                    *p_bus_ctrl,
    SV_MVB_ADMINISTRATOR    *p_mvb_administrator
)
{
    LS_RESULT               ls_result           = LS_OK;
    MUELL_BUS_CTRL          *p_muell_bus_ctrl   = \
                                (MUELL_BUS_CTRL*)p_bus_ctrl;

    SV_MVB_ADMINISTRATOR_PL *p_pl;

    UNSIGNED16              counter;
    UNSIGNED16              counter_max;
    UNSIGNED16              counter_mue_cmd     = 0;
    UNSIGNED16              counter_mue_cmd_max = 0;

    UNSIGNED16              i_slot;
    UNSIGNED16              i_slot_max          = 0;
    UNSIGNED16              c_slot              = 0;

    UNSIGNED16              i_cycle;
    UNSIGNED16              c_cycle;
    UNSIGNED16              c_cycle_1           = 0;
    WORD16                  *p_cycle[11];
    WORD16                  *p_cycle_first[11];

    UNSIGNED16              i_split;
    UNSIGNED16              c_split;
    UNSIGNED16              c_split_max;
    SV_MVB_ADMINISTRATOR_SL *p_split;
    SV_MVB_ADMINISTRATOR_SL *p_split_first[5];

    WORD16                  ba_list_entry       = 0;
#ifdef MUELL_LS_BA_PD_DUMMY
    WORD16                  pd_dummy            = 0x0000;
#endif /* #ifdef MUELL_LS_BA_PD_DUMMY */

#ifdef MUELL_LS_PRINT
    UNSIGNED16              print_intro_done;
#endif /* #ifdef MUELL_LS_PRINT */

#ifndef MUELL_LS_SIM_BA
    MUE_RESULT              mue_result;
    BITSET16                mue_device_config   = 0x0000;
#endif /* #ifndef MUELL_LS_SIM_BA */

    /* ----------------------------------------------------------------------
     *  check MVB_Administrator object
     * ----------------------------------------------------------------------
     */
    ls_result = muell_ls_check_mvb_administrator_object(p_mvb_administrator);

    /* ----------------------------------------------------------------------
     *  check, if LINE_A/LINE_B of the SV device configuration is set
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if (0x0000 == (p_muell_bus_ctrl->mue_sv_device_config & \
            mue_sv_device_config_line_mask))
        {
            ls_result = LS_CALL_SEQUENCE;
        } /* if (0x0000 == (p_muell_bus_ctrl->mue_sv_device_config & ...)) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        /* get pointer to first entry of the 'periodic list' */
        p_pl = SV_MVB_ADMINISTRATOR_GET_P_PL(p_mvb_administrator);

        /* get size of 'cycle list 1' */
        c_cycle_1 = SV_MVB_ADMINISTRATOR_GET_C_CL(p_pl, 0);

        /* - get pointer to first entry of each 'cycle list' */
        /* - determine the slot number, after the BA list is */
        /*   repeated (use size of the 'cycle lists')        */
        i_slot_max = 0;
        for (i_cycle=0; i_cycle<11; i_cycle++)
        {
            p_cycle_first[i_cycle] = \
                (WORD16*)SV_MVB_ADMINISTRATOR_GET_P_CL(p_pl, i_cycle);
            c_cycle = SV_MVB_ADMINISTRATOR_GET_C_CL(p_pl, i_cycle);
            if (0 != c_cycle)
            {
                i_slot_max = (UNSIGNED16)(1 << i_cycle);
            } /* if (0 != c_cycle) */
        } /* for (i_cycle=0; i_cycle<11; i_cycle++) */
#ifdef MUELL_LS_PRINT
        TCN_OSL_PRINTF("i_slot_max=%d\n", i_slot_max);
#endif /* #ifdef MUELL_LS_PRINT */

        /* get pointer to first entry of each 'split list' */
        for (i_split=0; i_split<5; i_split++)
        {
            p_split_first[i_split] = \
                SV_MVB_ADMINISTRATOR_GET_P_SL(p_pl, i_split);
        } /* for (i_split=0; i_split<5; i_split++) */
    } /* if (LS_OK == ls_result) */

#ifndef MUELL_LS_SIM_BA
    /* ----------------------------------------------------------------------
     *  put device config (clear bits LINE_A, LINE_B and BA_EN)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        mue_device_config = (BITSET16)                     \
            (p_muell_bus_ctrl->mue_sv_device_config      & \
            (BITSET16)~(mue_sv_device_config_line_mask |   \
                        mue_sv_device_config_ba_en));
        mue_result = mue_sv_put_device_config(p_bus_ctrl, mue_device_config);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  put device config (restore without bit BA_EN)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        mue_device_config = (BITSET16)                \
            (p_muell_bus_ctrl->mue_sv_device_config & \
            (BITSET16)~mue_sv_device_config_ba_en);
        mue_result = mue_sv_put_device_config(p_bus_ctrl, mue_device_config);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */
#endif /* #ifndef MUELL_LS_SIM_BA */

    /* ----------------------------------------------------------------------
     *  build BA list (per slot)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        counter_mue_cmd     = 0;
        counter_mue_cmd_max = p_muell_bus_ctrl->ls_ba_list_entries;
        i_slot = 0;
        while ((i_slot < i_slot_max) && (LS_OK == ls_result))
        {
            c_slot = 0;
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("SLOT=%4d\n", (i_slot+1));
#endif /* #ifdef MUELL_LS_PRINT */

            /* Cycle_1 */
            i_cycle = 0;
#ifdef MUELL_LS_PRINT
            print_intro_done = 0;
#endif /* #ifdef MUELL_LS_PRINT */
            if (0 < c_cycle_1)
            {
                p_cycle[i_cycle] = p_cycle_first[i_cycle];
                counter = 0;
                while ((counter < c_cycle_1) && (LS_OK == ls_result))
                {
                    /* ------------------------------------------------------
                     *  put one list entry of BA list
                     * ------------------------------------------------------
                     */
                    ba_list_entry = (WORD16)(*p_cycle[i_cycle] & 0x7FFF);
#ifdef MUELL_LS_BA_PD_DUMMY
                    if (0x0000 == (ba_list_entry & 0x0FFF))
                    {
                        pd_dummy = ba_list_entry;
                    } /* if (0x0000 == (ba_list_entry & 0x0FFF)) */
#endif /* #ifdef MUELL_LS_BA_PD_DUMMY */
#ifdef MUELL_LS_PRINT
                    if (0 == print_intro_done)
                    {
                        print_intro_done = 1;
                        TCN_OSL_PRINTF(" C%d:", (1 << i_cycle));
                        TCN_OSL_PRINTF("0x%04X", ba_list_entry);
                    } /* if (0 == print_intro_done) */
                    else
                    {
                        TCN_OSL_PRINTF(",0x%04X", ba_list_entry);
                    } /* else */
    #endif /* #ifdef MUELL_LS_PRINT */

                    if (counter_mue_cmd >= counter_mue_cmd_max)
                    {
#ifdef MUELL_LS_PRINT
                        TCN_OSL_PRINTF("\nERROR: to much BA list entries\n");
#endif /* #ifdef MUELL_LS_PRINT */
                        ls_result = LS_CONFIG;
                    } /* if (counter_mue_cmd >= counter_mue_cmd_max) */
                
#ifndef MUELL_LS_SIM_BA
                    if (LS_OK == ls_result)
                    {
                        mue_result = \
                            mue_sv_put_ba_config(p_bus_ctrl, ba_list_entry);
                        if (MUE_RESULT_OK != mue_result)
                        {
                            /* store last result of MUE */
                            p_muell_bus_ctrl->mue_result = mue_result;

                            ls_result = LS_ERROR;
                        } /* if (MUE_RESULT_OK != mue_result) */
                    } /* if (LS_OK == ls_result) */
#endif /* #ifndef MUELL_LS_SIM_BA */
                    if (LS_OK == ls_result)
                    {
                        counter_mue_cmd++;
                    } /* if (LS_OK == ls_result) */

                    c_slot++;
                    p_cycle[i_cycle]++;
                    counter++;
                } /* while ((counter < c_cycle_1) && (LS_OK == ls_result)) */
            } /* if (0 < c_cycle_1) */

            /* Cycle_x (2, 4, 8, 16, 32, 64, 128, 256, 512, 1024) */
            i_cycle = 1;
            while ((i_cycle < 11) && (LS_OK == ls_result))
            {
#ifdef MUELL_LS_PRINT
                print_intro_done = 0;
#endif /* #ifdef MUELL_LS_PRINT */
                i_split     = (UNSIGNED16) \
                                (((i_cycle / 2) + (i_cycle % 2)) - 1);
                c_split     = 0;
                c_split_max = (UNSIGNED16)(1 << i_cycle);
                p_split     = p_split_first[i_split];
                if (0 == (i_slot & (c_split_max - 1)))
                {
                    p_cycle[i_cycle] = p_cycle_first[i_cycle];
                } /* if (0 == (i_slot & (c_split_max - 1))) */
                while ((c_split < c_split_max) && (LS_OK == ls_result))
                {
                    if ((i_slot & (c_split_max - 1)) == c_split)
                    {
                        if (0 != (i_cycle % 2))
                        {
                            counter_max = p_split->lower_count;
                        } /* if (0 != (i_cycle % 2)) */
                        else
                        {
                            counter_max = p_split->upper_count;
                        } /* else */
                        if (0 < counter_max)
                        {
                            counter = 0;
                            while ((counter < counter_max) && \
                                   (LS_OK == ls_result))
                            {
                                /* ------------------------------------------
                                 *  put one list entry of BA list
                                 * ------------------------------------------
                                 */
                                ba_list_entry = \
                                    (WORD16)(*p_cycle[i_cycle] & 0x7FFF);
#ifdef MUELL_LS_BA_PD_DUMMY
                                if (0x0000 == (ba_list_entry & 0x0FFF))
                                {
                                    pd_dummy = ba_list_entry;
                                } /* if (0x0000 == (...)) */
#endif /* #ifdef MUELL_LS_BA_PD_DUMMY */
#ifdef MUELL_LS_PRINT
                                if (0 == print_intro_done)
                                {
                                    print_intro_done = 1;
                                    TCN_OSL_PRINTF(" C%d:", (1 << i_cycle));
                                    TCN_OSL_PRINTF("0x%04X", ba_list_entry);
                                } /* if (0 == print_intro_done) */
                                else
                                {
                                    TCN_OSL_PRINTF(",0x%04X", ba_list_entry);
                                } /* else */
#endif /* #ifdef MUELL_LS_PRINT */

                                if (counter_mue_cmd >= counter_mue_cmd_max)
                                {
#ifdef MUELL_LS_PRINT
                                    TCN_OSL_PRINTF \
                                      ("\nERROR: to much BA list entries\n");
#endif /* #ifdef MUELL_LS_PRINT */
                                    ls_result = LS_CONFIG;
                                } /* if (counter_mue_cmd >= ...) */
#ifndef MUELL_LS_SIM_BA
                                if (LS_OK == ls_result)
                                {
                                    mue_result =                \
                                        mue_sv_put_ba_config    \
                                        (                       \
                                            p_bus_ctrl,         \
                                            ba_list_entry       \
                                        );                      \
                                    if (MUE_RESULT_OK != mue_result)
                                    {
                                        /* store last result of MUE */
                                        p_muell_bus_ctrl->mue_result = \
                                            mue_result;

                                        ls_result = LS_ERROR;
                                    } /* if (MUE_RESULT_OK != mue_result) */
                                } /* if (LS_OK == ls_result) */
#endif /* #ifndef MUELL_LS_SIM_BA */
                                if (LS_OK == ls_result)
                                {
                                    counter_mue_cmd++;
                                } /* if (LS_OK == ls_result) */

                                c_slot++;
                                p_cycle[i_cycle]++;
                                counter++;
                            } /* while ((counter < counter_max) && (...)) */
                        } /* if (0 < counter_max) */
                    } /* if ((i_slot & (c_split_max - 1)) == c_split) */
                    p_split++;
                    c_split++;
                } /* while ((c_split < c_split_max) && (...)) */
                i_cycle++;
            } /* while ((i_cycle < 11) && (LS_OK == ls_result)) */

            if (LS_OK == ls_result)
            {
                if ((i_slot + 1) < i_slot_max)
                {
                    /* ------------------------------------------------------
                     *  put sync statement of BA list
                     * ------------------------------------------------------
                     */
                    ba_list_entry = (WORD16)(0xA000 | i_slot);
#ifdef MUELL_LS_PRINT
                    TCN_OSL_PRINTF(" [SYNC=0x%04X]\n", ba_list_entry);
#endif /* #ifdef MUELL_LS_PRINT */

                    if (counter_mue_cmd >= counter_mue_cmd_max)
                    {
#ifdef MUELL_LS_PRINT
                        TCN_OSL_PRINTF("\nERROR: to much BA list entries\n");
#endif /* #ifdef MUELL_LS_PRINT */
                        ls_result = LS_CONFIG;
                    } /* if (counter_mue_cmd >= counter_mue_cmd_max) */
            
#ifndef MUELL_LS_SIM_BA
                    if (LS_OK == ls_result)
                    {
                        mue_result = mue_sv_put_ba_config(p_bus_ctrl, \
                            ba_list_entry);
                        if (MUE_RESULT_OK != mue_result)
                        {
                            /* store last result of MUE */
                            p_muell_bus_ctrl->mue_result = mue_result;

                            ls_result = LS_ERROR;
                        } /* if (MUE_RESULT_OK != mue_result) */
                    } /* if (LS_OK == ls_result) */
#endif /* #ifndef MUELL_LS_SIM_BA */
                    if (LS_OK == ls_result)
                    {
                        counter_mue_cmd++;
                    } /* if (LS_OK == ls_result) */
            
                } /* if ((i_slot + 1) < i_slot_max) */
            } /* if (LS_OK == ls_result) */

            i_slot++;
        } /* while ((i_slot < i_slot_max) && (LS_OK == ls_result)) */
    } /* if (LS_OK == ls_result) */

#ifdef MUELL_LS_BA_PD_DUMMY
    /* ----------------------------------------------------------------------
     *  put one list entry of BA list (PD dummy)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        if (0 == c_slot)
        {
            ba_list_entry = pd_dummy; /* PD dummy */
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF(" [PD_DUMMY=0x%04X]", ba_list_entry);
#endif /* #ifdef MUELL_LS_PRINT */

            if (counter_mue_cmd >= counter_mue_cmd_max)
            {
#ifdef MUELL_LS_PRINT
                TCN_OSL_PRINTF("\nERROR: to much BA list entries\n");
#endif /* #ifdef MUELL_LS_PRINT */
                ls_result = LS_CONFIG;
            } /* if (counter_mue_cmd >= counter_mue_cmd_max) */

#ifndef MUELL_LS_SIM_BA
            if (LS_OK == ls_result)
            {
                mue_result = mue_sv_put_ba_config(p_bus_ctrl, ba_list_entry);
                if (MUE_RESULT_OK != mue_result)
                {
                    /* store last result of MUE */
                    p_muell_bus_ctrl->mue_result = mue_result;

                    ls_result = LS_ERROR;
                } /* if (MUE_RESULT_OK != mue_result) */
            } /* if (LS_OK == ls_result) */
#endif /* #ifndef MUELL_LS_SIM_BA */
            if (LS_OK == ls_result)
            {
                counter_mue_cmd++;
            } /* if (LS_OK == ls_result) */

        } /* if (0 == c_slot) */
    } /* if (LS_OK == ls_result) */
#endif /* #ifdef MUELL_LS_BA_PD_DUMMY */

    /* ----------------------------------------------------------------------
     *  put end statment of BA list
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        ba_list_entry = (WORD16)0xAFFF;
    #ifdef MUELL_LS_PRINT
        TCN_OSL_PRINTF(" [END=0x%04X]", ba_list_entry);
    #endif /* #ifdef MUELL_LS_PRINT */

        if (counter_mue_cmd >= counter_mue_cmd_max)
        {
#ifdef MUELL_LS_PRINT
            TCN_OSL_PRINTF("\nERROR: to much BA list entries\n");
#endif /* #ifdef MUELL_LS_PRINT */
            ls_result = LS_CONFIG;
        } /* if (counter_mue_cmd >= counter_mue_cmd_max) */
    } /* if (LS_OK == ls_result) */

#ifndef MUELL_LS_SIM_BA
    if (LS_OK == ls_result)
    {
        mue_result = mue_sv_put_ba_config(p_bus_ctrl, ba_list_entry);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */
#endif /* #ifndef MUELL_LS_SIM_BA */
    if (LS_OK == ls_result)
    {
        counter_mue_cmd++;
    } /* if (LS_OK == ls_result) */

#ifndef MUELL_LS_SIM_BA
    /* ----------------------------------------------------------------------
     *  put device config (restore)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        mue_device_config = p_muell_bus_ctrl->mue_sv_device_config;
        mue_result = mue_sv_put_device_config(p_bus_ctrl, mue_device_config);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  get device config
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        mue_result = mue_sv_get_device_config(p_bus_ctrl, \
            &mue_device_config);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */

    /* ----------------------------------------------------------------------
     *  store MUE SV device configuration
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        p_muell_bus_ctrl->mue_sv_device_config = mue_device_config;
    } /* if (LS_OK == ls_result) */
#endif /* #ifndef MUELL_LS_SIM_BA */

#ifdef MUELL_LS_PRINT
    if (LS_OK == ls_result)
    {
        TCN_OSL_PRINTF("\n[counter_mue_cmd=%d]\n", counter_mue_cmd);
    } /* if (LS_OK == ls_result) */
#endif /* #ifdef MUELL_LS_PRINT */

    return(ls_result);

} /* muell_ls_service_write_administrator */


/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_service_errcnt_get_global
 *
 *  Purpose   : Handles link layer service
 *                  'SV_MVB_SERVICE_ERRCNT_GET_GLOBAL'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              muell_ls_service_errcnt_get_global
 *              (
 *                  void                    *p_bus_ctrl,
 *                  SV_MVB_ERRCNT_GLOBAL    *p_errcnt_global
 *              );
 *
 *  Input     : p_bus_ctrl        - pointer to bus controller specific values
 *
 *  In-/Output: p_errcnt_global   - pointer to a memory buffer of structured
 *                                  type 'SV_MVB_ERRCNT_GLOBAL'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
muell_ls_service_errcnt_get_global
(
    void                    *p_bus_ctrl,
    SV_MVB_ERRCNT_GLOBAL    *p_errcnt_global
)
{
    LS_RESULT       ls_result         = LS_OK;
    MUELL_BUS_CTRL  *p_muell_bus_ctrl = (MUELL_BUS_CTRL*)p_bus_ctrl;

    /* clear global error counter object */
    memset((void*)p_errcnt_global, 0, sizeof(SV_MVB_ERRCNT_GLOBAL));

    p_errcnt_global->errors_lineA = \
        p_muell_bus_ctrl->errcnt_global_lineA;
    p_errcnt_global->errors_lineB = \
        p_muell_bus_ctrl->errcnt_global_lineB;
    if ((p_errcnt_global->command & SV_MVB_ERRCNT_CMD_BIT_RESET_SEL) || \
        (p_errcnt_global->command & SV_MVB_ERRCNT_CMD_BIT_RESET_ALL))
    {
        p_muell_bus_ctrl->errcnt_global_lineA = 0;
        p_muell_bus_ctrl->errcnt_global_lineB = 0;
    } /* if ((p_errcnt_global->command & ...) || (...)) */

    return(ls_result);

} /* muell_ls_service_errcnt_get_global */


#ifdef MUELL_BC_ERRCNT_ALL_DS

/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_service_errcnt_get_ds_port
 *
 *  Purpose   : Handles link layer service
 *              'SV_MVB_SERVICE_ERRCNT_GET_DS_PORT'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              muell_ls_service_errcnt_get_ds_port
 *              (
 *                  void                    *p_bus_ctrl,
 *                  SV_MVB_ERRCNT_DS_PORT   *p_errcnt_ds_port
 *              );
 *
 *  Input     : p_bus_ctrl        - pointer to bus controller specific values
 *
 *  In-/Output: p_errcnt_ds_port  - pointer to a memory buffer of structured
 *                                  type 'SV_MVB_ERRCNT_DS_PORT'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
muell_ls_service_errcnt_get_ds_port
(
    void                    *p_bus_ctrl,
    SV_MVB_ERRCNT_DS_PORT   *p_errcnt_ds_port
)
{
    LS_RESULT       ls_result         = LS_OK;
    MUELL_BUS_CTRL  *p_muell_bus_ctrl = (MUELL_BUS_CTRL*)p_bus_ctrl;
    WORD16          device_address;
    UNSIGNED16      counter;

    /* clear device status port specific error counter object */
    device_address = p_errcnt_ds_port->device_address;
    memset((void*)p_errcnt_ds_port, 0, sizeof(SV_MVB_ERRCNT_DS_PORT));
    p_errcnt_ds_port->device_address = device_address;

    /* check input parameters */
    if (device_address > mue_sv_device_config_da_mask)
    {
        ls_result = LS_CONFIG;
    } /* if (device_address > mue_sv_device_config_da_mask) */

    if (LS_OK == ls_result)
    {
        p_errcnt_ds_port->errors_lineA = \
            p_muell_bus_ctrl->errcnt_ds_port_lineA[device_address];
        p_errcnt_ds_port->errors_lineB = \
            p_muell_bus_ctrl->errcnt_ds_port_lineB[device_address];
        if (p_errcnt_ds_port->command & SV_MVB_ERRCNT_CMD_BIT_RESET_SEL)
        {
            p_muell_bus_ctrl->errcnt_ds_port_lineA[device_address] = 0;
            p_muell_bus_ctrl->errcnt_ds_port_lineB[device_address] = 0;
        } /* if (p_errcnt_ds_port->command & ...) */
        if (p_errcnt_ds_port->command & SV_MVB_ERRCNT_CMD_BIT_RESET_ALL)
        {
            for (counter=0; counter<muell_ls_device_number_max; counter++)
            {
                p_muell_bus_ctrl->errcnt_ds_port_lineA[counter] = 0;
                p_muell_bus_ctrl->errcnt_ds_port_lineB[counter] = 0;
            } /* for (counter=0; counter<muell_ls_device_number_max; ...) */
        } /* if (p_errcnt_ds_port->command & ...) */
    } /* if (LS_OK == ls_result) */

    return(ls_result);

} /* muell_ls_service_errcnt_get_ds_port */

#endif /* #ifdef MUELL_BC_ERRCNT_ALL_DS */


/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_mue_sv_put_ba_config
 *
 *  Purpose   : Handles link layer service
 *              'SV_MVB_SERVICE_MUE_SV_PUT_BA_CONFIG'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              muell_ls_mue_sv_put_ba_config
 *              (
 *                  void                    *p_bus_ctrl,
 *                  SV_MVB_MUE_SV_BA_CONFIG *p_mue_sv_ba_config
 *              );
 *
 *  Input     : p_bus_ctrl            - pointer to bus controller
 *                                      specific values
 *
 *  In-/Output: p_mue_sv_ba_config    - pointer to a memory buffer of
 *                                      structured type
 *                                      'SV_MVB_MUE_SV_BA_CONFIG'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
muell_ls_mue_sv_put_ba_config
(
    void                    *p_bus_ctrl,
    SV_MVB_MUE_SV_BA_CONFIG *p_mue_sv_ba_config
)
{
    LS_RESULT       ls_result         = LS_OK;
    MUELL_BUS_CTRL  *p_muell_bus_ctrl = (MUELL_BUS_CTRL*)p_bus_ctrl;
    UNSIGNED16      nr_entries;
    UNSIGNED16      nr_entries_max;
    WORD16          *p_ba_list_entry;
    MUE_RESULT      mue_result;

    nr_entries      = 0;
    nr_entries_max  = p_mue_sv_ba_config->nr_entries;
    p_ba_list_entry = p_mue_sv_ba_config->ba_list;

    /* check input parameters */
    if (nr_entries_max > p_muell_bus_ctrl->ls_ba_list_entries)
    {
#ifdef MUELL_LS_PRINT
        TCN_OSL_PRINTF("\nERROR: to much BA list entries\n");
#endif /* #ifdef MUELL_LS_PRINT */
        ls_result = LS_CONFIG;
    } /* if (nr_entries_max > p_muell_bus_ctrl->ls_ba_list_entries) */

    while ((nr_entries < nr_entries_max) && (LS_OK == ls_result))
    {
        mue_result = mue_sv_put_ba_config(p_bus_ctrl, *p_ba_list_entry);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
        else
        {
            nr_entries++;
            p_ba_list_entry++;
        } /* else */

    } /* while ((nr_entries < nr_entries_max) && (LS_OK == ls_result)) */

    return(ls_result);

} /* muell_ls_mue_sv_put_ba_config */


/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_mue_sv_get_device_status
 *
 *  Purpose   : Handles link layer service
 *              'SV_MVB_SERVICE_MUE_SV_GET_DEVICE_STATUS'.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              LS_RESULT
 *              muell_ls_mue_sv_get_device_status
 *              (
 *                  void                        *p_bus_ctrl,
 *                  SV_MVB_MUE_SV_DEVICE_STATUS *p_mue_sv_device_status
 *              );
 *
 *  Input     : p_bus_ctrl                - pointer to bus controller
 *                                          specific values
 *
 *  In-/Output: p_mue_sv_device_status    - pointer to a memory buffer of
 *                                          structured type
 *                                          'SV_MVB_MUE_SV_DEVICE_STATUS'
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
LS_RESULT
muell_ls_mue_sv_get_device_status
(
    void                        *p_bus_ctrl,
    SV_MVB_MUE_SV_DEVICE_STATUS *p_mue_sv_device_status
)
{
    LS_RESULT       ls_result         = LS_OK;
    MUELL_BUS_CTRL  *p_muell_bus_ctrl = (MUELL_BUS_CTRL*)p_bus_ctrl;
    WORD16          device_address;
    MUE_RESULT      mue_result;

    device_address = p_mue_sv_device_status->device_address;

    /* clear MUE SV device status object */
    memset((void*)p_mue_sv_device_status, 0, \
        sizeof(SV_MVB_MUE_SV_DEVICE_STATUS));

    p_mue_sv_device_status->device_address = device_address;

    /* check input parameters */
    if (device_address > mue_sv_device_config_da_mask)
    {
        ls_result = LS_CONFIG;
    } /* if (device_address > mue_sv_device_config_da_mask) */

    if (LS_OK == ls_result)
    {
        mue_result =                                    \
            mue_sv_get_device_status                    \
            (                                           \
                p_bus_ctrl,                             \
                device_address,                         \
                (BOOLEAN1)FALSE,                        \
                &p_mue_sv_device_status->port_status,   \
                &p_mue_sv_device_status->device_status, \
                &p_mue_sv_device_status->freshness      \
            );
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        /* handle Error Counters */
        muell_ls_errcnt_ds                      \
        (                                       \
            p_muell_bus_ctrl,                   \
            device_address,                     \
            p_mue_sv_device_status->port_status \
        );
    } /* if (LS_OK == ls_result) */

    return(ls_result);

} /* muell_ls_mue_sv_get_device_status */


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_init (of procedure type 'LS_INIT')
 *
 *  Purpose   : Initialises a link layer as a whole.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              muell_ls_init
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
 *                initialisation before calling any other 'xxx_ls_xxx',
 *                'xxx_lp_xxx' or 'xxx_lm_xxx' procedure referencing
 *                the same link layer (parameter 'p_bus_ctrl').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
muell_ls_init
(
    void        *p_bus_ctrl
)
{
    LS_RESULT       ls_result             = LS_OK;
    MUELL_BUS_CTRL  *p_muell_bus_ctrl     = (MUELL_BUS_CTRL*)p_bus_ctrl;
    MUE_RESULT      mue_result;
    BITSET16        mue_device_config_put = 0x0000;
    BITSET16        mue_device_config_get = 0x0000;
#ifdef MUELL_BC_ERRCNT_ALL_DS
    UNSIGNED16      counter;
#endif

    /* ======================================================================
     *
     *  bus controller structure (dynamic values)
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  MUE
     * ----------------------------------------------------------------------
     */
    /* clear last result of MUE */
    p_muell_bus_ctrl->mue_result           = MUE_RESULT_OK;
    /* clear SV device configuration */
    p_muell_bus_ctrl->mue_sv_device_config = 0;


    /* ----------------------------------------------------------------------
     *  Clear Error Counters
     * ----------------------------------------------------------------------
     */
    /* error counters - global                                             */
    p_muell_bus_ctrl->errcnt_global_lineA = 0;
    p_muell_bus_ctrl->errcnt_global_lineB = 0;

#ifdef MUELL_BC_ERRCNT_ALL_DS
    /* error counters - all device status ports                            */
    if (p_muell_bus_ctrl->ls_ds_capability)
    {
        for (counter=0; counter<muell_ls_device_number_max; counter++)
        {
            p_muell_bus_ctrl->errcnt_ds_port_lineA[counter] = 0;
            p_muell_bus_ctrl->errcnt_ds_port_lineB[counter] = 0;
        } /* for (counter=0; counter<muell_ls_device_number_max; ...) */
    } /* if (p_muell_bus_ctrl->ls_ds_capability) */
#endif /* #ifdef MUELL_BC_ERRCNT_ALL_DS */


    /* ----------------------------------------------------------------------
     *  initialises the access to the UART (check for availability)
     * ----------------------------------------------------------------------
     */
    mue_result = mue_acc_init(p_bus_ctrl);
    if (MUE_RESULT_OK != mue_result)
    {
        /* store last result of MUE */
        p_muell_bus_ctrl->mue_result = mue_result;

        ls_result = LS_ERROR;
    } /* if (MUE_RESULT_OK != mue_result) */


    /* ----------------------------------------------------------------------
     *  perform a clean-up of the UART communication
     *  (synchronise access protocol of the MVB UART Emulation)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        mue_result = mue_acc_clean_up(p_bus_ctrl);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */


    /* ----------------------------------------------------------------------
     *  reset device
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        mue_result = mue_sv_put_device_config(p_bus_ctrl, 0x0000);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */


    /* ----------------------------------------------------------------------
     *  loopback selftest (use device configuration)
     * ----------------------------------------------------------------------
     */
    if (LS_OK == ls_result)
    {
        mue_device_config_put = (BITSET16)0x0123;
        mue_device_config_get = (BITSET16)0x0000;
        mue_result = mue_sv_put_device_config(p_bus_ctrl, \
            mue_device_config_put);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */
    if (LS_OK == ls_result)
    {
        mue_result = mue_sv_get_device_config(p_bus_ctrl, \
            &mue_device_config_get);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */
    if (LS_OK == ls_result)
    {
        if (mue_device_config_get != mue_device_config_put)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (mue_device_config_get != mue_device_config_put) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        mue_device_config_put = (BITSET16)0x0EDC;
        mue_device_config_get = (BITSET16)0x0000;
        mue_result = mue_sv_put_device_config(p_bus_ctrl, \
            mue_device_config_put);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */
    if (LS_OK == ls_result)
    {
        mue_result = mue_sv_get_device_config(p_bus_ctrl, \
            &mue_device_config_get);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */
    if (LS_OK == ls_result)
    {
        if (mue_device_config_get != mue_device_config_put)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (mue_device_config_get != mue_device_config_put) */
    } /* if (LS_OK == ls_result) */

    if (LS_OK == ls_result)
    {
        mue_device_config_put = (BITSET16)0x0000;
        mue_result = mue_sv_put_device_config(p_bus_ctrl, \
            mue_device_config_put);
        if (MUE_RESULT_OK != mue_result)
        {
            /* store last result of MUE */
            p_muell_bus_ctrl->mue_result = mue_result;

            ls_result = LS_ERROR;
        } /* if (MUE_RESULT_OK != mue_result) */
    } /* if (LS_OK == ls_result) */


    return(ls_result);

} /* muell_ls_init */


/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_service_handler
 *              (of procedure type 'LS_SERVICE_HANDLER')
 *
 *  Purpose   : Handles link layer specific services.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              muell_ls_service_handler
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
muell_ls_service_handler
(
    void        *p_bus_ctrl,
    ENUM8       service,
    void        *p_parameter
)
{
    LS_RESULT       ls_result         = LS_OK;
    MUELL_BUS_CTRL  *p_muell_bus_ctrl = (MUELL_BUS_CTRL*)p_bus_ctrl;

    switch (service)
    {
        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_READ_STATUS
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_READ_STATUS):
            ls_result =                                                     \
                muell_ls_service_read_status                                \
                (                                                           \
                    p_bus_ctrl,                                             \
                    (SV_MVB_STATUS*)p_parameter                             \
                );
            break;

        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_WRITE_CONTROL
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_WRITE_CONTROL):
            ls_result =                                                     \
                muell_ls_service_write_control                              \
                (                                                           \
                    p_bus_ctrl,                                             \
                    (SV_MVB_CONTROL*)p_parameter                            \
                );
            break;

        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_READ_DEVICES
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_READ_DEVICES):
            if (p_muell_bus_ctrl->ls_ds_capability)
            {
                ls_result =                                                 \
                    muell_ls_service_read_devices                           \
                    (                                                       \
                        p_bus_ctrl,                                         \
                        (SV_MVB_DEVICES**)p_parameter                       \
                    );
            } /* if (p_muell_bus_ctrl->ls_ds_capability) */
            else
            {
                ls_result = LS_SERVICE_NOT_SUPPORTED;
            } /* else */
            break;

        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_WRITE_ADMINISTRATOR
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_WRITE_ADMINISTRATOR):
            if (p_muell_bus_ctrl->ls_ba_capability)
            {
                ls_result =                                                 \
                    muell_ls_service_write_administrator                    \
                    (                                                       \
                        p_bus_ctrl,                                         \
                        (SV_MVB_ADMINISTRATOR*)p_parameter                  \
                    );
            } /* if (p_muell_bus_ctrl->ls_ba_capability) */
            else
            {
                ls_result = LS_SERVICE_NOT_SUPPORTED;
            } /* else */
            break;

        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_ERRCNT_GET_GLOBAL
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_ERRCNT_GET_GLOBAL):
            ls_result =                                                     \
                muell_ls_service_errcnt_get_global                          \
                (                                                           \
                    p_bus_ctrl,                                             \
                    (SV_MVB_ERRCNT_GLOBAL*)p_parameter                      \
                );
            break;

#ifdef MUELL_BC_ERRCNT_ALL_DS
        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_ERRCNT_GET_DS_PORT
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_ERRCNT_GET_DS_PORT):
            if (p_muell_bus_ctrl->ls_ds_capability)
            {
                ls_result =                                                 \
                    muell_ls_service_errcnt_get_ds_port                     \
                    (                                                       \
                        p_bus_ctrl,                                         \
                        (SV_MVB_ERRCNT_DS_PORT*)p_parameter                 \
                    );
            } /* if (p_muell_bus_ctrl->ls_ds_capability) */
            else
            {
                ls_result = LS_SERVICE_NOT_SUPPORTED;
            } /* else */
            break;
#endif /* #ifdef MUELL_BC_ERRCNT_ALL_DS */

        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_MUE_SV_PUT_BA_CONFIG
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_MUE_SV_PUT_BA_CONFIG):
            if (p_muell_bus_ctrl->ls_ba_capability)
            {
                ls_result =                                                 \
                    muell_ls_mue_sv_put_ba_config                           \
                    (                                                       \
                        p_bus_ctrl,                                         \
                        (SV_MVB_MUE_SV_BA_CONFIG*)p_parameter               \
                    );
            } /* if (p_muell_bus_ctrl->ls_ba_capability) */
            else
            {
                ls_result = LS_SERVICE_NOT_SUPPORTED;
            } /* else */
            break;

        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_MUE_SV_GET_DEVICE_STATUS
         * ------------------------------------------------------------------
         */
        case (SV_MVB_SERVICE_MUE_SV_GET_DEVICE_STATUS):
            if (p_muell_bus_ctrl->ls_ds_capability)
            {
                ls_result =                                                 \
                    muell_ls_mue_sv_get_device_status                       \
                    (                                                       \
                        p_bus_ctrl,                                         \
                        (SV_MVB_MUE_SV_DEVICE_STATUS*)p_parameter           \
                    );
            } /* if (p_muell_bus_ctrl->ls_ds_capability) */
            else
            {
                ls_result = LS_SERVICE_NOT_SUPPORTED;
            } /* else */
            break;

        default:
            ls_result = LS_SERVICE_NOT_SUPPORTED;
    } /* switch (service) */

    return(ls_result);

} /* muell_ls_service_handler */
