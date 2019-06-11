/* ==========================================================================
 *
 *  File      : TCN_AP.C
 *
 *  Purpose   : Application Layer Interface for Process Variables
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
 *  - TCN_AP_MUTEX_PORT   - multi-thread OSL mutex for port access,
 *                          i.e. read-modify-write
 *                          NOTE:
 *                          - if not specified, the pre-processor definition
 *                            will be defined and set to 1,
 *                            i.e. by default the OSL mutex is enabled
 *                          - disable the OSL mutex by setting the
 *                            pre-processor definition to 0
 *  - TCN_AP_TEST_VAR     - enable TCN process variables test application
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
 *  TCN Software Architecture
 * --------------------------------------------------------------------------
 */
#include <tcn_def.h>
#include <tcn_osl.h>
#include <tcn_ls.h>
#include <tcn_pd.h>
#include <tcn_lp.h>
#include <tcn_ap.h>


/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Mutex for port access (read-modify-write)
 * --------------------------------------------------------------------------
 */
#define TCN_AP_TEST_VAR  1

#ifdef TCN_AP_TEST_VAR

#   define TCN_AP_MACRO_MUTEX_LOCK()
#   define TCN_AP_MACRO_MUTEX_UNLOCK()

#else /* #ifdef TCN_AP_TEST_VAR */

#   ifndef TCN_AP_MUTEX_PORT
#       define TCN_AP_MUTEX_PORT 1 /* by default the OSL mutex is enabled */
#   endif

#   if (TCN_AP_MUTEX_PORT == 1)

#       define TCN_AP_MACRO_MUTEX_LOCK()                                    \
            pthread_mutex_lock(&tcn_ap_mutex_object_port)

#       define TCN_AP_MACRO_MUTEX_UNLOCK()                                  \
            pthread_mutex_unlock(&tcn_ap_mutex_object_port)

#   else /* #if (TCN_AP_MUTEX_PORT == 1) */

#       define TCN_AP_MACRO_MUTEX_LOCK()
#       define TCN_AP_MACRO_MUTEX_UNLOCK()

#   endif /* #else */

#endif /* #else */


/* ==========================================================================
 *
 *  General Constants and Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous Constants
 * --------------------------------------------------------------------------
 */
TCN_DECL_CONST UNSIGNED16   ap_max_traffic_store = AP_MAX_TRAFFIC_STORES;

/* --------------------------------------------------------------------------
 *  Union Type: TCN_AP_TMP32
 *
 *  Purpose   : 32-bit variable, which can be accessed by:
 *              - one 32-bit access
 *              or
 *              - two 16-bit access (i.e. array with two elements)
 * --------------------------------------------------------------------------
 */
typedef union
{
    WORD32  val32;
    WORD16  val16[2];
}   TCN_AP_TMP32;


/* ==========================================================================
 *
 *  Local Variables
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Mutex for port access (read-modify-write)
 * --------------------------------------------------------------------------
 */
#if (TCN_AP_MUTEX_PORT == 1)
    TCN_DECL_LOCAL  BOOLEAN1        tcn_ap_mutex_status_port;
    TCN_DECL_LOCAL  pthread_mutex_t tcn_ap_mutex_object_port;
#endif

/* --------------------------------------------------------------------------
 *  Test Process Variables
 * --------------------------------------------------------------------------
 */
#ifdef TCN_AP_TEST_VAR
    TCN_DECL_LOCAL  WORD8   tcn_ap_port_buffer[32];
#endif

/* --------------------------------------------------------------------------
 *  Port Buffer
 * --------------------------------------------------------------------------
 */
#ifndef TCN_AP_TEST_VAR
    TCN_DECL_LOCAL  WORD8   tcn_ap_port_buffer[128];
#endif
TCN_DECL_LOCAL  WORD8       *p_tcn_ap_port_buffer = &tcn_ap_port_buffer[0];


/* ==========================================================================
 *
 *  Initialisation
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ap_init
 *
 *  Purpose   : Initialises the process data related layers
 *              (incl. sub-layers).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_init (void);
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - This procedure has to be called at system initialisation
 *                before calling any other 'ap_xxx' procedure.
 *              - This procedure shall be called only one time.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_init (void)
{
    AP_RESULT   ap_result = AP_OK;
    LP_RESULT   lp_result = LP_OK;
    ENUM8       ts_id;

#if (TCN_AP_MUTEX_PORT == 1)
    /* initialise mutex for port access */
    if (FALSE == tcn_ap_mutex_status_port)
    {
        if (0 != pthread_mutex_init(&tcn_ap_mutex_object_port, NULL))
        {
            ap_result = AP_ERROR;
        } /* if (0 != pthread_mutex_init(&tcn_ap_mutex_object_port, NULL)) */
        else
        {
            tcn_ap_mutex_status_port = TRUE;
        } /* else */
    } /* if (FALSE == tcn_ap_mutex_status_port) */
    if (TRUE == tcn_ap_mutex_status_port)
    {
        if (0 != pthread_mutex_lock(&tcn_ap_mutex_object_port))
        {
            ap_result = AP_ERROR;
        } /* if (0 != pthread_mutex_lock(&tcn_ap_mutex_object_port)) */
        if (AP_OK == ap_result)
        {
            if (0 != pthread_mutex_unlock(&tcn_ap_mutex_object_port))
            {
                ap_result = AP_ERROR;
            } /* if (0 != pthread_mutex_unlock(&tcn_ap_mutex_object_port)) */
        } /* if (AP_OK == ap_result) */
    } /* if (TRUE == tcn_ap_mutex_status_port) */
#endif /* #if (TCN_AP_MUTEX_PORT == 1) */

    if (AP_OK == ap_result)
    {
        TCN_AP_MACRO_MUTEX_LOCK();

        ts_id = 0;
        while ((ts_id < lp_max_traffic_stores) && (LP_OK == lp_result))
        {
            lp_result = lp_init(ts_id, 0);
            /* ignore LP_UNKNOW_TS */
            if (LP_UNKNOW_TS  == lp_result)
            {
                lp_result = LP_OK;
            } /* if (LP_UNKNOW_TS  == lp_result) */
            ts_id++;
        } /* while ((ts_id < lp_max_traffic_stores) && (...)) */

        TCN_AP_MACRO_MUTEX_UNLOCK();

        ap_result = (AP_RESULT)lp_result;

    } /* if (AP_OK == ap_result) */

    return(ap_result);

} /* ap_init */


/* --------------------------------------------------------------------------
 *  Procedure : ap_show_traffic_stores
 *
 *  Purpose   : Retrieves the number of link layers supporting process data
 *              connected to this station and lists their traffic store
 *              identifiers.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_show_traffic_stores
 *              (
 *                  UNSIGNED8   *nr_of_busses,
 *                  UNSIGNED8   *link_id_list
 *              );
 *
 *  Output    : nr_of_busses - number of connected link layers supporting
 *                             process data (NOTE: the max. number of
 *                             connected link layers is 16)
 *              link_id_list - list of connected link layers supporting
 *                             process data, with at least the 'ts_id' of
 *                             each (NOTE: range of ts_id=0..15)
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - The application has to provide a buffer for the link
 *                identifier list (parameter 'link_id_list'). The size of
 *                this buffer should be 16, which is the max. number of
 *                connected link layers.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_show_traffic_stores
(
    UNSIGNED8   *nr_of_busses,
    UNSIGNED8   *link_id_list
)
{
    AP_RESULT   ap_result       = AP_OK;
    LS_RESULT   ls_result;
    BITSET16    pd_link_set;
    UNSIGNED8   *p_link_id_list;
    ENUM8       ts_id;

    *nr_of_busses  = 0;

    ls_result = ls_read_link_set(NULL, &pd_link_set, NULL);
    if (LS_OK != ls_result)
    {
        ap_result = AP_ERROR;
    } /* if (LS_OK != ls_result) */

    if (AP_OK == ap_result)
    {
        p_link_id_list = link_id_list;
        for (ts_id=0; ts_id<ap_max_traffic_store; ts_id++)
        {
            if (pd_link_set & 0x0001)
            {
                *p_link_id_list = (UNSIGNED8)ts_id;
                (*nr_of_busses)++;
                p_link_id_list++;
            } /* if (pd_link_set & 0x0001) */

            pd_link_set = (BITSET16)(pd_link_set >> 1);

        } /* for (ts_id=0; ts_id<ap_max_traffic_store; ts_id++) */
    } /* if (AP_OK == ap_result) */

    return(ap_result);

} /* ap_show_traffic_stores */


/* ==========================================================================
 *
 *  Ports Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ap_ts_config
 *
 *  Purpose   : Configures an empty traffic store and sets up the freshness
 *              supervision interval.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_ts_config
 *              (
 *                  ENUM8       ts_id,
 *                  UNSIGNED16  fsi
 *              );
 *
 *  Input     : ts_id - traffic store identifier (0..15)
 *              fsi   - freshness supervision interval
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 *              - This procedure has to be called at system initialisation
 *                before calling any other 'ap_xxx' procedure referencing
 *                the same traffic store (parameter 'ts_id').
 *              - This procedure shall be called only one time for each
 *                traffic store.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_ts_config
(
    ENUM8       ts_id,
    UNSIGNED16  fsi
)
{
    LP_RESULT   lp_result;

    TCN_AP_MACRO_MUTEX_LOCK();

    lp_result = lp_init(ts_id, fsi);

    TCN_AP_MACRO_MUTEX_UNLOCK();

    return((AP_RESULT)lp_result);

} /* ap_ts_config */


/* --------------------------------------------------------------------------
 *  Procedure : ap_port_manage
 *
 *  Purpose   : Manages a port located in a traffic store (e.g. configure,
 *              delete, modify, retrieve status).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_port_manage
 *              (
 *                  ENUM8       ts_id,
 *                  WORD16      port_address,
 *                  ENUM16      command,
 *                  PD_PRT_ATTR *prt_attr
 *              );
 *
 *  Input     : ts_id        - traffic store identifier (0..15)
 *              port_address - port address (0..4095)
 *              command      - port manage command (any PD_PRT_CMD)
 *  In-/Output: prt_attr     - port attributes of structured type
 *                             'PD_PRT_ATTR'
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - If command PD_PRT_CMD_CONFIG is used, then the port value
 *                will be cleared (set all bytes to 0x00) and the freshness
 *                timer of the port will be set to the largest value (65535).
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_port_manage
(
    ENUM8       ts_id,
    WORD16      port_address,
    ENUM16      command,
    PD_PRT_ATTR *prt_attr
)
{
    LP_RESULT   lp_result;

    TCN_AP_MACRO_MUTEX_LOCK();

    lp_result = lp_manage(ts_id, port_address, command, prt_attr);

    TCN_AP_MACRO_MUTEX_UNLOCK();

    return((AP_RESULT)lp_result);

} /* ap_port_manage */


/* ==========================================================================
 *
 *  Datasets Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ap_put_dataset
 *
 *  Purpose   : Copies a dataset from the application to a port in the
 *              traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_put_dataset
 *              (
 *                  DS_NAME     *dataset,
 *                  void        *p_value
 *              );
 *
 *  Input     : dataset - DS_NAME of the dataset to be published
 *              p_value - pointer to a memory buffer of the application
 *                        where the dataset value is copied from
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - The previous value of the dataset in the port is
 *                overwritten.
 *              - A port in a traffic store hold its dataset as it will be
 *                transmitted over the bus.
 *              - A dataset (parameter 'p_value') is handled as an array of
 *                octets (e.g. WORD8).
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_put_dataset
(
    DS_NAME     *dataset,
    void        *p_value
)
{
    LP_RESULT   lp_result;

    TCN_AP_MACRO_MUTEX_LOCK();

    lp_result =                                 \
        lp_put_dataset                          \
        (                                       \
            (ENUM8)dataset->traffic_store_id,   \
            (WORD16)dataset->port_address,      \
            p_value                             \
        );

    TCN_AP_MACRO_MUTEX_UNLOCK();

    return((AP_RESULT)lp_result);

} /* ap_put_dataset */


/* --------------------------------------------------------------------------
 *  Procedure : ap_get_dataset
 *
 *  Purpose   : Copies a dataset and its freshness timer from a port in the
 *              traffic store to the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_get_dataset
 *              (
 *                  DS_NAME     *dataset,
 *                  void        *p_value,
 *                  void        *p_fresh
 *              );
 *
 *  Input     : dataset - DS_NAME of the dataset to be received
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Input     : p_value - pointer to a memory buffer of the application
 *                        where the dataset value is copied to
 *              p_fresh - pointer to a memory buffer of the application
 *                        where the freshness timer is copied to
 *
 *  Remarks   : - A port in a traffic store hold its dataset as it will be
 *                transmitted over the bus.
 *              - A dataset (parameter 'p_value') is handled as an array of
 *                octets (e.g. WORD8).
 *              - A freshness timer (parameter 'p_fresh') is handled as
 *                type UNSIGNED16.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_get_dataset
(
    DS_NAME     *dataset,
    void        *p_value,
    void        *p_fresh
)
{
    LP_RESULT   lp_result;

    TCN_AP_MACRO_MUTEX_LOCK();

    lp_result =                                 \
        lp_get_dataset                          \
        (                                       \
            (ENUM8)dataset->traffic_store_id,   \
            (WORD16)dataset->port_address,      \
            p_value,                            \
            p_fresh                             \
        );

    TCN_AP_MACRO_MUTEX_UNLOCK();

    return((AP_RESULT)lp_result);

} /* ap_get_dataset */


/* ==========================================================================
 *
 *  Variables Interface (individual access)
 *
 * ==========================================================================
 */

#ifdef TCN_CHECK_PARAMETER

