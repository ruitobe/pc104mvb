/* ==========================================================================
 *
 *  File      : TCN_TEST.C
 *
 *  Purpose   : TCN Test Application
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
 *  - TCN_TEST_MUE_PD_GET_PORT_DATA       - use MUE service to get PD port
 *                                          data
 *  - TCN_TEST_MUE_SV_GET_DEVICE_STATUS   - use MUE service to get SV device
 *                                          status
 *  - TCN_TEST_BA                         - enable MVB bus administrator
 *  - TCN_TEST_BA_OBJECT                  - use MVB_Administrator object
 *                                          instead of D2000 file and
 *                                          configuration manager
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
#ifdef TCN_TEST


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
#include <tcn_sv.h>
#include <tcn_sv_m.h>

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - process data
 * --------------------------------------------------------------------------
 */
#include <tcn_pd.h>

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - application layer
 * --------------------------------------------------------------------------
 */
#include <tcn_as.h>
#include <tcn_ap.h>

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - extra (configuration manager)
 * --------------------------------------------------------------------------
 */
#include <tcn_cm.h>

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* clock       */


/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
#define TCN_TEST_ON_ERROR(_result_)

#define TCN_TEST_LOOP_MAX   3

/* --------------------------------------------------------------------------
 *  Process Data
 * --------------------------------------------------------------------------
 */
#define TCN_TEST_PD_PORT_NUMBER_MAX 100
#define TCN_TEST_PD_PORT_SIZE_MIN   2
#define TCN_TEST_PD_PORT_SIZE_MAX   32


/* ==========================================================================
 *
 *  Local Variables
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Process Data
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL  PD_PRT_ATTR \
    tcn_test_pd_prt_attr[PD_PRT_PORT_NUMBER_MAX];
TCN_DECL_LOCAL  WORD16      \
    tcn_test_pd_port_address[TCN_TEST_PD_PORT_NUMBER_MAX];
TCN_DECL_LOCAL  WORD8       \
    tcn_test_pd_port_data   \
        [TCN_TEST_PD_PORT_NUMBER_MAX][TCN_TEST_PD_PORT_SIZE_MAX];


/* ==========================================================================
 *
 *  Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : tcn_test_wait_ms
 * --------------------------------------------------------------------------
 */
void
tcn_test_wait_ms
(
    UNSIGNED32  time_ms
)
{
    clock_t     clock_start;
    clock_t     clock_stop;
    UNSIGNED32  clock_diff;

    clock_start = clock();

    clock_diff = 0;
    while (clock_diff < time_ms)
    {
        clock_stop = clock();
        clock_diff = (UNSIGNED32) \
            ((clock_stop - clock_start) / (CLOCKS_PER_SEC / 1000));
    } /* while (clock_diff < time_ms) */

} /* tcn_test_wait_ms */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_test_create_random_value
 * --------------------------------------------------------------------------
 */
UNSIGNED16
tcn_test_create_random_value
(
    UNSIGNED16  low_range,
    UNSIGNED16  high_range
)
{
UNSIGNED32  range;
UNSIGNED16  random_value;

    range        = (UNSIGNED32)((high_range - low_range) + 1);
    random_value = (UNSIGNED16)((rand() % range) + low_range);

    return(random_value);

} /* tcn_test_create_random_value */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_test_sv
 * --------------------------------------------------------------------------
 */
