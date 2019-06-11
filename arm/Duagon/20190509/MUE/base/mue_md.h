#ifndef MUE_MD_H
#define MUE_MD_H

/* ==========================================================================
 *
 *  File      : MUE_MD.H
 *
 *  Purpose   : MVB UART Emulation - Message Data Interface (PD)
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


/* ==========================================================================
 *
 *  Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Bits Constants : MUE_MD_CONTROL_BIT_xxx, mue_md_control_bit_xxx
 *
 *  Purpose        : MVB message data control bits
 *
 *  Remarks        : - Used for parameter 'control' of procedure
 *                     'mue_md_control_and_status'.
 * --------------------------------------------------------------------------
 */
#define MUE_MD_CONTROL_BIT_FTX        0x80 /* flush transmit queue         */
#define MUE_MD_CONTROL_BIT_FRX        0x40 /* flush receive  queue         */
#define MUE_MD_CONTROL_BIT_IERX       0x20 /* enable RX interrupt          */
#define MUE_MD_CONTROL_BIT_IETX       0x10 /* enable TX interrupt          */
#define MUE_MD_CONTROL_BIT_UNF        0x08 /* unfiltered (1=filter packet) */
#define MUE_MD_CONTROL_BIT_RESET_MASK 0x07 /* mask of the following bits   */
#define MUE_MD_CONTROL_BIT_RESET_RXOV 0x04 /* clear RXOV after status read */
#define MUE_MD_CONTROL_BIT_RESET_TXAC 0x02 /* clear TXAC after status read */
#define MUE_MD_CONTROL_BIT_RESET_RXAC 0x01 /* clear RXAC after status read */

TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_control_bit_ftx;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_control_bit_frx;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_control_bit_ierx;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_control_bit_ietx;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_control_bit_unf;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_control_bit_reset_mask;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_control_bit_reset_rxov;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_control_bit_reset_txac;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_control_bit_reset_rxac;


/* --------------------------------------------------------------------------
 *  Bits Constants : MUE_MD_STATUS_BIT_xxx, mue_md_status_bit_xxx
 *
 *  Purpose        : MVB message data status bits
 *
 *  Remarks        : - Used for parameter 'p_status' of procedure
 *                     'mue_md_control_and_status'.
 * --------------------------------------------------------------------------
 */
#define MUE_MD_STATUS_BIT_TXQF        0x80 /* transmit queue full          */
#define MUE_MD_STATUS_BIT_IERX        0x20 /* RX interrupt enabled         */
#define MUE_MD_STATUS_BIT_IETX        0x10 /* TX interrupt enabled         */
#define MUE_MD_STATUS_BIT_UNF         0x08 /* unfiltered (1=filter packet) */
#define MUE_MD_STATUS_BIT_RXOV        0x04 /* receive queue overflow       */
#define MUE_MD_STATUS_BIT_TXAC        0x02 /* MD packet has been sent      */
#define MUE_MD_STATUS_BIT_RXAC        0x01 /* MD packet has been received  */

TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_status_bit_txqf;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_status_bit_ierx;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_status_bit_ietx;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_status_bit_unf;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_status_bit_rxov;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_status_bit_txac;
TCN_DECL_PUBLIC TCN_DECL_CONST BITSET8 mue_md_status_bit_rxac;


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
);


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
);


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
);


#endif /* #ifndef MUE_MD_H */
