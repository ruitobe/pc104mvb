/* ==========================================================================
 *
 *  File      : MUE_MD.C
 *
 *  Purpose   : MVB UART Emulation - Message Data Interface (MD)
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

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>
#include <mue_osl.h>
#include <mue_acc.h>
#include <mue_md.h>


/* ==========================================================================
 *
 *  Public Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Bits Constants : mue_md_control_bit_xxx
 *
 *  Purpose        : MVB message data control bits
 *
 *  Remarks        : - See also bits constants 'MUE_MD_CONTROL_BIT_xxx'.
 *                   - Used for parameter 'control' of procedure
 *                     'mue_md_control_and_status'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_CONST \
    BITSET8 mue_md_control_bit_ftx        = MUE_MD_CONTROL_BIT_FTX;
TCN_DECL_CONST \
    BITSET8 mue_md_control_bit_frx        = MUE_MD_CONTROL_BIT_FRX;
TCN_DECL_CONST \
    BITSET8 mue_md_control_bit_ierx       = MUE_MD_CONTROL_BIT_IERX;
TCN_DECL_CONST \
    BITSET8 mue_md_control_bit_ietx       = MUE_MD_CONTROL_BIT_IETX;
TCN_DECL_CONST \
    BITSET8 mue_md_control_bit_unf        = MUE_MD_CONTROL_BIT_UNF;
TCN_DECL_CONST \
    BITSET8 mue_md_control_bit_reset_mask = MUE_MD_CONTROL_BIT_RESET_MASK;
TCN_DECL_CONST \
    BITSET8 mue_md_control_bit_reset_rxov = MUE_MD_CONTROL_BIT_RESET_RXOV;
TCN_DECL_CONST \
    BITSET8 mue_md_control_bit_reset_txac = MUE_MD_CONTROL_BIT_RESET_TXAC;
TCN_DECL_CONST \
    BITSET8 mue_md_control_bit_reset_rxac = MUE_MD_CONTROL_BIT_RESET_RXAC;


/* --------------------------------------------------------------------------
 *  Bits Constants : mue_md_status_bit_xxx
 *
 *  Purpose        : MVB message data status bits
 *
 *  Remarks        : - See also bits constants 'MUE_MD_STATUS_BIT_xxx'.
 *                   - Used for parameter 'p_status' of procedure
 *                     'mue_md_control_and_status'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_CONST \
    BITSET8 mue_md_status_bit_txqf = MUE_MD_STATUS_BIT_TXQF;
TCN_DECL_CONST \
    BITSET8 mue_md_status_bit_ierx = MUE_MD_STATUS_BIT_IERX;
TCN_DECL_CONST \
    BITSET8 mue_md_status_bit_ietx = MUE_MD_STATUS_BIT_IETX;
TCN_DECL_CONST \
    BITSET8 mue_md_status_bit_unf  = MUE_MD_STATUS_BIT_UNF;
TCN_DECL_CONST \
    BITSET8 mue_md_status_bit_rxov = MUE_MD_STATUS_BIT_RXOV;
TCN_DECL_CONST \
    BITSET8 mue_md_status_bit_txac = MUE_MD_STATUS_BIT_TXAC;
TCN_DECL_CONST \
    BITSET8 mue_md_status_bit_rxac = MUE_MD_STATUS_BIT_RXAC;


/* ==========================================================================
 *
 *  Local Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Constants : mue_md_command_xxx
 *
 *  Purpose   : Message data commands.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_md_command_control_and_status = 'M'; /* 0x4D */
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_md_command_transmit           = 'T'; /* 0x54 */
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_md_command_receive            = 'R'; /* 0x52 */

/* --------------------------------------------------------------------------
 *  Constants : mue_md_size_xxx
 *
 *  Purpose   : Message data command sizes.
 * --------------------------------------------------------------------------
 */
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_md_size_tx_control_and_status = 2;
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_md_size_rx_control_and_status = 1;

TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_md_size_tx_transmit           = 33;
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_md_size_rx_transmit           = 0;

TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_md_size_tx_receive            = 1;
TCN_DECL_LOCAL TCN_DECL_CONST \
    WORD8 mue_md_size_rx_receive            = 32;


