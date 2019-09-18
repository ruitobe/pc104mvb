/* ==========================================================================
 *
 *  File      : MUE_TEST.C
 *
 *  Purpose   : MVB UART Emulation - MUE Test Application
 *
 *  Project   : General TCN Driver Software
 *              - MVB UART Emulation Protocol (d-000206-nnnnnn)
 *              - TCN Software Architecture   (d-000487-nnnnnn)
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
 *  - MUE_TEST        - enable MUE test application
 *  - MUE_TEST_PD16   - enable MUE test for MVB UART Emulation 'PD16'
 *                      NOTE: 'MUE_PD_16' must be set (see file 'mue_pd.h')
 *  - MUE_TEST_MD16   - enable MUE test for MVB UART Emulation 'MD16'
 *                      NOTE: 'MUE_PD_16F' must be set (see file 'mue_pd.h')
 *  - MUE_TEST_MDFL   - enable MUE test for MVB UART Emulation 'MDFULL'
 *                      NOTE: 'MUE_PD_FULL' must be set (see file 'mue_pd.h')
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
#ifdef MUE_TEST


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


#define MUE_OSL_PRINTF printf
/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>
#include <mue_bc.h>
#include <mue_osl.h>
#include <mue_acc.h>
#include <mue_sv.h>
#include <mue_pd.h>
#include <mue_md.h>

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
#include <stdlib.h> /* srand, rand, exit */


/* ==========================================================================
 *
 *  Definitions
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
#define MUE_TEST_LOOP_MAX 4


/* ==========================================================================
 *
 *  Local Variables
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Bus Controller Structure
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC MUE_BUS_CTRL    mue_bus_ctrl;
TCN_DECL_LOCAL  void            *p_bus_ctrl = (void*)&mue_bus_ctrl;


/* ==========================================================================
 *
 *  Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_test_init_random_value
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
void
mue_test_init_random_value
(
    UNSIGNED32  seed
)
{

    /* ----------------------------------------------------------------------
     *  initialise random-number generator
     * ----------------------------------------------------------------------
     */
    srand((unsigned int)seed);

} /* mue_test_init_random_value */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_create_random_value
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
UNSIGNED16
mue_test_create_random_value
(
    UNSIGNED16  low_range,
    UNSIGNED16  high_range
)
{
UNSIGNED32  range;
UNSIGNED16  random_value;

    range        = (UNSIGNED32)(((UNSIGNED32)high_range - \
                                 (UNSIGNED32)low_range) + 1);
    random_value = (UNSIGNED16)((rand() % range) + low_range);

    return(random_value);

} /* mue_test_create_random_value */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_init_uart
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_init_uart (void)
{
    MUE_RESULT  result;

    /* ----------------------------------------------------------------------
     *  initialise MVB UART Emulation
     * ----------------------------------------------------------------------
     */
    MUE_OSL_PRINTF("- mue_acc_init()\n");
    result = mue_acc_init(p_bus_ctrl);
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("ERROR: mue_acc_init()\n");
    } /* if (MUE_RESULT_OK != result) */

    /* ----------------------------------------------------------------------
     *  cleanup MVB UART Emulation
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        MUE_OSL_PRINTF("- mue_acc_clean_up()\n");
        result = mue_acc_clean_up(p_bus_ctrl);
        if (MUE_RESULT_OK != result)
        {
            MUE_OSL_PRINTF("ERROR: mue_acc_clean_up()\n");
        } /* if (MUE_RESULT_OK != result) */
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_init_uart */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_sv
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_sv (void)
{
    MUE_RESULT  result;
    UNSIGNED16  loop;
    BITSET16    sv_device_config_put;
    BITSET16    sv_device_config_get;

    result = MUE_RESULT_OK;

    /* ----------------------------------------------------------------------
     *  mue_sv_put/get_device_config()
     * ----------------------------------------------------------------------
     */
    MUE_OSL_PRINTF("- mue_sv_put/get_device_config()\n");
    loop = 0;
    while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result))
    {
        /* ------------------------------------------------------------------
         *  mue_sv_put_device_config()
         * ------------------------------------------------------------------
         */
        MUE_OSL_PRINTF(".");
        switch (loop)
        {
            case (0):
                sv_device_config_put = 0x0000;
                break;
            case (1):
                sv_device_config_put = 0xFFFF;
                break;
            default:
                sv_device_config_put = \
                    (BITSET16)mue_test_create_random_value(0x0000, 0xFFFF);
        } /* switch (loop) */
        /* ignore bit 'mue_sv_device_config_ba_en' */
        sv_device_config_put &= (BITSET16)~mue_sv_device_config_ba_en;

        result = mue_sv_put_device_config(p_bus_ctrl, sv_device_config_put);
        if (MUE_RESULT_OK != result)
        {
            MUE_OSL_PRINTF("\nERROR:\n");
            MUE_OSL_PRINTF("- mue_sv_put_device_config(C=0x43)\n");
            MUE_OSL_PRINTF("  -> device_config=0x%04X\n", \
                sv_device_config_put);
            MUE_OSL_PRINTF("  <- result       =%d (0)\n", \
                (ENUM16)result);
        } /* if (MUE_RESULT_OK != result) */

        /* ------------------------------------------------------------------
         *  mue_sv_get_device_config()
         * ------------------------------------------------------------------
         */
        if (MUE_RESULT_OK == result)
        {
            MUE_OSL_PRINTF(":");
            sv_device_config_get = (BITSET16)~sv_device_config_put;

            result = \
                mue_sv_get_device_config(p_bus_ctrl, &sv_device_config_get);
            if (MUE_RESULT_OK != result)
            {
                MUE_OSL_PRINTF("\nERROR:\n");
                MUE_OSL_PRINTF("- mue_sv_get_device_config(S=0x53)\n");
                MUE_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)result);
            } /* if (MUE_RESULT_OK != result) */
            if (MUE_RESULT_OK == result)
            {
                if (sv_device_config_get != sv_device_config_put)
                {
                    MUE_OSL_PRINTF("\nERROR:\n");
                    MUE_OSL_PRINTF("- mue_sv_get_device_config(S=0x53)\n");
                    MUE_OSL_PRINTF("  <- result       =%d\n",              \
                        (ENUM16)result);
                    MUE_OSL_PRINTF("  <- device_config=0x%04X (0x%04X)\n", \
                        sv_device_config_get, sv_device_config_put);
                    result = MUE_RESULT_PARAMETER;
                } /* if (sv_device_config_get != sv_device_config_put) */
            } /* if (MUE_RESULT_OK == result) */
        } /* if (MUE_RESULT_OK == result) */

        loop++;
    } /* while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result)) */
    MUE_OSL_PRINTF("\n");

    return(result);

} /* mue_test_sv */


#ifdef MUE_TEST_MDFL