UNSIGNED16
tcn_test_sv
(
    ENUM8       link_id,
    UNSIGNED16  *pd_port_number_max
)
{
    /* miscellaneous */
    UNSIGNED16      result;
    UNSIGNED16      counter;
    UNSIGNED16      loop;

    /* supervision */
    AS_RESULT       as_result;

    /* SV_LL_INFO */
    SV_LL_INFO      sv_ll_info;

    /* MVB_Status */
    SV_MVB_STATUS   mvb_status;
    /* MVB_Control */
    SV_MVB_CONTROL  mvb_control;

    result = 0;

    /* ----------------------------------------------------------------------
     *  as_service_handler(SV_LL_SERVICE_READ_INFO)
     * ----------------------------------------------------------------------
     */
    TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n", link_id, \
        "SV_LL_SERVICE_READ_INFO");
    as_result =                         \
        as_service_handler              \
        (                               \
            link_id,                    \
            SV_LL_SERVICE_READ_INFO,    \
            (void*)&sv_ll_info          \
        );
    TCN_OSL_PRINTF("  <- result               =%d\n", \
        (ENUM16)as_result);
    TCN_OSL_PRINTF("  <- pd_port_number_max   =%d\n", \
        (UNSIGNED16)sv_ll_info.pd_port_number_max);
    TCN_OSL_PRINTF("  <- pd_port_size_number  =%d\n", \
        (UNSIGNED16)sv_ll_info.pd_port_size_number);
    if (0 < sv_ll_info.pd_port_size_number)
    {
        for (counter=0; counter<sv_ll_info.pd_port_size_number; counter++)
        {
            TCN_OSL_PRINTF("  <- pd_port_size_value[%d]=%d\n",  \
                counter,                                        \
                (UNSIGNED8)sv_ll_info.pd_port_size_value[counter]);
        } /* for (counter=0; counter<sv_ll_info.pd_port_size_number; ...) */
    } /* if (0 < sv_ll_info.pd_port_size_number) */
    *pd_port_number_max = sv_ll_info.pd_port_number_max;

    /* ----------------------------------------------------------------------
     *  as_service_handler(SV_MVB_SERVICE_READ_STATUS)
     * ----------------------------------------------------------------------
     */
    TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n", link_id, \
        "SV_MVB_SERVICE_READ_STATUS");
    as_result =                         \
        as_service_handler              \
        (                               \
            link_id,                    \
            SV_MVB_SERVICE_READ_STATUS, \
            (void*)&mvb_status          \
        );
    TCN_OSL_PRINTF("  <- result           =%d\n", (ENUM16)as_result);
    TCN_OSL_PRINTF("  <- reserved1        =0x%04X\n",   \
        mvb_status.reserved1);
    TCN_OSL_PRINTF("  <- device_address   =0x%04X\n",   \
        mvb_status.device_address);
    TCN_OSL_PRINTF("  <- mvb_hardware_name='%s'\n",     \
        (char*)&mvb_status.mvb_hardware_name);
    TCN_OSL_PRINTF("  <- mvb_software_name='%s'\n",     \
        (char*)&mvb_status.mvb_software_name);
    TCN_OSL_PRINTF("  <- device_status    =0x%04X\n",   \
        mvb_status.device_status);
    TCN_OSL_PRINTF("  <- reserved2        =0x%02X\n",   \
        mvb_status.reserved2);
    TCN_OSL_PRINTF("  <- t_ignore         =%d\n",       \
        mvb_status.t_ignore);
    TCN_OSL_PRINTF("  <- lineA_errors     =%ld\n",      \
        mvb_status.lineA_errors);
    TCN_OSL_PRINTF("  <- lineB_errors     =%ld\n",      \
        mvb_status.lineB_errors);
    if (AS_OK != as_result)
    {
        TCN_OSL_PRINTF("ERROR:\n");
        TCN_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)as_result);
        TCN_TEST_ON_ERROR(as_result);
        result = (UNSIGNED16)as_result;
    } /* if (AS_OK != as_result) */
    if (0 == result)
    {
        if (0x0000 != mvb_status.reserved1)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- reserved1=0x%04X (0x0000)\n", \
                mvb_status.reserved1);
            TCN_TEST_ON_ERROR(AS_ERROR);
            result = (UNSIGNED16)AS_ERROR;
        } /* if (0x0000 != mvb_status.reserved1) */
    } /* if (0 == result) */
    if (0 == result)
    {
        if (0x0000 != mvb_status.device_address)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- device_address=0x%04X (0x0000)\n", \
                mvb_status.device_address);
            TCN_TEST_ON_ERROR(AS_ERROR);
            result = (UNSIGNED16)AS_ERROR;
        } /* if (0x0000 != mvb_status.device_address) */
    } /* if (0 == result) */
    if (0 == result)
    {
        if (0x0000 != mvb_status.device_status)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- device_status=0x%04X (0x0000)\n", \
                mvb_status.device_address);
            TCN_TEST_ON_ERROR(AS_ERROR);
            result = (UNSIGNED16)AS_ERROR;
        } /* if (0x0000 != mvb_status.device_status) */
    } /* if (0 == result) */
    if (0 == result)
    {
        if (0x00 != mvb_status.reserved2)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- reserved2=0x%02X (0x00)\n", \
                mvb_status.reserved2);
            TCN_TEST_ON_ERROR(AS_ERROR);
            result = (UNSIGNED16)AS_ERROR;
        } /* if (0x00 != mvb_status.reserved2) */
    } /* if (0 == result) */
    if (0 == result)
    {
        if (0 != mvb_status.t_ignore)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- t_ignore=%d (0)\n", mvb_status.t_ignore);
            TCN_TEST_ON_ERROR(AS_ERROR);
            result = (UNSIGNED16)AS_ERROR;
        } /* if (0 != mvb_status.t_ignore) */
    } /* if (0 == result) */
    if (0 == result)
    {
        if (0 != mvb_status.lineA_errors)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- lineA_errors=%ld (0)\n", \
                mvb_status.lineA_errors);
            TCN_TEST_ON_ERROR(AS_ERROR);
            result = (UNSIGNED16)AS_ERROR;
        } /* if (0 != mvb_status.lineA_errors) */
    } /* if (0 == result) */
    if (0 == result)
    {
        if (0 != mvb_status.lineB_errors)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- lineB_errors=%ld (0)\n", \
                mvb_status.lineB_errors);
            TCN_TEST_ON_ERROR(AS_ERROR);
            result = (UNSIGNED16)AS_ERROR;
        } /* if (0 != mvb_status.lineB_errors) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  as_service_handler
     *  (
     *      SV_MVB_SERVICE_WRITE_CONTROL,
     *      SV_MVB_SERVICE_READ_STATUS
     *  )
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- as_service_handler(%d, %s, %s)\n", link_id, \
            "SV_MVB_SERVICE_WRITE_CONTROL", "..._READ_STATUS");
        /* clear MVB_Control object */
        mvb_control.device_address = 0x0000;
        mvb_control.reserved1      = 0x00;
        mvb_control.t_ignore       = 0x00;
        mvb_control.reserved2      = 0x00;
        mvb_control.command        = 0x00;
        mvb_control.reserved3      = 0x0000;
        loop = 0;
        while ((TCN_TEST_LOOP_MAX > loop) && (0 == result))
        {
            /* --------------------------------------------------------------
             *  as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)
             * --------------------------------------------------------------
             */
            TCN_OSL_PRINTF(".");
            switch (loop)
            {
                case (0):
                    mvb_control.device_address = 0x000;
                    break;
                case (1):
                    mvb_control.device_address = 0xFFF;
                    break;
                default:
                    mvb_control.device_address = (WORD16) \
                        tcn_test_create_random_value(0x000, 0xFFF);
            } /* switch (loop) */

            as_result =                             \
                as_service_handler                  \
                (                                   \
                    link_id,                        \
                    SV_MVB_SERVICE_WRITE_CONTROL,   \
                    (void*)&mvb_control             \
                );
            if (AS_OK != as_result)
            {
                TCN_OSL_PRINTF("\nERROR:\n");
                TCN_OSL_PRINTF("- as_service_handler(%s)\n", \
                    "SV_MVB_SERVICE_WRITE_CONTROL");
                TCN_OSL_PRINTF("  <- result=%d (0)\n",       \
                    (ENUM16)as_result);
                TCN_TEST_ON_ERROR(as_result);
                result = (UNSIGNED16)as_result;
            } /* if (AS_OK != as_result) */

            /* --------------------------------------------------------------
             *  as_service_handler(SV_MVB_SERVICE_READ_STATUS)
             * --------------------------------------------------------------
             */
            if (0 == result)
            {
                TCN_OSL_PRINTF(":");
                mvb_status.device_address = \
                    (WORD16)~mvb_control.device_address;

                as_result =                         \
                    as_service_handler              \
                    (                               \
                        link_id,                    \
                        SV_MVB_SERVICE_READ_STATUS, \
                        (void*)&mvb_status          \
                    );
                if ((AS_OK != as_result) ||         \
                    (mvb_status.device_address !=   \
                        mvb_control.device_address))
                {
                    TCN_OSL_PRINTF("\nERROR:\n");
                    TCN_OSL_PRINTF("- as_service_handler(%s)\n",            \
                        "SV_MVB_SERVICE_READ_STATUS");
                    TCN_OSL_PRINTF("  <- result        =%d (0)\n",          \
                        (ENUM16)as_result);
                    TCN_OSL_PRINTF("  <- device_address=0x%04X (0x%04X)\n", \
                        mvb_status.device_address,                          \
                        mvb_control.device_address);
                    TCN_TEST_ON_ERROR(AS_ERROR);
                    result = (UNSIGNED16)AS_ERROR;
                } /* if ((...)) */
            } /* if (0 == result) */

            loop++;
        } /* while ((TCN_TEST_LOOP_MAX > loop) && (0 == result)) */
        if (0 == result)
        {
            TCN_OSL_PRINTF("\n");
        } /* if (0 == result) */
    } /* if (0 == result) */

    return(result);

} /* tcn_test_sv */

/* --------------------------------------------------------------------------
 *  Procedure : tcn_test_pd
 * --------------------------------------------------------------------------
 */
