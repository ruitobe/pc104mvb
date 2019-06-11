#ifndef PD16_BC_H
#define PD16_BC_H

/* ==========================================================================
 *
 *  File      : PD16_BC.H
 *
 *  Purpose   : Bus Controller Link Layer for PD16 - Bus Controller Structure
 *              - UART: MVB UART Emulation 'PD16' with FIFO
 *              - NOTE: Standard access to UART registers.
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
 *  - PD16_BC_MUTEX_UART      - multi-thread OSL mutex for UART access
 *                              NOTE:
 *                              - if not specified, the pre-processor
 *                                definition will be defined and set to 1,
 *                                i.e. by default the OSL mutex is enabled
 *                              - disable the OSL mutex by setting the
 *                                pre-processor definition to 0
 *  - PD16_BC_ERRCNT_ALL_PD     - error counters for all process data ports
 *
 * ==========================================================================
 */


/* ==========================================================================
 *
 *  Project specific Definitions used for Conditional Compiling
 *
 * ==========================================================================
 */
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

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer
 * --------------------------------------------------------------------------
 */
#include <pd16_lp.h>

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>


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
#define PD16_BC_MUTEX_UART  2
 
#ifndef PD16_BC_MUTEX_UART
#   define PD16_BC_MUTEX_UART 1 /* by default the OSL mutex is enabled */
#endif


/* --------------------------------------------------------------------------
 *  Structured Type   : PD16_BUS_CTRL
 *
 *  Purpose           : Description of bus controller specific values.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          * -----------------------------------------------
 *                          *  UART (static values)
 *                          * -----------------------------------------------
 *                          WORD32      acc_reg_rbr_thr;
 *                          WORD32      acc_reg_lsr;
 *                          UNSIGNED32  acc_reg_size;
 *                          UNSIGNED32  acc_reg_timeout;
 *
 *                          * -----------------------------------------------
 *                          *  LS (static values)
 *                          * -----------------------------------------------
 *                          BOOLEAN1    ls_ds_capability;
 *                          BOOLEAN1    ls_ba_capability;
 *                          UNSIGNED16  ls_ba_list_entries;
 *
 *                          * -----------------------------------------------
 *                          *  MUE (dynamic values)
 *                          * -----------------------------------------------
 *                          MUE_RESULT  mue_result;
 *                          BITSET16    mue_sv_device_config;
 *
 *                          * -----------------------------------------------
 *                          *  Error Counters (dynamic values)
 *                          * -----------------------------------------------
 *                          UNSIGNED32  errcnt_global_lineA;
 *                          UNSIGNED32  errcnt_global_lineB;
 *
 *                          #ifdef PD16_BC_ERRCNT_ALL_PD
 *                              UNSIGNED32  errcnt_pd_port_lineA
 *                                              [PD16_LP_PORT_NUMBER_MAX];
 *                              UNSIGNED32  errcnt_pd_port_lineB
 *                                              [PD16_LP_PORT_NUMBER_MAX];
 *                          #endif
 *
 *                          * -----------------------------------------------
 *                          *  OSL (dynamic values)
 *                          * -----------------------------------------------
 *                          #if (PD16_BC_MUTEX_UART == 1)
 *                              BOOLEAN1        osl_mutex_status_uart;
 *                              pthread_mutex_t osl_mutex_object_uart;
 *                          #endif
 *
 *                          * -----------------------------------------------
 *                          *  LP (dynamic values)
 *                          * -----------------------------------------------
 *                          UNSIGNED16  lp_fsi;
 *                          WORD16      lp_address[PD16_LP_PORT_NUMBER_MAX];
 *                          UNSIGNED8   lp_size[PD16_LP_PORT_NUMBER_MAX];
 *                          UNSIGNED16  lp_fresh[PD16_LP_PORT_NUMBER_MAX];
 *
 *                      }   PD16_BUS_CTRL;
 *
 *  Element           : acc_reg_rbr_thr       - address of UART register
 *                                              RBR/THR
 *                      acc_reg_lsr           - address of UART register
 *                                              LSR
 *                      acc_reg_size          - size of a UART register
 *                                              (number of bytes)
 *                      acc_reg_timeout       - timeout by loop
 *                                              (~100us for PD16)
 *                      -----------------------------------------------------
 *                      ls_ds_capability      - device status capability:
 *                                              - FALSE - no   DS capability
 *                                              - TRUE  - with DS capability
 *                      ls_ba_capability      - bus administrator capability:
 *                                              - FALSE - no   BA capability
 *                                              - TRUE  - with BA capability
 *                      ls_ba_list_entries    - max. number of entries in
 *                                              the BA list
 *                      -----------------------------------------------------
 *                      mue_result            - last result of MUE if error
 *                      mue_sv_device_config  - last value of MUE SV device
 *                                              configuration
 *                      -----------------------------------------------------
 *                      errcnt_global_lineA   - counter incremented each time
 *                                              a PD/DS port is read with
 *                                              SINK_A=0, SINK_B=1;
 *                                              only in redundant line mode;
 *                                              this counter does not
 *                                              wraparound when reaching its
 *                                              highest value
 *                      errcnt_global_lineB   - counter incremented each time
 *                                              a PD/DS port is read with
 *                                              SINK_A=1, SINK_B=0;
 *                                              only in redundant line mode;
 *                                              this counter does not
 *                                              wraparound when reaching its
 *                                              highest value
 *                      -----------------------------------------------------
 *                      errcnt_pd_port_lineA/ - similar to
 *                       errcnt_pd_port_lineB   'errcnt_global_lineA/B' but
 *                                              for all process data ports
 *                      -----------------------------------------------------
 *                      osl_mutex_status_uart - status of the mutex;
 *                                              default=FALSE (=0)
 *                      osl_mutex_object_uart - mutex object
 *                      -----------------------------------------------------
 *                      lp_fsi                - last value of freshness
 *                                              supervision interval
 *                      lp_address            - last value of all port
 *                                              addresses
 *                                              (0xFFFF=undefined)
 *                      lp_size               - last value of all port sizes
 *                                              (0=undefined)
 *                      lp_fresh              - last freshness timer
 *                                              (default=0xFFFF)
 *
 *  Remarks           : Bus controller structure for:
 *                      - MVB UART Emulation 'PD16'
 * --------------------------------------------------------------------------
 */