/* --------------------------------------------------------------------------
 *  Procedure : tcn_ap_check_parameter_pv_name
 *
 *  Purpose   : Checks the parameters of a PV_NAME.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              AP_RESULT
 *              tcn_ap_check_parameter_pv_name
 *              (
 *                  PV_NAME     *pv_name,
 *                  void        *p_value,
 *                  void        *p_check
 *              );
 *
 *  Input     : pv_name - PV_NAME of the process variable
 *              p_value - pointer to a memory buffer of the application
 *                        where the process variable value is loacted
 *              p_check - pointer to a memory buffer of the application
 *                        where the check variable value is located
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - Used only during development and debugging
 *                (requires pre-processor definition TCN_CHECK_PARAMETER).
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
AP_RESULT
tcn_ap_check_parameter_pv_name
(
    PV_NAME     *pv_name,
    void        *p_value,
    void        *p_check
)
{
    AP_RESULT   ap_result        = AP_OK;
    BOOLEAN1    done             = FALSE;
#ifndef TCN_AP_TEST_VAR
    LP_RESULT   lp_result;
    PD_PRT_ATTR prt_attr;
#endif /* #ifndef TCN_AP_TEST_VAR */
    UNSIGNED8   prt_size         = 0;
    UNSIGNED8   var_size         = 0;
    UNSIGNED8   var_type         = 0;
    UNSIGNED8   var_octet_offset = 0;
    UNSIGNED8   var_bit_number   = 0;

    if (NULL == p_value)
    {
        ap_result = AP_ERROR;
    } /* if (NULL == p_value) */


    /* ----------------------------------------------------------------------
     *  get port size
     * ----------------------------------------------------------------------
     */
#ifdef TCN_AP_TEST_VAR
    prt_size = 32;
#else /* #ifdef TCN_AP_TEST_VAR */
    lp_result =                                 \
        lp_manage                               \
        (                                       \
            (ENUM8)pv_name->traffic_store_id,   \
            (WORD16)pv_name->port_address,      \
            (ENUM16)PD_PRT_CMD_STATUS,          \
            &prt_attr                           \
        );
    if (LP_OK != lp_result)
    {
        ap_result = (AP_RESULT)lp_result;
    } /* if (LP_OK != lp_result) */
    else
    {
        prt_size = prt_attr.port_size;
    } /* else */
#endif /* #else */


    /* ----------------------------------------------------------------------
     *  check variable
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        if (NULL == p_check)
        {
            /* chk_octet_offset - 7-bit (0x7F) */
            /* chk_bit_number   - 3-bit (0x07) */
            if ((0x7F != pv_name->chk_octet_offset) && \
                (0x07 != pv_name->chk_bit_number))
            {
                ap_result = AP_RANGE;
            } /* if ((0x7F != pv_name->chk_octet_offset) && (...)) */
        } /* if (NULL == p_check) */
        else
        {
            if ((pv_name->chk_octet_offset > (UNSIGNED8)(prt_size - 1)) || \
                (6 < pv_name->chk_bit_number)                           || \
                (0 != (pv_name->chk_bit_number % 2)))
            {
                ap_result = AP_RANGE;
            } /* if ((...)) */
        } /* else */
    } /* if (AP_OK == ap_result) */


    if (AP_OK == ap_result)
    {
        var_size         = (UNSIGNED8)pv_name->var_size;
        var_type         = \
            (UNSIGNED8)(pv_name->var_type & (UNSIGNED8)~AP_VAR_TYPE_FLAG_LE);
        var_octet_offset = (UNSIGNED8)pv_name->var_octet_offset;
        var_bit_number   = (UNSIGNED8)pv_name->var_bit_number;
    } /* if (AP_OK == ap_result) */


    /* ----------------------------------------------------------------------
     *  structured variable types
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        switch (var_type)
        {
            /* --------------------------------------------------------------
             *  AP_VAR_TYPE_ARRAY_WORD8_ODD
             * --------------------------------------------------------------
             */
            case (AP_VAR_TYPE_ARRAY_WORD8_ODD):
                if ((prt_size < ((pv_name->var_size * 2) + 1))      || \
                    (var_octet_offset >                                \
                        (prt_size - ((pv_name->var_size * 2) + 1))) || \
                    (0 != var_bit_number))
                {
                    ap_result = AP_RANGE;
                } /* if ((...)) */
                else
                {
                    done = TRUE;
                } /* else */
                break;

            /* --------------------------------------------------------------
             *  AP_VAR_TYPE_ARRAY_WORD8_EVEN
             * --------------------------------------------------------------
             */
            case (AP_VAR_TYPE_ARRAY_WORD8_EVEN):
                if ((prt_size < ((pv_name->var_size * 2) + 2))      || \
                    (var_octet_offset >                                \
                        (prt_size - ((pv_name->var_size * 2) + 2))) || \
                    (0 != var_bit_number))
                {
                    ap_result = AP_RANGE;
                } /* if ((...)) */
                else
                {
                    done = TRUE;
                } /* else */
                break;

            /* --------------------------------------------------------------
             *  AP_VAR_TYPE_ARRAY_UNSIGNED16
             *  AP_VAR_TYPE_ARRAY_INTEGER16
             * --------------------------------------------------------------
             */
            case (AP_VAR_TYPE_ARRAY_UNSIGNED16):
            case (AP_VAR_TYPE_ARRAY_INTEGER16):
                if ((prt_size < (pv_name->var_size + 1))      || \
                    (var_octet_offset >                          \
                        (prt_size - (pv_name->var_size + 1))) || \
                    (0 != (var_octet_offset % 2))             || \
                    (0 != var_bit_number))
                {
                    ap_result = AP_RANGE;
                } /* if ((...)) */
                else
                {
                    done = TRUE;
                } /* else */
                break;

            /* --------------------------------------------------------------
             *  AP_VAR_TYPE_ARRAY_UNSIGNED32
             *  AP_VAR_TYPE_ARRAY_INTEGER32
             * --------------------------------------------------------------
             */
            case (AP_VAR_TYPE_ARRAY_UNSIGNED32):
            case (AP_VAR_TYPE_ARRAY_INTEGER32):
                if ((prt_size < ((pv_name->var_size + 1) / 2))      || \
                    (var_octet_offset >                                \
                        (prt_size - ((pv_name->var_size + 1) / 2))) || \
                    (0 != (var_octet_offset % 4))                   || \
                    (0 != var_bit_number))
                {
                    ap_result = AP_RANGE;
                } /* if ((...)) */
                else
                {
                    done = TRUE;
                } /* else */
                break;

            default:
                ;

        } /* switch (var_type) */
    } /* if (AP_OK == ap_result) */


    /* ----------------------------------------------------------------------
     *  primitive variable types
     * ----------------------------------------------------------------------
     */
    if ((AP_OK == ap_result) && (FALSE == done))
    {
        switch (var_size)
        {
            /* --------------------------------------------------------------
             *  primitive variable types with size less than one 16-bit word
             * --------------------------------------------------------------
             */
            case (AP_VAR_SIZE_8):
                switch (var_type)
                {
                    /* ------------------------------------------------------
                     *  AP_VAR_TYPE_BOOLEAN1
                     * ------------------------------------------------------
                     */
                    case (AP_VAR_TYPE_BOOLEAN1):
                        if ((var_octet_offset > (prt_size - 1)) || \
                            (7 < var_bit_number))
                        {
                            ap_result = AP_RANGE;
                        } /* if ((...)) */
                        else
                        {
                            done = TRUE;
                        } /* else */
                        break;

                    /* --------------------------------------------------------------
                     *  AP_VAR_TYPE_ANTIVALENT2
                     * --------------------------------------------------------------
                     */
                    case (AP_VAR_TYPE_ANTIVALENT2):
                        if ((var_octet_offset > (prt_size - 1)) || \
                            (6 < var_bit_number)                || \
                            (0 != (var_bit_number % 2)))
                        {
                            ap_result = AP_RANGE;
                        } /* if ((...)) */
                        else
                        {
                            done = TRUE;
                        } /* else */
                        break;

                    /* --------------------------------------------------------------
                     *  AP_VAR_TYPE_BCD4, AP_VAR_TYPE_ENUM4
                     * --------------------------------------------------------------
                     */
                    case (AP_VAR_TYPE_BCD4):
                    /* case (AP_VAR_TYPE_ENUM4): */
                        if ((var_octet_offset > (prt_size - 1)) || \
                            (4 < var_bit_number)                || \
                            (0 != (var_bit_number % 4)))
                        {
                           ap_result = AP_RANGE;
                        } /* if ((...)) */
                        else
                        {
                            done = TRUE;
                        } /* else */
                        break;

                    default:
                        if ((var_octet_offset > (prt_size - 1)) || \
                            (0 != var_bit_number))
                        {
                            ap_result = AP_RANGE;
                        } /* if ((...)) */
                        else
                        {
                            done = TRUE;
                        } /* else */
                } /* switch (var_type) */
                break;

            /* --------------------------------------------------------------
             *  primitive variable types with size of one 16-bit word
             * --------------------------------------------------------------
             */
            case (AP_VAR_SIZE_16):
                if ((2 > prt_size)                      || \
                    (var_octet_offset > (prt_size - 2)) || \
                    (0 != (var_octet_offset % 2))       || \
                    (0 != var_bit_number))
                {
                    ap_result = AP_RANGE;
                } /* if ((...)) */
                else
                {
                    done = TRUE;
                } /* else */
                break;

            /* ------------------------------------------------------------------
             *  primitive variable types with size of two 16-bit word
             * ------------------------------------------------------------------
             */
            case (AP_VAR_SIZE_32):
                if ((4 > prt_size)                      || \
                    (var_octet_offset > (prt_size - 4)) || \
                    (0 != (var_octet_offset % 4))       || \
                    (0 != var_bit_number))
                {
                    ap_result = AP_RANGE;
                } /* if ((...)) */
                else
                {
                    done = TRUE;
                } /* else */
                break;

            /* ------------------------------------------------------------------
             *  primitive variable types with size of three 16-bit word
             * ------------------------------------------------------------------
             */
            case (AP_VAR_SIZE_48):
                if ((6 > prt_size)                      || \
                    (var_octet_offset > (prt_size - 6)) || \
                    (0 != (var_octet_offset % 6))       || \
                    (0 != var_bit_number))
                {
                    ap_result = AP_RANGE;
                } /* if ((...)) */
                else
                {
                    done = TRUE;
                } /* else */
                break;

            /* ------------------------------------------------------------------
             *  primitive variable types with size of four 16-bit word
             * ------------------------------------------------------------------
             */
            case (AP_VAR_SIZE_64):
                if ((8 > prt_size)                      || \
                    (var_octet_offset > (prt_size - 8)) || \
                    (0 != (var_octet_offset % 8))       || \
                    (0 != var_bit_number))
                {
                    ap_result = AP_RANGE;
                } /* if ((...)) */
                else
                {
                    done = TRUE;
                } /* else */
                break;

            default:
                ap_result = AP_DATA_TYPE;

        } /* switch (var_size) */
    } /* if ((AP_OK == ap_result) && (FALSE == done)) */

    return(ap_result);

} /* tcn_ap_check_parameter_pv_name */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_ap_check_parameter_pv
 *
 *  Purpose   : Checks the parameters of a PV (process variable).
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              AP_RESULT
 *              tcn_ap_check_parameter_pv
 *              (
 *                  UNSIGNED8   buffer_size,
 *                  UNSIGNED8   derived_type,
 *                  UNSIGNED8   array_count,
 *                  UNSIGNED8   octet_offset,
 *                  UNSIGNED8   bit_number
 *              );
 *
 *  Input     : buffer_size  - size of the buffer (number of bytes)
 *              derived_type - any AP_DERIVED_TYPE
 *              array_count  - number of elements in the array
 *              octet_offset - offset in number of octets of
 *                             a process variable
 *              bit_number   - bit number of a process variable
 *                             smaller than one octet
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - Used only during development and debugging
 *                (requires pre-processor definition TCN_CHECK_PARAMETER).
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
AP_RESULT
tcn_ap_check_parameter_pv
(
    UNSIGNED8   buffer_size,
    UNSIGNED8   derived_type,
    UNSIGNED8   array_count,
    UNSIGNED8   octet_offset,
    UNSIGNED8   bit_number
)
{
    AP_RESULT   ap_result   = AP_OK;
    AP_VAR_SIZE ap_var_size;

    /* determine variable size (clear LE flag) */
    ap_var_size = (AP_VAR_SIZE) \
        ((derived_type & (UNSIGNED8)~AP_DERIVED_TYPE_FLAG_LE) >> 4);

    switch (ap_var_size)
    {
        /* ------------------------------------------------------------------
         *  primitive variable types with size less than one 16-bit word
         * ------------------------------------------------------------------
         */
        case (AP_VAR_SIZE_8):
            switch (derived_type)
            {
                case (AP_DERIVED_TYPE_BOOLEAN1):
                    if ((1 != array_count)                 || \
                        (octet_offset > (buffer_size - 1)) || \
                        (7 < bit_number))
                    {
                        ap_result = AP_RANGE;
                    } /* if ((...)) */
                    break;

                case (AP_DERIVED_TYPE_ANTIVALENT2):
                    if ((1 != array_count)                 || \
                        (octet_offset > (buffer_size - 1)) || \
                        (6 < bit_number)                   || \
                        (0 != (bit_number % 2)))
                    {
                        ap_result = AP_RANGE;
                    } /* if ((...)) */
                    break;

                case (AP_DERIVED_TYPE_BCD4):
                /* case (AP_DERIVED_TYPE_ENUM4): */
                    if ((1 != array_count)                 || \
                        (octet_offset > (buffer_size - 1)) || \
                        (4 < bit_number)                   || \
                        (0 != (bit_number % 4)))
                    {
                       ap_result = AP_RANGE;
                    } /* if ((...)) */
                    break;

                default:
                    if ((0 == array_count)                           || \
                        (octet_offset > (buffer_size - array_count)) || \
                        (0 != bit_number))
                    {
                        ap_result = AP_RANGE;
                    } /* if ((...)) */
            } /* switch (derived_type) */
            break;

        /* ------------------------------------------------------------------
         *  primitive variable types with size of one 16-bit word
         * ------------------------------------------------------------------
         */
        case (AP_VAR_SIZE_16):
            if ((2 > buffer_size)                                  || \
                (0 == array_count)                                 || \
                (octet_offset > (buffer_size - (array_count * 2))) || \
                (0 != (octet_offset % 2))                          || \
                (0 != bit_number))
            {
                ap_result = AP_RANGE;
            } /* if ((...)) */
            break;

        /* ------------------------------------------------------------------
         *  primitive variable types with size of two 16-bit word
         * ------------------------------------------------------------------
         */
        case (AP_VAR_SIZE_32):
            if ((4 > buffer_size)                                  || \
                (0 == array_count)                                 || \
                (octet_offset > (buffer_size - (array_count * 4))) || \
                (0 != (octet_offset % 4))                          || \
                (0 != bit_number))
            {
                ap_result = AP_RANGE;
            } /* if ((...)) */
            break;

        /* ------------------------------------------------------------------
         *  primitive variable types with size of three 16-bit word
         * ------------------------------------------------------------------
         */
        case (AP_VAR_SIZE_48):
            if ((6 > buffer_size)                                  || \
                (0 == array_count)                                 || \
                (octet_offset > (buffer_size - (array_count * 6))) || \
                (0 != (octet_offset % 6))                          || \
                (0 != bit_number))
            {
                ap_result = AP_RANGE;
            } /* if ((...)) */
            break;

        /* ------------------------------------------------------------------
         *  primitive variable types with size of four 16-bit word
         * ------------------------------------------------------------------
         */
        case (AP_VAR_SIZE_64):
            if ((8 > buffer_size)                                  || \
                (0 == array_count)                                 || \
                (octet_offset > (buffer_size - (array_count * 8))) || \
                (0 != (octet_offset % 8))                          || \
                (0 != bit_number))
            {
                ap_result = AP_RANGE;
            } /* if ((...)) */
            break;

        default:
            ap_result = AP_DATA_TYPE;

    } /* switch (ap_var_size) */

    return(ap_result);

} /* tcn_ap_check_parameter_pv */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_ap_check_parameter_pv_list
 *
 *  Purpose   : Checks the parameters of a PV_LIST.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              AP_RESULT
 *              tcn_ap_check_parameter_pv_list
 *              (
 *                  DS_NAME     dataset,
 *                  PV_LIST     *pv_list
 *              );
 *
 *  Input     : dataset - DS_NAME of the dataset
 *              PV_LIST - PV_LIST of the process variable
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - Used only during development and debugging
 *                (requires pre-processor definition TCN_CHECK_PARAMETER).
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
AP_RESULT
tcn_ap_check_parameter_pv_list
(
    DS_NAME     dataset,
    PV_LIST     *pv_list
)
{
    AP_RESULT   ap_result = AP_OK;
#ifndef TCN_AP_TEST_VAR
    LP_RESULT   lp_result;
    PD_PRT_ATTR prt_attr;
#endif /* #ifndef TCN_AP_TEST_VAR */
    UNSIGNED8   prt_size  = 0;


#ifdef TCN_AP_TEST_VAR
    /* avoid warnings */
    dataset = dataset;
#endif /* #ifdef TCN_AP_TEST_VAR */


    if (NULL == pv_list->p_variable)
    {
        ap_result = AP_ERROR;
    } /* if (NULL == pv_list->p_variable) */


    /* ----------------------------------------------------------------------
     *  get port size
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
#ifdef TCN_AP_TEST_VAR
        prt_size = 32;
#else /* #ifdef TCN_AP_TEST_VAR */
        lp_result =                                 \
            lp_manage                               \
            (                                       \
                (ENUM8)dataset.traffic_store_id,    \
                (WORD16)dataset.port_address,       \
                (ENUM16)PD_PRT_CMD_STATUS,          \
                &prt_attr                           \
            );
        if (LP_OK != lp_result)
        {
            ap_result = (AP_RESULT)lp_result;
        } /* if (LP_OK != lp_result) */
        else
        {
            prt_size = prt_attr.port_size;
        } /* else */
#endif /* #else */
    } /* if (AP_OK == ap_result) */


    if (AP_OK == ap_result)
    {
        ap_result =                     \
            tcn_ap_check_parameter_pv   \
            (                           \
                prt_size,               \
                pv_list->derived_type,  \
                pv_list->array_count,   \
                pv_list->octet_offset,  \
                pv_list->bit_number     \
            );
    } /* if (AP_OK == ap_result) */

    return(ap_result);

} /* tcn_ap_check_parameter_pv_list */