UNSIGNED16
tcn_test_pd
(
    ENUM8       ts_id,
    UNSIGNED16  pd_port_number_max
)
{
    /* miscellaneous */
    UNSIGNED16  result;
    UNSIGNED16  counter;
    UNSIGNED16  loop;

    /* process variables */
    AP_RESULT   ap_result;

    /* ap_port_manage */
    WORD16      pd_port_address;
    ENUM16      pd_port_command = PD_PRT_CMD_CONFIG;
    PD_PRT_ATTR pd_prt_attr;

    /* ap_put/get_dataset */
    UNSIGNED16  pd_port_counter;
    DS_NAME     ds_name;
    WORD8       pd_port_data_byte;
    WORD8       pd_port_data[TCN_TEST_PD_PORT_SIZE_MAX];
#ifdef TCN_TEST_MUE_PD_GET_PORT_DATA
    AS_RESULT   as_result;
    SV_MVB_MUE_PD_PORT_DATA \
                mue_pd_port_data;
#else /* #ifdef TCN_TEST_MUE_PD_GET_PORT_DATA */
    UNSIGNED16  pd_port_freshness;
#endif /* #else */

    result          = 0;
    pd_port_address = 0;

    /* ----------------------------------------------------------------------
     *  ap_ts_config()
     * ----------------------------------------------------------------------
     */
    TCN_OSL_PRINTF("- ap_ts_config(%d)\n", ts_id);
    ap_result = ap_ts_config(ts_id, 16);
    TCN_OSL_PRINTF("  <- result=%d\n", (ENUM16)ap_result);
    if (AP_OK != ap_result)
    {
        TCN_OSL_PRINTF("ERROR:\n");
        TCN_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)ap_result);
        TCN_TEST_ON_ERROR(ap_result);
        result = (UNSIGNED16)ap_result;
    } /* if (AP_OK != ap_result) */

    /* ----------------------------------------------------------------------
     *  ap_port_manage(PD_PRT_CMD_STATUS)
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- ap_port_manage(%d, %s)\n", ts_id, \
            "PD_PRT_CMD_STATUS");
        pd_port_address = 0;
        while ((pd_port_address < pd_port_number_max) && (0 == result))
        {
            pd_prt_attr.port_size      = 0;
            pd_prt_attr.port_config    = 0x00;
            pd_prt_attr.p_bus_specific = NULL;
            ap_result =                 \
                ap_port_manage          \
                (                       \
                    ts_id,              \
                    pd_port_address,    \
                    PD_PRT_CMD_STATUS,  \
                    &pd_prt_attr        \
                );
            if ((AP_OK != ap_result) && (AP_PRT_PASSIVE != ap_result))
            {
                TCN_OSL_PRINTF("ERROR:\n");
                TCN_OSL_PRINTF("- ap_port_manage(%d, %s, 0x%04X)\n",    \
                    ts_id, "PD_PRT_CMD_STATUS", pd_port_address);
                TCN_OSL_PRINTF("  <- result     =%d (0)\n",             \
                    (ENUM16)ap_result);
                TCN_OSL_PRINTF("  <- port_size  =%d (0)\n",             \
                    pd_prt_attr.port_size);
                TCN_OSL_PRINTF("  <- port_config=0x%02X (0x00)\n",      \
                    pd_prt_attr.port_config);
                TCN_TEST_ON_ERROR(ap_result);
                result = (UNSIGNED16)ap_result;
            } /* if ((...)) */

            pd_port_address++;
        } /* while ((pd_port_address < pd_port_number_max) && (...)) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  ap_port_manage()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- ap_port_manage(%d)\n", ts_id);
        pd_prt_attr.port_size      = 0;
        pd_prt_attr.port_config    = 0x00;
        pd_prt_attr.p_bus_specific = NULL;
        loop = 0;
        while ((TCN_TEST_LOOP_MAX > loop) && (0 == result))
        {
            if (0 == loop)
            {
                pd_port_command = PD_PRT_CMD_CONFIG;
            } /* if (0 == loop) */
            else if (1 == loop)
            {
                pd_port_command = PD_PRT_CMD_MODIFY;
            } /* else if (1 == loop) */
            else if (TCN_TEST_LOOP_MAX == (loop + 1))
            {
                pd_port_command = PD_PRT_CMD_DELETE;
            } /* else if (TCN_TEST_LOOP_MAX == (loop + 1)) */

            /* --------------------------------------------------------------
             *  ap_port_manage(PD_PRT_CMD_CONFIG/MODIFY/DELETE)
             * --------------------------------------------------------------
             */
            TCN_OSL_PRINTF(".");
            pd_port_address = 0;
            while ((pd_port_address < pd_port_number_max) && (0 == result))
            {
                switch (loop)
                {
                    case (0):
                        pd_prt_attr.port_size   = TCN_TEST_PD_PORT_SIZE_MIN;
                        pd_prt_attr.port_config = PD_SINK_PORT;
                        break;
                    case (1):
                        pd_prt_attr.port_size   = TCN_TEST_PD_PORT_SIZE_MAX;
                        pd_prt_attr.port_config = PD_SOURCE_PORT;
                        break;
                    default:
                        pd_prt_attr.port_size   = (UNSIGNED8) \
                            (2 << tcn_test_create_random_value(0, 4));
                        if (0 == tcn_test_create_random_value(0, 1))
                        {
                            pd_prt_attr.port_config = PD_SINK_PORT;
                        } /* if (0 == tcn_test_create_random_value(0, 1)) */
                        else
                        {
                            pd_prt_attr.port_config = PD_SOURCE_PORT;
                        } /* else */
                } /* switch (loop) */
                if (PD_PRT_CMD_DELETE == pd_port_command)
                {
                    pd_prt_attr.port_size   = 0;
                    pd_prt_attr.port_config = 0x00;
                } /* if (PD_PRT_CMD_DELETE == pd_port_command) */
                tcn_test_pd_prt_attr[pd_port_address] = pd_prt_attr;

                ap_result =                 \
                    ap_port_manage          \
                    (                       \
                        ts_id,              \
                        pd_port_address,    \
                        pd_port_command,    \
                        &pd_prt_attr        \
                    );
                if (AP_OK != ap_result)
                {
                    TCN_OSL_PRINTF("\nERROR:\n");
                    if (PD_PRT_CMD_CONFIG == pd_port_command)
                    {
                        TCN_OSL_PRINTF                              \
                            ("- ap_port_manage(%d, %s, 0x%04X)\n",  \
                            ts_id, "PD_PRT_CMD_CONFIG", pd_port_address);
                    } /* if (PD_PRT_CMD_CONFIG == pd_port_command) */
                    else
                    {
                        TCN_OSL_PRINTF                              \
                            ("- ap_port_manage(%d, %s, 0x%04X)\n",  \
                            ts_id, "PD_PRT_CMD_MODIFY", pd_port_address);
                    } /* else */
                    TCN_OSL_PRINTF("  -> port_size  =%d\n",         \
                        pd_prt_attr.port_size);
                    TCN_OSL_PRINTF("  -> port_config=0x%02X\n",     \
                        pd_prt_attr.port_config);
                    TCN_OSL_PRINTF("  <- result     =%d (0)\n",     \
                        (ENUM16)ap_result);
                    TCN_TEST_ON_ERROR(ap_result);
                    result = (UNSIGNED16)ap_result;
                } /* if (AP_OK != ap_result) */

                pd_port_address++;
            } /* while ((pd_port_address < pd_port_number_max) && (...)) */

            /* --------------------------------------------------------------
             *  ap_port_manage(PD_PRT_CMD_STATUS)
             * --------------------------------------------------------------
             */
            if (0 == result)
            {
                TCN_OSL_PRINTF(":");
            } /* if (0 == result) */
            pd_port_address = 0;
            while ((pd_port_address < pd_port_number_max) && (0 == result))
            {
                pd_prt_attr.port_size   = (UNSIGNED8) \
                    ~tcn_test_pd_prt_attr[pd_port_address].port_size;
                pd_prt_attr.port_config = (BITSET8) \
                    ~tcn_test_pd_prt_attr[pd_port_address].port_config;

                ap_result =                 \
                    ap_port_manage          \
                    (                       \
                        ts_id,              \
                        pd_port_address,    \
                        PD_PRT_CMD_STATUS,  \
                        &pd_prt_attr        \
                    );
                if (PD_PRT_CMD_DELETE == pd_port_command)
                {
                    if ((AP_OK != ap_result) && \
                        (AP_PRT_PASSIVE != ap_result))
                    {
                        TCN_OSL_PRINTF("\nERROR:\n");
                        TCN_OSL_PRINTF                                  \
                            ("- ap_port_manage(%d, %s, 0x%04X)\n",      \
                            ts_id, "PD_PRT_CMD_STATUS", pd_port_address);
                        TCN_OSL_PRINTF("  <- result     =%d (0)\n",     \
                            (ENUM16)ap_result);
                        TCN_OSL_PRINTF("  <- port_size  =%d (%d)\n",    \
                            pd_prt_attr.port_size,                      \
                            tcn_test_pd_prt_attr[pd_port_address].port_size);
                        TCN_OSL_PRINTF                                  \
                            ("  <- port_config=0x%02X (0x%02X)\n",      \
                            pd_prt_attr.port_config,                    \
                            tcn_test_pd_prt_attr                        \
                                [pd_port_address].port_config);
                        TCN_TEST_ON_ERROR(ap_result);
                        result = (UNSIGNED16)ap_result;
                    } /* if ((...)) */
                } /* if (PD_PRT_CMD_DELETE == pd_port_command) */
                else
                {
                    if (AP_OK != ap_result)
                    {
                        TCN_OSL_PRINTF("\nERROR:\n");
                        TCN_OSL_PRINTF                                  \
                            ("- ap_port_manage(%d, %s, 0x%04X)\n",      \
                            ts_id, "PD_PRT_CMD_STATUS", pd_port_address);
                        TCN_OSL_PRINTF("  <- result     =%d (0)\n",     \
                            (ENUM16)ap_result);
                        TCN_OSL_PRINTF("  <- port_size  =%d (%d)\n",    \
                            pd_prt_attr.port_size,                      \
                            tcn_test_pd_prt_attr                        \
                                [pd_port_address].port_size);
                        TCN_OSL_PRINTF                                  \
                            ("  <- port_config=0x%02X (0x%02X)\n",      \
                            pd_prt_attr.port_config,                    \
                            tcn_test_pd_prt_attr                        \
                                [pd_port_address].port_config);
                        TCN_TEST_ON_ERROR(ap_result);
                        result = (UNSIGNED16)ap_result;
                    } /* if (AP_OK != ap_result) */
                } /* else */

                pd_port_address++;
            } /* while ((pd_port_address < pd_port_number_max) && (...)) */

            loop++;
        } /* while ((TCN_TEST_LOOP_MAX > loop) && (0 == result)) */
        if (0 == result)
        {
            TCN_OSL_PRINTF("\n");
        } /* if (0 == result) */
    } /* if (0 == result) */


    if (0 == result)
    {
        if (TCN_TEST_PD_PORT_NUMBER_MAX < pd_port_number_max)
        {
            pd_port_number_max = TCN_TEST_PD_PORT_NUMBER_MAX;
        } /* if (TCN_TEST_PD_PORT_NUMBER_MAX < pd_port_number_max) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  ap_put/get_dataset()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- ap_put/get_dataset(%d)\n", ts_id);
        pd_port_address = 0x0000;
    } /* if (0 == result) */
    pd_port_counter = 0;
    while ((pd_port_counter < pd_port_number_max) && (0 == result))
    {
        /* ------------------------------------------------------------------
         *  ap_port_manage(PD_PRT_CMD_CONFIG)
         * ------------------------------------------------------------------
         */
        pd_port_address = (WORD16)(pd_port_address + \
            (WORD16)tcn_test_create_random_value(1, 15));
        pd_prt_attr.port_size   = TCN_TEST_PD_PORT_SIZE_MAX;
        pd_prt_attr.port_config = PD_SOURCE_PORT;

        tcn_test_pd_port_address[pd_port_counter] = pd_port_address;
        tcn_test_pd_prt_attr[pd_port_counter]     = pd_prt_attr;

        ap_result =                 \
            ap_port_manage          \
            (                       \
                ts_id,              \
                pd_port_address,    \
                PD_PRT_CMD_CONFIG,  \
                &pd_prt_attr        \
            );
        if (AP_OK != ap_result)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("- ap_port_manage(%d, %s, 0x%04X)\n",    \
                ts_id, "PD_PRT_CMD_CONFIG", pd_port_address);
            TCN_OSL_PRINTF("  -> port_size  =%d\n",                 \
                pd_prt_attr.port_size);
            TCN_OSL_PRINTF("  -> port_config=0x%02X\n",             \
                pd_prt_attr.port_config);
            TCN_OSL_PRINTF("  <- result     =%d (0)\n",             \
                (ENUM16)ap_result);
            TCN_TEST_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (AP_OK != ap_result) */

        pd_port_counter++;
    } /* while ((pd_port_counter < pd_port_number_max) && (...)) */

    loop = 0;
    while ((TCN_TEST_LOOP_MAX > loop) && (0 == result))
    {
        /* ------------------------------------------------------------------
         *  ap_put_dataset()
         * ------------------------------------------------------------------
         */
        if (0 == result)
        {
            TCN_OSL_PRINTF(".");
        } /* if (0 == result) */
        pd_port_counter = 0;
        while ((pd_port_counter < pd_port_number_max) && (0 == result))
        {
            ds_name.traffic_store_id = ts_id;
            ds_name.port_address     = \
                tcn_test_pd_port_address[pd_port_counter];

            for (counter=0; counter<TCN_TEST_PD_PORT_SIZE_MAX; counter++)
            {
                switch (loop)
                {
                    case (0):
                        pd_port_data_byte = 0x00;
                        break;
                    case (1):
                        pd_port_data_byte = 0xFF;
                        break;
                    default:
                        pd_port_data_byte = (WORD8) \
                            tcn_test_create_random_value(0x00, 0xFF);
                } /* switch (loop) */
                pd_port_data[counter] = pd_port_data_byte;
                tcn_test_pd_port_data[pd_port_counter][counter] = \
                    pd_port_data_byte;
            } /* for (counter=0; counter<TCN_TEST_PD_PORT_SIZE_MAX; ...) */

            ap_result = ap_put_dataset(&ds_name, &pd_port_data[0]);
            if (AP_OK != ap_result)
            {
                TCN_OSL_PRINTF("\nERROR:\n");
                TCN_OSL_PRINTF("- ap_put_dataset()\n");
                TCN_OSL_PRINTF("  -> ds_name.ts_id       =%d\n",        \
                    ds_name.traffic_store_id);
                TCN_OSL_PRINTF("  -> ds_name.port_address=0x%04X\n",    \
                    ds_name.port_address);
                TCN_OSL_PRINTF("  <- result              =%d (0)\n",    \
                    (ENUM16)ap_result);
                TCN_TEST_ON_ERROR(ap_result);
                result = (UNSIGNED16)ap_result;
            } /* if (AP_OK != ap_result) */

            pd_port_counter++;
        } /* while ((pd_port_counter < pd_port_number_max) && (...)) */

#ifdef TCN_TEST_MUE_PD_GET_PORT_DATA
        /* ------------------------------------------------------------------
         *  SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA
         * ------------------------------------------------------------------
         */
        if (0 == result)
        {
            TCN_OSL_PRINTF(":");
        } /* if (0 == result) */
        pd_port_counter = 0;
        while ((pd_port_counter < pd_port_number_max) && (0 == result))
        {
            memset((void*)&mue_pd_port_data, 0, sizeof(mue_pd_port_data));
            mue_pd_port_data.port_address = \
                tcn_test_pd_port_address[pd_port_counter];

            for (counter=0; counter<TCN_TEST_PD_PORT_SIZE_MAX; counter++)
            {
                mue_pd_port_data.port_data[counter] = (WORD8) \
                    ~tcn_test_pd_port_data[pd_port_counter][counter];
            } /* for (counter=0; counter<TCN_TEST_PD_PORT_SIZE_MAX; ...) */
            as_result =                                     \
                as_service_handler                          \
                (                                           \
                    ts_id,                                  \
                    SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA,    \
                    (void*)&mue_pd_port_data                \
                );
            if ((AS_OK != as_result)                                || \
                (tcn_test_pd_prt_attr[pd_port_counter].port_size !=    \
                    mue_pd_port_data.port_size)                     || \
                (0x00 != mue_pd_port_data.port_status)              || \
                (0xFFFF != mue_pd_port_data.port_freshness))
            {
                TCN_OSL_PRINTF("\nERROR:\n");
                TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n",        \
                    ts_id, "SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA");
                TCN_OSL_PRINTF("  -> port_address  =0x%04X\n",          \
                    mue_pd_port_data.port_address);
                TCN_OSL_PRINTF("  <- result        =%d (0)\n",          \
                    (ENUM16)as_result);
                TCN_OSL_PRINTF("  <- port_size     =%d (%d)\n",         \
                    mue_pd_port_data.port_size,                         \
                    tcn_test_pd_prt_attr[pd_port_counter].port_size);
                TCN_OSL_PRINTF("  <- port_status   =0x%02X (0x00)\n",   \
                    mue_pd_port_data.port_status);
                TCN_OSL_PRINTF("  <- port_freshness=0x%04X (0xFFFF)\n", \
                    mue_pd_port_data.port_freshness);
                TCN_TEST_ON_ERROR(AP_ERROR);
                result = (UNSIGNED16)AP_ERROR;
            } /* if ((AP_OK != ap_result) || (...)) */
            counter = 0;
            while ((counter < TCN_TEST_PD_PORT_SIZE_MAX) && (0 == result))
            {
                if (mue_pd_port_data.port_data[counter] != \
                    tcn_test_pd_port_data[pd_port_counter][counter])
                {
                    TCN_OSL_PRINTF("\nERROR:\n");
                    TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n",        \
                        ts_id, "SV_MVB_SERVICE_MUE_PD_GET_PORT_DATA");
                    TCN_OSL_PRINTF("  -> port_address  =0x%04X\n",          \
                        mue_pd_port_data.port_address);
                    TCN_OSL_PRINTF("  <- result        =%d (0)\n",          \
                        (ENUM16)as_result);
                    TCN_OSL_PRINTF("  <- port_size     =%d (%d)\n",         \
                        mue_pd_port_data.port_size,                         \
                        tcn_test_pd_prt_attr[pd_port_counter].port_size);
                    TCN_OSL_PRINTF("  <- port_status   =0x%02X (0x00)\n",   \
                        mue_pd_port_data.port_status);
                    TCN_OSL_PRINTF("  <- port_freshness=0x%04X (0xFFFF)\n", \
                        mue_pd_port_data.port_freshness);
                    for (loop=0; loop<2; loop++)
                    {
                        TCN_OSL_PRINTF("  <- port_data (%d) =", (loop + 1));
                        for (counter=0; counter<16; counter++)
                        {
                            TCN_OSL_PRINTF("%02X",         \
                                mue_pd_port_data.port_data \
                                    [((loop * 16) + counter)]);
                            if (15 > counter)
                            {
                                TCN_OSL_PRINTF(",");
                            } /* if (15 > counter) */
                        } /* for (counter=0; counter<16; counter++) */
                        TCN_OSL_PRINTF("\n");
                        TCN_OSL_PRINTF("                   (");
                        for (counter=0; counter<16; counter++)
                        {
                            TCN_OSL_PRINTF("%02X",      \
                                tcn_test_pd_port_data   \
                                    [pd_port_counter]   \
                                        [((loop * 16) + counter)]);
                            if (15 > counter)
                            {
                                TCN_OSL_PRINTF(",");
                            } /* if (15 > counter) */
                        } /* for (counter=0; counter<16; counter++) */
                        TCN_OSL_PRINTF(")\n");
                    } /* for (loop=0; loop<2; loop++) */
                    TCN_TEST_ON_ERROR(AP_ERROR);
                    result = (UNSIGNED16)AP_ERROR;
                } /* if (...) */

                counter++;
            } /* while ((counter < TCN_TEST_PD_PORT_SIZE_MAX) && (...)) */

            pd_port_counter++;
        } /* while ((pd_port_counter < pd_port_number_max) && (...)) */
#else /* #ifdef TCN_TEST_MUE_PD_GET_PORT_DATA */
        /* ------------------------------------------------------------------
         *  ap_get_dataset()
         * ------------------------------------------------------------------
         */
        if (0 == result)
        {
            TCN_OSL_PRINTF(":");
        } /* if (0 == result) */
        pd_port_counter = 0;
        while ((pd_port_counter < pd_port_number_max) && (0 == result))
        {
            ds_name.traffic_store_id = ts_id;
            ds_name.port_address     = \
                tcn_test_pd_port_address[pd_port_counter];

            for (counter=0; counter<TCN_TEST_PD_PORT_SIZE_MAX; counter++)
            {
                pd_port_data[counter] = (WORD8) \
                    ~tcn_test_pd_port_data[pd_port_counter][counter];
            } /* for (counter=0; counter<TCN_TEST_PD_PORT_SIZE_MAX; ...) */

            pd_port_freshness = 0x0000;

            ap_result = ap_get_dataset(&ds_name, &pd_port_data[0], \
                &pd_port_freshness);
            if ((AP_OK != ap_result) || (0xFFFF != pd_port_freshness))
            {
                TCN_OSL_PRINTF("\nERROR:\n");
                TCN_OSL_PRINTF("- ap_get_dataset()\n");
                TCN_OSL_PRINTF("  -> ds_name.ts_id       =%d\n",        \
                    ds_name.traffic_store_id);
                TCN_OSL_PRINTF("  -> ds_name.port_address=0x%04X\n",    \
                    ds_name.port_address);
                TCN_OSL_PRINTF("  <- result              =%d (0)\n",    \
                    (ENUM16)ap_result);
                TCN_OSL_PRINTF("  <- freshness           =0x%04X",      \
                    pd_port_freshness);
                TCN_OSL_PRINTF(" (0xFFFF)\n");
                TCN_TEST_ON_ERROR(AP_ERROR);
                result = (UNSIGNED16)AP_ERROR;
            } /* if ((AP_OK != ap_result) || (...)) */
            counter = 0;
            while ((counter < TCN_TEST_PD_PORT_SIZE_MAX) && (0 == result))
            {
                if (pd_port_data[counter] != \
                    tcn_test_pd_port_data[pd_port_counter][counter])
                {
                    TCN_OSL_PRINTF("\nERROR:\n");
                    TCN_OSL_PRINTF("- ap_get_dataset()\n");
                    TCN_OSL_PRINTF("  -> ds_name.ts_id       =%d\n",        \
                        ds_name.traffic_store_id);
                    TCN_OSL_PRINTF("  -> ds_name.port_address=0x%04X\n",    \
                        ds_name.port_address);
                    TCN_OSL_PRINTF("  <- result              =%d (0)\n",    \
                        (ENUM16)ap_result);
                    TCN_OSL_PRINTF("  <- freshness           =0x%04X",      \
                        pd_port_freshness);
                    TCN_OSL_PRINTF(" (0xFFFF)\n");
                    for (loop=0; loop<2; loop++)
                    {
                        TCN_OSL_PRINTF("  <- pd_port_data (%d)    =", \
                            (loop + 1));
                        for (counter=0; counter<16; counter++)
                        {
                            TCN_OSL_PRINTF("%02X", \
                                pd_port_data[((loop * 16) + counter)]);
                            if (15 > counter)
                            {
                                TCN_OSL_PRINTF(",");
                            } /* if (15 > counter) */
                        } /* for (counter=0; counter<16; counter++) */
                        TCN_OSL_PRINTF("\n");
                        TCN_OSL_PRINTF("                         (");
                        for (counter=0; counter<16; counter++)
                        {
                            TCN_OSL_PRINTF("%02X",      \
                                tcn_test_pd_port_data   \
                                    [pd_port_counter]   \
                                        [((loop * 16) + counter)]);
                            if (15 > counter)
                            {
                                TCN_OSL_PRINTF(",");
                            } /* if (15 > counter) */
                        } /* for (counter=0; counter<16; counter++) */
                        TCN_OSL_PRINTF(")\n");
                    } /* for (loop=0; loop<2; loop++) */
                    TCN_TEST_ON_ERROR(AP_ERROR);
                    result = (UNSIGNED16)AP_ERROR;
                } /* if (...) */

                counter++;
            } /* while ((counter < TCN_TEST_PD_PORT_SIZE_MAX) && (...)) */

            pd_port_counter++;
        } /* while ((pd_port_counter < pd_port_number_max) && (...)) */
#endif /* #else */

        loop++;
    } /* while ((TCN_TEST_LOOP_MAX > loop) && (0 == result)) */
    if (0 == result)
    {
        TCN_OSL_PRINTF("\n");
    } /* if (0 == result) */

    return(result);

} /* tcn_test_pd */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_test
 * --------------------------------------------------------------------------
 */
