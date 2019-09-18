#ifndef TCN_PD_H
#define TCN_PD_H

/* ==========================================================================
 *
 *  File      : TCN_PD.H
 *
 *  Purpose   : Common Interface for Process Data
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


/* ==========================================================================
 *
 *  Ports
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous Constants
 * --------------------------------------------------------------------------
 */
#define PD_PRT_PORT_NUMBER_MAX              4096
TCN_DECL_PUBLIC TCN_DECL_CONST UNSIGNED16   pd_prt_port_number_max;


/* --------------------------------------------------------------------------
 *  Enumerated Type   : PD_PRT_CMD
 *
 *  Purpose           : Commands to manage a port.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    PD_PRT_CMD_NONE     = 0,
    PD_PRT_CMD_CONFIG   = 1,
    PD_PRT_CMD_DELETE   = 2,
    PD_PRT_CMD_MODIFY   = 3,
    PD_PRT_CMD_STATUS   = 4
}   PD_PRT_CMD;


/* --------------------------------------------------------------------------
 *  Structured Type   : PD_PRT_ATTR
 *
 *  Purpose           : Characterises a port by its attributes.
 *
 *  Syntax            : typedef struct
 *                      {
 *                          UNSIGNED8   port_size;
 *                          BITSET8     port_config;
 *                          WORD16      reserved;
 *                          void        *p_bus_specific;
 *                      }   PD_PRT_ATTR;
 *
 *  Element           : port_size      - size of a port (number of bytes)
 *                      port_config    - configuration of a port
 *                                       (any combination of bit constants
 *                                       'PD_PRT_CFG_BIT_xxx' or any
 *                                       predefined port configuration
 *                                       constant)
 *                      reserved       - reserved (=0)
 *                      p_bus_specific - pointer to a memory buffer, which
 *                                       contains the bus specific port
 *                                       attributes (the type of that memory
 *                                       buffer depends on the kind of bus
 *                                       controller connected)
 *
 *  Remarks           : - The pointer 'p_bus_specific' may NULL, if no bus
 *                        specific attributes for a port exist.
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_PD_PRT_ATTR 8
typedef struct
{
    UNSIGNED8   port_size;
    BITSET8     port_config;
    WORD16      reserved;
    void        *p_bus_specific;
}   PD_PRT_ATTR;


/* --------------------------------------------------------------------------
 *  Bit Constants : PD_PRT_CFG_BIT_xxx
 *
 *  Purpose       : Defines the bits of a port configuration.
 *
 *  Remarks       : - Used for element 'port_config' of structured type
 *                    'PD_PRT_ATTR'.
 * --------------------------------------------------------------------------
 */
#define PD_PRT_CFG_BIT_SOURCE       0x80 /* SRC; see TNM */
#define PD_PRT_CFG_BIT_SINK         0x40 /* SNK; see TNM */
#define PD_PRT_CFG_BIT_RESERVED1    0x20 /* TWC; see TNM */
#define PD_PRT_CFG_BIT_RESERVED2    0x10 /* FRC; see TNM */


/* --------------------------------------------------------------------------
 *  Constants : PD_xxx
 *
 *  Purpose   : Port configuration constants.
 *
 *  Remarks   : - Combinations of bit constants 'PD_PRT_CFG_BIT_xxx'.
 *              - Used for element 'port_config' of structured type
 *                'PD_PRT_ATTR'.
 * --------------------------------------------------------------------------
 */
#define PD_SOURCE_PORT          (PD_PRT_CFG_BIT_SOURCE)
#define PD_SINK_PORT            (PD_PRT_CFG_BIT_SINK)


#endif /* #ifndef TCN_PD_H */