#endif /* #ifdef TCN_CHECK_PARAMETER */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_ap_put_variable
 *
 *  Purpose   : Copies a process variable from the application to
 *              a traffic store.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              AP_RESULT
 *              tcn_ap_put_variable
 *              (
 *                  void        *p_value,
 *                  void        *p_octet,
 *                  UNSIGNED8   derived_type,
 *                  UNSIGNED8   array_count,
 *                  UNSIGNED8   bit_number
 *              );
 *
 *  Input     : p_value      - pointer to a memory buffer of the application
 *                             where the process variable value is copied
 *                             from
 *              p_octet      - pointer to an octet of a port buffer
 *              derived_type - any AP_DERIVED_TYPE
 *              array_count  - number of elements in the array
 *              bit_number   - bit number of a process variable smaller
 *                             than one octet (only for variables of type
 *                             BOOLEAN1, ANTIVALENT2, BCD4, ENUM4)
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - The previous value of the process variable dataset is
 *                overwritten.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
AP_RESULT
tcn_ap_put_variable
(
    void        *p_value,
    void        *p_octet,
    UNSIGNED8   derived_type,
    UNSIGNED8   array_count,
    UNSIGNED8   bit_number
)
{
    AP_RESULT       ap_result      = AP_OK;
    BOOLEAN1        bFlagLE;
    UNSIGNED8       counter;
    TCN_AP_TMP32    tmp32;
    TIMEDATE48      *p_tmp_value48;
    WORD8           *p_tmp_value8;
    WORD16          *p_tmp_value16;
    WORD32          *p_tmp_value32;
    WORD64          *p_tmp_value64;
    WORD8           *p_tmp_octet8;
    WORD16          *p_tmp_octet16;
    WORD32          *p_tmp_octet32;
    WORD64          *p_tmp_octet64;

    /* determine LE flag */
    if (derived_type & AP_DERIVED_TYPE_FLAG_LE)
    {
        bFlagLE = TRUE;
    } /* if (derived_type & AP_DERIVED_TYPE_FLAG_LE) */
    else
    {
        bFlagLE = FALSE;
    } /* else */

    /* clear LE flag */
    derived_type = \
        (UNSIGNED8)(derived_type & (UNSIGNED8)~AP_DERIVED_TYPE_FLAG_LE);

    switch (derived_type)
    {
        case (AP_DERIVED_TYPE_BOOLEAN1):
            *(BOOLEAN1*)p_octet = (BOOLEAN1)                                \
            (                                                               \
                (BOOLEAN1)                                                  \
                ( /* clear bit positions of variable */                     \
                    *(BOOLEAN1*)p_octet &                                   \
                    (BOOLEAN1)~(0x1 << bit_number)                          \
                )                                                         | \
                (BOOLEAN1)                                                  \
                ( /* set bit positions of variable */                       \
                    (BOOLEAN1)                                              \
                        ((*(BOOLEAN1*)p_value & 0x1) << bit_number) &       \
                    (BOOLEAN1)(0x1 << bit_number)                           \
                )                                                           \
            );
            break;

        case (AP_DERIVED_TYPE_ANTIVALENT2):
            *(ANTIVALENT2*)p_octet = (ANTIVALENT2)                          \
            (                                                               \
                (ANTIVALENT2)                                               \
                ( /* clear bit positions of variable */                     \
                    *(ANTIVALENT2*)p_octet &                                \
                    (ANTIVALENT2)~(0x3 << bit_number)                       \
                )                                                         | \
                (ANTIVALENT2)                                               \
                ( /* set bit positions of variable */                       \
                    (ANTIVALENT2)                                           \
                        ((*(ANTIVALENT2*)p_value & 0x3) << bit_number)    & \
                    (ANTIVALENT2)(0x3 << bit_number)                        \
                )                                                           \
            );
            break;

        case (AP_DERIVED_TYPE_BCD4):
        /* case (AP_DERIVED_TYPE_ENUM4): */
            *(BCD4*)p_octet = (BCD4)                                        \
            (                                                               \
                (BCD4)                                                      \
                ( /* clear bit positions of variable */                     \
                    *(BCD4*)p_octet &                                       \
                    (BCD4)~(0xF << bit_number)                              \
                )                                                         | \
                (BCD4)                                                      \
                ( /* set bit positions of variable */                       \
                    (BCD4)((*(BCD4*)p_value & 0xF) << bit_number) &         \
                    (BCD4)(0xF << bit_number)                               \
                )                                                           \
            );
            break;

        case (AP_DERIVED_TYPE_TIMEDATE48):
            p_tmp_value48 = (TIMEDATE48*)p_value;
            p_tmp_octet16 = (WORD16*)p_octet;
            for (counter=0; counter<array_count; counter++)
            {
                if (bFlagLE)
                {
                    /* element 'seconds' */
                    tmp32.val32 = (WORD32)p_tmp_value48->seconds;
                    TCN_MACRO_CONVERT_CPU_TO_LE32(&tmp32.val32);
                    *p_tmp_octet16++ = tmp32.val16[0];
                    *p_tmp_octet16++ = tmp32.val16[1];

                    /* element 'ticks' */
                    *p_tmp_octet16 = (WORD16)p_tmp_value48->ticks;
                    TCN_MACRO_CONVERT_CPU_TO_LE16(p_tmp_octet16);
                    p_tmp_octet16++;
                } /* if (bFlagLE) */
                else
                {
                    tmp32.val32 = (WORD32)p_tmp_value48->seconds;
                    TCN_MACRO_CONVERT_CPU_TO_BE32(&tmp32.val32);
                    *p_tmp_octet16++ = tmp32.val16[0];
                    *p_tmp_octet16++ = tmp32.val16[1];

                    /* element 'ticks' */
                    *p_tmp_octet16 = (WORD16)p_tmp_value48->ticks;
                    TCN_MACRO_CONVERT_CPU_TO_BE16(p_tmp_octet16);
                    p_tmp_octet16++;
                } /*  else */
                p_tmp_value48++;
            } /* for (counter=0; counter<array_count; counter++) */
            break;

        default:
            switch ((derived_type >> 4)) /* AP_VAR_SIZE */
            {
                case (AP_VAR_SIZE_8):
                    p_tmp_value8 = (WORD8*)p_value;
                    p_tmp_octet8 = (WORD8*)p_octet;
                    for (counter=0; counter<array_count; counter++)
                    {
                        *p_tmp_octet8++ = *p_tmp_value8++;
                    } /* for (counter=0; counter<array_count; counter++) */
                    break;

                case (AP_VAR_SIZE_16):
                    p_tmp_value16 = (WORD16*)p_value;
                    p_tmp_octet16 = (WORD16*)p_octet;
                    for (counter=0; counter<array_count; counter++)
                    {
                        *p_tmp_octet16 = *p_tmp_value16++;
                        if (bFlagLE)
                        {
                            TCN_MACRO_CONVERT_CPU_TO_LE16(p_tmp_octet16);
                        } /* if (bFlagLE) */
                        else
                        {
                            TCN_MACRO_CONVERT_CPU_TO_BE16(p_tmp_octet16);
                        } /* else */
                        p_tmp_octet16++;
                    } /* for (counter=0; counter<array_count; counter++) */
                    break;

                case (AP_VAR_SIZE_32):
                    p_tmp_value32 = (WORD32*)p_value;
                    p_tmp_octet32 = (WORD32*)p_octet;
                    for (counter=0; counter<array_count; counter++)
                    {
                        *p_tmp_octet32 = *p_tmp_value32++;
                        if (bFlagLE)
                        {
                            TCN_MACRO_CONVERT_CPU_TO_LE32(p_tmp_octet32);
                        } /* if (bFlagLE) */
                        else
                        {
                            TCN_MACRO_CONVERT_CPU_TO_BE32(p_tmp_octet32);
                        } /* else */
                        p_tmp_octet32++;
                    } /* for (counter=0; counter<array_count; counter++) */
                    break;

                case (AP_VAR_SIZE_64):
                    p_tmp_value64 = (WORD64*)p_value;
                    p_tmp_octet64 = (WORD64*)p_octet;
                    for (counter=0; counter<array_count; counter++)
                    {
                        *p_tmp_octet64 = *p_tmp_value64++;
                        if (bFlagLE)
                        {
                            TCN_MACRO_CONVERT_CPU_TO_LE64(p_tmp_octet64);
                        } /* if (bFlagLE) */
                        else
                        {
                            TCN_MACRO_CONVERT_CPU_TO_BE64(p_tmp_octet64);
                        } /* else */
                        p_tmp_octet64++;
                    } /* for (counter=0; counter<array_count; counter++) */
                    break;

                default:
                    ap_result = AP_DATA_TYPE;

            } /* switch ((derived_type >> 4)) */

    } /* switch (derived_type) */

    return(ap_result);

} /* tcn_ap_put_variable */