UNSIGNED16
tcn_test (void)
{
    /* miscellaneous */
    UNSIGNED16      result;
    UNSIGNED16      counter;

    /* supervision */
    AS_RESULT       as_result;
    ENUM8           link_id;

    /* as_read_link_set */
    BITSET16        link_set;
    BITSET16        pd_link_set;
    BITSET16        md_link_set;

    /* as_read_link_descriptor */
    ENUM8           link_type;
    STRING32        link_name;

    /* MVB_Control */
    SV_MVB_CONTROL  mvb_control;

    /* process variables */
    AP_RESULT       ap_result;
    UNSIGNED16      pd_port_number_max;

    /* ap_show_traffic_stores */
    UNSIGNED8       nr_of_busses;
    UNSIGNED8       link_id_list[16];

#ifdef TCN_TEST_BA
    /* bus administrator */
    UNSIGNED16      line;
    BITSET16        device_status_compare;
#ifdef TCN_TEST_BA_OBJECT
    FILE            *stream;
    WORD8           *mvb_administrator;
    WORD8           *p_ba;
    int             ba_byte1;
    int             ba_byte2;
#else /* #ifdef TCN_TEST_BA_OBJECT */
    CM_RESULT       cm_result;
    CM_D2000_PARAMETER          \
                    *p_cm_d2000_parameter;
    CM_D2000_ENTRIES_LIST       \
                    *p_entries_list;
    CM_D2000_ENTRIES_LIST       \
                    entries_list[2];
#endif /* #else */
#ifdef TCN_TEST_MUE_SV_GET_DEVICE_STATUS
    BITSET8         port_status;
    SV_MVB_MUE_SV_DEVICE_STATUS \
                    mue_sv_device_status;
#else /* #ifdef TCN_TEST_MUE_SV_GET_DEVICE_STATUS */
    SV_MVB_DEVICES  *mvb_devices;
#endif /* #else */
#endif /* #ifdef TCN_TEST_BA */


    result       = 0;
    nr_of_busses = 0;

    TCN_OSL_PRINTF("\n");
    TCN_OSL_PRINTF("Test TCN Driver API\n");
    TCN_OSL_PRINTF("===================\n");


    /* ----------------------------------------------------------------------
     *  initialise random-number generator
     * ----------------------------------------------------------------------
     */
    srand((unsigned int)time(NULL));


    /* ======================================================================
     *
     *  supervision
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  as_init()
     * ----------------------------------------------------------------------
     */
    TCN_OSL_PRINTF("- as_init()\n");
    as_result = as_init();
    TCN_OSL_PRINTF("  <- result=%d\n", (ENUM16)as_result);
    if (AS_OK != as_result)
    {
        TCN_OSL_PRINTF("ERROR:\n");
        TCN_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)as_result);
        TCN_TEST_ON_ERROR(as_result);
        result = (UNSIGNED16)as_result;
    } /* if (AS_OK != as_result) */

    /* ----------------------------------------------------------------------
     *  as_read_link_set()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- as_read_link_set()\n");
        as_result = as_read_link_set(&link_set, &pd_link_set, &md_link_set);
        TCN_OSL_PRINTF("  <- result     =%d\n", (ENUM16)as_result);
        TCN_OSL_PRINTF("  <- link_set   =0x%04X\n", link_set);
        TCN_OSL_PRINTF("  <- pd_link_set=0x%04X\n", pd_link_set);
        TCN_OSL_PRINTF("  <- md_link_set=0x%04X\n", md_link_set);
        if (AS_OK != as_result)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)as_result);
            TCN_TEST_ON_ERROR(as_result);
            result = (UNSIGNED16)as_result;
        } /* if (AS_OK != as_result) */
    } /* if (0 == result) */


    /* ======================================================================
     *
     *  process data ports
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  ap_init()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- ap_init()\n");
        ap_result = ap_init();
        TCN_OSL_PRINTF("  <- result=%d\n", (ENUM16)ap_result);
        if (AP_OK != ap_result)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)ap_result);
            TCN_TEST_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (AP_OK != ap_result) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  ap_show_traffic_stores()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- ap_show_traffic_stores()\n");
        ap_result = ap_show_traffic_stores(&nr_of_busses, link_id_list);
        TCN_OSL_PRINTF("  <- result      =%d\n", (ENUM16)ap_result);
        TCN_OSL_PRINTF("  <- nr_of_busses=%d\n", nr_of_busses);
        if (AP_OK != ap_result)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)ap_result);
            TCN_TEST_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (AP_OK != ap_result) */
    } /* if (0 == result) */
    if (0 == result)
    {
        TCN_OSL_PRINTF("  <- link_id_list=");
        if (0 < nr_of_busses)
        {
            counter = 0;
            while (counter < nr_of_busses)
            {
                TCN_OSL_PRINTF("%d", link_id_list[counter]);
                counter++;
                if (counter < nr_of_busses)
                {
                    TCN_OSL_PRINTF(",");
                } /* if (counter < nr_of_busses) */
            } /* while (counter < nr_of_busses) */
        } /* if (0 < nr_of_busses) */
        TCN_OSL_PRINTF("\n");
    } /* if (0 == result) */


    link_id = 0;
    while ((16 > link_id) && (0 == result))
    {
        /* ------------------------------------------------------------------
         *  as_read_link_descriptor()
         * ------------------------------------------------------------------
         */
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("- as_read_link_descriptor(%2d)\n", link_id);
        as_result = as_read_link_descriptor(link_id, &link_type, &link_name);
        if (AS_OK == as_result)
        {
            TCN_OSL_PRINTF("  <- result   =%d\n", (ENUM16)as_result);
            TCN_OSL_PRINTF("  <- link_type=%d\n", link_type);
            TCN_OSL_PRINTF("  <- link_name='%s'\n", (char*)&link_name);
        } /* if (AS_OK == as_result) */
        else if (AS_UNKNOWN_LINK == as_result)
        {
            TCN_OSL_PRINTF("  <- result=%d,", (ENUM16)as_result);
            TCN_OSL_PRINTF(" link_type=%d,", link_type);
            TCN_OSL_PRINTF(" link_name='%s'\n", (char*)&link_name);
        } /* else if (AS_UNKNOWN_LINK == as_result) */
        else
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)as_result);
            TCN_TEST_ON_ERROR(as_result);
            result = (UNSIGNED16)as_result;
        } /* else */

        if ((0 == result) && (AS_OK == as_result))
        {
            /* --------------------------------------------------------------
             *  tcn_test_sv
             * --------------------------------------------------------------
             */
            result = tcn_test_sv(link_id, &pd_port_number_max);

            /* --------------------------------------------------------------
             *  tcn_test_pd
             * --------------------------------------------------------------
             */
            if (0 == result)
            {
                result = tcn_test_pd(link_id, pd_port_number_max);
            } /* if (0 == result) */

            /* --------------------------------------------------------------
             *  as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)
             * --------------------------------------------------------------
             */
            if (0 == result)
            {
                /* clear MVB_Control object */
                mvb_control.device_address = 0x0000;
                mvb_control.reserved1      = 0x00;
                mvb_control.t_ignore       = 0x00;
                mvb_control.reserved2      = 0x00;
                mvb_control.command        = 0x00;
                mvb_control.reserved3      = 0x0000;
                as_result =                             \
                    as_service_handler                  \
                    (                                   \
                        link_id,                        \
                        SV_MVB_SERVICE_WRITE_CONTROL,   \
                        (void*)&mvb_control             \
                    );
                if (AS_OK != as_result)
                {
                    TCN_OSL_PRINTF("ERROR:\n");
                    TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n",    \
                        link_id, "SV_MVB_SERVICE_WRITE_CONTROL");
                    TCN_OSL_PRINTF("  -> device_address=0x%04X\n",      \
                        mvb_control.device_address);
                    TCN_OSL_PRINTF("  -> command       =0x%02X\n",      \
                        mvb_control.command);
                    TCN_OSL_PRINTF("  <- result        =%d (0)\n",      \
                        (ENUM16)as_result);
                    TCN_TEST_ON_ERROR(as_result);
                    result = (UNSIGNED16)as_result;
                } /* if (AS_OK != as_result) */
            } /* if (0 == result) */

        } /* if ((0 == result) && (AS_OK == as_result)) */

        link_id++;
    } /* while ((16 > link_id) && (0 == result)) */