typedef struct
{
    /* ----------------------------------------------------------------------
     *  UART (static values)
     * ----------------------------------------------------------------------
     */
    WORD32      acc_reg_rbr_thr;
    WORD32      acc_reg_lsr;
    UNSIGNED32  acc_reg_size;
    UNSIGNED32  acc_reg_timeout;

    /* ----------------------------------------------------------------------
     *  LS (static values)
     * ----------------------------------------------------------------------
     */
    BOOLEAN1    ls_ds_capability;
    BOOLEAN1    ls_ba_capability;
    UNSIGNED16  ls_ba_list_entries;

    /* ----------------------------------------------------------------------
     *  MUE (dynamic values)
     * ----------------------------------------------------------------------
     */
    MUE_RESULT  mue_result;
    BITSET16    mue_sv_device_config;

    /* ----------------------------------------------------------------------
     *  Error Counters (dynamic values)
     * ----------------------------------------------------------------------
     */
    /* error counters - global                                             */
    UNSIGNED32  errcnt_global_lineA;
    UNSIGNED32  errcnt_global_lineB;

#ifdef PD16_BC_ERRCNT_ALL_PD
    /* error counters - all process data ports                             */
    UNSIGNED32  errcnt_pd_port_lineA[PD16_LP_PORT_NUMBER_MAX];
    UNSIGNED32  errcnt_pd_port_lineB[PD16_LP_PORT_NUMBER_MAX];
#endif /* #ifdef PD16_BC_ERRCNT_ALL_PD */

    /* ----------------------------------------------------------------------
     *  OSL (dynamic values)
     * ----------------------------------------------------------------------
     */
#if (PD16_BC_MUTEX_UART == 1)
    BOOLEAN1        osl_mutex_status_uart;
    pthread_mutex_t osl_mutex_object_uart;
#endif /* #if (PD16_BC_MUTEX_UART == 1) */

    /* ----------------------------------------------------------------------
     *  LS (dynamic values)
     * ----------------------------------------------------------------------
     */
    UNSIGNED32  ls_lineA_errors;
    UNSIGNED32  ls_lineB_errors;

    /* ----------------------------------------------------------------------
     *  LP (dynamic values)
     * ----------------------------------------------------------------------
     */
    UNSIGNED16  lp_fsi;
    WORD16      lp_address[PD16_LP_PORT_NUMBER_MAX];
    UNSIGNED8   lp_size[PD16_LP_PORT_NUMBER_MAX];
    UNSIGNED16  lp_fresh[PD16_LP_PORT_NUMBER_MAX];

}   PD16_BUS_CTRL;


#endif /* !PD16_BC_H */