/* --------------------------------------------------------------------------
 *  Procedure : tcn_ap_get_variable
 *
 *  Purpose   : Copies a process variable from a traffic store to
 *              the application.
 *
 *  Syntax    : TCN_DECL_LOCAL
 *              AP_RESULT
 *              tcn_ap_get_variable
 *              (
 *                  void        *p_value,
 *                  void        *p_octet,
 *                  UNSIGNED8   derived_type,
 *                  UNSIGNED8   array_count,
 *                  UNSIGNED8   bit_number
 *              );
 *
 *  Input     : p_value      - pointer to a memory buffer of the application
 *                             where the process variable value is copied to
 *              p_octet      - pointer to an octet of a port buffer
 *              derived_type - any AP_DERIVED_TYPE
 *              array_count  - number of elements in the array
 *              bit_number   - bit number of a process variable smaller
 *                             than one octet (only for variables of type
 *                             BOOLEAN1, ANTIVALENT2, BCD4, ENUM4)
 *
 *  Return    : result code; any AP_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL
AP_RESULT
tcn_ap_get_variable
(
    void        *p_value,
    void        *p_octet,
    UNSIGNED8   derived_type,
    UNSIGNED8   array_count,
    UNSIGNED8   bit_number
)
{
    AP_RESULT       ap_result      = AP_OK;
    BOOLEAN1        bFlagLE;
    UNSIGNED8       counter;
    TCN_AP_TMP32    tmp32;
    TIMEDATE48      *p_tmp_value48;
    WORD8           *p_tmp_value8;
    WORD16          *p_tmp_value16;
    WORD32          *p_tmp_value32;
    WORD64          *p_tmp_value64;
    WORD8           *p_tmp_octet8;
    WORD16          *p_tmp_octet16;
    WORD32          *p_tmp_octet32;
    WORD64          *p_tmp_octet64;

    /* determine LE flag */
    if (derived_type & AP_DERIVED_TYPE_FLAG_LE)
    {
        bFlagLE = TRUE;
    } /* if (derived_type & AP_DERIVED_TYPE_FLAG_LE) */
    else
    {
        bFlagLE = FALSE;
    } /* else */

    /* clear LE flag */
    derived_type = \
        (UNSIGNED8)(derived_type & (UNSIGNED8)~AP_DERIVED_TYPE_FLAG_LE);

    switch (derived_type)
    {
        case (AP_DERIVED_TYPE_BOOLEAN1):
            *(BOOLEAN1*)p_value = (BOOLEAN1)                                \
            (                                                               \
                (BOOLEAN1)(*(BOOLEAN1*)p_value & (BOOLEAN1)~0x1) |          \
                (BOOLEAN1)((*(BOOLEAN1*)p_octet >> bit_number) & 0x1)       \
            );
            break;

        case (AP_DERIVED_TYPE_ANTIVALENT2):
            *(ANTIVALENT2*)p_value = (ANTIVALENT2)                          \
            (                                                               \
                (ANTIVALENT2)(*(ANTIVALENT2*)p_value & (ANTIVALENT2)~0x3) | \
                (ANTIVALENT2)                                               \
                    ((*(ANTIVALENT2*)p_octet >> bit_number) & 0x3)          \
            );
            break;

        case (AP_DERIVED_TYPE_BCD4):
            *(BCD4*)p_value = (BCD4)                                        \
            (                                                               \
                (BCD4)(*(BCD4*)p_value & (BCD4)~0xF) |                      \
                (BCD4)((*(BCD4*)p_octet >> bit_number) & 0xF)               \
            );
            break;

        case (AP_DERIVED_TYPE_TIMEDATE48):
            p_tmp_value48 = (TIMEDATE48*)p_value;
            p_tmp_octet16 = (WORD16*)p_octet;
            for (counter=0; counter<array_count; counter++)
            {
                if (bFlagLE)
                {
                    /* element 'seconds' */
                    tmp32.val16[0] = *p_tmp_octet16++;
                    tmp32.val16[1] = *p_tmp_octet16++;
                    p_tmp_value48->seconds = (UNSIGNED32)tmp32.val32;
                    TCN_MACRO_CONVERT_LE32_TO_CPU(&p_tmp_value48->seconds);

                    /* element 'ticks' */
                    p_tmp_value48->ticks = (UNSIGNED16)*p_tmp_octet16++;
                    TCN_MACRO_CONVERT_LE16_TO_CPU(&p_tmp_value48->ticks);
                } /* if (bFlagLE) */
                else
                {
                    /* element 'seconds' */
                    tmp32.val16[0] = *p_tmp_octet16++;
                    tmp32.val16[1] = *p_tmp_octet16++;
                    p_tmp_value48->seconds = (UNSIGNED32)tmp32.val32;
                    TCN_MACRO_CONVERT_BE32_TO_CPU(&p_tmp_value48->seconds);

                    /* element 'ticks' */
                    p_tmp_value48->ticks = (UNSIGNED16)*p_tmp_octet16++;
                    TCN_MACRO_CONVERT_BE16_TO_CPU(&p_tmp_value48->ticks);
                } /* else */
                p_tmp_value48++;
            } /* for (counter=0; counter<array_count; counter++) */
            break;

        default:
            switch ((derived_type >> 4)) /* AP_VAR_SIZE */
            {
                case (AP_VAR_SIZE_8):
                    p_tmp_value8 = (WORD8*)p_value;
                    p_tmp_octet8 = (WORD8*)p_octet;
                    for (counter=0; counter<array_count; counter++)
                    {
                        *p_tmp_value8++ = *p_tmp_octet8++;
                    } /* for (counter=0; counter<array_count; counter++) */
                    break;

                case (AP_VAR_SIZE_16):
                    p_tmp_value16 = (void*)p_value;
                    p_tmp_octet16 = (void*)p_octet;
                    for (counter=0; counter<array_count; counter++)
                    {
                        *p_tmp_value16 = *p_tmp_octet16++;
                        if (bFlagLE)
                        {
                            TCN_MACRO_CONVERT_LE16_TO_CPU(p_tmp_value16);
                        } /* if (bFlagLE) */
                        else
                        {
                            TCN_MACRO_CONVERT_BE16_TO_CPU(p_tmp_value16);
                        } /* else */
                        p_tmp_value16++;
                    } /* for (counter=0; counter<array_count; counter++) */
                    break;

                case (AP_VAR_SIZE_32):
                    p_tmp_value32 = (WORD32*)p_value;
                    p_tmp_octet32 = (WORD32*)p_octet;
                    for (counter=0; counter<array_count; counter++)
                    {
                        *p_tmp_value32 = *p_tmp_octet32++;
                        if (bFlagLE)
                        {
                            TCN_MACRO_CONVERT_LE32_TO_CPU(p_tmp_value32);
                        } /* if (bFlagLE) */
                        else
                        {
                            TCN_MACRO_CONVERT_BE32_TO_CPU(p_tmp_value32);
                        } /* else */
                        p_tmp_value32++;
                    } /* for (counter=0; counter<array_count; counter++) */
                    break;

                case (AP_VAR_SIZE_64):
                    p_tmp_value64 = (WORD64*)p_value;
                    p_tmp_octet64 = (WORD64*)p_octet;
                    for (counter=0; counter<array_count; counter++)
                    {
                        *p_tmp_value64 = *p_tmp_octet64++;
                        if (bFlagLE)
                        {
                            TCN_MACRO_CONVERT_LE64_TO_CPU(p_tmp_value64);
                        } /* if (bFlagLE) */
                        else
                        {
                            TCN_MACRO_CONVERT_BE64_TO_CPU(p_tmp_value64);
                        } /* else */
                        p_tmp_value64++;
                    } /* for (counter=0; counter<array_count; counter++) */
                    break;

                default:
                    ap_result = AP_DATA_TYPE;

            } /* switch ((derived_type >> 4)) */

    } /* switch (derived_type) */

    return(ap_result);

} /* tcn_ap_get_variable */


/* --------------------------------------------------------------------------
 *  Procedure : ap_put_variable
 *
 *  Purpose   : Copies a process variable from the application to
 *              a traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_put_variable
 *              (
 *                  PV_NAME     *ts_variable,
 *                  void        *p_value,
 *                  void        *p_check
 *              );
 *
 *  Input     : ts_variable - PV_NAME of the process variable to be published
 *              p_value     - pointer to a memory buffer of the application
 *                            where the process variable value is copied from
 *              p_check     - pointer to a memory buffer of the application
 *                            where the check variable value is copied from
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Remarks   : - The previous value of the process variable dataset is
 *                overwritten.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_put_variable
(
    PV_NAME     *ts_variable,
    void        *p_value,
    void        *p_check
)
{
    AP_RESULT   ap_result    = AP_OK;
#ifndef TCN_AP_TEST_VAR
    LP_RESULT   lp_result;
    ENUM8       ts_id        = 0;
    WORD16      port_address = 0;
#endif /* #ifndef TCN_AP_TEST_VAR */
    UNSIGNED8   derived_type;
    UNSIGNED8   array_count;

    TCN_AP_MACRO_MUTEX_LOCK();

#ifdef TCN_CHECK_PARAMETER
    /* ----------------------------------------------------------------------
     *  check parameters
     * ----------------------------------------------------------------------
     */
    ap_result = \
        tcn_ap_check_parameter_pv_name(ts_variable, p_value, p_check);
#endif /* #ifdef TCN_CHECK_PARAMETER */

#ifndef TCN_AP_TEST_VAR
    /* ----------------------------------------------------------------------
     *  get dataset
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        ts_id        = (ENUM8)ts_variable->traffic_store_id;
        port_address = (WORD16)ts_variable->port_address;

        lp_result =                     \
            lp_get_dataset              \
            (                           \
                ts_id,                  \
                port_address,           \
                p_tcn_ap_port_buffer,   \
                NULL                    \
            );
        if (LP_OK != lp_result)
        {
            ap_result = (AP_RESULT)lp_result;
        } /* if (LP_OK != lp_result) */
    } /* if (AP_OK == ap_result) */
#endif /* #ifndef TCN_AP_TEST_VAR */

    /* ----------------------------------------------------------------------
     *  put process variable
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        switch (ts_variable->var_type)
        {
            case (AP_VAR_TYPE_ARRAY_WORD8_ODD):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_WORD8;
                array_count  = (UNSIGNED8)((ts_variable->var_size * 2) + 1);
                break;

            case (AP_VAR_TYPE_ARRAY_WORD8_EVEN):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_WORD8;
                array_count  = (UNSIGNED8)((ts_variable->var_size * 2) + 2);
                break;

            case (AP_VAR_TYPE_ARRAY_UNSIGNED16):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_UNSIGNED16;
                array_count  = (UNSIGNED8)(ts_variable->var_size + 1);
                break;

            case (AP_VAR_TYPE_ARRAY_INTEGER16):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_INTEGER16;
                array_count  = (UNSIGNED8)(ts_variable->var_size + 1);
                break;

            case (AP_VAR_TYPE_ARRAY_UNSIGNED32):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_UNSIGNED32;
                array_count  = (UNSIGNED8)((ts_variable->var_size + 1) / 2);
                break;

            case (AP_VAR_TYPE_ARRAY_INTEGER32):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_INTEGER32;
                array_count  = (UNSIGNED8)((ts_variable->var_size + 1) / 2);
                break;

            default:
                derived_type = (UNSIGNED8)                      \
                    (                                           \
                        (ts_variable->var_size << 4) |          \
                        (ts_variable->var_type &                \
                            (UNSIGNED8)~AP_VAR_TYPE_FLAG_LE)    \
                    );
                array_count  = (UNSIGNED8)1;
        } /* switch (ts_variable->var_type) */

        if (ts_variable->var_type & AP_VAR_TYPE_FLAG_LE)
        {
            derived_type |= AP_DERIVED_TYPE_FLAG_LE;
        } /* if (ts_variable->var_type & AP_VAR_TYPE_FLAG_LE) */

        ap_result =                                         \
            tcn_ap_put_variable                             \
            (                                               \
                (void*)p_value,                             \
                (void*)&p_tcn_ap_port_buffer                \
                        [ts_variable->var_octet_offset],    \
                (UNSIGNED8)derived_type,                    \
                (UNSIGNED8)array_count,                     \
                (UNSIGNED8)ts_variable->var_bit_number      \
            );
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  put check variable
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        if (NULL != p_check)
        {
            ap_result =                                         \
                tcn_ap_put_variable                             \
                (                                               \
                    (void*)p_check,                             \
                    (void*)&p_tcn_ap_port_buffer                \
                            [ts_variable->chk_octet_offset],    \
                    (UNSIGNED8)AP_DERIVED_TYPE_ANTIVALENT2,     \
                    (UNSIGNED8)1,                               \
                    (UNSIGNED8)ts_variable->chk_bit_number      \
                );
        } /* if (NULL != p_check) */
    } /* if (AP_OK == ap_result) */

#ifndef TCN_AP_TEST_VAR
    /* ----------------------------------------------------------------------
     *  put dataset
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        lp_result =                     \
            lp_put_dataset              \
            (                           \
                ts_id,                  \
                port_address,           \
                p_tcn_ap_port_buffer    \
            );
        if (LP_OK != lp_result)
        {
            ap_result = (AP_RESULT)lp_result;
        } /* if (LP_OK != lp_result) */
    } /* if (AP_OK == ap_result) */
#endif /* #ifndef TCN_AP_TEST_VAR */

    TCN_AP_MACRO_MUTEX_UNLOCK();

    return(ap_result);

} /* ap_put_variable */


/* --------------------------------------------------------------------------
 *  Procedure : ap_get_variable
 *
 *  Purpose   : Copies a process variable from a traffic store to
 *              the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_get_variable
 *              (
 *                  PV_NAME     *ts_variable,
 *                  void        *p_value,
 *                  void        *p_check,
 *                  void        *p_fresh
 *              );
 *
 *  Input     : ts_variable - PV_NAME of the process variable to be received
 *
 *  Return    : result code; any AP_RESULT
 *
 *  Output    : p_value     - pointer to a memory buffer of the application
 *                            where the process variable value is copied to
 *              p_check     - pointer to a memory buffer of the application
 *                            where the check variable value is copied to
 *              p_fresh     - pointer to a memory buffer of the application
 *                            where the freshness timer is copied to
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_get_variable
(
    PV_NAME     *ts_variable,
    void        *p_value,
    void        *p_check,
    void        *p_fresh
)
{
    AP_RESULT   ap_result    = AP_OK;
#ifndef TCN_AP_TEST_VAR
    LP_RESULT   lp_result;
#endif /* #ifndef TCN_AP_TEST_VAR */
    UNSIGNED8   derived_type;
    UNSIGNED8   array_count;

#ifdef TCN_AP_TEST_VAR
    /* avoid warnings */
    p_fresh = p_fresh;
