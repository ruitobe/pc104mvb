#ifndef MUE_ACC_INL
#define MUE_ACC_INL

/* ==========================================================================
 *
 *  File      : MUE_ACC.INL
 *
 *  Purpose   : MVB UART Emulation - Access Interface
 *              - UART: MVB UART Emulation with FIFO (e.g. MDFULL)
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
 *  - MUE_ACC_HW_LE   - the MVB UART Emulation of the MVB controller / 
 *                      MVB interface board is configured to handle words
 *                      (16-bit values) with little-endian number
 *                      representation; this applies to parameters and data
 *                      of all MVB UART Emulation commands
 *                      (default is big-endian number representation)
 *  - MUE_ACC_PRINT   - debug printouts
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
 *  Definitions
 *
 * ==========================================================================
 */
#ifndef MUE_ACC_DECL_INLINE
#   define MUE_ACC_DECL_INLINE TCN_DECL_INLINE
#endif

#if defined (MUE_ACC_HW_LE) && defined (TCN_LE)
#   undef  MUE_ACC_BYTE_SWAP
#endif
#if defined (MUE_ACC_HW_LE) && !defined (TCN_LE)
#   define MUE_ACC_BYTE_SWAP
#endif
#if !defined (MUE_ACC_HW_LE) && defined (TCN_LE)
#   define MUE_ACC_BYTE_SWAP
#endif
#if !defined (MUE_ACC_HW_LE) && !defined (TCN_LE)
#   undef  MUE_ACC_BYTE_SWAP
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

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>
#ifdef MUE_ACC_PRINT
#   include <mue_osl.h>
#endif
#include <mue_bc.h>
#include <mue_accr.h>


