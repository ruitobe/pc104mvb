/* ==========================================================================
 *
 *  File      : TCN_BLS.C
 *
 *  Purpose   : Bus Controller Link Layer Selector
 *              - HOST: Intel 386 compatible PC platform
 *              - OS  : eCos for Intel 386 (GNU GCC)
 *              - UART: MVB UART Emulation with FIFO (e.g. MDFULL)
 *                      - 8 registers a 8-bit
 *              - NOTE: Standard access to UART registers.
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
 *  - MUELL_PD16  - enable MUE Link Layer 'PD16'
 *  - MUELL_MD16  - enable MUE Link Layer 'MD16'
 *  - MUELL_MDFL  - enable MUE Link Layer 'MDFULL'
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
#include <tcn_bls.h>

//#define MUELL_PD16  1

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer - PD16
 * --------------------------------------------------------------------------
 */
#ifdef MUELL_PD16
#include <pd16_bc.h>
#include <pd16_ls.h>
#include <pd16_lp.h>

#endif /* #ifdef MUELL_PD16 */

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer - MD16
 * --------------------------------------------------------------------------
 */
#ifdef MUELL_MD16
#   include <md16_bc.h>
#   include <md16_ls.h>
#   include <md16_lp.h>
#   ifdef TCN_MD
#       include <md16_lm.h>
#   endif /* #ifdef TCN_MD */
#endif /* #ifdef MUELL_MD16 */

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer - MDFULL
 * --------------------------------------------------------------------------
 */
#ifdef MUELL_MDFL
#   include <mdfl_bc.h>
#   include <mdfl_ls.h>
#   include <mdfl_lp.h>
#   ifdef TCN_MD
#       include <mdfl_lm.h>
#   endif /* #ifdef TCN_MD */
#endif /* #ifdef MUELL_MDFL */




/* ==========================================================================
 *
 *  MUE Link Layer 'PD16'
 *
 * ==========================================================================
 */

#ifdef MUELL_PD16

/* --------------------------------------------------------------------------
 *  Bus Controller Structure for MUE Link Layer 'PD16'
 * --------------------------------------------------------------------------
 */
PD16_BUS_CTRL bc_pd16 =                                                     \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  UART (static values)                                                \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* address of UART register RBR/THR                                 */  \
    (WORD32)0x4F8,                                                          \
                                                                            \
    /* address of UART register LSR                                     */  \
    (WORD32)0x4FD,                                                          \
                                                                            \
    /* size of a UART register (number of bytes)                        */  \
    (UNSIGNED32)1,                                                          \
                                                                            \
    /* timeout by loop (~100us for PD16)                                */  \
    /* HW=D114L-PD16, OS=Win98, HOST=PC AMD K6-2 500MHz                 */  \
    /* - 650000 => 1000ms                                               */  \
    (UNSIGNED32)65,                                                         \
                                                                            \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  LS (static values)                                                  \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* device status capability                                         */  \
    /* - FALSE - no   DS capability                                     */  \
    /* - TRUE  - with DS capability                                     */  \
    FALSE,                                                                  \
                                                                            \
    /* bus administrator capability                                     */  \
    /* - FALSE - no   BA capability                                     */  \
    /* - TRUE  - with BA capability                                     */  \
    FALSE,                                                                  \
                                                                            \
    /* max. number of entries in the BA list                            */  \
    0                                                                       \
                                                                            \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  dynamic values (ignore)                                             \
     *  NOTE:                                                               \
     *  Dynamic values will be set to 0 by initialisation of the            \
     *  variable and later modified by the software.                        \
     * ------------------------------------------------------------------   \
     */                                                                     \
};

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Supervision (SV)
 *  (MUE Link Layer 'PD16')
 * --------------------------------------------------------------------------
 */
#define BLS_DESC_SV_PD16 /* (of type 'BLS_DESC_SV')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LSBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LS_INIT'                                    */  \
    pd16_ls_init,                                                           \
    /* a procedure of type 'LS_SERVICE_HANDLER'                         */  \
    pd16_ls_service_handler                                                 \
}

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Process Data (PD)
 *  (MUE Link Layer 'PD16')
 * --------------------------------------------------------------------------
 */