#ifdef TCN_TEST_BA
    if (0 == result)
    {
        link_id = 0;
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("Test TCN Driver API with BA\n");
        TCN_OSL_PRINTF("===========================\n");
    } /* if (0 == result) */

    /* ======================================================================
     *
     *  supervision
     *
     * ======================================================================
     */
    line = 0;
    while ((2 > line) && (0 == result))
    {
        /* clear MVB_Control object */
        mvb_control.device_address = 0x0000;
        mvb_control.reserved1      = 0x00;
        mvb_control.t_ignore       = 0x00;
        mvb_control.reserved2      = 0x00;
        mvb_control.command        = 0x00;
        mvb_control.reserved3      = 0x0000;

        if (0 == line)
        {
            TCN_OSL_PRINTF("- use BA on MVB line A\n");
        } /* if (0 == line) */
        else
        {
            TCN_OSL_PRINTF("\n");
            TCN_OSL_PRINTF("- use BA on MVB line B\n");
        } /* else */

        /* ------------------------------------------------------------------
         *  as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)
         * ------------------------------------------------------------------
         */
        mvb_control.device_address = (WORD16)(line + 0x001);
        as_result =                             \
            as_service_handler                  \
            (                                   \
                link_id,                        \
                SV_MVB_SERVICE_WRITE_CONTROL,   \
                (void*)&mvb_control             \
            );
        if (AS_OK != as_result)
        {
            TCN_OSL_PRINTF("ERROR:\n");
            TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n",    \
                link_id, "SV_MVB_SERVICE_WRITE_CONTROL");
            TCN_OSL_PRINTF("  -> device_address=0x%04X\n",      \
                mvb_control.device_address);
            TCN_OSL_PRINTF("  -> command       =0x%02X\n",      \
                mvb_control.command);
            TCN_OSL_PRINTF("  <- result        =%d (0)\n",      \
                (ENUM16)as_result);
            TCN_TEST_ON_ERROR(as_result);
            result = (UNSIGNED16)as_result;
        } /* if (AS_OK != as_result) */

        /* ------------------------------------------------------------------
         *  as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)
         * ------------------------------------------------------------------
         */
        if (0 == result)
        {
            if (0 == line)
            {
                mvb_control.command |= SV_MVB_CTRL_CMD_BIT_SLA;
            } /* if (0 == line) */
            else
            {
                mvb_control.command |= SV_MVB_CTRL_CMD_BIT_SLB;
            } /* else */
            as_result =                             \
                as_service_handler                  \
                (                                   \
                    link_id,                        \
                    SV_MVB_SERVICE_WRITE_CONTROL,   \
                    (void*)&mvb_control             \
                );
            if (AS_OK != as_result)
            {
                TCN_OSL_PRINTF("ERROR:\n");
                TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n",    \
                    link_id, "SV_MVB_SERVICE_WRITE_CONTROL");
                TCN_OSL_PRINTF("  -> device_address=0x%04X\n",      \
                    mvb_control.device_address);
                TCN_OSL_PRINTF("  -> command       =0x%02X\n",      \
                    mvb_control.command);
                TCN_OSL_PRINTF("  <- result        =%d (0)\n",      \
                    (ENUM16)as_result);
                TCN_TEST_ON_ERROR(as_result);
                result = (UNSIGNED16)as_result;
            } /* if (AS_OK != as_result) */
        } /* if (0 == result) */

#ifdef TCN_TEST_BA_OBJECT
        /* ------------------------------------------------------------------
         *  as_service_handler(SV_MVB_SERVICE_WRITE_ADMINISTRATOR)
         * ------------------------------------------------------------------
         */
        if (0 == result)
        {
            stream = fopen("ba_tcn.bin", "r");
            if (NULL == stream)
            {
                TCN_OSL_PRINTF("  -> no BA supported\n");
            } /* if (NULL == stream) */
            else
            {
                counter = 0;
                while (fgetc(stream) != EOF)
                {
                    counter++;
                } /* while (fgetc(stream) != EOF) */
                mvb_administrator = (WORD8*)malloc(counter);
                if (NULL == mvb_administrator)
                {
                    TCN_OSL_PRINTF("  -> no BA supported\n");
                } /* if (NULL == mvb_administrator) */
                else
                {
                    if (0 != fseek(stream, 0, SEEK_SET))
                    {
                        TCN_OSL_PRINTF("  -> no BA supported\n");
                    } /* if (0 != fseek(stream, 0, SEEK_SET)) */
                    else
                    {
                        p_ba = (WORD8*)mvb_administrator;
                        ba_byte1 = fgetc(stream);
                        ba_byte2 = fgetc(stream);
                        while ((EOF != ba_byte1) && (EOF != ba_byte2))
                        {
#ifdef TCN_LE
                            /* perform byte swap */
                            *p_ba++ = (WORD8)ba_byte2;
                            *p_ba++ = (WORD8)ba_byte1;
#else /* #ifdef TCN_LE */
                            *p_ba++ = (WORD8)ba_byte1;
                            *p_ba++ = (WORD8)ba_byte2;
#endif /* #else */
                            ba_byte1 = fgetc(stream);
                            ba_byte2 = fgetc(stream);
                        } /* while ((EOF != ba_byte1) && (...)) */
                        TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n", \
                            link_id, "SV_MVB_SERVICE_WRITE_ADMINISTRATOR");
                        as_result =                                 \
                            as_service_handler                      \
                            (                                       \
                                link_id,                            \
                                SV_MVB_SERVICE_WRITE_ADMINISTRATOR, \
                                (void*)mvb_administrator            \
                            );
                        if (AS_OK != as_result)
                        {
                            TCN_OSL_PRINTF("ERROR:\n");
                            TCN_OSL_PRINTF                          \
                                ("- as_service_handler(%d, %s)\n",  \
                                link_id,                            \
                                "SV_MVB_SERVICE_WRITE_ADMINISTRATOR");
                            TCN_OSL_PRINTF("  <- result=%d (0)\n",  \
                                (ENUM16)as_result);
                            TCN_TEST_ON_ERROR(as_result);
                            result = (UNSIGNED16)as_result;
                        } /* if (AS_OK != as_result) */
                    } /* else */
                    free((void*)mvb_administrator);
                } /* else */
                fclose(stream);
            } /* else */
        } /* if (0 == result) */
#else /* #ifdef TCN_TEST_BA_OBJECT */
        /* ------------------------------------------------------------------
         *  cm_config_device(CM_CONFIG_TYPE_D2000)
         * ------------------------------------------------------------------
         */
        if (0 == result)
        {
            memset                                  \
            (                                       \
                (void*)entries_list,                \
                0,                                  \
                (2 * sizeof(CM_D2000_ENTRIES_LIST)) \
            );

            p_cm_d2000_parameter = (CM_D2000_PARAMETER*)&entries_list[0];
            p_entries_list       = (CM_D2000_ENTRIES_LIST*)&entries_list[1];

            p_entries_list = p_cm_d2000_parameter->entries_list;
            p_entries_list[0].entry  = CM_D2000_ENTRY_MVB_ADMINISTRATOR;
            p_entries_list[0].bus_id = 0;

            p_cm_d2000_parameter->nr_entries = 1;

            TCN_OSL_PRINTF("- cm_config_device(ba_d2000.bin)\n");
            cm_result =                     \
                cm_config_device            \
                (                           \
                    CM_CONFIG_TYPE_D2000,   \
                    "ba_d2000.bin",         \
                    p_cm_d2000_parameter    \
                );
            if (CM_OK != cm_result)
            {
                TCN_OSL_PRINTF("  <- result =%d (0)\n", (ENUM16)cm_result);
                TCN_TEST_ON_ERROR(cm_result);
                result = (UNSIGNED16)cm_result;
            } /* if (CM_OK != cm_result) */
        } /* if (0 == result) */
#endif /* #else */

        /* ------------------------------------------------------------------
         *  as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)
         * ------------------------------------------------------------------
         */
        if (0 == result)
        {
            mvb_control.command |= SV_MVB_CTRL_CMD_BIT_AON;
            as_result =                             \
                as_service_handler                  \
                (                                   \
                    link_id,                        \
                    SV_MVB_SERVICE_WRITE_CONTROL,   \
                    (void*)&mvb_control             \
                );
            if (AS_OK != as_result)
            {
                TCN_OSL_PRINTF("ERROR:\n");
                TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n",    \
                    link_id, "SV_MVB_SERVICE_WRITE_CONTROL");
                TCN_OSL_PRINTF("  -> device_address=0x%04X\n",      \
                    mvb_control.device_address);
                TCN_OSL_PRINTF("  -> command       =0x%02X\n",      \
                    mvb_control.command);
                TCN_OSL_PRINTF("  <- result        =%d (0)\n",      \
                    (ENUM16)as_result);
                TCN_TEST_ON_ERROR(as_result);
                result = (UNSIGNED16)as_result;
            } /* if (AS_OK != as_result) */
        } /* if (0 == result) */

        /* ------------------------------------------------------------------
         *  wait (20sec)
         * ------------------------------------------------------------------
         */
        if (0 == result)
        {
            TCN_OSL_PRINTF("- wait 20sec...");
            //tcn_test_wait_ms(20000);
					  delay_ms(20000);
            TCN_OSL_PRINTF("\n");
        } /* if (0 == result) */

        /* ------------------------------------------------------------------
         *  get and check device status
         * ------------------------------------------------------------------
         */
        if (0 == result)
        {
            if (0 == line)
            {
                device_status_compare = 0x43C0;
            } /* if (0 == line) */
            else
            {
                device_status_compare = 0x4340;
            } /* else */
#ifdef TCN_TEST_MUE_SV_GET_DEVICE_STATUS
            /* --------------------------------------------------------------
             *  as_service_handler(SV_MVB_SERVICE_MUE_SV_GET_DEVICE_STATUS)
             * --------------------------------------------------------------
             */
            if (0 == line)
            {
                port_status = SV_MVB_MUE_PORT_STATUS_SINK_A;
            } /* if (0 == line) */
            else
            {
                port_status = SV_MVB_MUE_PORT_STATUS_SINK_B;
            } /* else */
            memset((void*)&mue_sv_device_status, 0, \
                sizeof(mue_sv_device_status));
            mue_sv_device_status.device_address = mvb_control.device_address;
            as_result =                                         \
                as_service_handler                              \
                (                                               \
                    link_id,                                    \
                    SV_MVB_SERVICE_MUE_SV_GET_DEVICE_STATUS,    \
                    (void*)&mue_sv_device_status                \
                );
            if ((AS_OK                 != \
                    as_result                         ) || \
                (port_status           !=                  \
                    mue_sv_device_status.port_status  ) || \
                (device_status_compare !=                  \
                    mue_sv_device_status.device_status) || \
                (0xFFFF                ==                  \
                    mue_sv_device_status.freshness))
            {
                TCN_OSL_PRINTF("ERROR:\n");
                TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n",        \
                    link_id, "SV_MVB_SERVICE_MUE_SV_GET_DEVICE_STATUS");
                TCN_OSL_PRINTF("  -> device_address=0x%04X\n",          \
                    mue_sv_device_status.device_address);
                TCN_OSL_PRINTF("  <- result        =%d (0)\n",          \
                    (ENUM16)as_result);
                TCN_OSL_PRINTF("  <- port_status   =0x%02X (0x%02X)\n", \
                    mue_sv_device_status.port_status,                   \
                    port_status);
                TCN_OSL_PRINTF("  <- device_status =0x%04X (0x%04X)\n", \
                    mue_sv_device_status.device_status,                 \
                    device_status_compare);
                TCN_OSL_PRINTF("  <- freshness     =0x%04X\n",          \
                    mue_sv_device_status.freshness);
                TCN_TEST_ON_ERROR(AS_ERROR);
                result = (UNSIGNED16)AS_ERROR;
            } /* if ((...)) */
#else /* #ifdef TCN_TEST_MUE_SV_GET_DEVICE_STATUS */
            /* --------------------------------------------------------------
             *  as_service_handler(SV_MVB_SERVICE_READ_DEVICES)
             * --------------------------------------------------------------
             */
            mvb_devices = NULL;
            TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n", link_id, \
                "SV_MVB_SERVICE_READ_DEVICES");
            as_result =                             \
                as_service_handler                  \
                (                                   \
                    link_id,                        \
                    SV_MVB_SERVICE_READ_DEVICES,    \
                    (void*)&mvb_devices             \
                );
            if ((AS_OK != as_result)                            ||  \
                (NULL == mvb_devices)                           ||  \
                (1 != mvb_devices->nr_devices)                  ||  \
                (mvb_devices->devices_list[0].device_address !=     \
                    (WORD16)(line + 0x001))                     ||  \
                (mvb_devices->devices_list[0].device_status !=      \
                    device_status_compare))
            {
                TCN_OSL_PRINTF("ERROR:\n");
                TCN_OSL_PRINTF("- as_service_handler(%d, %s)\n",    \
                    link_id, "SV_MVB_SERVICE_READ_DEVICES");
                TCN_OSL_PRINTF("  <- result        =%d (0)\n",      \
                    (ENUM16)as_result);
                TCN_OSL_PRINTF("  <- nr_devices    =%d (1)\n",      \
                    mvb_devices->nr_devices);
                if (0 != mvb_devices->nr_devices)
                {
                    TCN_OSL_PRINTF("  <- device_address=0x%04X (0x%04X)\n", \
                        mvb_devices->devices_list[0].device_address,        \
                        (WORD16)(line + 0x001));
                    TCN_OSL_PRINTF("  <- device_status =0x%04X (0x%04X)\n", \
                        mvb_devices->devices_list[0].device_status,         \
                        device_status_compare);
                } /* if (0 != mvb_devices->nr_devices) */
                TCN_TEST_ON_ERROR(AS_ERROR);
                result = (UNSIGNED16)AS_ERROR;
            } /* if ((...)) */
            if (NULL != mvb_devices)
            {
                free((void*)mvb_devices);
            } /* if (NULL != mvb_devices) */
#endif /* #else */
        } /* if (0 == result) */

        line++;
    } /* while ((2 > line) && (0 == result)) */
#endif /* #ifdef TCN_TEST_BA */

    if (0 == result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF \
            ("***************************************************\n");
        TCN_OSL_PRINTF \
            ("* Test of TCN Driver API terminates successfully! *\n");
        TCN_OSL_PRINTF \
            ("***************************************************\n");
    } /* if (0 == result) */
    else
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF \
            ("******************************************************\n");
        TCN_OSL_PRINTF \
            ("* Test of TCN Driver API terminates with error (%2d). *\n", \
                result);
        TCN_OSL_PRINTF \
            ("******************************************************\n");
    } /* else */

    return(result);

} /* tcn_test */


/* --------------------------------------------------------------------------
 *  Procedure : main
 * --------------------------------------------------------------------------
 */
/*hjint
main (void)
{
    UNSIGNED16  result;

    result = tcn_test();

    TCN_MACRO_TERMINATE_MAIN(result);

    return((int)result);

}*/ /* main */


#endif /* #ifdef TCN_TEST */
