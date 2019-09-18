#ifndef TCN_LP_H
#define TCN_LP_H

/* ==========================================================================
 *
 *  File      : TCN_LP.H
 *
 *  Purpose   : Link Layer Interface for Process Data
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
#include <tcn_def.h>
#include <tcn_pd.h>


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
#define LP_MAX_TRAFFIC_STORES               16
TCN_DECL_PUBLIC TCN_DECL_CONST UNSIGNED16   lp_max_traffic_stores;


/* --------------------------------------------------------------------------
 *  Enumerated Type : LP_RESULT
 *
 *  Purpose         : Result of a procedure.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    LP_OK           = 0,    /* correct termination                         */
    LP_PRT_PASSIVE  = 1,    /* port (dataset) not active                   */
    LP_ERROR        = 2,    /* unspecified error                           */
    LP_CONFIG       = 3,    /* configuration error                         */
    LP_MEMORY       = 4,    /* not enough memory                           */
    LP_UNKNOW_TS    = 5,    /* unknown traffic store (e.g. not connected)  */
    LP_RANGE        = 6,    /* memory address error                        */
    LP_DATA_TYPE    = 7     /* unsupported data type                       */
}   LP_RESULT;


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure Type: LP_INIT
 *
 *  Purpose       : Initialises a process data link layer.
 *
 *  Syntax        : typedef LP_RESULT
 *                  (*LP_INIT)
 *                  (
 *                      void        *p_bus_ctrl,
 *                      UNSIGNED16  fsi
 *                  );
 *
 *  Input         : p_bus_ctrl - pointer to bus controller specific values
 *                  fsi        - freshness supervision interval (value range
 *                               depends on the bus controller)
 *
 *  Return        : result code; any LP_RESULT
 *
 *  Remarks       : - A traffic store is identified by 'p_bus_ctrl'.
 *                  - This procedure has to be called at system
 *                    initialisation before calling any other 'LP_xxx'
 *                    procedure referencing the same traffic store
 *                    (parameter 'p_bus_ctrl').
 * --------------------------------------------------------------------------
 */
typedef LP_RESULT
(*LP_INIT)
(
    void        *p_bus_ctrl,
    UNSIGNED16  fsi
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LP_MANAGE
 *
 *  Purpose       : Manages a port located in a traffic store.
 *
 *  Syntax        : typedef LP_RESULT
 *                  (*LP_MANAGE)
 *                  (
 *                      void        *p_bus_ctrl,
 *                      WORD16      port_address,
 *                      ENUM16      command,
 *                      PD_PRT_ATTR *prt_attr
 *                  );
 *
 *  Input         : p_bus_ctrl   - pointer to bus controller specific values
 *                  port_address - port address (0..4096)
 *                  command      - port manage command; any PD_PRT_CMD
 *
 *  In-/Output    : prt_attr     - pointer to a memory buffer which contains
 *                                 the port attributes (memory buffer of
 *                                 structured type 'PD_PRT_ATTR')
 *
 *  Return        : result code; any LP_RESULT
 *
 *  Remarks       : - A traffic store is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
typedef LP_RESULT
(*LP_MANAGE)
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    ENUM16      command,
    PD_PRT_ATTR *prt_attr
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LP_PUT_DATASET
 *
 *  Purpose       : Copies a dataset from the application to a port.
 *
 *  Syntax        : typedef LP_RESULT
 *                  (*LP_PUT_DATASET)
 *                  (
 *                      void        *p_bus_ctrl,
 *                      WORD16      port_address,
 *                      void        *p_value
 *                  );
 *
 *  Input         : p_bus_ctrl   - pointer to bus controller specific values
 *                  port_address - port address (0..4096)
 *                  p_value      - pointer to a memory buffer of
 *                                 the application where the dataset
 *                                 is copied from
 *
 *  Return        : result code; any LP_RESULT
 *
 *  Remarks       : - A traffic store is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
typedef LP_RESULT
(*LP_PUT_DATASET)
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    void        *p_value
);