#endif /* #ifdef TCN_AP_TEST_VAR */

    TCN_AP_MACRO_MUTEX_LOCK();

#ifdef TCN_CHECK_PARAMETER
    /* ----------------------------------------------------------------------
     *  check parameters
     * ----------------------------------------------------------------------
     */
    ap_result = \
        tcn_ap_check_parameter_pv_name(ts_variable, p_value, p_check);
#endif /* #ifdef TCN_CHECK_PARAMETER */

#ifndef TCN_AP_TEST_VAR
    /* ----------------------------------------------------------------------
     *  get dataset
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        lp_result =                                     \
            lp_get_dataset                              \
            (                                           \
                (ENUM8)ts_variable->traffic_store_id,   \
                (WORD16)ts_variable->port_address,      \
                p_tcn_ap_port_buffer,                   \
                p_fresh                                 \
            );
        if (LP_OK != lp_result)
        {
            ap_result = (AP_RESULT)lp_result;
        } /* if (LP_OK != lp_result) */
    } /* if (AP_OK == ap_result) */
#endif /* #ifndef TCN_AP_TEST_VAR */

    /* ----------------------------------------------------------------------
     *  get process variable
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        switch (ts_variable->var_type)
        {
            case (AP_VAR_TYPE_ARRAY_WORD8_ODD):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_WORD8;
                array_count  = (UNSIGNED8)((ts_variable->var_size * 2) + 1);
                break;

            case (AP_VAR_TYPE_ARRAY_WORD8_EVEN):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_WORD8;
                array_count  = (UNSIGNED8)((ts_variable->var_size * 2) + 2);
                break;

            case (AP_VAR_TYPE_ARRAY_UNSIGNED16):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_UNSIGNED16;
                array_count  = (UNSIGNED8)(ts_variable->var_size + 1);
                break;

            case (AP_VAR_TYPE_ARRAY_INTEGER16):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_INTEGER16;
                array_count  = (UNSIGNED8)(ts_variable->var_size + 1);
                break;

            case (AP_VAR_TYPE_ARRAY_UNSIGNED32):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_UNSIGNED32;
                array_count  = (UNSIGNED8)((ts_variable->var_size + 1) / 2);
                break;

            case (AP_VAR_TYPE_ARRAY_INTEGER32):
                derived_type = (UNSIGNED8)AP_DERIVED_TYPE_INTEGER32;
                array_count  = (UNSIGNED8)((ts_variable->var_size + 1) / 2);
                break;

            default:
                derived_type = (UNSIGNED8)                      \
                    (                                           \
                        (ts_variable->var_size << 4) |          \
                        (ts_variable->var_type &                \
                            (UNSIGNED8)~AP_VAR_TYPE_FLAG_LE)    \
                    );
                array_count  = (UNSIGNED8)1;
        } /* switch (ts_variable->var_type) */

        if (ts_variable->var_type & AP_VAR_TYPE_FLAG_LE)
        {
            derived_type |= AP_DERIVED_TYPE_FLAG_LE;
        } /* if (ts_variable->var_type & AP_VAR_TYPE_FLAG_LE) */

        ap_result =                                         \
            tcn_ap_get_variable                             \
            (                                               \
                (void*)p_value,                             \
                (void*)&p_tcn_ap_port_buffer                \
                        [ts_variable->var_octet_offset],    \
                (UNSIGNED8)derived_type,                    \
                (UNSIGNED8)array_count,                     \
                (UNSIGNED8)ts_variable->var_bit_number      \
            );
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  get check variable
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        if (NULL != p_check)
        {
            ap_result =                                         \
                tcn_ap_get_variable                             \
                (                                               \
                    (void*)p_check,                             \
                    (void*)&p_tcn_ap_port_buffer                \
                            [ts_variable->chk_octet_offset],    \
                    (UNSIGNED8)AP_DERIVED_TYPE_ANTIVALENT2,     \
                    (UNSIGNED8)1,                               \
                    (UNSIGNED8)ts_variable->chk_bit_number      \
                );
        } /* if (NULL != p_check) */
    } /* if (AP_OK == ap_result) */

    TCN_AP_MACRO_MUTEX_UNLOCK();

    return(ap_result);

} /* ap_get_variable */


/* ==========================================================================
 *
 *  Variables Interface (set access)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ap_put_set
 *
 *  Purpose   : Copies a set of variables from the application to
 *              a traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_put_set
 *              (
 *                  PV_SET      *pv_set
 *              );
 *
 *  Input     : pv_set - pointer to a memory buffer of the application where
 *                       a PV_SET is stored
 *
 *  Return    : result code; any AP_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_put_set
(
    PV_SET      *pv_set
)
{
    AP_RESULT   ap_result     = AP_OK;
#ifndef TCN_AP_TEST_VAR
    LP_RESULT   lp_result;
    ENUM8       ts_id;
    WORD16      port_address;
#endif /* #ifndef TCN_AP_TEST_VAR */
    UNSIGNED16  c_pv_list;
    UNSIGNED16  c_pv_list_max;
    PV_LIST     *p_pv_list;

    TCN_AP_MACRO_MUTEX_LOCK();

#ifndef TCN_AP_TEST_VAR
    /* ----------------------------------------------------------------------
     *  get dataset
     * ----------------------------------------------------------------------
     */
    ts_id        = (ENUM8)pv_set->dataset.traffic_store_id;
    port_address = (WORD16)pv_set->dataset.port_address;

    lp_result =                     \
        lp_get_dataset              \
        (                           \
            ts_id,                  \
            port_address,           \
            p_tcn_ap_port_buffer,   \
            NULL                    \
        );
    if (LP_OK != lp_result)
    {
        ap_result = (AP_RESULT)lp_result;
    } /* if (LP_OK != lp_result) */
#endif /* #ifndef TCN_AP_TEST_VAR */

    /* ----------------------------------------------------------------------
     *  put variables
     * ----------------------------------------------------------------------
     */
    c_pv_list     = 0;
    c_pv_list_max = pv_set->c_pv_list;
    p_pv_list     = pv_set->p_pv_list;
    while ((c_pv_list < c_pv_list_max) && (AP_OK == ap_result))
    {
#ifdef TCN_CHECK_PARAMETER
        /* ------------------------------------------------------------------
         *  check parameters
         * ------------------------------------------------------------------
         */
        ap_result = \
            tcn_ap_check_parameter_pv_list(pv_set->dataset, p_pv_list);
#endif /* #ifdef TCN_CHECK_PARAMETER */

        if (AP_OK == ap_result)
        {
            ap_result =                                                     \
                tcn_ap_put_variable                                         \
                (                                                           \
                    (void*)p_pv_list->p_variable,                           \
                    (void*)&p_tcn_ap_port_buffer[p_pv_list->octet_offset],  \
                    (UNSIGNED8)p_pv_list->derived_type,                     \
                    (UNSIGNED8)p_pv_list->array_count,                      \
                    (UNSIGNED8)p_pv_list->bit_number                        \
                );
        } /* if (AP_OK == ap_result) */

        c_pv_list++;
        p_pv_list++;
    } /* while ((c_pv_list < c_pv_list_max) && (AP_OK == ap_result)) */

#ifndef TCN_AP_TEST_VAR
    /* ----------------------------------------------------------------------
     *  put dataset
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        lp_result =                     \
            lp_put_dataset              \
            (                           \
                ts_id,                  \
                port_address,           \
                p_tcn_ap_port_buffer    \
            );
        if (LP_OK != lp_result)
        {
            ap_result = (AP_RESULT)lp_result;
        } /* if (LP_OK != lp_result) */
    } /* if (AP_OK == ap_result) */
#endif /* #ifndef TCN_AP_TEST_VAR */

    TCN_AP_MACRO_MUTEX_UNLOCK();

    return(ap_result);

} /* ap_put_set */


/* --------------------------------------------------------------------------
 *  Procedure : ap_get_set
 *
 *  Purpose   : Copies a set of variables from a traffic store to
 *              the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_get_set
 *              (
 *                  PV_SET      *pv_set
 *              );
 *
 *  Input     : pv_set - pointer to a memory buffer of the application where
 *                       a PV_SET is stored
 *
 *  Return    : result code; any AP_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_get_set
(
    PV_SET      *pv_set
)
{
    AP_RESULT   ap_result     = AP_OK;
#ifndef TCN_AP_TEST_VAR
    LP_RESULT   lp_result;
#endif /* #ifndef TCN_AP_TEST_VAR */
    UNSIGNED16  c_pv_list;
    UNSIGNED16  c_pv_list_max;
    PV_LIST     *p_pv_list;

    TCN_AP_MACRO_MUTEX_LOCK();

#ifndef TCN_AP_TEST_VAR
    /* ----------------------------------------------------------------------
     *  get dataset
     * ----------------------------------------------------------------------
     */
    lp_result =                                         \
        lp_get_dataset                                  \
        (                                               \
            (ENUM8)pv_set->dataset.traffic_store_id,    \
            (WORD16)pv_set->dataset.port_address,       \
            p_tcn_ap_port_buffer,                       \
            (void*)pv_set->p_freshtime                  \
        );
    if (LP_OK != lp_result)
    {
        ap_result = (AP_RESULT)lp_result;
    } /* if (LP_OK != lp_result) */
#endif /* #ifndef TCN_AP_TEST_VAR */

    /* ----------------------------------------------------------------------
     *  get variables
     * ----------------------------------------------------------------------
     */
    c_pv_list     = 0;
    c_pv_list_max = pv_set->c_pv_list;
    p_pv_list     = pv_set->p_pv_list;
    while ((c_pv_list < c_pv_list_max) && (AP_OK == ap_result))
    {
#ifdef TCN_CHECK_PARAMETER
        /* ------------------------------------------------------------------
         *  get variables
         * ------------------------------------------------------------------
         */
        ap_result = \
            tcn_ap_check_parameter_pv_list(pv_set->dataset, p_pv_list);
#endif /* #ifdef TCN_CHECK_PARAMETER */

        if (AP_OK == ap_result)
        {
            ap_result =                                                     \
                tcn_ap_get_variable                                         \
                (                                                           \
                    (void*)p_pv_list->p_variable,                           \
                    (void*)&p_tcn_ap_port_buffer[p_pv_list->octet_offset],  \
                    (UNSIGNED8)p_pv_list->derived_type,                     \
                    (UNSIGNED8)p_pv_list->array_count,                      \
                    (UNSIGNED8)p_pv_list->bit_number                        \
                );
        } /* if (AP_OK == ap_result) */

        c_pv_list++;
        p_pv_list++;
    } /* while ((c_pv_list < c_pv_list_max) && (AP_OK == ap_result)) */

    TCN_AP_MACRO_MUTEX_UNLOCK();

    return(ap_result);

} /* ap_get_set */


/* ==========================================================================
 *
 *  Variables Interface (cluster access)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ap_put_cluster
 *
 *  Purpose   : Copies a cluster of variables from the application to
 *              the traffic store(s).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_put_cluster
 *              (
 *                  PV_CLUSTER  *pv_cluster
 *              );
 *
 *  Input     : pv_cluster - pointer to a memory buffer of the application
 *                           where a PV_CLUSTER is stored
 *
 *  Return    : result code; any AP_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_put_cluster
(
    PV_CLUSTER  *pv_cluster
)
{
    AP_RESULT   ap_result     = AP_OK;
#ifndef TCN_AP_TEST_VAR
    LP_RESULT   lp_result;
    ENUM8       ts_id;
    WORD16      port_address;
#endif /* #ifndef TCN_AP_TEST_VAR */
    UNSIGNED16  c_pv_set;
    UNSIGNED16  c_pv_set_max;
    PV_SET      **p_pv_set;
    UNSIGNED16  c_pv_list;
    UNSIGNED16  c_pv_list_max;
    PV_LIST     *p_pv_list;

    pv_cluster->ts_id = pv_cluster->ts_id; /* ignored */

    TCN_AP_MACRO_MUTEX_LOCK();

    c_pv_set     = 0;
    c_pv_set_max = pv_cluster->c_pv_set;
    p_pv_set     = pv_cluster->p_pv_set;
    while ((c_pv_set < c_pv_set_max) && (AP_OK == ap_result))
    {
#ifndef TCN_AP_TEST_VAR
        /* ------------------------------------------------------------------
         *  get dataset
         * ------------------------------------------------------------------
         */
        ts_id        = (ENUM8)(*p_pv_set)->dataset.traffic_store_id;
        port_address = (WORD16)(*p_pv_set)->dataset.port_address;

        lp_result =                     \
            lp_get_dataset              \
            (                           \
                ts_id,                  \
                port_address,           \
                p_tcn_ap_port_buffer,   \
                NULL                    \
            );
        if (LP_OK != lp_result)
        {
            ap_result = (AP_RESULT)lp_result;
        } /* if (LP_OK != lp_result) */
#endif /* #ifndef TCN_AP_TEST_VAR */

        /* ------------------------------------------------------------------
         *  put variables
         * ------------------------------------------------------------------
         */
        c_pv_list     = 0;
        c_pv_list_max = (*p_pv_set)->c_pv_list;
        p_pv_list     = (*p_pv_set)->p_pv_list;
        while ((c_pv_list < c_pv_list_max) && (AP_OK == ap_result))
        {
#ifdef TCN_CHECK_PARAMETER
            /* --------------------------------------------------------------
             *  check parameters
             * --------------------------------------------------------------
             */
            ap_result =                         \
                tcn_ap_check_parameter_pv_list  \
                (                               \
                    (*p_pv_set)->dataset,       \
                    p_pv_list                   \
                );
#endif /* #ifdef TCN_CHECK_PARAMETER */

            if (AP_OK == ap_result)
            {
                ap_result =                                 \
                    tcn_ap_put_variable                     \
                    (                                       \
                        (void*)p_pv_list->p_variable,       \
                        (void*)&p_tcn_ap_port_buffer        \
                                [p_pv_list->octet_offset],  \
                        (UNSIGNED8)p_pv_list->derived_type, \
                        (UNSIGNED8)p_pv_list->array_count,  \
                        (UNSIGNED8)p_pv_list->bit_number    \
                    );
            } /* if (AP_OK == ap_result) */

            c_pv_list++;
            p_pv_list++;
        } /* while ((c_pv_list < c_pv_list_max) && (AP_OK == ap_result)) */

#ifndef TCN_AP_TEST_VAR
        /* ------------------------------------------------------------------
         *  put dataset
         * ------------------------------------------------------------------
         */
        lp_result =                     \
            lp_put_dataset              \
            (                           \
                ts_id,                  \
                port_address,           \
                p_tcn_ap_port_buffer    \
            );
        if (LP_OK != lp_result)
        {
            ap_result = (AP_RESULT)lp_result;
        } /* if (LP_OK != lp_result) */
#endif /* #ifndef TCN_AP_TEST_VAR */

        c_pv_set++;
        p_pv_set++;
    } /* while ((c_pv_set < c_pv_set_max) && (AP_OK == ap_result)) */

    TCN_AP_MACRO_MUTEX_UNLOCK();

    return(ap_result);

} /* ap_put_cluster */