#define BLS_DESC_PD_PD16 /* (of type 'BLS_DESC_PD')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LPBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LP_INIT'                                    */  \
    pd16_lp_init,                                                           \
    /* a procedure of type 'LP_MANAGE'                                  */  \
    pd16_lp_manage,                                                         \
    /* a procedure of type 'LP_PUT_DATASET'                             */  \
    pd16_lp_put_dataset,                                                    \
    /* a procedure of type 'LP_GET_DATASET'                             */  \
    pd16_lp_get_dataset                                                     \
}

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Message Data (MD)
 *  (MUE Link Layer 'PD16')
 * --------------------------------------------------------------------------
 */
#define BLS_DESC_MD_PD16 /* (of type 'BLS_DESC_MD')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LMBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_INIT'                                    */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_DEV_ADDRESS'                         */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_STATUS'                              */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_QUEUE_FLUSH'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_REQUEST'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_RECEIVE_POLL'                            */  \
    NULL,                                                                   \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  subscribed procedures to the LMBI                                   \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_RECEIVE_INDICATE'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_PACK'                                */  \
    NULL,                                                                   \
    /* identifies the packet pool used by the link layer                */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_CONFIRM'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_STATUS_INDICATE'                         */  \
    NULL                                                                    \
}

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Descriptor for MUE Link Layer 'PD16'
 * --------------------------------------------------------------------------
 */
BLS_DESC bls_desc_0 =                                                       \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  bus controller description                                          \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* pointer to bus controller specific values                        */  \
    &bc_pd16,                                                               \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  link description                                                    \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* type of the link (any SV_LINK_TYPE)                              */  \
    SV_LINK_TYPE_MVB,                                                       \
    /* name of the link                                                 */  \
    /* STRING32 (should be null terminated)                             */  \
    /* <-- 10 --><-- 20 --><-- 30 -->32                                 */  \
    /* 12345678901234567890123456789012                                 */  \
    { "duagon MVB Controller PD16" },                                       \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  service descriptors                                                 \
     * ------------------------------------------------------------------   \
     */                                                                     \
    BLS_DESC_SV_PD16,                                                       \
    BLS_DESC_PD_PD16,                                                       \
    BLS_DESC_MD_PD16                                                        \
};

#endif /* #ifdef MUELL_PD16 */


/* ==========================================================================
 *
 *  MUE Link Layer 'MD16'
 *
 * ==========================================================================
 */

#ifdef MUELL_MD16

/* --------------------------------------------------------------------------
 *  Bus Controller Structure for MUE Link Layer 'MD16'
 * --------------------------------------------------------------------------
 */
MD16_BUS_CTRL bc_md16 =                                                     \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  UART (static values)                                                \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* address of UART register RBR/THR                                 */  \
    (WORD32)0x4F8,                                                          \
                                                                            \
    /* address of UART register LSR                                     */  \
    (WORD32)0x4FD,                                                          \
                                                                            \
    /* size of a UART register (number of bytes)                        */  \
    (UNSIGNED32)1,                                                          \
                                                                            \
    /* timeout by loop (~10ms for MD16/MDFULL)                          */  \
    /* HW=D113L-MDFULL, OS=Win98, HOST=PC AMD K6-2 500MHz               */  \
    /* - 650000 => 1000ms                                               */  \
    (UNSIGNED32)6500,                                                       \
                                                                            \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  LS (static values)                                                  \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* device status capability                                         */  \
    /* - FALSE - no   DS capability                                     */  \
    /* - TRUE  - with DS capability                                     */  \
    FALSE,                                                                  \
                                                                            \
    /* bus administrator capability                                     */  \
    /* - FALSE - no   BA capability                                     */  \
    /* - TRUE  - with BA capability                                     */  \
    TRUE,                                                                   \
                                                                            \
    /* max. number of entries in the BA list                            */  \
    256                                                                     \
                                                                            \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  dynamic values (ignore)                                             \
     *  NOTE:                                                               \
     *  Dynamic values will be set to 0 by initialisation of the            \
     *  variable and later modified by the software.                        \
     * ------------------------------------------------------------------   \
     */                                                                     \
};

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Supervision (SV)
 *  (MUE Link Layer 'MD16')
 * --------------------------------------------------------------------------
 */
