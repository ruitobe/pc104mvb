#ifndef MD16_LP_H
#define MD16_LP_H

/* ==========================================================================
 *
 *  File      : MD16_LP.H
 *
 *  Purpose   : Bus Controller Link Layer for MD16 - 
 *                  Link Layer Bus Controller Interface for Process Data
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
#include <tcn_lp.h>


/* ==========================================================================
 *
 *  Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
#define MD16_LP_PORT_NUMBER_MAX             16
TCN_DECL_PUBLIC TCN_DECL_CONST UNSIGNED16   md16_lp_port_number_max;

#define MD16_LP_PORT_SIZE_NUMBER            5
TCN_DECL_PUBLIC TCN_DECL_CONST UNSIGNED16   md16_lp_port_size_number;

#define MD16_LP_PORT_SIZE_VALUE     \
{                                   \
    2,  4,  8, 16, 32,  0,  0,  0,  \
    0,  0,  0,  0,  0,  0,  0,  0   \
}
TCN_DECL_PUBLIC TCN_DECL_CONST UNSIGNED8    md16_lp_port_size_value[16];


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : md16_lp_init (of procedure type 'LP_INIT')
 *
 *  Purpose   : Initialises a process data link layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              md16_lp_init
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  fsi
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              fsi        - freshness supervision interval;
 *                           0=disable or 64ms
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - This procedure has to be called at system
 *                initialisation before calling any other
 *                'md16_lp_xxx' procedure referencing the
 *                same traffic store (parameter 'p_bus_ctrl').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
md16_lp_init
(
    void        *p_bus_ctrl,
    UNSIGNED16  fsi
);


/* --------------------------------------------------------------------------
 *  Procedure : md16_lp_manage (of procedure type 'LP_MANAGE')
 *
 *  Purpose   : Manages a port located in a traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              md16_lp_manage
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      port_address,
 *                  ENUM16      command,
 *                  PD_PRT_ATTR *prt_attr
 *              );
 *
 *  Input     : p_bus_ctrl   - pointer to bus controller specific values
 *              port_address - port address (0..4096)
 *              command      - port manage command; any PD_PRT_CMD
 *
 *  In-/Output: prt_attr     - pointer to a memory buffer which contains
 *                             the port attributes (memory buffer of
 *                             structured type 'PD_PRT_ATTR')
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
md16_lp_manage
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    ENUM16      command,
    PD_PRT_ATTR *prt_attr
);


/* --------------------------------------------------------------------------
 *  Procedure : md16_lp_put_dataset (of procedure type 'LP_PUT_DATASET')
 *
 *  Purpose   : Copies a dataset from the application to a port.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              md16_lp_put_dataset
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      port_address,
 *                  void        *p_value
 *              );
 *
 *  Input     : p_bus_ctrl   - pointer to bus controller specific values
 *              port_address - port address (0..4096)
 *              p_value      - pointer to a memory buffer of
 *                             the application where the dataset
 *                             is copied from
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
md16_lp_put_dataset
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    void        *p_value
);


/* --------------------------------------------------------------------------
 *  Procedure : md16_lp_get_dataset (of procedure type 'LP_GET_DATASET')
 *
 *  Purpose   : Copies a dataset from a port to the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              md16_lp_get_dataset
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      port_address,
 *                  void        *p_value,
 *                  void        *p_fresh
 *              );
 *
 *  Input     : p_bus_ctrl   - pointer to bus controller specific values
 *              port_address - port address (0..4096)
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Output    : p_value      - pointer to a memory buffer of
 *                             the application where the dataset
 *                             is copied to
 *              p_fresh      - pointer to a memory buffer of
 *                             the application where the freshness timer
 *                             is copied to
 *
 *  Remarks   : - A traffic store is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
md16_lp_get_dataset
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    void        *p_value,
    void        *p_fresh
);


#endif /* #ifndef MD16_LP_H */
