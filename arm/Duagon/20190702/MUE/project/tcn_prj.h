#ifndef TCN_PRJ_H
#define TCN_PRJ_H

/* ==========================================================================
 *
 *  File      : TCN_PRJ.H
 *
 *  Purpose   : Project specific Definitions
 *              - HOST: PC
 *              - OS  : DOS (Microsoft Visual C++ V1.52)
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
 *  Project configuration by pre-processor definitions:
 *  ---------------------------------------------------
 *
 *  MVB interface hardware, i.e. bus controller link layer:
 *  - TCN_HW_PD16     - MVB interface with MVB UART Emulation 'PD16'
 *  - TCN_HW_MD16     - MVB interface with MVB UART Emulation 'MD16'
 *  - TCN_HW_MDFULL   - MVB interface with MVB UART Emulation 'MDFULL'
 *  - TCN_HW_PCI      - MVB interface is a PCI device (optional, e.g. D213)
 *
 *  TCN application:
 *  - TCN_TEST        - select TCN test application
 *  - TCN_DEMO        - select TCN demo application
 *
 * ==========================================================================
 */
//#define TCN_HW_MD16
#define TCN_HW_MDFULL
//#define TCN_TEST


/* ==========================================================================
 *
 *  Project specific pre-processor definitions
 *
 *  (MVB interface hardware, i.e. bus controller link layer)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  MVB interface hardware
 * --------------------------------------------------------------------------
 */
#ifdef TCN_HW_PD16
#   define MUE_PD_16
#   define MUE_TEST_PD16
#   define MUELL_PD16
#   define TCN_DEMO
//#   define MUE_TEST
//#   define MUE_TEST_PD16
//#   define TCN_DEMO_MD 
#endif /* #ifdef TCN_HW_PD16 */

#ifdef TCN_HW_MD16
#   define MUE_PD_16F
#   define MUE_TEST_MD16
#   define MUELL_MD16
//#   define TCN_MD
#endif /* #ifdef TCN_HW_MD16 */

#ifdef TCN_HW_MDFULL
#   define MUE_PD_FULL
#   define MUE_TEST_MDFL
#   define MUELL_MDFL
#   define TCN_DEMO
#   define MUE_CALIBRATE
#endif /* #ifdef TCN_HW_MDFULL */

#ifdef TCN_HW_PCI
#   define MUE_ACCR_PCI
#endif /* #ifdef TCN_HW_PCI */


/* ==========================================================================
 *
 *  Project specific pre-processor definitions
 *
 *  (driver software configuration)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  DOS did not support 64-bit data types. So define 'long' (32-bit)
 *  as dummy data type (don't use 64-bit data types, e.g. UNSIGNED64).
 * --------------------------------------------------------------------------
 */
 //#define TCN_DEF_BASE_TYPE64 long

/* --------------------------------------------------------------------------
 *  Special data type - BITFIELD16:
 *  16-bit data type used to build structured types like DS_NAME,
 *  PV_NAME, SV_MVB_DEVICE_STATUS.
 *  This will result in structured types, which size are multiplies
 *  of 16-bits.
 * --------------------------------------------------------------------------
 */
//#define TCN_DEF_BITFIELD16  unsigned int

/* --------------------------------------------------------------------------
 *  Don't support POSIX, since DOS did not support REALTIME POSIX
 *  (e.g. semaphores, execution scheduling, threads, mutex).
 * --------------------------------------------------------------------------
 */
#define TCN_OSL_INCLUDE_POSIX_SEMAPHORE_H   0
#define TCN_OSL_INCLUDE_POSIX_SCHED_H       0
#define TCN_OSL_INCLUDE_POSIX_PTHREAD_H     0

/* --------------------------------------------------------------------------
 *  Don't support MUTEX for UART access.
 * --------------------------------------------------------------------------
 */
#define PD16_BC_MUTEX_UART      0
#define MD16_BC_MUTEX_UART      0
#define MDFULL_BC_MUTEX_UART    0

/* --------------------------------------------------------------------------
 *  Don't support MUTEX for port access (read-modify-write).
 * --------------------------------------------------------------------------
 */
#define TCN_AP_MUTEX_PORT   0

/* --------------------------------------------------------------------------
 *  Support for a little-endian CPU, e.g. Intel (PC).
 * --------------------------------------------------------------------------
 */
//Rui: this will make sense!
#define TCN_LE

/* --------------------------------------------------------------------------
 *  Check range of procedure input parameters; only during development.
 * --------------------------------------------------------------------------
 */
#define TCN_CHECK_PARAMETER
#define MUE_ACC_PRINT


#endif /* #ifndef TCN_PRJ_H */