/* --------------------------------------------------------------------------
 *  Procedure Type: LP_GET_DATASET
 *
 *  Purpose       : Copies a dataset from a port to the application.
 *
 *  Syntax        : typedef LP_RESULT
 *                  (*LP_GET_DATASET)
 *                  (
 *                      void        *p_bus_ctrl,
 *                      WORD16      port_address,
 *                      void        *p_value,
 *                      void        *p_fresh
 *                  );
 *
 *  Input         : p_bus_ctrl   - pointer to bus controller specific values
 *                  port_address - port address (0..4096)
 *
 *  Return        : result code; any LP_RESULT
 *
 *  Output        : p_value      - pointer to a memory buffer of
 *                                 the application where the dataset
 *                                 is copied to
 *                  p_fresh      - pointer to a memory buffer of
 *                                 the application where the freshness timer
 *                                 is copied to
 *
 *  Remarks       : - A traffic store is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
typedef LP_RESULT
(*LP_GET_DATASET)
(
    void        *p_bus_ctrl,
    WORD16      port_address,
    void        *p_value,
    void        *p_fresh
);


/* ==========================================================================
 *
 *  Common Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : lp_init
 *
 *  Purpose   : Initialises a process data link layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              lp_init
 *              (
 *                  ENUM8       ts_id,
 *                  UNSIGNED16  fsi
 *              );
 *
 *  Input     : ts_id - traffic store identifier (0..15)
 *              fsi   - freshness supervision interval (value range depends
 *                      on the bus controller)
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 *              - This procedure has to be called at system initialisation
 *                before calling any other 'lp_xxx' procedure referencing
 *                the same traffic store (parameter 'ts_id').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
lp_init
(
    ENUM8       ts_id,
    UNSIGNED16  fsi
);


/* --------------------------------------------------------------------------
 *  Procedure : lp_manage
 *
 *  Purpose   : Manages a port located in a traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              lp_manage
 *              (
 *                  ENUM8       ts_id,
 *                  WORD16      port_address,
 *                  ENUM16      command,
 *                  PD_PRT_ATTR *prt_attr
 *              );
 *
 *  Input     : ts_id        - traffic store identifier (0..15)
 *              port_address - port address (0..4096)
 *              command      - port manage command; any PD_PRT_CMD
 *
 *  In-/Output: prt_attr     - pointer to a memory buffer which contains
 *                             the port attributes (memory buffer of
 *                             structured type 'PD_PRT_ATTR')
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
lp_manage
(
    ENUM8       ts_id,
    WORD16      port_address,
    ENUM16      command,
    PD_PRT_ATTR *prt_attr
);


/* --------------------------------------------------------------------------
 *  Procedure : lp_put_dataset
 *
 *  Purpose   : Copies a dataset from the application to a port.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              lp_put_dataset
 *              (
 *                  ENUM8       ts_id,
 *                  WORD16      port_address,
 *                  void        *p_value
 *              );
 *
 *  Input     : ts_id        - traffic store identifier (0..15)
 *              port_address - port address (0..4096)
 *              p_value      - pointer to a memory buffer of
 *                             the application where the dataset
 *                             is copied from
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
lp_put_dataset
(
    ENUM8       ts_id,
    WORD16      port_address,
    void        *p_value
);


/* --------------------------------------------------------------------------
 *  Procedure : lp_get_dataset
 *
 *  Purpose   : Copies a dataset from a port to the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              lp_get_dataset
 *              (
 *                  ENUM8       ts_id,
 *                  WORD16      port_address,
 *                  void        *p_value,
 *                  void        *p_fresh
 *              );
 *
 *  Input     : ts_id        - traffic store identifier (0..15)
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
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
lp_get_dataset
(
    ENUM8       ts_id,
    WORD16      port_address,
    void        *p_value,
    void        *p_fresh
);


#endif /* #ifndef TCN_LP_H */