/* ==========================================================================
 *
 *  Local Definitions (general)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Bit constants : mue_acc_uart_reg_lsr_bit_xxx
 *
 *  Purpose       : Defines the bits of the UART register LSR.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_acc_uart_reg_lsr_bit_dr        = 0x01;
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_acc_uart_reg_lsr_bit_thre      = 0x20;
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_acc_uart_reg_lsr_bit_temt      = 0x40;
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_acc_uart_reg_lsr_bit_thre_temt = 0x60;


/* ==========================================================================
 *
 *  Public Procedures (general)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_timeout_calibrate
 *
 *  Purpose   : Calibrate the timeout value for getting ready status bits
 *              of the UART register LSR.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_timeout_calibrate
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED32  timeout_cal
 *              );
 *
 *  Input     : p_bus_ctrl  - pointer to bus controller specific values
 *              timeout_cal - timeout value used for calibration
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 *              - The structure of this procedure is the same like
 *                "mue_acc_tx_start()" and "mue_acc_rx_wait()".
 *              - For calibration of the timeout value the condition for
 *                UART register LSR will never hit.
 *                This will trigger the timeout.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_timeout_calibrate
(
    void        *p_bus_ctrl,
    UNSIGNED32  timeout_cal
)
{
    MUE_RESULT  mue_result;
    WORD8       reg_lsr;
    UNSIGNED32  timeout;
    UNSIGNED32  timeout_max;

    reg_lsr     = 0;
    timeout     = 0;
    timeout_max = timeout_cal;

    /* ----------------------------------------------------------------------
     *  NOTES:
     *  This procedure is used to simulate procedures "mue_acc_tx_start"
     *  and "mue_acc_rx_wait". Both procedures are polling the UART register
     *  LSR until a specific bit is set or timeout.
     *  For calibration of the timeout value the condition
     *  (reg_lsr != mue_acc_uart_reg_lsr_bit_thre) will never hit.
     *  This will cause a timeout error (i.e. MUE_RESULT_TIMEOUT).
     * ----------------------------------------------------------------------
     */
    while ((reg_lsr != mue_acc_uart_reg_lsr_bit_thre) && \
           (timeout < timeout_max))
    {
        /* get value from UART register LSR */
        reg_lsr = mue_accr_get_reg_lsr(p_bus_ctrl);

        /* extract bit DR from UART register LSR */
        reg_lsr &= mue_acc_uart_reg_lsr_bit_dr;

        timeout++;
    } /* while ((...) && (...)) */

    if (reg_lsr == mue_acc_uart_reg_lsr_bit_thre)
    {
        mue_result = MUE_RESULT_OK;
    } /* if (reg_lsr == mue_acc_uart_reg_lsr_bit_thre) */
    else
    {
        mue_result = MUE_RESULT_TIMEOUT;
    } /* else */

    return(mue_result);

} /* mue_acc_timeout_calibrate */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_init
 *
 *  Purpose   : Initialises the access to the UART (check for availability).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_init
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_init
(
    void        *p_bus_ctrl
)
{
    MUE_RESULT  mue_result;
#ifdef MUE_ACC_PRINT
    UNSIGNED16  c;
#endif /* #ifdef MUE_ACC_PRINT */
    WORD8       reg_val;
    WORD8       reg_lsr;


#ifdef MUE_ACC_PRINT
    /* ----------------------------------------------------------------------
     *  print access info (before 'mue_accr_init')
     * ----------------------------------------------------------------------
     */
    MUE_OSL_PRINTF("MUE_ACCR_INFO before 'mue_accr_init':\n");
    MUE_OSL_PRINTF("- acc_reg_rbr_thr=0x%08lX\n", \
        ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_rbr_thr);
    MUE_OSL_PRINTF("- acc_reg_lsr    =0x%08lX\n", \
        ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_lsr);
    MUE_OSL_PRINTF("- acc_reg_size   =%ld\n", \
        ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_size);
    MUE_OSL_PRINTF("- acc_reg_timeout=%ld\n", \
        ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_timeout);
#endif /* #ifdef MUE_ACC_PRINT */


    mue_result = mue_accr_init(p_bus_ctrl);
    if (MUE_RESULT_OK == mue_result)
    {
#ifdef MUE_ACC_PRINT
        /* ------------------------------------------------------------------
         *  print access info (after 'mue_accr_init')
         * ------------------------------------------------------------------
         */
        MUE_OSL_PRINTF("MUE_ACCR_INFO after 'mue_accr_init':\n");
        MUE_OSL_PRINTF("- acc_reg_rbr_thr=0x%08lX\n", \
            ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_rbr_thr);
        MUE_OSL_PRINTF("- acc_reg_lsr    =0x%08lX\n", \
            ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_lsr);
        MUE_OSL_PRINTF("- acc_reg_size   =%ld\n", \
            ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_size);
        MUE_OSL_PRINTF("- acc_reg_timeout=%ld\n", \
            ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_timeout);
        /* ------------------------------------------------------------------
         *  print value from UART registers (index=0-7)
         * ------------------------------------------------------------------
         */
        MUE_OSL_PRINTF("Regs=");
        for (c=0; c<8; c++)
        {
            reg_val = mue_accr_get_reg(p_bus_ctrl, c);
            MUE_OSL_PRINTF("0x%02X ", reg_val);
        } /* for (c=0; c<8; c++) */
        MUE_OSL_PRINTF("\n");
#endif /* #ifdef MUE_ACC_PRINT */


        /* ------------------------------------------------------------------
         *  check value of UART register LSR
         * ------------------------------------------------------------------
         */
        /* get value from UART register (index=5) */
        reg_val = mue_accr_get_reg(p_bus_ctrl, 5);
#ifdef MUE_ACC_PRINT
        MUE_OSL_PRINTF("Reg5=0x%02X ? ", reg_val);
#endif /* #ifdef MUE_ACC_PRINT */

        /* get value from UART register LSR (index=5) */
        reg_lsr = mue_accr_get_reg_lsr(p_bus_ctrl);
#ifdef MUE_ACC_PRINT
        MUE_OSL_PRINTF("RegLSR=0x%02X ", reg_lsr);
#endif /* #ifdef MUE_ACC_PRINT */

        if (reg_val == reg_lsr)
        {
#ifdef MUE_ACC_PRINT
            MUE_OSL_PRINTF("-> OK\n");
#endif /* #ifdef MUE_ACC_PRINT */
        } /* if (reg_val == reg_lsr) */
        else
        {
#ifdef MUE_ACC_PRINT
            MUE_OSL_PRINTF("-> ERROR\n");
#endif /* #ifdef MUE_ACC_PRINT */
            mue_result = MUE_RESULT_ERROR;
        } /* else */
    } /* if (MUE_RESULT_OK == mue_result) */

    if (MUE_RESULT_OK == mue_result)
    {
        /* mask bit DR of UART register LSR */
        reg_lsr &= ~mue_acc_uart_reg_lsr_bit_dr;
        /* check, if bits THRE and TEMT are set (all other 0) */
        if (reg_lsr != mue_acc_uart_reg_lsr_bit_thre_temt)
        {
#ifdef MUE_ACC_PRINT
            MUE_OSL_PRINTF("ERROR: RegLSR=0x%02X\n", reg_lsr);
#endif /* #ifdef MUE_ACC_PRINT */
            mue_result = MUE_RESULT_ERROR;
        } /* if (reg_lsr != mue_acc_uart_reg_lsr_bit_thre_temt) */
    } /* if (MUE_RESULT_OK == mue_result) */


    return(mue_result);

} /* mue_acc_init */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_clean_up
 *
 *  Purpose   : Perform a clean-up of the UART communication
 *              (synchronise access protocol of the MVB UART Emulation).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_clean_up
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - If a process using the MVB UART Emulation was interrupted
 *                the access protocol of the MVB UART Emulation remains in
 *                one of the following three states (Sx):
 *                S1: The MVB UART Emulation is ready to receive a new
 *                    command character -> nothing must be done
 *                    * any transmitted data bytes with value 0x00 (see
 *                      state S2) will be ignored
 *                S2: A MVB UART Emulation command is still in progress and
 *                    requests more data bytes (in minimum the command
 *                    character was sent) -> synchronisation is necessary
 *                    * transmits 34 data bytes with value 0x00 (command 'P'
 *                      requests the max. number of data bytes; PD_16=33,
 *                      PD_16F=33, PD_FULL=34, i.e. 34 data bytes); after
 *                      this the MVB UART Emulation may enter the state S3
 *                S3: A MVB UART Emulation command is still in progress and
 *                    transmits more data bytes -> synchronisation is
 *                    necessary
 *                    * transmit any command character which flushes the
 *                      transmit queue of the MVB UART Emulation
 *                    or (alternative)
 *                    * receive until nothing left (command 'G' transmits
 *                      the max. number of data bytes; PD_16=33, PD_16F=35,
 *                      PD_FULL=35, i.e. 35 data bytes)
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_clean_up
(
    void        *p_bus_ctrl
)
{
    MUE_RESULT  mue_result = MUE_RESULT_OK;
    UNSIGNED16  c;
    WORD8       reg_lsr;

    /* ----------------------------------------------------------------------
     *  transmit 34 bytes with value 0x00
     * ----------------------------------------------------------------------
     */
    for (c=0; c<34; c++)
    {
        mue_accr_put_reg_thr(p_bus_ctrl, 0x00);
    } /* for (c=0; c<34; c++) */

    /* ----------------------------------------------------------------------
     *  check, if access protocol is ready
     * ----------------------------------------------------------------------
     */
    /* get value from UART register LSR */
    reg_lsr = mue_accr_get_reg_lsr(p_bus_ctrl);
    /* check, if bits THRE and TEMT are set (all other 0) */
    if (reg_lsr != mue_acc_uart_reg_lsr_bit_thre_temt)
    {
        mue_result = MUE_RESULT_ERROR;
    } /* if (reg_lsr != mue_acc_uart_reg_lsr_bit_thre_temt) */

    return(mue_result);

} /* mue_acc_clean_up */