#define BLS_DESC_SV_MD16 /* (of type 'BLS_DESC_SV')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LSBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LS_INIT'                                    */  \
    md16_ls_init,                                                           \
    /* a procedure of type 'LS_SERVICE_HANDLER'                         */  \
    md16_ls_service_handler                                                 \
}

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Process Data (PD)
 *  (MUE Link Layer 'MD16')
 * --------------------------------------------------------------------------
 */
#define BLS_DESC_PD_MD16 /* (of type 'BLS_DESC_PD')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LPBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LP_INIT'                                    */  \
    md16_lp_init,                                                           \
    /* a procedure of type 'LP_MANAGE'                                  */  \
    md16_lp_manage,                                                         \
    /* a procedure of type 'LP_PUT_DATASET'                             */  \
    md16_lp_put_dataset,                                                    \
    /* a procedure of type 'LP_GET_DATASET'                             */  \
    md16_lp_get_dataset                                                     \
}

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Message Data (MD)
 *  (MUE Link Layer 'MD16')
 * --------------------------------------------------------------------------
 */
#ifdef TCN_MD

#define BLS_DESC_MD_MD16 /* (of type 'BLS_DESC_MD')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LMBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_INIT'                                    */  \
    md16_lm_init,                                                           \
    /* a procedure of type 'LM_GET_DEV_ADDRESS'                         */  \
    md16_lm_get_dev_address,                                                \
    /* a procedure of type 'LM_GET_STATUS'                              */  \
    md16_lm_get_status,                                                     \
    /* a procedure of type 'LM_SEND_QUEUE_FLUSH'                        */  \
    md16_lm_send_queue_flush,                                               \
    /* a procedure of type 'LM_SEND_REQUEST'                            */  \
    md16_lm_send_request,                                                   \
    /* a procedure of type 'LM_RECEIVE_POLL'                            */  \
    md16_lm_receive_poll,                                                   \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  subscribed procedures to the LMBI                                   \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_RECEIVE_INDICATE'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_PACK'                                */  \
    NULL,                                                                   \
    /* identifies the packet pool used by the link layer                */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_CONFIRM'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_STATUS_INDICATE'                         */  \
    NULL                                                                    \
}

#else /* #ifdef TCN_MD */

#define BLS_DESC_MD_MD16 /* (of type 'BLS_DESC_MD')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LMBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_INIT'                                    */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_DEV_ADDRESS'                         */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_STATUS'                              */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_QUEUE_FLUSH'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_REQUEST'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_RECEIVE_POLL'                            */  \
    NULL,                                                                   \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  subscribed procedures to the LMBI                                   \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_RECEIVE_INDICATE'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_PACK'                                */  \
    NULL,                                                                   \
    /* identifies the packet pool used by the link layer                */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_CONFIRM'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_STATUS_INDICATE'                         */  \
    NULL                                                                    \
}

#endif /* #else */

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Descriptor for MUE Link Layer 'MD16'
 * --------------------------------------------------------------------------
 */
BLS_DESC bls_desc_0 =                                                       \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  bus controller description                                          \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* pointer to bus controller specific values                        */  \
    &bc_md16,                                                               \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  link description                                                    \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* type of the link (any SV_LINK_TYPE)                              */  \
    SV_LINK_TYPE_MVB,                                                       \
    /* name of the link                                                 */  \
    /* STRING32 (should be null terminated)                             */  \
    /* <-- 10 --><-- 20 --><-- 30 -->32                                 */  \
    /* 12345678901234567890123456789012                                 */  \
    { "duagon MVB Controller MD16" },                                       \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  service descriptors                                                 \
     * ------------------------------------------------------------------   \
     */                                                                     \
    BLS_DESC_SV_MD16,                                                       \
    BLS_DESC_PD_MD16,                                                       \
    BLS_DESC_MD_MD16                                                        \
};