/* --------------------------------------------------------------------------
 *  Procedure : ap_get_cluster
 *
 *  Purpose   : Copies a cluster of variables from the traffic store(s) to
 *              the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              AP_RESULT
 *              ap_get_cluster
 *              (
 *                  PV_CLUSTER  *pv_cluster
 *              );
 *
 *  Input     : pv_cluster - pointer to a memory buffer of the application
 *                           where a PV_CLUSTER is stored
 *
 *  Return    : result code; any AP_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
AP_RESULT
ap_get_cluster
(
    PV_CLUSTER  *pv_cluster
)
{
    AP_RESULT   ap_result     = AP_OK;
#ifndef TCN_AP_TEST_VAR
    LP_RESULT   lp_result;
#endif /* #ifndef TCN_AP_TEST_VAR */
    UNSIGNED16  c_pv_set;
    UNSIGNED16  c_pv_set_max;
    PV_SET      **p_pv_set;
    UNSIGNED16  c_pv_list;
    UNSIGNED16  c_pv_list_max;
    PV_LIST     *p_pv_list;

    pv_cluster->ts_id = pv_cluster->ts_id; /* ignored */

    TCN_AP_MACRO_MUTEX_LOCK();

    c_pv_set     = 0;
    c_pv_set_max = pv_cluster->c_pv_set;
    p_pv_set     = pv_cluster->p_pv_set;
    while ((c_pv_set < c_pv_set_max) && (AP_OK == ap_result))
    {
#ifndef TCN_AP_TEST_VAR
        /* ------------------------------------------------------------------
         *  get dataset
         * ------------------------------------------------------------------
         */
        lp_result =                                             \
            lp_get_dataset                                      \
            (                                                   \
                (ENUM8)(*p_pv_set)->dataset.traffic_store_id,   \
                (WORD16)(*p_pv_set)->dataset.port_address,      \
                p_tcn_ap_port_buffer,                           \
                (void*)(*p_pv_set)->p_freshtime                 \
            );
        if (LP_OK != lp_result)
        {
            ap_result = (AP_RESULT)lp_result;
        } /* if (LP_OK != lp_result) */
#endif /* #ifndef TCN_AP_TEST_VAR */

        /* ------------------------------------------------------------------
         *  get variables
         * ------------------------------------------------------------------
         */
        c_pv_list     = 0;
        c_pv_list_max = (*p_pv_set)->c_pv_list;
        p_pv_list     = (*p_pv_set)->p_pv_list;
        while ((c_pv_list < c_pv_list_max) && (AP_OK == ap_result))
        {
#ifdef TCN_CHECK_PARAMETER
            /* --------------------------------------------------------------
             *  check parameters
             * --------------------------------------------------------------
             */
            ap_result =                         \
                tcn_ap_check_parameter_pv_list  \
                (                               \
                    (*p_pv_set)->dataset,       \
                    p_pv_list                   \
                );
#endif /* #ifdef TCN_CHECK_PARAMETER */

            if (AP_OK == ap_result)
            {
                ap_result =                                 \
                    tcn_ap_get_variable                     \
                    (                                       \
                        (void*)p_pv_list->p_variable,       \
                        (void*)&p_tcn_ap_port_buffer        \
                                [p_pv_list->octet_offset],  \
                        (UNSIGNED8)p_pv_list->derived_type, \
                        (UNSIGNED8)p_pv_list->array_count,  \
                        (UNSIGNED8)p_pv_list->bit_number    \
                    );
            } /* if (AP_OK == ap_result) */

            c_pv_list++;
            p_pv_list++;
        } /* while ((c_pv_list < c_pv_list_max) && (AP_OK == ap_result)) */

        c_pv_set++;
        p_pv_set++;
    } /* while ((c_pv_set < c_pv_set_max) && (AP_OK == ap_result)) */

    TCN_AP_MACRO_MUTEX_UNLOCK();

    return(ap_result);

} /* ap_get_cluster */


/* ==========================================================================
 *
 *  Test of process variables
 *
 * ==========================================================================
 */
#ifdef TCN_AP_TEST_VAR

#define TCN_AP_TEST_VAR_ON_ERROR(_result_)


/* --------------------------------------------------------------------------
 *  Procedure : tcn_ap_test_var_check
 * --------------------------------------------------------------------------
 */
void
tcn_ap_test_var_check (void)
{
    UNSIGNED16  counter;

    TCN_OSL_PRINTF("\n");

    for (counter=0; counter<16; counter++)
    {
        if (0x00 == p_tcn_ap_port_buffer[counter])
        {
            TCN_OSL_PRINTF(".. ");
        } /* if (0x00 == p_tcn_ap_port_buffer[counter]) */
        else
        {
            TCN_OSL_PRINTF("%02X ", p_tcn_ap_port_buffer[counter]);
        } /* else */
    } /* for (counter=0; counter<16; counter++) */

} /* tcn_ap_test_var_check */


/* --------------------------------------------------------------------------
 *  Procedure : ap_test_var
 * --------------------------------------------------------------------------
 */