/* ==========================================================================
 *
 *  Public Procedures for Transmit (TX)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_start
 *
 *  Purpose   : Starts a transmission over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_tx_start
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - number of bytes to transmit over the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_tx_start
(
    void        *p_bus_ctrl,
    UNSIGNED16  size
)
{
    MUE_RESULT  mue_result;
    WORD8       reg_lsr;
    UNSIGNED32  timeout;
    UNSIGNED32  timeout_max;

    /* avoid warnings */
    size = size;

    reg_lsr     = 0;
    timeout     = 0;
    timeout_max = ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_timeout;

    while ((reg_lsr != mue_acc_uart_reg_lsr_bit_thre) && \
           (timeout < timeout_max))
    {
        /* get value from UART register LSR */
        reg_lsr = mue_accr_get_reg_lsr(p_bus_ctrl);

        /* extract bit THRE from UART register LSR */
        reg_lsr &= mue_acc_uart_reg_lsr_bit_thre;

        timeout++;
    } /* while ((...) && (...)) */

    if (reg_lsr == mue_acc_uart_reg_lsr_bit_thre)
    {
        mue_result = MUE_RESULT_OK;
    } /* if (reg_lsr == mue_acc_uart_reg_lsr_bit_thre) */
    else
    {
        mue_result = MUE_RESULT_TIMEOUT;
    } /* else */

    return(mue_result);

} /* mue_acc_tx_start */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_value8
 *
 *  Purpose   : Transmits a 8-bit value over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_tx_value8
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD8       value8
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              value8     - 8-bit value to transmit over the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for general parameters.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_tx_value8
(
    void        *p_bus_ctrl,
    WORD8       value8
)
{

    mue_accr_put_reg_thr(p_bus_ctrl, value8);

    return(MUE_RESULT_OK);

} /* mue_acc_tx_value8 */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_value16
 *
 *  Purpose   : Transmits a 16-bit value over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_tx_value16
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      value16
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              value16    - 16-bit value to transmit over the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for general parameters.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_tx_value16
(
    void        *p_bus_ctrl,
    WORD16      value16
)
{
    WORD8       *p_word8 = (WORD8*)&value16;

#ifdef MUE_ACC_BYTE_SWAP

    /* first low then high byte */
    mue_accr_put_reg_thr(p_bus_ctrl, p_word8[1]);
    mue_accr_put_reg_thr(p_bus_ctrl, p_word8[0]);

#else /* #ifdef MUE_ACC_BYTE_SWAP */

    /* first high then low byte */
    mue_accr_put_reg_thr(p_bus_ctrl, *p_word8++);
    mue_accr_put_reg_thr(p_bus_ctrl, *p_word8);

#endif /* #else */

    return(MUE_RESULT_OK);

} /* mue_acc_tx_value16 */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_array
 *
 *  Purpose   : Transmits data of a buffer over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_tx_array
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size,
 *                  void        *p_buffer
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - size of the buffer (number of bytes to
 *                           transmit over the UART)
 *              p_buffer   - pointer to buffer that contains data
 *                           to transmit over the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for data related to a MVB process data port
 *                (size = 2, 4, 8, 16 or 32 bytes).
 *              - Used for data related to a MVB message data frame
 *                (size = 32 bytes).
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_tx_array
(
    void        *p_bus_ctrl,
    UNSIGNED16  size,
    void        *p_buffer
)
{
    UNSIGNED16  i;
    WORD8       *p_word8 = (WORD8*)p_buffer;

#ifdef MUE_ACC_HW_LE

    for (i=0; i<size; i+=2)
    {
        mue_accr_put_reg_thr(p_bus_ctrl, p_word8[i+1]); /* low  byte */
        mue_accr_put_reg_thr(p_bus_ctrl, p_word8[i+0]); /* high byte */
    } /* for (i=0; i<size; i+=2) */

#else /* #ifdef MUE_ACC_HW_LE */

    for (i=0; i<size; i+=2)
    {
        mue_accr_put_reg_thr(p_bus_ctrl, *p_word8++);   /* high byte */
        mue_accr_put_reg_thr(p_bus_ctrl, *p_word8++);   /* low  byte */
    } /* for (i=0; i<size; i+=2) */

#endif /* #else */

    return(MUE_RESULT_OK);

} /* mue_acc_tx_array */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_fill
 *
 *  Purpose   : Transmits fill bytes over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_tx_fill
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - number of fill bytes to transmit over the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for fill bytes related to a MVB process data port
 *                (size = 0, 16, 24, 28 or 30 bytes).
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_tx_fill
(
    void        *p_bus_ctrl,
    UNSIGNED16  size
)
{
    UNSIGNED16  i;

    for (i=0; i<size; i++)
    {
        mue_accr_put_reg_thr(p_bus_ctrl, 0x00);
    } /* for (i=0; i<size; i++) */

    return(MUE_RESULT_OK);

} /* mue_acc_tx_fill */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_trigger
 *
 *  Purpose   : Triggers a transmission over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_tx_trigger
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_tx_trigger
(
    void        *p_bus_ctrl
)
{

    /* avoid warnings */
    p_bus_ctrl = p_bus_ctrl;

    return(MUE_RESULT_OK);

} /* mue_acc_tx_trigger */


