#ifndef TCN_MD_H
#define TCN_MD_H

/* ==========================================================================
 *
 *  File      : TCN_MD.H
 *
 *  Purpose   : Common Interface for Message Data
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
 *  General Constants and Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Enumerated Type   : MD_RESULT
 *
 *  Purpose           : Result of a procedure.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    MD_OK           = 0,    /* correct execution                        */
    MD_READY        = 0,    /* ready                                    */
    MD_REJECT       = 1,    /* not accepted (queue full or empty)       */
    MD_INAUGURATION = 2     /* inauguration - possible inconsistency    */
}   MD_RESULT;


/* ==========================================================================
 *
 *  Packets Handling
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Structured Type   : MD_PACKET
 *
 *  Purpose           : Defines the packet format.
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_MD_PACKET_NETWORK_HEADER 4
typedef struct
{
    WORD8       final[2];
    WORD8       origin[2];
}   MD_PACKET_NETWORK_HEADER;

#define TCN_SIZEOF_STRUCT_MD_PACKET_NETWORK_DATA 2
typedef struct
{
    WORD8       mtc;
    WORD8       transport_data[1]; /* dynamic size */
    /* NOTE:                          */
    /* The size of the transport data */
    /* depends on the bus.            */
    /* MVB: 176 bits =  22 bytes      */
    /* WTB: 984 bits = 123 bytes      */
}   MD_PACKET_NETWORK_DATA;

typedef struct
{
    MD_PACKET_NETWORK_HEADER    network_header;
    MD_PACKET_NETWORK_DATA      network_data;
}   MD_PACKET_LINK_DATA;

typedef struct
{
    WORD8               link_header[4];
    WORD8               link_header_sz;
    MD_PACKET_LINK_DATA link_data;
}   MD_PACKET_FRAME_DATA;

#define TCN_SIZEOF_STRUCT_MD_PACKET_LINK_HEADER_MVB 4
#ifdef TCN_LE
    typedef struct /* little-endian representation */
    {
        BITFIELD16  sd      : 12;
        BITFIELD16  pt      : 4;
        BITFIELD16  dd      : 12;
        BITFIELD16  mode    : 4;
    }   MD_PACKET_LINK_HEADER_MVB;
#else /* #ifdef TCN_LE */
    typedef struct /* big-endian representation */
    {
        BITFIELD16  mode    : 4;
        BITFIELD16  dd      : 12;
        BITFIELD16  pt      : 4;
        BITFIELD16  sd      : 12;
    }   MD_PACKET_LINK_HEADER_MVB;
#endif /* #else */
#define MD_PACKET_LINK_HEADER_MVB_MODE_SINGLE_CAST  0x1
#define MD_PACKET_LINK_HEADER_MVB_MODE_BROADCAST    0xF
#define MD_PACKET_LINK_HEADER_MVB_PT_TCN            0x8

#define TCN_SIZEOF_STRUCT_MD_PACKET_LINK_HEADER_WTB 4
typedef struct
{
    WORD8       reserved;
    WORD8       dd;
    WORD8       lc;
    WORD8       sd;
}   MD_PACKET_LINK_HEADER_WTB;

typedef enum
{
    MD_PENDING  = 1,
    MD_FLUSHED  = 2,
    MD_SENT     = 3
}   MD_PACKET_STATUS;

typedef struct
{
    void                    *next;
    void                    *owner;
    WORD8                   control;
    ENUM8                   status;
    MD_PACKET_FRAME_DATA    frame_data;
}   MD_PACKET;


/* --------------------------------------------------------------------------
 *  Procedure Type: MD_GET_PACKET
 *
 *  Purpose       : Gets a new packet from a packet pool.
 *
 *  Syntax        : typedef void
 *                  (*MD_GET_PACKET)
 *                  (
 *                      void        **pool,
 *                      MD_PACKET   **packet
 *                  );
 *
 *  Input         : pool   - identifies the pool which the packet is taken
 *                           from
 *  Output        : packet - pointer to a data structure where the packet
 *                           is stored.
 *
 *  Remarks       : - This procedure type is compatible with the procedure
 *                    type 'LM_GET_PACK', which can be directly called.
 * --------------------------------------------------------------------------
 */
typedef void
(*MD_GET_PACKET)
(
    void        **pool,
    MD_PACKET   **packet
);


/* --------------------------------------------------------------------------
 *  Procedure Type: MD_PUT_PACKET
 *
 *  Purpose       : Returns a single packet not anymore used to
 *                  a packet pool.
 *
 *  Syntax        : typedef void
 *                  (*MD_PUT_PACKET)
 *                  (
 *                      MD_PACKET   *packet
 *                  );
 *
 *  Input         : packet - pointer to a data structure where the packet
 *                           can be found. The owner pool is marked in the
 *                           packet.
 *
 *  Remarks       : - This procedure type is compatible with the procedure
 *                    type 'LM_SEND_CONFIRM', which can be directly called.
 * --------------------------------------------------------------------------
 */
typedef void
(*MD_PUT_PACKET)
(
    MD_PACKET   *packet
);


#endif /* #ifndef TCN_MD_H */