#endif /* #ifdef MUELL_MD16 */


/* ==========================================================================
 *
 *  MUE Link Layer 'MDFULL'
 *
 * ==========================================================================
 */

#ifdef MUELL_MDFL

/* --------------------------------------------------------------------------
 *  Bus Controller Structure for MUE Link Layer 'MDFULL'
 * --------------------------------------------------------------------------
 */
MDFULL_BUS_CTRL bc_mdfl =                                                   \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  UART (static values)                                                \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* address of UART register RBR/THR                                 */  \
    (WORD32)0x4F8,                                                         \
                                                                            \
    /* address of UART register LSR                                     */  \
    (WORD32)0x4FD,                                                          \
                                                                            \
    /* size of a UART register (number of bytes)                        */  \
    (UNSIGNED32)1,                                                          \
                                                                            \
    /* timeout by loop (~10ms for MD16/MDFULL)                          */  \
    /* HW=D113L-MDFULL, OS=Win98, HOST=PC AMD K6-2 500MHz               */  \
    /* - 650000 => 1000ms                                               */  \
    //(UNSIGNED32)6500, 
    (UNSIGNED32)650000,	                                                      \
                                                                            \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  LS (static values)                                                  \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* device status capability                                         */  \
    /* - FALSE - no   DS capability                                     */  \
    /* - TRUE  - with DS capability                                     */  \
    TRUE,                                                                   \
                                                                            \
    /* bus administrator capability                                     */  \
    /* - FALSE - no   BA capability                                     */  \
    /* - TRUE  - with BA capability                                     */  \
    TRUE,                                                                   \
                                                                            \
    /* max. number of entries in the BA list                            */  \
    8192                                                                    \
                                                                            \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  dynamic values (ignore)                                             \
     *  NOTE:                                                               \
     *  Dynamic values will be set to 0 by initialisation of the            \
     *  variable and later modified by the software.                        \
     * ------------------------------------------------------------------   \
     */                                                                     \
};

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Supervision (SV)
 *  (MUE Link Layer 'MDFULL')
 * --------------------------------------------------------------------------
 */
#define BLS_DESC_SV_MDFL /* (of type 'BLS_DESC_SV')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LSBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LS_INIT'                                    */  \
    mdfull_ls_init,                                                         \
    /* a procedure of type 'LS_SERVICE_HANDLER'                         */  \
    mdfull_ls_service_handler                                               \
}

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Process Data (PD)
 *  (MUE Link Layer 'MDFULL')
 * --------------------------------------------------------------------------
 */
#define BLS_DESC_PD_MDFL /* (of type 'BLS_DESC_PD')                     */  \
{      \
    mdfull_lp_init,                                                         \
    /* a procedure of type 'LP_MANAGE'                                  */  \
    mdfull_lp_manage,                                                       \
    /* a procedure of type 'LP_PUT_DATASET'                             */  \
    mdfull_lp_put_dataset,                                                  \
    /* a procedure of type 'LP_GET_DATASET'                             */  \
    mdfull_lp_get_dataset                                                   \
}
    /* ------------------------------------------------------------------   \
     *  LPBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LP_INIT'                                    */  \
    //mdfull_lp_init,                                                         \
    /* a procedure of type 'LP_MANAGE'                                  */  \
    mdfull_lp_manage,                                                       \
    /* a procedure of type 'LP_PUT_DATASET'                             */  \
    mdfull_lp_put_dataset,                                                  \
    /* a procedure of type 'LP_GET_DATASET'                             */  \
    mdfull_lp_get_dataset                                                   \
}

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Message Data (MD)
 *  (MUE Link Layer 'MDFULL')
 * --------------------------------------------------------------------------
 */
#ifdef TCN_MD