/* --------------------------------------------------------------------------
 *  Procedure : mue_test_sv_mdfl
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_sv_mdfl (void)
{
    MUE_RESULT  result;
    BITSET16    sv_device_config_put;
    BITSET16    sv_device_config_get;
    WORD16      sv_device_address;
    BITSET8     sv_port_status;
    BITSET16    sv_device_status;
    UNSIGNED16  sv_freshness;

    /* ----------------------------------------------------------------------
     *  mue_sv_put_device_config()
     * ----------------------------------------------------------------------
     */
    sv_device_config_put = 0x0000;
    result = mue_sv_put_device_config(p_bus_ctrl, sv_device_config_put);
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("ERROR:\n");
        MUE_OSL_PRINTF("- mue_sv_put_device_config(C=0x43)\n");
        MUE_OSL_PRINTF("  -> device_config=0x%04X\n", sv_device_config_put);
        MUE_OSL_PRINTF("  <- result       =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */

    /* ----------------------------------------------------------------------
     *  mue_sv_get_device_config()
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        sv_device_config_get = (BITSET16)~sv_device_config_put;
        result = mue_sv_get_device_config(p_bus_ctrl, &sv_device_config_get);
        if (MUE_RESULT_OK != result)
        {
            MUE_OSL_PRINTF("ERROR:\n");
            MUE_OSL_PRINTF("- mue_sv_get_device_config(S=0x53)\n");
            MUE_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)result);
        } /* if (MUE_RESULT_OK != result) */
        if (MUE_RESULT_OK == result)
        {
            if (sv_device_config_get != sv_device_config_put)
            {
                MUE_OSL_PRINTF("ERROR:\n");
                MUE_OSL_PRINTF("- mue_sv_get_device_config(S=0x53)\n");
                MUE_OSL_PRINTF("  <- result       =%d\n", (ENUM16)result);
                MUE_OSL_PRINTF("  <- device_config=0x%04X (0x%04X)\n", \
                    sv_device_config_get, sv_device_config_put);
                result = MUE_RESULT_PARAMETER;
            } /* if (sv_device_config_get != sv_device_config_put) */
        } /* if (MUE_RESULT_OK == result) */
    } /* if (MUE_RESULT_OK == result) */

    /* ----------------------------------------------------------------------
     *  mue_sv_get_device_status()
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        MUE_OSL_PRINTF("- mue_sv_get_device_status()\n");
        sv_device_address = 0;
        while ((sv_device_address < mue_sv_device_number_max) && \
               (MUE_RESULT_OK == result))
        {
            sv_port_status   = 0xFF;
            sv_device_status = 0x0000;
            sv_freshness     = 0x0000;

            result =                        \
                mue_sv_get_device_status    \
                (                           \
                    p_bus_ctrl,             \
                    sv_device_address,      \
                    (BOOLEAN1)FALSE,        \
                    &sv_port_status,        \
                    &sv_device_status,      \
                    &sv_freshness           \
                );
            if (MUE_RESULT_OK != result)
            {
                MUE_OSL_PRINTF("ERROR:\n");
                MUE_OSL_PRINTF("- mue_sv_get_device_status(D=0x44)\n");
                MUE_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)result);
            } /* if (MUE_RESULT_OK != result) */
            if (MUE_RESULT_OK == result)
            {
                if ((0x00 != sv_port_status) || (0xFFFF != sv_freshness))
                {
                    MUE_OSL_PRINTF("ERROR:\n");
                    MUE_OSL_PRINTF("- mue_sv_get_device_status(D=0x44)\n");
                    MUE_OSL_PRINTF("  <- result       =%d\n",              \
                        (ENUM16)result);
                    MUE_OSL_PRINTF("  <- port_status  =0x%02X (0x00)\n",   \
                        sv_port_status);
                    MUE_OSL_PRINTF("  <- device_status=0x%04X\n",          \
                        sv_device_status);
                    MUE_OSL_PRINTF("  <- freshness    =0x%04X (0xFFFF)\n", \
                        sv_freshness);
                    result = MUE_RESULT_PARAMETER;
                } /* if ((0x00 != sv_port_status) || (0xFFFF != ...)) */
            } /* if (MUE_RESULT_OK == result) */

            sv_device_address++;
        } /* while ((sv_device_address < ...) && (...)) */
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_sv_mdfl */