AP_RESULT
ap_test_var (void)
{
    /* miscellaneous */
    AP_RESULT   ap_result = AP_OK;
    UNSIGNED16  counter;
    UNSIGNED16  loop;
    UNSIGNED16  ignore;

    /* variables */
    BOOLEAN1    var_boolean1;
    BOOLEAN1    var_boolean1_chk;
    ANTIVALENT2 var_antivalent2;
    ANTIVALENT2 var_antivalent2_chk;
    ANTIVALENT2 var_check;
    ANTIVALENT2 var_check_chk;
    BCD4        var_bcd4;
    BCD4        var_bcd4_chk;
    WORD8       var_word8;
    WORD8       var_word8_chk;
    WORD16      var_word16;
    WORD16      var_word16_chk;
    WORD32      var_word32;
    WORD32      var_word32_chk;
    WORD64      var_word64;
    WORD64      var_word64_chk;
    TIMEDATE48  var_timedate48;
    TIMEDATE48  var_timedate48_chk;
    WORD8       var_array_word8[4];
    WORD8       var_array_word8_chk[4];
    WORD16      var_array_word16[2];
    WORD16      var_array_word16_chk[2];
    WORD32      var_array_word32[2];
    WORD32      var_array_word32_chk[2];

    /* ap_put/get_dataset */
    DS_NAME     ds_name;

    /* ap_put/get_variable */
    PV_NAME     pv_name;
    UNSIGNED8   var_octet_offset;
    UNSIGNED8   var_bit_number;

    /* ap_put/get_set */
    PV_LIST     pv_list1[3];
    PV_LIST     pv_list2[3];
    PV_SET      pv_set[2];

    /* ap_put/get_cluster */
    PV_CLUSTER  *pv_cluster;


    TCN_OSL_PRINTF("\n");
    TCN_OSL_PRINTF("Test Process Variables (AV)\n");
    TCN_OSL_PRINTF("===========================\n");


    /* ----------------------------------------------------------------------
     *  clear port buffer
     * ----------------------------------------------------------------------
     */
    for (counter=0; counter<32; counter++)
    {
        p_tcn_ap_port_buffer[counter] = 0;
    } /* for (counter=0; counter<32; counter++) */


    /* ----------------------------------------------------------------------
     *  VAR_TYPE_BOOLEAN1
     * ----------------------------------------------------------------------
     */
    TCN_OSL_PRINTF("\n");
    TCN_OSL_PRINTF("VAR_TYPE_BOOLEAN1:\n");
    TCN_OSL_PRINTF("------------------");
    ignore           = 0;
    var_octet_offset = 0;
    var_bit_number   = 0;
    while ((2 > var_octet_offset) && (AP_OK == ap_result))
    {
        var_boolean1             = 0xAA;
        pv_name.traffic_store_id = 0;
        pv_name.port_address     = 0x10;
        pv_name.var_size         = AP_VAR_SIZE_BOOLEAN1;
        pv_name.var_octet_offset = var_octet_offset;
        pv_name.var_bit_number   = var_bit_number;
        pv_name.var_type         = AP_VAR_TYPE_BOOLEAN1;
        pv_name.chk_octet_offset = 0xFF;
        pv_name.chk_bit_number   = 0xFF;
        for (loop=0; loop<2; loop++)
        {
            var_boolean1 = (BOOLEAN1)~var_boolean1;
            ap_result = ap_put_variable(&pv_name, &var_boolean1, NULL);
            if (AP_OK == ap_result)
            {
                var_boolean1_chk = 0;
                ap_result = ap_get_variable(&pv_name, &var_boolean1_chk, \
                    NULL, NULL);
                if (AP_OK == ap_result)
                {
                    if ((var_boolean1 & 0x1) != var_boolean1_chk)
                    {
                        ap_result = AP_ERROR;
                    } /* if ((var_boolean1 & 0x1) != var_boolean1_chk) */
                    else
                    {
                        if (0 == loop)
                        {
                            tcn_ap_test_var_check();
                        } /* if (0 == loop) */
                        ignore = 0;
                    } /* else */
                } /* if (AP_OK == ap_result) */
            } /* if (AP_OK == ap_result) */
        } /* for (loop=0; loop<2; loop++) */

        var_bit_number++;
        if (var_bit_number == 8)
        {
            var_octet_offset++;
            var_bit_number = 0;
        } /* if (var_bit_number == 8) */
    } /* while ((2 > var_octet_offset) && (AP_OK == ap_result)) */
    TCN_OSL_PRINTF("\n");

    /* ----------------------------------------------------------------------
     *  VAR_TYPE_ANTIVALENT2 with check variable
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("VAR_TYPE_ANTIVALENT2:\n");
        TCN_OSL_PRINTF("---------------------");
        ignore           = 0;
        var_octet_offset = 0;
        var_bit_number   = 0;
        while ((2 > var_octet_offset) && (AP_OK == ap_result))
        {
            var_antivalent2          = 0xAA;
            var_check                = 0x55;
            pv_name.traffic_store_id = 0;
            pv_name.port_address     = 0x10;
            pv_name.var_size         = AP_VAR_SIZE_ANTIVALENT2;
            pv_name.var_octet_offset = var_octet_offset;
            pv_name.var_bit_number   = var_bit_number;
            pv_name.var_type         = AP_VAR_TYPE_ANTIVALENT2;
            pv_name.chk_octet_offset = \
                (TCN_DEF_BITFIELD16)(2 + var_octet_offset);
            pv_name.chk_bit_number   = var_bit_number;
            for (loop=0; loop<3; loop++)
            {
                if (2 == loop)
                {
                    var_antivalent2 = 0;
                    var_check       = 0;
                } /* if (2 == loop) */
                else
                {
                    var_antivalent2 = (ANTIVALENT2)~var_antivalent2;
                    var_check       = (ANTIVALENT2)~var_check;
                } /* else */
                ap_result = ap_put_variable(&pv_name, &var_antivalent2, \
                    &var_check);
                if (AP_OK == ap_result)
                {
                    var_antivalent2_chk = 0;
                    var_check_chk       = 0;
                    ap_result = ap_get_variable(&pv_name, \
                        &var_antivalent2_chk, &var_check_chk, NULL);
                    if (AP_OK == ap_result)
                    {
                        if ((var_antivalent2 & 0x3) != var_antivalent2_chk)
                        {
                            ap_result = AP_ERROR;
                        } /* if ((var_antivalent2 & 0x3) != ...) */
                        else
                        {
                            if (2 > loop)
                            {
                                tcn_ap_test_var_check();
                            } /* if (2 > loop) */
                            ignore = 0;
                        } /* else */
                    } /* if (AP_OK == ap_result) */
                } /* if (AP_OK == ap_result) */
                else
                {
                    if ((AP_RANGE == ap_result) && \
                        (0 != (var_bit_number % 2)))
                    {
                        /* ignore */
                        TCN_OSL_PRINTF("i");
                        ap_result = AP_OK;
                    } /* if ((AP_RANGE == ap_result) && (...)) */
                } /* else */
            } /* for (loop=0; loop<3; loop++) */

            var_bit_number++;
            if (var_bit_number == 8)
            {
                var_octet_offset++;
                var_bit_number   = 0;
            } /* if (var_bit_number == 8) */
        } /* while ((2 > var_octet_offset) && (AP_OK == ap_result)) */
        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  VAR_TYPE_BCD4
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("VAR_TYPE_BCD4:\n");
        TCN_OSL_PRINTF("--------------");
        ignore           = 0;
        var_octet_offset = 0;
        var_bit_number   = 0;
        while ((2 > var_octet_offset) && (AP_OK == ap_result))
        {
            var_bcd4                 = 0xAA;
            pv_name.traffic_store_id = 0;
            pv_name.port_address     = 0x10;
            pv_name.var_size         = AP_VAR_SIZE_BCD4;
            pv_name.var_octet_offset = var_octet_offset;
            pv_name.var_bit_number   = var_bit_number;
            pv_name.var_type         = AP_VAR_TYPE_BCD4;
            pv_name.chk_octet_offset = 0xFF;
            pv_name.chk_bit_number   = 0xFF;
            for (loop=0; loop<3; loop++)
            {
                if (2 == loop)
                {
                    var_bcd4 = 0;
                } /* if (2 == loop) */
                else
                {
                    var_bcd4 = (BCD4)~var_bcd4;
                } /* else */
                ap_result = ap_put_variable(&pv_name, &var_bcd4, NULL);
                if (AP_OK == ap_result)
                {
                    var_bcd4_chk = 0;
                    ap_result = ap_get_variable(&pv_name, &var_bcd4_chk, \
                        NULL, NULL);
                    if (AP_OK == ap_result)
                    {
                        if ((var_bcd4 & 0xF) != var_bcd4_chk)
                        {
                            ap_result = AP_ERROR;
                        } /* if ((var_bcd4 & 0xF) != var_bcd4_chk) */
                        else
                        {
                            if (2 > loop)
                            {
                                tcn_ap_test_var_check();
                            } /* if (2 > loop) */
                            ignore = 0;
                        } /* else */
                    } /* if (AP_OK == ap_result) */
                } /* if (AP_OK == ap_result) */
                else
                {
                    if ((AP_RANGE == ap_result) && \
                        (0 != (var_bit_number % 4)))
                    {
                        /* ignore */
                        TCN_OSL_PRINTF("i");
                        ap_result = AP_OK;
                    } /* if ((AP_RANGE == ap_result) && (...)) */
                } /* else */
            } /* for (loop=0; loop<3; loop++) */

            var_bit_number++;
            if (var_bit_number == 8)
            {
                var_octet_offset++;
                var_bit_number   = 0;
            } /* if (var_bit_number == 8) */
        } /* while ((2 > var_octet_offset) && (AP_OK == ap_result)) */
        TCN_OSL_PRINTF("\n");

        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  VAR_TYPE_WORD8
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("VAR_TYPE_WORD8:\n");
        TCN_OSL_PRINTF("---------------");
        ignore           = 0;
        var_octet_offset = 0;
        while ((2 > var_octet_offset) && (AP_OK == ap_result))
        {
            var_word8                = 0x55;
            pv_name.traffic_store_id = 0;
            pv_name.port_address     = 0x10;
            pv_name.var_size         = AP_VAR_SIZE_WORD8;
            pv_name.var_octet_offset = var_octet_offset;
            pv_name.var_bit_number   = 0;
            pv_name.var_type         = AP_VAR_TYPE_WORD8;
            pv_name.chk_octet_offset = 0xFF;
            pv_name.chk_bit_number   = 0xFF;
            for (loop=0; loop<3; loop++)
            {
                if (2 == loop)
                {
                    var_word8 = 0;
                } /* if (2 == loop) */
                else
                {
                    var_word8 = (WORD8)~var_word8;
                } /* else */
                ap_result = ap_put_variable(&pv_name, &var_word8, NULL);
                if (AP_OK == ap_result)
                {
                    var_word8_chk = 0;
                    ap_result = ap_get_variable(&pv_name, &var_word8_chk, \
                        NULL, NULL);
                    if (AP_OK == ap_result)
                    {
                        if (var_word8 != var_word8_chk)
                        {
                            ap_result = AP_ERROR;
                        } /* if (var_word8 != var_word8_chk) */
                        else
                        {
                            if (2 > loop)
                            {
                                tcn_ap_test_var_check();
                            } /* if (2 > loop) */
                            ignore = 0;
                        } /* else */
                    } /* if (AP_OK == ap_result) */
                } /* if (AP_OK == ap_result) */
            } /* for (loop=0; loop<3; loop++) */

            var_octet_offset++;
        } /* while ((2 > var_octet_offset) && (AP_OK == ap_result)) */
        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  VAR_TYPE_WORD16
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("VAR_TYPE_WORD16:\n");
        TCN_OSL_PRINTF("----------------");
        ignore           = 0;
        var_octet_offset = 0;
        while ((2 >= var_octet_offset) && (AP_OK == ap_result))
        {
            var_word16               = 0x5F5E;
            pv_name.traffic_store_id = 0;
            pv_name.port_address     = 0x10;
            pv_name.var_size         = AP_VAR_SIZE_WORD16;
            pv_name.var_octet_offset = var_octet_offset;
            pv_name.var_bit_number   = 0;
            pv_name.var_type         = AP_VAR_TYPE_WORD16;
            pv_name.chk_octet_offset = 0xFF;
            pv_name.chk_bit_number   = 0xFF;
            for (loop=0; loop<3; loop++)
            {
                if (2 == loop)
                {
                    var_word16 = 0;
                } /* if (2 == loop) */
                else
                {
                    var_word16 = (WORD16)~var_word16;
                } /* else */
                ap_result = ap_put_variable(&pv_name, &var_word16, NULL);
                if (AP_OK == ap_result)
                {
                    var_word16_chk = 0;
                    ap_result = ap_get_variable(&pv_name, &var_word16_chk, \
                        NULL, NULL);
                    if (AP_OK == ap_result)
                    {
                        if (var_word16 != var_word16_chk)
                        {
                            ap_result = AP_ERROR;
                        } /* if (var_word16 != var_word16_chk) */
                        else
                        {
                            if (2 > loop)
                            {
                                tcn_ap_test_var_check();
                            } /* if (2 > loop) */
                            ignore = 0;
                        } /* else */
                    } /* if (AP_OK == ap_result) */
                } /* if (AP_OK == ap_result) */
                else
                {
                    if ((AP_RANGE == ap_result) && \
                        (0 != (var_octet_offset % 2)))
                    {
                        /* ignore */
                        TCN_OSL_PRINTF("I");
                        ap_result = AP_OK;
                    } /* if ((AP_RANGE == ap_result) && (...)) */
                } /* else */
            } /* for (loop=0; loop<3; loop++) */

            var_octet_offset++;
        } /* while ((2 >= var_octet_offset) && (AP_OK == ap_result)) */
        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  VAR_TYPE_WORD32
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("VAR_TYPE_WORD32:\n");
        TCN_OSL_PRINTF("----------------");
        ignore           = 0;
        var_octet_offset = 0;
        while ((4 >= var_octet_offset) && (AP_OK == ap_result))
        {
            var_word32               = 0x5F5E5D5C;
            pv_name.traffic_store_id = 0;
            pv_name.port_address     = 0x10;
            pv_name.var_size         = AP_VAR_SIZE_WORD32;
            pv_name.var_octet_offset = var_octet_offset;
            pv_name.var_bit_number   = 0;
            pv_name.var_type         = AP_VAR_TYPE_WORD32;
            pv_name.chk_octet_offset = 0xFF;
            pv_name.chk_bit_number   = 0xFF;
            for (loop=0; loop<3; loop++)
            {
                if (2 == loop)
                {
                    var_word32 = 0;
                } /* if (2 == loop) */
                else
                {
                    var_word32 = (WORD32)~var_word32;
                } /* else */
                ap_result = ap_put_variable(&pv_name, &var_word32, NULL);
                if (AP_OK == ap_result)
                {
                    var_word32_chk = 0;
                    ap_result = ap_get_variable(&pv_name, &var_word32_chk, \
                        NULL, NULL);
                    if (AP_OK == ap_result)
                    {
                        if (var_word32 != var_word32_chk)
                        {
                            ap_result = AP_ERROR;
                        } /* if (var_word32 != var_word32_chk) */
                        else
                        {
                            if (2 > loop)
                            {
                                tcn_ap_test_var_check();
                            } /* if (2 > loop) */
                            ignore = 0;
                        } /* else */
                    } /* if (AP_OK == ap_result) */
                } /* if (AP_OK == ap_result) */
                else
                {
                    if ((AP_RANGE == ap_result) && \
                        (0 != (var_octet_offset % 4)))
                    {
                        /* ignore */
                        TCN_OSL_PRINTF("I");
                        ap_result = AP_OK;
                    } /* if ((AP_RANGE == ap_result) && (...)) */
                } /* else */
            } /* for (loop=0; loop<3; loop++) */

            var_octet_offset++;
        } /* while ((4 >= var_octet_offset) && (AP_OK == ap_result)) */
        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  VAR_TYPE_TIMEDATE48
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("VAR_TYPE_TIMEDATE48:\n");
        TCN_OSL_PRINTF("--------------------");
        ignore           = 0;
        var_octet_offset = 0;
        while ((6 >= var_octet_offset) && (AP_OK == ap_result))
        {
            var_timedate48.seconds   = 0x5F5E5D5C;
            var_timedate48.ticks     = 0x5B5A;
            pv_name.traffic_store_id = 0;
            pv_name.port_address     = 0x10;
            pv_name.var_size         = AP_VAR_SIZE_TIMEDATE48;
            pv_name.var_octet_offset = var_octet_offset;
            pv_name.var_bit_number   = 0;
            pv_name.var_type         = AP_VAR_TYPE_TIMEDATE48;
            pv_name.chk_octet_offset = 0xFF;
            pv_name.chk_bit_number   = 0xFF;
            for (loop=0; loop<3; loop++)
            {
                if (2 == loop)
                {
                    var_timedate48.seconds = 0;
                    var_timedate48.ticks   = 0;
                } /* if (2 == loop) */
                else
                {
                    var_timedate48.seconds = \
                        (UNSIGNED32)~(var_timedate48.seconds);
                    var_timedate48.ticks   = \
                        (UNSIGNED16)~(var_timedate48.ticks);
                } /* else */
                ap_result = ap_put_variable(&pv_name, &var_timedate48, NULL);
                if (AP_OK == ap_result)
                {
                    var_timedate48_chk.seconds = 0;
                    var_timedate48_chk.ticks   = 0;
                    ap_result = ap_get_variable(&pv_name, \
                        &var_timedate48_chk, NULL, NULL);
                    if (AP_OK == ap_result)
                    {
                        if ((var_timedate48.seconds !=         \
                                var_timedate48_chk.seconds) || \
                            (var_timedate48.ticks !=           \
                                var_timedate48_chk.ticks))
                        {
                            ap_result = AP_ERROR;
                        } /* if ((...)) */
                        else
                        {
                            if (2 > loop)
                            {
                                tcn_ap_test_var_check();
                            } /* if (2 > loop) */
                            ignore = 0;
                        } /* else */
                    } /* if (AP_OK == ap_result) */
                } /* if (AP_OK == ap_result) */
                else
                {
                    if ((AP_RANGE == ap_result) && \
                        (0 != (var_octet_offset % 6)))
                    {
                        /* ignore */
                        TCN_OSL_PRINTF("I");
                        ap_result = AP_OK;
                    } /* if ((AP_RANGE == ap_result) && (...)) */
                } /* else */
            } /* for (loop=0; loop<3; loop++) */

            var_octet_offset++;
        } /* while ((6 >= var_octet_offset) && (AP_OK == ap_result)) */
        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  VAR_TYPE_WORD64
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("VAR_TYPE_WORD64:\n");
        TCN_OSL_PRINTF("----------------");
        ignore           = 0;
        var_octet_offset = 0;
        while ((8 >= var_octet_offset) && (AP_OK == ap_result))
        {
            var_word64               = 0x5F5E5D5C5B5A5958;
            pv_name.traffic_store_id = 0;
            pv_name.port_address     = 0x10;
            pv_name.var_size         = AP_VAR_SIZE_WORD64;
            pv_name.var_octet_offset = var_octet_offset;
            pv_name.var_bit_number   = 0;
            pv_name.var_type         = AP_VAR_TYPE_WORD64;
            pv_name.chk_octet_offset = 0xFF;
            pv_name.chk_bit_number   = 0xFF;
            for (loop=0; loop<3; loop++)
            {
                if (2 == loop)
                {
                    var_word64 = 0;
                } /* if (2 == loop) */
                else
                {
                    var_word64 = (WORD64)~var_word64;
                } /* else */
                ap_result = ap_put_variable(&pv_name, &var_word64, NULL);
                if (AP_OK == ap_result)
                {
                    var_word64_chk = 0;
                    ap_result = ap_get_variable(&pv_name, &var_word64_chk, \
                        NULL, NULL);
                    if (AP_OK == ap_result)
                    {
                        if (var_word64 != var_word64_chk)
                        {
                            ap_result = AP_ERROR;
                        } /* if (var_word64 != var_word64_chk) */
                        else
                        {
                            if (2 > loop)
                            {
                                tcn_ap_test_var_check();
                            } /* if (2 > loop) */
                            ignore = 0;
                        } /* else */
                    } /* if (AP_OK == ap_result) */
                } /* if (AP_OK == ap_result) */
                else
                {
                    if ((AP_RANGE == ap_result) && \
                        (0 != (var_octet_offset % 8)))
                    {
                        /* ignore */
                        TCN_OSL_PRINTF("I");
                        ap_result = AP_OK;
                    } /* if ((AP_RANGE == ap_result) && (...)) */
                } /* else */
            } /* for (loop=0; loop<3; loop++) */

            var_octet_offset++;
        } /* while ((8 >= var_octet_offset) && (AP_OK == ap_result)) */
        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  AP_VAR_TYPE_ARRAY_WORD8_ODD
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("AP_VAR_TYPE_ARRAY_WORD8_ODD:\n");
        TCN_OSL_PRINTF("----------------------------");
        var_array_word8[0]       = 0x5F;
        var_array_word8[1]       = 0x5E;
        var_array_word8[2]       = 0x5D;
        var_array_word8[3]       = 0x5C;
        pv_name.traffic_store_id = 0;
        pv_name.port_address     = 0x10;
        pv_name.var_size         = 1;
        pv_name.var_octet_offset = 0;
        pv_name.var_bit_number   = 0;
        pv_name.var_type         = AP_VAR_TYPE_ARRAY_WORD8_ODD;
        pv_name.chk_octet_offset = 0xFF;
        pv_name.chk_bit_number   = 0xFF;
        for (loop=0; loop<3; loop++)
        {
            if (2 == loop)
            {
                var_array_word8[0] = 0;
                var_array_word8[1] = 0;
                var_array_word8[2] = 0;
                var_array_word8[3] = 0;
            } /* if (2 == loop) */
            else
            {
                var_array_word8[0] = (WORD8)~var_array_word8[0];
                var_array_word8[1] = (WORD8)~var_array_word8[1];
                var_array_word8[2] = (WORD8)~var_array_word8[2];
                var_array_word8[3] = (WORD8)~var_array_word8[3];
            } /* else */
            ap_result = ap_put_variable(&pv_name, var_array_word8, NULL);
            if (AP_OK == ap_result)
            {
                var_array_word8_chk[0] = 0;
                var_array_word8_chk[1] = 0;
                var_array_word8_chk[2] = 0;
                var_array_word8_chk[3] = 0;
                ap_result = ap_get_variable(&pv_name, var_array_word8_chk, \
                    NULL, NULL);
                if (AP_OK == ap_result)
                {
                    if ((var_array_word8[0] != var_array_word8_chk[0]) || \
                        (var_array_word8[1] != var_array_word8_chk[1]) || \
                        (var_array_word8[2] != var_array_word8_chk[2]))
                    {
                        ap_result = AP_ERROR;
                    } /* if ((...)) */
                    else
                    {
                        if (2 > loop)
                        {
                            tcn_ap_test_var_check();
                        } /* if (2 > loop) */
                        ignore = 0;
                    } /* else */
                } /* if (AP_OK == ap_result) */
            } /* if (AP_OK == ap_result) */
        } /* for (loop=0; loop<3; loop++) */
        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  AP_VAR_TYPE_ARRAY_WORD8_EVEN
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("AP_VAR_TYPE_ARRAY_WORD8_EVEN:\n");
        TCN_OSL_PRINTF("-----------------------------");
        var_array_word8[0]       = 0x5F;
        var_array_word8[1]       = 0x5E;
        var_array_word8[2]       = 0x5D;
        var_array_word8[3]       = 0x5C;
        pv_name.traffic_store_id = 0;
        pv_name.port_address     = 0x10;
        pv_name.var_size         = 1;
        pv_name.var_octet_offset = 0;
        pv_name.var_bit_number   = 0;
        pv_name.var_type         = AP_VAR_TYPE_ARRAY_WORD8_EVEN;
        pv_name.chk_octet_offset = 0xFF;
        pv_name.chk_bit_number   = 0xFF;
        for (loop=0; loop<3; loop++)
        {
            if (2 == loop)
            {
                var_array_word8[0] = 0;
                var_array_word8[1] = 0;
                var_array_word8[2] = 0;
                var_array_word8[3] = 0;
            } /* if (2 == loop) */
            else
            {
                var_array_word8[0] = (WORD8)~var_array_word8[0];
                var_array_word8[1] = (WORD8)~var_array_word8[1];
                var_array_word8[2] = (WORD8)~var_array_word8[2];
                var_array_word8[3] = (WORD8)~var_array_word8[3];
            } /* else */
            ap_result = ap_put_variable(&pv_name, var_array_word8, NULL);
            if (AP_OK == ap_result)
            {
                var_array_word8_chk[0] = 0;
                var_array_word8_chk[1] = 0;
                var_array_word8_chk[2] = 0;
                var_array_word8_chk[3] = 0;
                ap_result = ap_get_variable(&pv_name, \
                    &var_array_word8_chk[0], NULL, NULL);
                if (AP_OK == ap_result)
                {
                    if ((var_array_word8[0] != var_array_word8_chk[0]) || \
                        (var_array_word8[1] != var_array_word8_chk[1]) || \
                        (var_array_word8[2] != var_array_word8_chk[2]) || \
                        (var_array_word8[3] != var_array_word8_chk[3]))
                    {
                        ap_result = AP_ERROR;
                    } /* if ((...)) */
                    else
                    {
                        if (2 > loop)
                        {
                            tcn_ap_test_var_check();
                        } /* if (2 > loop) */
                        ignore = 0;
                    } /* else */
                } /* if (AP_OK == ap_result) */
            } /* if (AP_OK == ap_result) */
        } /* for (loop=0; loop<3; loop++) */
        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  VAR_TYPE_ARRAY_UNSIGNED16/INTEGER16
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("VAR_TYPE_ARRAY_UNSIGNED16/INTEGER16:\n");
        TCN_OSL_PRINTF("------------------------------------");
        var_array_word16[0]      = 0x5F5E;
        var_array_word16[1]      = 0x5D5C;
        pv_name.traffic_store_id = 0;
        pv_name.port_address     = 0x10;
        pv_name.var_size         = 1;
        pv_name.var_octet_offset = 0;
        pv_name.var_bit_number   = 0;
        pv_name.var_type         = AP_VAR_TYPE_ARRAY_UNSIGNED16;
        pv_name.chk_octet_offset = 0xFF;
        pv_name.chk_bit_number   = 0xFF;
        for (loop=0; loop<3; loop++)
        {
            if (2 == loop)
            {
                var_array_word16[0] = 0;
                var_array_word16[1] = 0;
            } /* if (2 == loop) */
            else
            {
                if (1 == loop)
                {
                    pv_name.var_type = AP_VAR_TYPE_ARRAY_INTEGER16;
                } /* if (1 == loop) */
                var_array_word16[0] = (WORD16)~var_array_word16[0];
                var_array_word16[1] = (WORD16)~var_array_word16[1];
            } /* else */
            ap_result = ap_put_variable(&pv_name, var_array_word16, NULL);
            if (AP_OK == ap_result)
            {
                var_array_word16_chk[0] = 0;
                var_array_word16_chk[1] = 0;
                ap_result = ap_get_variable(&pv_name, var_array_word16_chk, \
                    NULL, NULL);
                if (AP_OK == ap_result)
                {
                    if ((var_array_word16[0] != var_array_word16_chk[0]) || \
                        (var_array_word16[1] != var_array_word16_chk[1]))
                    {
                        ap_result = AP_ERROR;
                    } /* if ((...)) */
                    else
                    {
                        if (2 > loop)
                        {
                            tcn_ap_test_var_check();
                        } /* if (2 > loop) */
                        ignore = 0;
                    } /* else */
                } /* if (AP_OK == ap_result) */
            } /* if (AP_OK == ap_result) */
        } /* for (loop=0; loop<3; loop++) */
        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  VAR_TYPE_ARRAY_UNSIGNED32/INTEGER32
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("VAR_TYPE_ARRAY_UNSIGNED32/INTEGER32:\n");
        TCN_OSL_PRINTF("------------------------------------");
        var_array_word32[0]      = 0x5F5E5D5C;
        var_array_word32[1]      = 0x5B5A5958;
        pv_name.traffic_store_id = 0;
        pv_name.port_address     = 0x10;
        pv_name.var_size         = 3;
        pv_name.var_octet_offset = 0;
        pv_name.var_bit_number   = 0;
        pv_name.var_type         = AP_VAR_TYPE_ARRAY_UNSIGNED32;
        pv_name.chk_octet_offset = 0xFF;
        pv_name.chk_bit_number   = 0xFF;
        for (loop=0; loop<3; loop++)
        {
            if (2 == loop)
            {
                var_array_word32[0] = 0;
                var_array_word32[1] = 0;
            } /* if (2 == loop) */
            else
            {
                if (1 == loop)
                {
                    pv_name.var_type = AP_VAR_TYPE_ARRAY_INTEGER32;
                } /* if (1 == loop) */
                var_array_word32[0] = (WORD32)~var_array_word32[0];
                var_array_word32[1] = (WORD32)~var_array_word32[1];
            } /* else */
            ap_result = ap_put_variable(&pv_name, var_array_word32, NULL);
            if (AP_OK == ap_result)
            {
                var_array_word32_chk[0] = 0;
                var_array_word32_chk[1] = 0;
                ap_result = ap_get_variable(&pv_name, var_array_word32_chk, \
                    NULL, NULL);
                if (AP_OK == ap_result)
                {
                    if ((var_array_word32[0] != var_array_word32_chk[0]) || \
                        (var_array_word32[1] != var_array_word32_chk[1]))
                    {
                        ap_result = AP_ERROR;
                    } /* if ((...)) */
                    else
                    {
                        if (2 > loop)
                        {
                            tcn_ap_test_var_check();
                        } /* if (2 > loop) */
                        ignore = 0;
                    } /* else */
                } /* if (AP_OK == ap_result) */
            } /* if (AP_OK == ap_result) */
        } /* for (loop=0; loop<3; loop++) */
        TCN_OSL_PRINTF("\n");

        TCN_OSL_PRINTF("\n");
    } /* if (AP_OK == ap_result) */

    /* ----------------------------------------------------------------------
     *  PV_SET
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("PV_SET:\n");
        TCN_OSL_PRINTF("-------");

        var_boolean1             = 0x55;
        pv_list1[0].p_variable   = &var_boolean1;
        pv_list1[0].derived_type = AP_DERIVED_TYPE_BOOLEAN1;
        pv_list1[0].array_count  = 1;
        pv_list1[0].octet_offset = 0;
        pv_list1[0].bit_number   = 4;

        var_word8                = 0xAA;
        pv_list1[1].p_variable   = &var_word8;
        pv_list1[1].derived_type = AP_DERIVED_TYPE_WORD8;
        pv_list1[1].array_count  = 1;
        pv_list1[1].octet_offset = 1;
        pv_list1[1].bit_number   = 0;

        var_word16               = 0xA1A2;
        pv_list1[2].p_variable   = &var_word16;
        pv_list1[2].derived_type = AP_DERIVED_TYPE_WORD16;
        pv_list1[2].array_count  = 1;
        pv_list1[2].octet_offset = 2;
        pv_list1[2].bit_number   = 0;

        ds_name.traffic_store_id = 0;
        ds_name.port_address     = 0x10;
        pv_set[0].p_pv_list      = pv_list1;
        pv_set[0].c_pv_list      = 3;
        pv_set[0].p_freshtime    = NULL;
        pv_set[0].dataset        = ds_name;

        ap_result = ap_put_set(&pv_set[0]);
        if (AP_OK == ap_result)
        {
            tcn_ap_test_var_check();
            TCN_OSL_PRINTF("\n");
        } /* if (AP_OK == ap_result) */
    } /* if (AP_OK == ap_result) */


    /* ----------------------------------------------------------------------
     *  clear port buffer
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        for (counter=0; counter<32; counter++)
        {
            p_tcn_ap_port_buffer[counter] = 0;
        } /* for (counter=0; counter<32; counter++) */
    } /* if (AP_OK == ap_result) */


    /* ----------------------------------------------------------------------
     *  PV_CLUSTER
     * ----------------------------------------------------------------------
     */
    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF("PV_CLUSTER:\n");
        TCN_OSL_PRINTF("-----------");

        var_boolean1             = 0x55;
        pv_list1[0].p_variable   = &var_boolean1;
        pv_list1[0].derived_type = AP_DERIVED_TYPE_BOOLEAN1;
        pv_list1[0].array_count  = 1;
        pv_list1[0].octet_offset = 0;
        pv_list1[0].bit_number   = 4;

        var_word8                = 0xAA;
        pv_list1[1].p_variable   = &var_word8;
        pv_list1[1].derived_type = AP_DERIVED_TYPE_WORD8;
        pv_list1[1].array_count  = 1;
        pv_list1[1].octet_offset = 1;
        pv_list1[1].bit_number   = 0;

        ds_name.traffic_store_id = 0;
        ds_name.port_address     = 0x10;
        pv_set[0].p_pv_list      = pv_list1;
        pv_set[0].c_pv_list      = 2;
        pv_set[0].p_freshtime    = NULL;
        pv_set[0].dataset        = ds_name;

        var_word16               = 0xA1A2;
        pv_list2[0].p_variable   = &var_word16;
        pv_list2[0].derived_type = AP_DERIVED_TYPE_WORD16;
        pv_list2[0].array_count  = 1;
        pv_list2[0].octet_offset = 2;
        pv_list2[0].bit_number   = 0;

        ds_name.traffic_store_id = 0;
        ds_name.port_address     = 0x10;
        pv_set[1].p_pv_list      = pv_list2;
        pv_set[1].c_pv_list      = 1;
        pv_set[1].p_freshtime    = NULL;
        pv_set[1].dataset        = ds_name;

        pv_cluster = (PV_CLUSTER*)malloc((sizeof(PV_CLUSTER) * 2));
        if (NULL != pv_cluster)
        {
            pv_cluster->ts_id = 0;
            pv_cluster->c_pv_set = 2;
            pv_cluster->p_pv_set[0] = &pv_set[0];
            pv_cluster->p_pv_set[1] = &pv_set[1];

            ap_result = ap_put_cluster(pv_cluster);
            if (AP_OK == ap_result)
            {
                tcn_ap_test_var_check();
                TCN_OSL_PRINTF("\n");
            } /* if (AP_OK == ap_result) */
            free((void*)pv_cluster);
        } /* if (NULL != pv_cluster) */
    } /* if (AP_OK == ap_result) */

    if (AP_OK == ap_result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF \
           ("***********************************************************\n");
        TCN_OSL_PRINTF \
           ("* Test of Process Variables (AV) terminates successfully! *\n");
        TCN_OSL_PRINTF \
           ("***********************************************************\n");
    } /* if (AP_OK == ap_result) */
    else
    {
        TCN_OSL_PRINTF("ERROR\n");
        TCN_AP_TEST_VAR_ON_ERROR(ap_result);
    } /* else */

    return(ap_result);

} /* ap_test_var */


/* --------------------------------------------------------------------------
 *  Procedure : main
 * --------------------------------------------------------------------------
 */
/*int
main (void)
{
    AP_RESULT   result;

    result = ap_test_var();

    TCN_MACRO_TERMINATE_MAIN(result);

    return((int)result);

} *//* main */

#endif /* #ifdef TCN_AP_TEST_VAR */