/* ==========================================================================
 *
 *  Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_md_control_and_status
 *
 *  Purpose   : First puts MVB message data related control information to
 *              a MVB controller,
 *              then gets MVB message data status from a MVB controller.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_md_control_and_status
 *              (
 *                  void        *p_bus_ctrl,
 *                  BITSET8     control,
 *                  BITSET8     *p_status
 *              );
 *
 *  Input     : p_bus_ctrl    - pointer to bus controller specific values
 *              control       - control information; any combination
 *                              of bit constants 'MUE_MD_CONTROL_BIT_xxx'
 *                              or bit constants 'mue_md_control_bit_xxx'
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Output    : p_status      - pointer to buffer that receives the status;
 *                              any combination of bit constants
 *                              'MUE_MD_STATUS_BIT_xxx' or bit constants
 *                              'mue_md_status_bit_xxx'
 *
 *  Remarks   : - See MVB UART Emulation command 'M'.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_md_control_and_status
(
    void        *p_bus_ctrl,
    BITSET8     control,
    BITSET8     *p_status
)
{
    MUE_RESULT  mue_result;

    /* ----------------------------------------------------------------------
     *  TX
     * ----------------------------------------------------------------------
     */
    mue_result = \
        mue_acc_tx_start(p_bus_ctrl, mue_md_size_tx_control_and_status);
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = \
            mue_acc_tx_value8(p_bus_ctrl, mue_md_command_control_and_status);
    } /* if (MUE_RESULT_OK == mue_result) */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_tx_value8(p_bus_ctrl, control);
    } /* if (MUE_RESULT_OK == mue_result) */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_tx_trigger(p_bus_ctrl);
    } /* if (MUE_RESULT_OK == mue_result) */

    /* ----------------------------------------------------------------------
     *  RX
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = \
            mue_acc_rx_wait(p_bus_ctrl, mue_md_size_rx_control_and_status);
    } /* if (MUE_RESULT_OK == mue_result) */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_rx_value8(p_bus_ctrl, p_status);
    } /* if (MUE_RESULT_OK == mue_result) */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_rx_close(p_bus_ctrl);
    } /* if (MUE_RESULT_OK == mue_result) */

    return(mue_result);

} /* mue_md_control_and_status */


/* --------------------------------------------------------------------------
 *  Procedure : mue_md_transmit
 *
 *  Purpose   : Puts data related to a MVB message data frame to the
 *              transmit queue of a MVB controller.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_md_transmit
 *              (
 *                  void        *p_bus_ctrl,
 *                  void        *p_frame
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              p_frame    - pointer to buffer that contains the data
 *                           related to a MVB message data frame (32 bytes)
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - See MVB UART Emulation command 'T'.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 *              - The MVB UART Emulation handles the data of a MVB message
 *                data frame (parameter 'p_frame') as an array of WORD16.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_md_transmit
(
    void        *p_bus_ctrl,
    void        *p_frame
)
{
    MUE_RESULT  mue_result;

    /* ----------------------------------------------------------------------
     *  TX
     * ----------------------------------------------------------------------
     */
    mue_result = mue_acc_tx_start(p_bus_ctrl, mue_md_size_tx_transmit);
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_tx_value8(p_bus_ctrl, mue_md_command_transmit);
    } /* if (MUE_RESULT_OK == mue_result) */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_tx_array(p_bus_ctrl, 32, p_frame);
    } /* if (MUE_RESULT_OK == mue_result) */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_tx_trigger(p_bus_ctrl);
    } /* if (MUE_RESULT_OK == mue_result) */

    /* ----------------------------------------------------------------------
     *  RX
     * ----------------------------------------------------------------------
     */
    /* NOTE: nothing to receive */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_rx_wait(p_bus_ctrl, mue_md_size_rx_transmit);
    } /* if (MUE_RESULT_OK == mue_result) */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_rx_close(p_bus_ctrl);
    } /* if (MUE_RESULT_OK == mue_result) */

    return(mue_result);

} /* mue_md_transmit */


/* --------------------------------------------------------------------------
 *  Procedure : mue_md_receive
 *
 *  Purpose   : Gets data related to a MVB message data frame from the
 *              receive queue of a MVB controller.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_md_receive
 *              (
 *                  void        *p_bus_ctrl,
 *                  void        *p_frame
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Output    : p_frame    - pointer to buffer that receives the data
 *                           related to a MVB message data frame (32 bytes)
 *
 *  Remarks   : - See MVB UART Emulation command 'R'.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 *              - The MVB UART Emulation handles the data of a MVB message
 *                data frame (parameter 'p_frame') as an array of WORD16.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_md_receive
(
    void        *p_bus_ctrl,
    void        *p_frame
)
{
    MUE_RESULT  mue_result;

    /* ----------------------------------------------------------------------
     *  TX
     * ----------------------------------------------------------------------
     */
    mue_result = mue_acc_tx_start(p_bus_ctrl, mue_md_size_tx_receive);
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_tx_value8(p_bus_ctrl, mue_md_command_receive);
    } /* if (MUE_RESULT_OK == mue_result) */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_tx_trigger(p_bus_ctrl);
    } /* if (MUE_RESULT_OK == mue_result) */

    /* ----------------------------------------------------------------------
     *  RX
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_rx_wait(p_bus_ctrl, mue_md_size_rx_receive);
    } /* if (MUE_RESULT_OK == mue_result) */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_rx_array(p_bus_ctrl, 32, p_frame);
    } /* if (MUE_RESULT_OK == mue_result) */
    if (MUE_RESULT_OK == mue_result)
    {
        mue_result = mue_acc_rx_close(p_bus_ctrl);
    } /* if (MUE_RESULT_OK == mue_result) */

    return(mue_result);

} /* mue_md_receive */