#define BLS_DESC_MD_MDFL /* (of type 'BLS_DESC_MD')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LMBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
     /* a procedure of type 'LM_INIT'                                    */  \
    mdfull_lm_init,                                                         \
    /* a procedure of type 'LM_GET_DEV_ADDRESS'                         */  \
    mdfull_lm_get_dev_address,                                              \
    /* a procedure of type 'LM_GET_STATUS'                              */  \
    mdfull_lm_get_status,                                                   \
    /* a procedure of type 'LM_SEND_QUEUE_FLUSH'                        */  \
    mdfull_lm_send_queue_flush,                                             \
    /* a procedure of type 'LM_SEND_REQUEST'                            */  \
    mdfull_lm_send_request,                                                 \
    /* a procedure of type 'LM_RECEIVE_POLL'                            */  \
    mdfull_lm_receive_poll,                                                  \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  subscribed procedures to the LMBI                                   \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_RECEIVE_INDICATE'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_PACK'                                */  \
    NULL,                                                                   \
    /* identifies the packet pool used by the link layer                */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_CONFIRM'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_STATUS_INDICATE'                         */  \
    NULL                                                                    \
}

#else /* #ifdef TCN_MD */

#define BLS_DESC_MD_MDFL /* (of type 'BLS_DESC_MD')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LMBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_INIT'                                    */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_DEV_ADDRESS'                         */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_STATUS'                              */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_QUEUE_FLUSH'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_REQUEST'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_RECEIVE_POLL'                            */  \
    NULL,                                                                   \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  subscribed procedures to the LMBI                                   \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_RECEIVE_INDICATE'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_PACK'                                */  \
    NULL,                                                                   \
    /* identifies the packet pool used by the link layer                */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_CONFIRM'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_STATUS_INDICATE'                         */  \
    NULL                                                                    \
}

#endif /* #else */

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Descriptor for MUE Link Layer 'MDFULL'
 * --------------------------------------------------------------------------
 */
BLS_DESC bls_desc_0 =                                                       \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  bus controller description                                          \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* pointer to bus controller specific values                        */  \
    &bc_mdfl,                                                               \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  link description                                                    \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* type of the link (any SV_LINK_TYPE)                              */  \
    SV_LINK_TYPE_MVB,                                                       \
    /* name of the link                                                 */  \
    /* STRING32 (should be null terminated)                             */  \
    /* <-- 10 --><-- 20 --><-- 30 -->32                                 */  \
    /* 12345678901234567890123456789012                                 */  \
    { "duagon MVB Controller MDFULL" },                                     \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  service descriptors                                                 \
     * ------------------------------------------------------------------   \
     */                                                                     \
    BLS_DESC_SV_MDFL,                                                       \
    BLS_DESC_PD_MDFL,                                                       \
    BLS_DESC_MD_MDFL                                                        \
};

#endif /* #ifdef MUELL_MDFL */


/* ==========================================================================
 *
 *  Public Variables
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Public Variable   : bls_desc_table
 *
 *  Purpose           : Points to the connected bus controller link layer
 *                      descriptors.
 *
 *  Remarks           : - The link identifier (e.g. link_id) is the index to
 *                        the table of bus controller link layer descriptor
 *                        pointers. If a pointer is NULL, then there is no
 *                        connected bus controller link layer.
 * --------------------------------------------------------------------------
 */
BLS_DESC *bls_desc_table[BLS_MAX_LINKS] =                                   \
{                                                                           \
    &bls_desc_0,                                        /* link_id =  0 */  \
    NULL,                                               /* link_id =  1 */  \
    NULL,                                               /* link_id =  2 */  \
    NULL,                                               /* link_id =  3 */  \
    NULL,                                               /* link_id =  4 */  \
    NULL,                                               /* link_id =  5 */  \
    NULL,                                               /* link_id =  6 */  \
    NULL,                                               /* link_id =  7 */  \
    NULL,                                               /* link_id =  8 */  \
    NULL,                                               /* link_id =  9 */  \
    NULL,                                               /* link_id = 10 */  \
    NULL,                                               /* link_id = 11 */  \
    NULL,                                               /* link_id = 12 */  \
    NULL,                                               /* link_id = 13 */  \
    NULL,                                               /* link_id = 14 */  \
    NULL                                                /* link_id = 15 */  \
};