#endif /* MUE_TEST_MDFL */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_sv_cleanup
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_sv_cleanup (void)
{
    MUE_RESULT  result;
    BITSET16    sv_device_config_put;
    BITSET16    sv_device_config_get;

    /* ----------------------------------------------------------------------
     *  mue_sv_put_device_config()
     * ----------------------------------------------------------------------
     */
    sv_device_config_put = 0x0000;
    result = mue_sv_put_device_config(p_bus_ctrl, sv_device_config_put);
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("ERROR:\n");
        MUE_OSL_PRINTF("- mue_sv_put_device_config(C=0x43)\n");
        MUE_OSL_PRINTF("  -> device_config=0x%04X\n", sv_device_config_put);
        MUE_OSL_PRINTF("  <- result       =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */

    /* ----------------------------------------------------------------------
     *  mue_sv_get_device_config()
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        sv_device_config_get = (BITSET16)~sv_device_config_put;
        result = mue_sv_get_device_config(p_bus_ctrl, &sv_device_config_get);
        if (MUE_RESULT_OK != result)
        {
            MUE_OSL_PRINTF("ERROR:\n");
            MUE_OSL_PRINTF("- mue_sv_get_device_config(S=0x53)\n");
            MUE_OSL_PRINTF("  <- result=%d (0)\n", (ENUM16)result);
        } /* if (MUE_RESULT_OK != result) */
        if (MUE_RESULT_OK == result)
        {
            if (sv_device_config_get != sv_device_config_put)
            {
                MUE_OSL_PRINTF("ERROR:\n");
                MUE_OSL_PRINTF("- mue_sv_get_device_config(S=0x53)\n");
                MUE_OSL_PRINTF("  <- result       =%d\n", (ENUM16)result);
                MUE_OSL_PRINTF("  <- device_config=0x%04X (0x%04X)\n", \
                    sv_device_config_get, sv_device_config_put);
                result = MUE_RESULT_PARAMETER;
            } /* if (sv_device_config_get != sv_device_config_put) */
        } /* if (MUE_RESULT_OK == result) */
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_sv_cleanup */


#ifdef MUE_TEST_PD16

/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_16_put_port_config
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_16_put_port_config
(
    UNSIGNED16  loop,
    WORD8       pd_port_handle
)
{
    MUE_RESULT  result;
    WORD16      pd_port_address;
    BITSET16    pd_port_config_put;
    BITSET16    pd_port_config_special;

    /* ----------------------------------------------------------------------
     *  mue_pd_16_put_port_config()
     * ----------------------------------------------------------------------
     */
    switch (loop)
    {
        case (0):
            pd_port_address        = 0x000;
            pd_port_config_special = 0x0;
            break;
        case (1):
            pd_port_address        = 0xFFF;
            pd_port_config_special = 0xF;
            break;
        default:
            pd_port_address        = \
                (WORD16)mue_test_create_random_value(0x000, 0xFFF);
            pd_port_config_special = \
                (BITSET16)mue_test_create_random_value(0x0, 0xF);
    } /* switch (loop) */
    pd_port_config_put = \
        (BITSET16)((pd_port_config_special << 12) | pd_port_address);

    result =                        \
        mue_pd_16_put_port_config   \
        (                           \
            p_bus_ctrl,             \
            pd_port_handle,         \
            pd_port_config_put      \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_16_put_port_config(H=0x48)\n");
        MUE_OSL_PRINTF("  -> port_handle=0x%02X\n", pd_port_handle);
        MUE_OSL_PRINTF("  -> port_config=0x%04X\n", pd_port_config_put);
        MUE_OSL_PRINTF("  <- result     =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */

    return(result);

} /* mue_test_pd_16_put_port_config */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_16_get_port_config
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_16_get_port_config
(
    UNSIGNED16  loop,
    WORD8       pd_port_handle
)
{
    MUE_RESULT  result;
    WORD16      pd_port_address;
    BITSET16    pd_port_config_put;
    BITSET16    pd_port_config_get;
    BITSET16    pd_port_config_special;

    /* ----------------------------------------------------------------------
     *  mue_pd_16_get_port_config()
     * ----------------------------------------------------------------------
     */
    switch (loop)
    {
        case (0):
            pd_port_address        = 0x000;
            pd_port_config_special = 0x0;
            break;
        case (1):
            pd_port_address        = 0xFFF;
            pd_port_config_special = 0xF;
            break;
        default:
            pd_port_address        = \
                (WORD16)mue_test_create_random_value(0x000, 0xFFF);
            pd_port_config_special = \
                (BITSET16)mue_test_create_random_value(0x0, 0xF);
    } /* switch (loop) */
    pd_port_config_put = \
        (BITSET16)((pd_port_config_special << 12) | pd_port_address);
    pd_port_config_get = (BITSET16)~pd_port_config_put;

    result =                        \
        mue_pd_16_get_port_config   \
        (                           \
            p_bus_ctrl,             \
            pd_port_handle,         \
            &pd_port_config_get     \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_16_get_port_config(I=0x49)\n");
        MUE_OSL_PRINTF("  -> port_handle=0x%02X\n", pd_port_handle);
        MUE_OSL_PRINTF("  <- result     =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */
    if (MUE_RESULT_OK == result)
    {
        if (pd_port_config_get != pd_port_config_put)
        {
            MUE_OSL_PRINTF("\nERROR:\n");
            MUE_OSL_PRINTF("- mue_pd_16_get_port_config(I=0x49)\n");
            MUE_OSL_PRINTF("  -> port_handle=0x%02X\n", pd_port_handle);
            MUE_OSL_PRINTF("  <- result     =%d\n", (ENUM16)result);
            MUE_OSL_PRINTF("  <- port_config=0x%04X (0x%04X)\n", \
                pd_port_config_get, pd_port_config_put);
            result = MUE_RESULT_PARAMETER;
        } /* if (pd_port_config_get != pd_port_config_put) */
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_pd_16_get_port_config */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_16_put_port_data
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_16_put_port_data
(
    UNSIGNED16  loop,
    WORD8       pd_port_handle
)
{
    MUE_RESULT  result;
    UNSIGNED16  i;
    WORD8       pd_port_data_put[MUE_PD_PORT_SIZE_MAX];
    WORD8       pd_port_data_byte;

    /* ----------------------------------------------------------------------
     *  mue_pd_16_put_port_data()
     * ----------------------------------------------------------------------
     */
    for (i=0; i<mue_pd_port_size_max; i++)
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
                pd_port_data_byte = \
                    (WORD8)mue_test_create_random_value(0x00, 0xFF);
        } /* switch (loop) */
        pd_port_data_put[i] = pd_port_data_byte;
    } /* for (i=0; i<mue_pd_port_size_max; i++) */

    result =                        \
        mue_pd_16_put_port_data     \
        (                           \
            p_bus_ctrl,             \
            pd_port_handle,         \
            mue_pd_port_size_max,   \
            &pd_port_data_put[0]    \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_16_put_port_data(P=0x50)\n");
        MUE_OSL_PRINTF("  -> port_handle =0x%02X\n", pd_port_handle);
        MUE_OSL_PRINTF("  -> port_data   =");
        for (i=0; i<mue_pd_port_size_max; i++)
        {
            MUE_OSL_PRINTF("%02X", pd_port_data_put[i]);
            if (15 == i)
            {
                MUE_OSL_PRINTF("\n");
                MUE_OSL_PRINTF("                  ");
            } /* if (15 == i) */
            else
            {
                if (31 > i)
                {
                    MUE_OSL_PRINTF(",");
                } /* if (31 > i) */
                else
                {
                    MUE_OSL_PRINTF("\n");
                } /* else */
            } /* else */
        } /* for (i=0; i<mue_pd_port_size_max; i++) */
        MUE_OSL_PRINTF("  <- result      =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */

    return(result);

} /* mue_test_pd_16_put_port_data */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_16_get_port_data
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_16_get_port_data
(
    UNSIGNED16  loop,
    WORD8       pd_port_handle
)
{
    MUE_RESULT  result;
    UNSIGNED16  i;
    UNSIGNED16  line;
    BITSET8     pd_port_status;
    WORD8       pd_port_data_put[MUE_PD_PORT_SIZE_MAX];
    WORD8       pd_port_data_get[MUE_PD_PORT_SIZE_MAX];
    WORD8       pd_port_data_byte;

    /* ----------------------------------------------------------------------
     *  mue_pd_16_get_port_data()
     * ----------------------------------------------------------------------
     */
    pd_port_status = 0xFF;
    for (i=0; i<mue_pd_port_size_max; i++)
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
                pd_port_data_byte = \
                    (WORD8)mue_test_create_random_value(0x00, 0xFF);
        } /* switch (loop) */
        pd_port_data_put[i] = pd_port_data_byte;
        pd_port_data_get[i] = (WORD8)~pd_port_data_byte;
    } /* for (i=0; i<mue_pd_port_size_max; i++) */

    result =                        \
        mue_pd_16_get_port_data     \
        (                           \
            p_bus_ctrl,             \
            pd_port_handle,         \
            (BOOLEAN1)FALSE,        \
            mue_pd_port_size_max,   \
            &pd_port_status,        \
            &pd_port_data_get[0]    \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_16_get_port_data(G=0x47)\n");
        MUE_OSL_PRINTF("  -> port_handle=0x%02X\n", pd_port_handle);
        MUE_OSL_PRINTF("  <- result     =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */
    if (MUE_RESULT_OK == result)
    {
        if (0x00 != pd_port_status)
        {
            MUE_OSL_PRINTF("\nERROR:\n");
            MUE_OSL_PRINTF("- mue_pd_16_get_port_data(G=0x47)\n");
            MUE_OSL_PRINTF("  -> port_handle=0x%02X\n", pd_port_handle);
            MUE_OSL_PRINTF("  <- result     =%d\n", (ENUM16)result);
            MUE_OSL_PRINTF("  <- port_status=0x%02X (0x00)\n", \
                pd_port_status);
            result = MUE_RESULT_PARAMETER;
        } /* if (0x00 != pd_port_status) */
    } /* if (MUE_RESULT_OK == result) */
    if (MUE_RESULT_OK == result)
    {
        for (i=0; i<mue_pd_port_size_max; i++)
        {
            if (pd_port_data_get[i] != pd_port_data_put[i])
            {
                MUE_OSL_PRINTF("\nERROR:\n");
                MUE_OSL_PRINTF("- mue_pd_16_get_port_data(G=0x47)\n");
                MUE_OSL_PRINTF("  -> port_handle  =0x%04X\n", \
                    pd_port_handle);
                MUE_OSL_PRINTF("  <- result       =%d\n",     \
                    (ENUM16)result);
                MUE_OSL_PRINTF("  <- port_status  =0x%02X\n", \
                    pd_port_status);
                for (line=0; line<2; line++)
                {
                    MUE_OSL_PRINTF("  <- port_data (%d)=", (line + 1));
                    for (i=0; i<16; i++)
                    {
                        MUE_OSL_PRINTF("%02X", \
                            pd_port_data_get[((line * 16) + i)]);
                        if (15 > i)
                        {
                            MUE_OSL_PRINTF(",");
                        } /* if (15 > i) */
                    } /* for (i=0; i<16; i++) */
                    MUE_OSL_PRINTF("\n");
                    MUE_OSL_PRINTF("                  (");
                    for (i=0; i<16; i++)
                    {
                        MUE_OSL_PRINTF("%02X", \
                            pd_port_data_put[((line * 16) + i)]);
                        if (15 > i)
                        {
                            MUE_OSL_PRINTF(",");
                        } /* if (15 > i) */
                    } /* for (i=0; i<16; i++) */
                    MUE_OSL_PRINTF(")\n");
                } /* for (line=0; line<2; line++) */
                result = MUE_RESULT_PARAMETER;
            } /* if (pd_port_data_get[i] != pd_port_data_put[i]) */
        } /* for (i=0; i<mue_pd_port_size_max; i++) */
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_pd_16_get_port_data */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_16
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_16 (void)
{
    MUE_RESULT  result;
    UNSIGNED16  loop;
    WORD8       pd_port_handle;

    result = MUE_RESULT_OK;

    /* ----------------------------------------------------------------------
     *  mue_pd_16_put/get_port_config()
     * ----------------------------------------------------------------------
     */
    MUE_OSL_PRINTF("- mue_pd_16_put/get_port_config()\n");
    loop = 0;
    while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result))
    {
        /* ------------------------------------------------------------------
         *  mue_pd_16_put_port_config()
         * ------------------------------------------------------------------
         */
        MUE_OSL_PRINTF(".");
        mue_test_init_random_value((loop+1));
        pd_port_handle = 0;
        while ((pd_port_handle < mue_pd_16_port_number_max) && \
               (MUE_RESULT_OK == result))
        {
            result = mue_test_pd_16_put_port_config(loop, pd_port_handle);
            pd_port_handle++;
        } /* while ((pd_port_handle < mue_pd_16_port_number_max) && (...)) */

        /* ------------------------------------------------------------------
         *  mue_pd_16_get_port_config()
         * ------------------------------------------------------------------
         */
        if (MUE_RESULT_OK == result)
        {
            MUE_OSL_PRINTF(":");
            mue_test_init_random_value((loop+1));
            pd_port_handle = 0;
            while ((pd_port_handle < mue_pd_16_port_number_max) && \
                   (MUE_RESULT_OK == result))
            {
                result = \
                    mue_test_pd_16_get_port_config(loop, pd_port_handle);
                pd_port_handle++;
            } /* while ((pd_port_handle < ...) && (...)) */
        } /* if (MUE_RESULT_OK == result) */

        loop++;
    } /* while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result)) */
    MUE_OSL_PRINTF("\n");

    /* ----------------------------------------------------------------------
     *  mue_pd_16_put/get_port_data()
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        MUE_OSL_PRINTF("- mue_pd_16_put/get_port_data()\n");
        loop = 0;
        while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result))
        {
            /* --------------------------------------------------------------
             *  mue_pd_16_put_port_data()
             * --------------------------------------------------------------
             */
            MUE_OSL_PRINTF(".");
            mue_test_init_random_value((loop+1));
            pd_port_handle = 0;
            while ((pd_port_handle < mue_pd_16_port_number_max) && \
                   (MUE_RESULT_OK == result))
            {
                result = mue_test_pd_16_put_port_data(loop, pd_port_handle);
                pd_port_handle++;
            } /* while ((pd_port_handle < ...) && (...)) */

            /* --------------------------------------------------------------
             *  mue_pd_16_get_port_data()
             * --------------------------------------------------------------
             */
            if (MUE_RESULT_OK == result)
            {
                MUE_OSL_PRINTF(":");
                mue_test_init_random_value((loop+1));
                pd_port_handle = 0;
                while ((pd_port_handle < mue_pd_16_port_number_max) && \
                       (MUE_RESULT_OK == result))
                {
                    result = \
                        mue_test_pd_16_get_port_data(loop, pd_port_handle);
                    pd_port_handle++;
                } /* while ((pd_port_handle < ...) && (...)) */
            } /* if (MUE_RESULT_OK == result) */

            loop++;
        } /* while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == ...)) */
        MUE_OSL_PRINTF("\n");
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_pd_16 */

#endif /* #ifdef MUE_TEST_PD16 */


#ifdef MUE_TEST_MD16

/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_16f_put_port_config
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_16f_put_port_config
(
    UNSIGNED16  loop,
    WORD8       pd_port_handle
)
{
    MUE_RESULT  result;
    WORD16      pd_port_address;
    BITSET16    pd_port_config_put;
    BITSET16    pd_port_config_special;

    /* ----------------------------------------------------------------------
     *  mue_pd_16f_put_port_config()
     * ----------------------------------------------------------------------
     */
    switch (loop)
    {
        case (0):
            pd_port_address        = 0x000;
            pd_port_config_special = 0x0;
            break;
        case (1):
            pd_port_address        = 0xFFF;
            pd_port_config_special = 0xF;
            break;
        default:
            pd_port_address        = \
                (WORD16)mue_test_create_random_value(0x000, 0xFFF);
            pd_port_config_special = \
                (BITSET16)mue_test_create_random_value(0x0, 0xF);
    } /* switch (loop) */
    pd_port_config_put = \
        (BITSET16)((pd_port_config_special << 12) | pd_port_address);

    result =                        \
        mue_pd_16f_put_port_config  \
        (                           \
            p_bus_ctrl,             \
            pd_port_handle,         \
            pd_port_config_put      \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_16f_put_port_config(H=0x48)\n");
        MUE_OSL_PRINTF("  -> port_handle=0x%02X\n", pd_port_handle);
        MUE_OSL_PRINTF("  -> port_config=0x%04X\n", pd_port_config_put);
        MUE_OSL_PRINTF("  <- result     =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */

    return(result);

} /* mue_test_pd_16f_put_port_config */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_16f_get_port_config
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_16f_get_port_config
(
    UNSIGNED16  loop,
    WORD8       pd_port_handle
)
{
    MUE_RESULT  result;
    WORD16      pd_port_address;
    BITSET16    pd_port_config_put;
    BITSET16    pd_port_config_get;
    BITSET16    pd_port_config_special;

    /* ----------------------------------------------------------------------
     *  mue_pd_16f_get_port_config()
     * ----------------------------------------------------------------------
     */
    switch (loop)
    {
        case (0):
            pd_port_address        = 0x000;
            pd_port_config_special = 0x0;
            break;
        case (1):
            pd_port_address        = 0xFFF;
            pd_port_config_special = 0xF;
            break;
        default:
            pd_port_address        = \
                (WORD16)mue_test_create_random_value(0x000, 0xFFF);
            pd_port_config_special = \
                (BITSET16)mue_test_create_random_value(0x0, 0xF);
    } /* switch (loop) */
    pd_port_config_put = \
        (BITSET16)((pd_port_config_special << 12) | pd_port_address);
    pd_port_config_get = (BITSET16)~pd_port_config_put;

    result =                        \
        mue_pd_16f_get_port_config  \
        (                           \
            p_bus_ctrl,             \
            pd_port_handle,         \
            &pd_port_config_get     \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_16f_get_port_config(I=0x49)\n");
        MUE_OSL_PRINTF("  -> port_handle=0x%02X\n", pd_port_handle);
        MUE_OSL_PRINTF("  <- result     =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */
    if (MUE_RESULT_OK == result)
    {
        if (pd_port_config_get != pd_port_config_put)
        {
            MUE_OSL_PRINTF("\nERROR:\n");
            MUE_OSL_PRINTF("- mue_pd_16f_get_port_config(I=0x49)\n");
            MUE_OSL_PRINTF("  -> port_handle=0x%02X\n",          \
                pd_port_handle);
            MUE_OSL_PRINTF("  <- result     =%d\n",              \
                (ENUM16)result);
            MUE_OSL_PRINTF("  <- port_config=0x%04X (0x%04X)\n", \
                pd_port_config_get, pd_port_config_put);
            result = MUE_RESULT_PARAMETER;
        } /* if (pd_port_config_get != pd_port_config_put) */
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_pd_16f_get_port_config */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_16f_put_port_data
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_16f_put_port_data
(
    UNSIGNED16  loop,
    WORD8       pd_port_handle
)
{
    MUE_RESULT  result;
    UNSIGNED16  i;
    WORD8       pd_port_data_put[MUE_PD_PORT_SIZE_MAX];
    WORD8       pd_port_data_byte;

    /* ----------------------------------------------------------------------
     *  mue_pd_16f_put_port_data()
     * ----------------------------------------------------------------------
     */
    for (i=0; i<mue_pd_port_size_max; i++)
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
                pd_port_data_byte = \
                    (WORD8)mue_test_create_random_value(0x00, 0xFF);
        } /* switch (loop) */
        pd_port_data_put[i] = pd_port_data_byte;
    } /* for (i=0; i<mue_pd_port_size_max; i++) */

    result =                        \
        mue_pd_16f_put_port_data    \
        (                           \
            p_bus_ctrl,             \
            pd_port_handle,         \
            mue_pd_port_size_max,   \
            &pd_port_data_put[0]    \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_16f_put_port_data(P=0x50)\n");
        MUE_OSL_PRINTF("  -> port_handle =0x%02X\n", pd_port_handle);
        MUE_OSL_PRINTF("  -> port_data   =");
        for (i=0; i<mue_pd_port_size_max; i++)
        {
            MUE_OSL_PRINTF("%02X", pd_port_data_put[i]);
            if (15 == i)
            {
                MUE_OSL_PRINTF("\n");
                MUE_OSL_PRINTF("                  ");
            } /* if (15 == i) */
            else
            {
                if (31 > i)
                {
                    MUE_OSL_PRINTF(",");
                } /* if (31 > i) */
                else
                {
                    MUE_OSL_PRINTF("\n");
                } /* else */
            } /* else */
        } /* for (i=0; i<mue_pd_port_size_max; i++) */
        MUE_OSL_PRINTF("  <- result      =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */

    return(result);

} /* mue_test_pd_16f_put_port_data */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_16f_get_port_data
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_16f_get_port_data
(
    UNSIGNED16  loop,
    WORD8       pd_port_handle
)
{
    MUE_RESULT  result;
    UNSIGNED16  i;
    UNSIGNED16  line;
    BITSET8     pd_port_status;
    UNSIGNED16  pd_port_freshness;
    WORD8       pd_port_data_put[MUE_PD_PORT_SIZE_MAX];
    WORD8       pd_port_data_get[MUE_PD_PORT_SIZE_MAX];
    WORD8       pd_port_data_byte;

    /* ----------------------------------------------------------------------
     *  mue_pd_16f_get_port_data()
     * ----------------------------------------------------------------------
     */
    pd_port_status    = 0xFF;
    pd_port_freshness = 0x0000;
    for (i=0; i<mue_pd_port_size_max; i++)
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
                pd_port_data_byte = \
                    (WORD8)mue_test_create_random_value(0x00, 0xFF);
        } /* switch (loop) */
        pd_port_data_put[i] = pd_port_data_byte;
        pd_port_data_get[i] = (WORD8)~pd_port_data_byte;
    } /* for (i=0; i<mue_pd_port_size_max; i++) */

    result =                        \
        mue_pd_16f_get_port_data    \
        (                           \
            p_bus_ctrl,             \
            pd_port_handle,         \
            (BOOLEAN1)FALSE,        \
            mue_pd_port_size_max,   \
            &pd_port_status,        \
            &pd_port_data_get[0],   \
            &pd_port_freshness      \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF \
            ("- mue_pd_16f_get_port_data(G=0x47)\n");
        MUE_OSL_PRINTF("  -> port_handle=0x%02X\n", pd_port_handle);
        MUE_OSL_PRINTF("  <- result     =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */
    if (MUE_RESULT_OK == result)
    {
        if ((0x00 != pd_port_status) || (0xFFFF != pd_port_freshness))
        {
            MUE_OSL_PRINTF("\nERROR:\n");
            MUE_OSL_PRINTF("- mue_pd_16f_get_port_data(G=0x47)\n");
            MUE_OSL_PRINTF("  -> port_handle   =0x%02X\n",          \
                pd_port_handle);
            MUE_OSL_PRINTF("  <- result        =%d\n",              \
                (ENUM16)result);
            MUE_OSL_PRINTF("  <- port_status   =0x%02X (0x00)\n",   \
                pd_port_status);
            MUE_OSL_PRINTF("  <- port_freshness=0x%04X (0xFFFF)\n", \
                pd_port_freshness);
            result = MUE_RESULT_PARAMETER;
        } /* if ((0x00 != pd_port_status) || (0xFFFF != ...)) */
    } /* if (MUE_RESULT_OK == result) */
    if (MUE_RESULT_OK == result)
    {
        for (i=0; i<mue_pd_port_size_max; i++)
        {
            if (pd_port_data_get[i] != pd_port_data_put[i])
            {
                MUE_OSL_PRINTF("\nERROR:\n");
                MUE_OSL_PRINTF("- mue_pd_16f_get_port_data(G=0x47)\n");
                MUE_OSL_PRINTF("  -> port_handle  =0x%04X\n",  \
                    pd_port_handle);
                MUE_OSL_PRINTF("  <- result       =%d\n",      \
                    (ENUM16)result);
                MUE_OSL_PRINTF("  <- port_status  =0x%02X\n",  \
                    pd_port_status);
                MUE_OSL_PRINTF("  <- port_freshness=0x%04X\n", \
                    pd_port_freshness);
                for (line=0; line<2; line++)
                {
                    MUE_OSL_PRINTF("  <- port_data (%d)=", (line + 1));
                    for (i=0; i<16; i++)
                    {
                        MUE_OSL_PRINTF("%02X", \
                            pd_port_data_get[((line * 16) + i)]);
                        if (15 > i)
                        {
                            MUE_OSL_PRINTF(",");
                        } /* if (15 > i) */
                    } /* for (i=0; i<16; i++) */
                    MUE_OSL_PRINTF("\n");
                    MUE_OSL_PRINTF("                  (");
                    for (i=0; i<16; i++)
                    {
                        MUE_OSL_PRINTF("%02X", \
                            pd_port_data_put[((line * 16) + i)]);
                        if (15 > i)
                        {
                            MUE_OSL_PRINTF(",");
                        } /* if (15 > i) */
                    } /* for (i=0; i<16; i++) */
                    MUE_OSL_PRINTF(")\n");
                } /* for (line=0; line<2; line++) */
                result = MUE_RESULT_PARAMETER;
            } /* if (pd_port_data_get[i] != pd_port_data_put[i]) */
        } /* for (i=0; i<mue_pd_port_size_max; i++) */
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_pd_16f_get_port_data */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_16f
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_16f (void)
{
    MUE_RESULT  result;
    UNSIGNED16  loop;
    WORD8       pd_port_handle;

    result = MUE_RESULT_OK;

    /* ----------------------------------------------------------------------
     *  mue_pd_16f_put/get_port_config()
     * ----------------------------------------------------------------------
     */
    MUE_OSL_PRINTF("- mue_pd_16f_put/get_port_config()\n");
    loop = 0;
    while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result))
    {
        /* ------------------------------------------------------------------
         *  mue_pd_16f_put_port_config()
         * ------------------------------------------------------------------
         */
        MUE_OSL_PRINTF(".");
        mue_test_init_random_value((loop+1));
        pd_port_handle = 0;
        while ((pd_port_handle < mue_pd_16f_port_number_max) && \
               (MUE_RESULT_OK == result))
        {
            result = mue_test_pd_16f_put_port_config(loop, pd_port_handle);
            pd_port_handle++;
        } /* while ((pd_port_handle < ...) && (...)) */

        /* ------------------------------------------------------------------
         *  mue_pd_16f_get_port_config()
         * ------------------------------------------------------------------
         */
        if (MUE_RESULT_OK == result)
        {
            MUE_OSL_PRINTF(":");
            mue_test_init_random_value((loop+1));
            pd_port_handle = 0;
            while ((pd_port_handle < mue_pd_16f_port_number_max) && \
                    (MUE_RESULT_OK == result))
            {
                result = \
                    mue_test_pd_16f_get_port_config(loop, pd_port_handle);
                pd_port_handle++;
            } /* while ((pd_port_handle < ...) && (...)) */
        } /* if (MUE_RESULT_OK == result) */

        loop++;
    } /* while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result)) */
    MUE_OSL_PRINTF("\n");

    /* ----------------------------------------------------------------------
     *  mue_pd_16f_put/get_port_data()
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        MUE_OSL_PRINTF("- mue_pd_16f_put/get_port_data()\n");
        loop = 0;
        while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result))
        {
            /* --------------------------------------------------------------
             *  mue_pd_16f_put_port_data()
             * --------------------------------------------------------------
             */
            MUE_OSL_PRINTF(".");
            mue_test_init_random_value((loop+1));
            pd_port_handle = 0;
            while ((pd_port_handle < mue_pd_16f_port_number_max) && \
                   (MUE_RESULT_OK == result))
            {
                result = mue_test_pd_16f_put_port_data(loop, pd_port_handle);
                pd_port_handle++;
            } /* while ((pd_port_handle < ...) && (...)) */

            /* --------------------------------------------------------------
             *  mue_pd_16f_get_port_data()
             * --------------------------------------------------------------
             */
            if (MUE_RESULT_OK == result)
            {
                MUE_OSL_PRINTF(":");
                mue_test_init_random_value((loop+1));
                pd_port_handle = 0;
                while ((pd_port_handle < mue_pd_16f_port_number_max) && \
                       (MUE_RESULT_OK == result))
                {
                    result = \
                        mue_test_pd_16f_get_port_data(loop, pd_port_handle);
                    pd_port_handle++;
                } /* while ((pd_port_handle < ...) && (...)) */
            } /* if (MUE_RESULT_OK == result) */

            loop++;
        } /* while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == ...)) */
        MUE_OSL_PRINTF("\n");
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_pd_16f */

#endif /* #ifdef MUE_TEST_MD16 */


#ifdef MUE_TEST_MDFL

/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_full_put_port_config
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_full_put_port_config
(
    UNSIGNED16  loop,
    WORD16      pd_port_address
)
{
    MUE_RESULT  result;
    BITSET16    pd_port_config_put;
    BITSET16    pd_port_config_special;

    /* ----------------------------------------------------------------------
     *  mue_pd_full_put_port_config()
     * ----------------------------------------------------------------------
     */
    switch (loop)
    {
        case (0):
            pd_port_config_special = 0x0;
            break;
        case (1):
            pd_port_config_special = 0xF;
            break;
        default:
            pd_port_config_special = \
                (BITSET16)mue_test_create_random_value(0x0, 0xF);
    } /* switch (loop) */
    pd_port_config_put = \
        (BITSET16)((pd_port_config_special << 12) | pd_port_address);

    result =                        \
        mue_pd_full_put_port_config \
        (                           \
            p_bus_ctrl,             \
            pd_port_config_put      \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_full_put_port_config(H=0x48)\n");
        MUE_OSL_PRINTF("  -> port_config=0x%04X\n", pd_port_config_put);
        MUE_OSL_PRINTF("  <- result     =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */

    return(result);

} /* mue_test_pd_full_put_port_config */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_full_get_port_config
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_full_get_port_config
(
    UNSIGNED16  loop,
    WORD16      pd_port_address
)
{
    MUE_RESULT  result;
    BITSET16    pd_port_config_put;
    BITSET16    pd_port_config_get;
    BITSET16    pd_port_config_special;

    /* ----------------------------------------------------------------------
     *  mue_pd_full_get_port_config()
     * ----------------------------------------------------------------------
     */
    switch (loop)
    {
        case (0):
            pd_port_config_special = 0x0;
            break;
        case (1):
            pd_port_config_special = 0xF;
            break;
        default:
            pd_port_config_special = \
                (BITSET16)mue_test_create_random_value(0x0, 0xF);
    } /* switch (loop) */
    pd_port_config_put = \
        (BITSET16)((pd_port_config_special << 12) | pd_port_address);
    pd_port_config_get = (BITSET16)~pd_port_config_put;

    result =                        \
        mue_pd_full_get_port_config \
        (                           \
            p_bus_ctrl,             \
            pd_port_address,        \
            &pd_port_config_get     \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_full_get_port_config(I=0x49)\n");
        MUE_OSL_PRINTF("  -> port_address=0x%04X\n", pd_port_address);
        MUE_OSL_PRINTF("  <- result      =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */
    else
    {
        if (pd_port_config_get != pd_port_config_put)
        {
            MUE_OSL_PRINTF("\nERROR:\n");
            MUE_OSL_PRINTF("- mue_pd_full_get_port_config(I=0x49)\n");
            MUE_OSL_PRINTF("  -> port_address=0x%04X\n", pd_port_address);
            MUE_OSL_PRINTF("  <- result      =%d\n", (ENUM16)result);
            MUE_OSL_PRINTF("  <- port_config =0x%04X (0x%04X)\n", \
                pd_port_config_get, pd_port_config_put);
            result = MUE_RESULT_PARAMETER;
        } /* if (pd_port_config_get != pd_port_config_put) */
    } /* else */

    return(result);

} /* mue_test_pd_full_get_port_config */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_full_put_port_data
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_full_put_port_data
(
    UNSIGNED16  loop,
    WORD16      pd_port_address
)
{
    MUE_RESULT  result;
    UNSIGNED16  i;
    WORD8       pd_port_data_put[MUE_PD_PORT_SIZE_MAX];
    WORD8       pd_port_data_byte;

    /* ----------------------------------------------------------------------
     *  mue_pd_full_put_port_data()
     * ----------------------------------------------------------------------
     */
    for (i=0; i<mue_pd_port_size_max; i++)
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
                pd_port_data_byte = \
                    (WORD8)mue_test_create_random_value(0x00, 0xFF);
        } /* switch (loop) */
        pd_port_data_put[i] = pd_port_data_byte;
    } /* for (i=0; i<mue_pd_port_size_max; i++) */

    result =                        \
        mue_pd_full_put_port_data   \
        (                           \
            p_bus_ctrl,             \
            pd_port_address,        \
            mue_pd_port_size_max,   \
            &pd_port_data_put[0]    \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_full_put_port_data(P=0x50)\n");
        MUE_OSL_PRINTF("  -> port_address=0x%04X\n", pd_port_address);
        MUE_OSL_PRINTF("  -> port_data   =");
        for (i=0; i<mue_pd_port_size_max; i++)
        {
            MUE_OSL_PRINTF("%02X", pd_port_data_put[i]);
            if (15 == i)
            {
                MUE_OSL_PRINTF("\n");
                MUE_OSL_PRINTF("                  ");
            } /* if (15 == i) */
            else
            {
                if (31 > i)
                {
                    MUE_OSL_PRINTF(",");
                } /* if (31 > i) */
                else
                {
                    MUE_OSL_PRINTF("\n");
                } /* else */
            } /* else */
        } /* for (i=0; i<mue_pd_port_size_max; i++) */
        MUE_OSL_PRINTF("  <- result      =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */

    return(result);

} /* mue_test_pd_full_put_port_data */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_full_get_port_data
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_full_get_port_data
(
    UNSIGNED16  loop,
    WORD16      pd_port_address
)
{
    MUE_RESULT  result;
    UNSIGNED16  i;
    UNSIGNED16  line;
    BITSET8     pd_port_status;
    UNSIGNED16  pd_port_freshness;
    WORD8       pd_port_data_put[MUE_PD_PORT_SIZE_MAX];
    WORD8       pd_port_data_get[MUE_PD_PORT_SIZE_MAX];
    WORD8       pd_port_data_byte;

    /* ----------------------------------------------------------------------
     *  mue_pd_full_get_port_data()
     * ----------------------------------------------------------------------
     */
    pd_port_status    = 0xFF;
    pd_port_freshness = 0x0000;
    for (i=0; i<mue_pd_port_size_max; i++)
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
                pd_port_data_byte = \
                    (WORD8)mue_test_create_random_value(0x00, 0xFF);
        } /* switch (loop) */
        pd_port_data_put[i] = pd_port_data_byte;
        pd_port_data_get[i] = (WORD8)~pd_port_data_byte;
    } /* for (i=0; i<mue_pd_port_size_max; i++) */

    result =                        \
        mue_pd_full_get_port_data   \
        (                           \
            p_bus_ctrl,             \
            pd_port_address,        \
            (BOOLEAN1)FALSE,        \
            mue_pd_port_size_max,   \
            &pd_port_status,        \
            &pd_port_data_get[0],   \
            &pd_port_freshness      \
        );
    if (MUE_RESULT_OK != result)
    {
        MUE_OSL_PRINTF("\nERROR:\n");
        MUE_OSL_PRINTF("- mue_pd_full_get_port_data(G=0x47)\n");
        MUE_OSL_PRINTF("  -> port_address=0x%04X\n", pd_port_address);
        MUE_OSL_PRINTF("  <- result      =%d (0)\n", (ENUM16)result);
    } /* if (MUE_RESULT_OK != result) */
    if (MUE_RESULT_OK == result)
    {
        if ((0x00 != pd_port_status) || (0xFFFF != pd_port_freshness))
        {
            MUE_OSL_PRINTF("\nERROR:\n");
            MUE_OSL_PRINTF("- mue_pd_full_get_port_data(G=0x47)\n");
            MUE_OSL_PRINTF("  -> port_address  =0x%04X\n",          \
                pd_port_address);
            MUE_OSL_PRINTF("  <- result        =%d\n",              \
                (ENUM16)result);
            MUE_OSL_PRINTF("  <- port_status   =0x%02X (0x00)\n",   \
                pd_port_status);
            MUE_OSL_PRINTF("  <- port_freshness=0x%04X (0xFFFF)\n", \
                pd_port_freshness);
            result = MUE_RESULT_PARAMETER;
        } /* if ((0x00 != pd_port_status) || (0xFFFF != ...)) */
    } /* if (MUE_RESULT_OK == result) */
    if (MUE_RESULT_OK == result)
    {
        for (i=0; i<mue_pd_port_size_max; i++)
        {
            if (pd_port_data_get[i] != pd_port_data_put[i])
            {
                MUE_OSL_PRINTF("\nERROR:\n");
                MUE_OSL_PRINTF("- mue_pd_full_get_port_data(G=0x47)");
                MUE_OSL_PRINTF("\n");
                MUE_OSL_PRINTF("  -> port_address  =0x%04X\n", \
                    pd_port_address);
                MUE_OSL_PRINTF("  <- result        =%d\n",     \
                    (ENUM16)result);
                MUE_OSL_PRINTF("  <- port_status   =0x%02X\n", \
                    pd_port_status);
                MUE_OSL_PRINTF("  <- port_freshness=0x%04X\n", \
                    pd_port_freshness);
                for (line=0; line<2; line++)
                {
                    MUE_OSL_PRINTF("  <- port_data (%d) =", (line + 1));
                    for (i=0; i<16; i++)
                    {
                        MUE_OSL_PRINTF("%02X", \
                            pd_port_data_get[((line * 16) + i)]);
                        if (15 > i)
                        {
                            MUE_OSL_PRINTF(",");
                        } /* if (15 > i) */
                    } /* for (i=0; i<16; i++) */
                    MUE_OSL_PRINTF("\n");
                    MUE_OSL_PRINTF("                   (");
                    for (i=0; i<16; i++)
                    {
                        MUE_OSL_PRINTF("%02X", \
                            pd_port_data_put[((line * 16) + i)]);
                        if (15 > i)
                        {
                            MUE_OSL_PRINTF(",");
                        } /* if (15 > i) */
                    } /* for (i=0; i<16; i++) */
                    MUE_OSL_PRINTF(")\n");
                } /* for (line=0; line<2; line++) */
                result = MUE_RESULT_PARAMETER;
            } /* if (pd_port_data_get[i] != pd_port_data_put[i]) */
        } /* for (i=0; i<mue_pd_port_size_max; i++) */
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_pd_full_get_port_data */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test_pd_full
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
MUE_RESULT
mue_test_pd_full (void)
{
    MUE_RESULT  result;
    UNSIGNED16  loop;
    WORD16      pd_port_address;

    result = MUE_RESULT_OK;

    /* ----------------------------------------------------------------------
     *  mue_pd_full_put/get_port_config()
     * ----------------------------------------------------------------------
     */
    MUE_OSL_PRINTF("- mue_pd_full_put/get_port_config()\n");
    loop = 0;
    while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result))
    {
        /* ------------------------------------------------------------------
         *  mue_pd_full_put_port_config()
         * ------------------------------------------------------------------
         */
        MUE_OSL_PRINTF(".");
        mue_test_init_random_value((loop+1));
        pd_port_address = 0;
        while ((pd_port_address < mue_pd_full_port_number_max) && \
               (MUE_RESULT_OK == result))
        {
            result = mue_test_pd_full_put_port_config(loop, pd_port_address);
            pd_port_address++;
        } /* while ((pd_port_address < ...) && (...)) */

        /* ------------------------------------------------------------------
         *  mue_pd_full_get_port_config()
         * ------------------------------------------------------------------
         */
        if (MUE_RESULT_OK == result)
        {
            MUE_OSL_PRINTF(":");
            mue_test_init_random_value((loop+1));
            pd_port_address = 0;
            while ((pd_port_address < mue_pd_full_port_number_max) && \
                   (MUE_RESULT_OK == result))
            {
                result = \
                    mue_test_pd_full_get_port_config(loop, pd_port_address);
                pd_port_address++;
            } /* while ((pd_port_address < ...) && (...)) */
        } /* if (MUE_RESULT_OK == result) */

        loop++;
    } /* while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result)) */
    MUE_OSL_PRINTF("\n");

    /* ----------------------------------------------------------------------
     *  mue_pd_full_put/get_port_data()
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        MUE_OSL_PRINTF("- mue_pd_full_put/get_port_data()\n");
        loop = 0;
        while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == result))
        {
            /* --------------------------------------------------------------
             *  mue_pd_full_put_port_data()
             * --------------------------------------------------------------
             */
            MUE_OSL_PRINTF(".");
            mue_test_init_random_value((loop+1));
            pd_port_address = 0;
            while ((pd_port_address < mue_pd_full_port_number_max) && \
                   (MUE_RESULT_OK == result))
            {
                result = \
                    mue_test_pd_full_put_port_data(loop, pd_port_address);
                pd_port_address++;
            } /* while ((pd_port_address < ...) && (...)) */

            /* --------------------------------------------------------------
             *  mue_pd_full_get_port_data()
             * --------------------------------------------------------------
             */
            if (MUE_RESULT_OK == result)
            {
                MUE_OSL_PRINTF(":");
                mue_test_init_random_value((loop+1));
                pd_port_address = 0;
                while ((pd_port_address < mue_pd_full_port_number_max) && \
                       (MUE_RESULT_OK == result))
                {
                    result =                            \
                        mue_test_pd_full_get_port_data  \
                        (                               \
                            loop,                       \
                            pd_port_address             \
                        );
                    pd_port_address++;
                } /* while ((pd_port_address < ...) && (...)) */
            } /* if (MUE_RESULT_OK == result) */

            loop++;
        } /* while ((MUE_TEST_LOOP_MAX > loop) && (MUE_RESULT_OK == ...)) */
        MUE_OSL_PRINTF("\n");
    } /* if (MUE_RESULT_OK == result) */

    return(result);

} /* mue_test_pd_full */

#endif /* #ifdef MUE_TEST_MDFL */


/* --------------------------------------------------------------------------
 *  Procedure : mue_test
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_test (void)
{
    MUE_RESULT  result;


    /* ----------------------------------------------------------------------
     *  intro message
     * ----------------------------------------------------------------------
     */
#ifdef MUE_TEST_PD16
    MUE_OSL_PRINTF("\n");
    MUE_OSL_PRINTF("Test MVB UART Emulation 'PD16' (MUE)\n");
    MUE_OSL_PRINTF("====================================\n");
#endif /* #ifdef MUE_TEST_PD16 */
#ifdef MUE_TEST_MD16
    MUE_OSL_PRINTF("\n");
    MUE_OSL_PRINTF("Test MVB UART Emulation 'MD16' (MUE)\n");
    MUE_OSL_PRINTF("====================================\n");
#endif /* #ifdef MUE_TEST_MD16 */
#ifdef MUE_TEST_MDFL
    MUE_OSL_PRINTF("\n");
    MUE_OSL_PRINTF("Test MVB UART Emulation 'MDFULL' (MUE)\n");
    MUE_OSL_PRINTF("======================================\n");
#endif /* #ifdef MUE_TEST_MDFL */


    /* ----------------------------------------------------------------------
     *  initialise random-number generator
     * ----------------------------------------------------------------------
     */
    mue_test_init_random_value(1);


    /* ----------------------------------------------------------------------
     *  initialise MVB UART Emulation
     * ----------------------------------------------------------------------
     */
    result = mue_test_init_uart();


    /* ----------------------------------------------------------------------
     *  supervision - common
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        result = mue_test_sv();
    } /* if (MUE_RESULT_OK == result) */


#ifdef MUE_TEST_MDFL
    /* ----------------------------------------------------------------------
     *  supervision - MDFL
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        result = mue_test_sv_mdfl();
    } /* if (MUE_RESULT_OK == result) */
#endif /* MUE_TEST_MDFL */


#ifdef MUE_TEST_PD16
    /* ----------------------------------------------------------------------
     *  process data - PD_16
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        result = mue_test_pd_16();
    } /* if (MUE_RESULT_OK == result) */
#endif /* MUE_TEST_PD16 */


#ifdef MUE_TEST_MD16
    /* ----------------------------------------------------------------------
     *  process data - PD_16F
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        result = mue_test_pd_16f();
    } /* if (MUE_RESULT_OK == result) */
#endif /* MUE_TEST_MD16 */


#ifdef MUE_TEST_MDFL
    /* ----------------------------------------------------------------------
     *  process data - PD_FULL
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        result = mue_test_pd_full();
    } /* if (MUE_RESULT_OK == result) */
#endif /* MUE_TEST_MDFL */


    /* ----------------------------------------------------------------------
     *  supervision - cleanup
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
        result = mue_test_sv_cleanup();
    } /* if (MUE_RESULT_OK == result) */


    /* ----------------------------------------------------------------------
     *  result message
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == result)
    {
#ifdef MUE_TEST_PD16
       MUE_OSL_PRINTF("\n");
       MUE_OSL_PRINTF("***********************************************\n");
       MUE_OSL_PRINTF("* Test of MUE 'PD16' terminates successfully! *\n");
       MUE_OSL_PRINTF("***********************************************\n");
#endif /* #ifdef MUE_TEST_PD16 */
#ifdef MUE_TEST_MD16
       MUE_OSL_PRINTF("\n");
       MUE_OSL_PRINTF("***********************************************\n");
       MUE_OSL_PRINTF("* Test of MUE 'MD16' terminates successfully! *\n");
       MUE_OSL_PRINTF("***********************************************\n");
#endif /* #ifdef MUE_TEST_MD16 */
#ifdef MUE_TEST_MDFL
       MUE_OSL_PRINTF("\n");
       MUE_OSL_PRINTF("*************************************************\n");
       MUE_OSL_PRINTF("* Test of MUE 'MDFULL' terminates successfully! *\n");
       MUE_OSL_PRINTF("*************************************************\n");
#endif /* #ifdef MUE_TEST_MDFL */
    } /* if (MUE_RESULT_OK == result) */


    return(result);

} /* mue_test */


#ifndef MUE_TEST_MAIN_OTHER

/* --------------------------------------------------------------------------
 *  Procedure : main
 * --------------------------------------------------------------------------
 */
/*int
main (void)
{
    MUE_RESULT  result;

    result = mue_test();
    if (MUE_RESULT_OK != result)
    {
        
        mue_sv_put_device_config(p_bus_ctrl, 0x0000);
    }

    TCN_MACRO_TERMINATE_MAIN(result);

    return((int)result);

}*/

#endif /* #ifndef MUE_TEST_MAIN_OTHER */


#endif /* #ifdef MUE_TEST */