/* ==========================================================================
 *
 *  Public Procedures for Receive (RX)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_wait
 *
 *  Purpose   : Waits, until something to receive from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_rx_wait
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - number of bytes to receive from the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_rx_wait
(
    void        *p_bus_ctrl,
    UNSIGNED16  size
)
{
    MUE_RESULT  mue_result;
    WORD8       reg_lsr;
    UNSIGNED32  timeout;
    UNSIGNED32  timeout_max;

    if (0 == size)
    {
        mue_result = MUE_RESULT_OK;
    } /* if (0 == size) */
    else
    {
        reg_lsr     = 0;
        timeout     = 0;
        timeout_max = ((MUE_BUS_CTRL*)p_bus_ctrl)->acc_reg_timeout;

        while ((reg_lsr != mue_acc_uart_reg_lsr_bit_dr) && \
               (timeout < timeout_max))
        {
            /* get value from UART register LSR */
            reg_lsr = mue_accr_get_reg_lsr(p_bus_ctrl);

            /* extract bit DR from UART register LSR */
            reg_lsr &= mue_acc_uart_reg_lsr_bit_dr;

            timeout++;
        } /* while ((...) && (...)) */

        if (reg_lsr == mue_acc_uart_reg_lsr_bit_dr)
        {
            mue_result = MUE_RESULT_OK;
        } /* if (reg_lsr == mue_acc_uart_reg_lsr_bit_dr) */
        else
        {
            mue_result = MUE_RESULT_TIMEOUT;
        } /* else */
    } /* else */

    return(mue_result);

} /* mue_acc_rx_wait */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_value8
 *
 *  Purpose   : Receives a 8-bit value from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_rx_value8
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD8       *p_value8
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Output    : p_value8   - pointer to buffer that receives a 8-bit value
 *                           from the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for general parameters.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_rx_value8
(
    void        *p_bus_ctrl,
    WORD8       *p_value8
)
{

    *p_value8 = mue_accr_get_reg_rbr(p_bus_ctrl);

    return(MUE_RESULT_OK);

} /* mue_acc_tx_value8 */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_value16
 *
 *  Purpose   : Receives a 16-bit value from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_rx_value16
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      *p_value16
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Output    : p_value16  - pointer to buffer that receives a 16-bit value
 *                             from the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for general parameters.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_rx_value16
(
    void        *p_bus_ctrl,
    WORD16      *p_value16
)
{
    WORD8       *p_word8 = (WORD8*)p_value16;

#ifdef MUE_ACC_BYTE_SWAP

    /* first low then high byte */
    p_word8[1] = mue_accr_get_reg_rbr(p_bus_ctrl);
    p_word8[0] = mue_accr_get_reg_rbr(p_bus_ctrl);

#else /* #ifdef MUE_ACC_BYTE_SWAP */

    /* first high then low byte */
    *p_word8++ = mue_accr_get_reg_rbr(p_bus_ctrl);
    *p_word8   = mue_accr_get_reg_rbr(p_bus_ctrl);

#endif /* #else */

    return(MUE_RESULT_OK);

} /* mue_acc_rx_value16 */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_array
 *
 *  Purpose   : Receives data into a buffer from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_rx_array
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size,
 *                  void        *p_buffer
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - size of the buffer (number of bytes to receive
 *                           from the UART)
 *
 *  Output    : p_buffer   - pointer to buffer that receives data from
 *                           the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for data related to a MVB process data port
 *                (size = 2, 4, 8, 16 or 32 bytes).
 *              - Used for data related to a MVB message data frame
 *                (size = 32 bytes).
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_rx_array
(
    void        *p_bus_ctrl,
    UNSIGNED16  size,
    void        *p_buffer
)
{
    UNSIGNED16  i;
    WORD8       *p_word8 = (WORD8*)p_buffer;

#ifdef MUE_ACC_HW_LE

    for (i=0; i<size; i+=2)
    {
        p_word8[i+1] = mue_accr_get_reg_rbr(p_bus_ctrl); /* low  byte */
        p_word8[i+0] = mue_accr_get_reg_rbr(p_bus_ctrl); /* high byte */
    } /* for (i=0; i<size; i+=2) */

#else /* #ifdef MUE_ACC_HW_LE */

    for (i=0; i<size; i+=2)
    {
        *p_word8++ = mue_accr_get_reg_rbr(p_bus_ctrl);   /* high byte */
        *p_word8++ = mue_accr_get_reg_rbr(p_bus_ctrl);   /* low  byte */
    } /* for (i=0; i<size; i+=2) */

#endif /* #else */

    return(MUE_RESULT_OK);

} /* mue_acc_rx_array */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_fill
 *
 *  Purpose   : Receives fill bytes from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_rx_fill
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - number of fill bytes to receive from the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for fill bytes related to a MVB process data port
 *                (size = 0, 16, 24, 28 or 30 bytes).
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_rx_fill
(
    void        *p_bus_ctrl,
    UNSIGNED16  size
)
{
    UNSIGNED16  i;

    for (i=0; i<size; i++)
    {
        mue_accr_get_reg_rbr(p_bus_ctrl);
    } /* for (i=0; i<size; i++) */

    return(MUE_RESULT_OK);

} /* mue_acc_rx_fill */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_ignore
 *
 *  Purpose   : Ignore surplus data (fill bytes) from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_rx_ignore
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - number of fill bytes to ignore from the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for surplus bytes related to a MVB device status or
 *                a MVB process data port.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_rx_ignore
(
    void        *p_bus_ctrl,
    UNSIGNED16  size
)
{

    /* avoid warnings */
    p_bus_ctrl = p_bus_ctrl;
    size       = size;

    return(MUE_RESULT_OK);

} /* mue_acc_rx_ignore */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_close
 *
 *  Purpose   : Closes a receive block from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_ACC_DECL_INLINE
 *              MUE_RESULT
 *              mue_acc_rx_close
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_ACC_DECL_INLINE
MUE_RESULT
mue_acc_rx_close
(
    void        *p_bus_ctrl
)
{

    /* avoid warnings */
    p_bus_ctrl = p_bus_ctrl;

    return(MUE_RESULT_OK);

} /* mue_acc_rx_close */


#endif /* #ifndef MUE_ACC_INL */
