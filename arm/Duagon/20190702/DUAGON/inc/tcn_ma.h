#ifndef TCN_MA_H
#define TCN_MA_H

/* ==========================================================================
 *
 *  File      : TCN_MA.H
 *
 *  Purpose   : Agent Interface (Train Communication Network)
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
#include <tcn_tnm.h>


/* ==========================================================================
 *
 *  Initialisation
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : ma_init
 *
 *  Purpose   : Initialises the Agent.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MM_RESULT
 *              ma_init (void);
 *
 *  Return    : any MM_RESULT
 *
 *  Remarks   : - This procedure has to be called at system initialisation
 *                before calling any other 'ma_xxx' procedure.
 *              - This procedure shall be called only one time.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MM_RESULT
ma_init (void);


/* ==========================================================================
 *
 *  User Service Subscription
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure Type: MA_SERVICE_CALL
 *
 *  Purpose       : Type declaration of a procedure to be called for
 *                  a given service call, which returns the parameters
 *                  needed for the Reply_Message.
 *
 *  Syntax        : typedef void
 *                  (*MA_SERVICE_CALL)
 *                  (
 *                      AM_ADDRESS  *manager_address,
 *                      void        *call_msg_adr,
 *                      UNSIGNED32  call_msg_size,
 *                      void        **reply_msg_adr,
 *                      UNSIGNED32  *reply_msg_size,
 *                      MM_RESULT   *agent_status
 *                  );
 *
 *  Input         : manager_address - pointer to the full network address
 *                                    of the calling Manager
 *                  call_msg_adr    - pointer to the start of the service
 *                                    Call_Message to be processed
 *                                    (tnm_key field)
 *                  call_msg_size   - size of the Call_Message in octets
 *                  reply_msg_adr   - pointer to the start of the service
 *                                    Reply_Message to be returned
 *                                    (tnm_key field)
 *                  reply_msg_size  - size of the Call_Message in octets
 *                  agent_status    - result to be communicated as Agent
 *                                    status to the Manager
 * --------------------------------------------------------------------------
 */
typedef void
(*MA_SERVICE_CALL)
(
    AM_ADDRESS  *manager_address,
    void        *call_msg_adr,
    UNSIGNED32  call_msg_size,
    void        **reply_msg_adr,
    UNSIGNED32  *reply_msg_size,
    MM_RESULT   *agent_status
);


/* --------------------------------------------------------------------------
 *  Procedure Type: MA_SERVICE_CLOSE
 *
 *  Purpose       : Type declaration of a procedure to be called for closing
 *                  a service.
 *
 *  Syntax        : typedef void
 *                  (*MA_SERVICE_CLOSE) (void);
 * --------------------------------------------------------------------------
 */
typedef void
(*MA_SERVICE_CLOSE) (void);


/* --------------------------------------------------------------------------
 *  Procedure : ma_subscribe
 *
 *  Purpose   : Indicates which user procedure to call when a user-defined
 *              service call is received. A previously assigned SIF_code is
 *              overwritten without a warning.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MM_RESULT
 *              ma_subscribe
 *              (
 *                  ENUM16              command,
 *                  ENUM8               sif_code,
 *                  MA_SERVICE_CALL     service_call,
 *                  MA_SERVICE_CLOSE    service_close,
 *                  void                *service_desc
 *              );
 *
 *  Input     : command       - 0=subscribe, 1=desubscribe
 *              sif_code      - user SIF_code (>= 128); see also enumerated
 *                              type 'TCN_SIF_CODE'
 *              service_call  - procedure of type 'MA_SERVICE_CALL' which
 *                              will execute the service when called
 *              service_close - procedure of type 'MA_SERVICE_CLOSE' which
 *                              the Agent will call when the Reply_Message
 *                              has been completely sent (e.g. to free the
 *                              buffer)
 *              service_desc  - descriptor of the service, as a visible
 *                              string terminated by a '00'H character
 *
 *  Return    : any MM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MM_RESULT
ma_subscribe
(
    ENUM16              command,
    ENUM8               sif_code,
    MA_SERVICE_CALL     service_call,
    MA_SERVICE_CLOSE    service_close,
    void                *service_desc
);


/* ==========================================================================
 *
 *  Restart Procedure Subscription
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure Type: MA_STATION_RESTART
 *
 *  Purpose       : Type declaration of a procedure to be called for
 *                  restarting the Station after a time-out or a restart
 *                  command. This procedure will probably not return.
 *
 *  Syntax        : typedef void
 *                  (*MA_STATION_RESTART) (void);
 * --------------------------------------------------------------------------
 */
typedef void
(*MA_STATION_RESTART) (void);


/* --------------------------------------------------------------------------
 *  Procedure : ma_subscribe_restart
 *
 *  Purpose   : Indicates which user procedure to call when the Station is
 *              reset or a reservation time-out takes place.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MM_RESULT
 *              ma_subscribe_restart
 *              (
 *                  MA_STATION_RESTART  station_restart
 *              );
 *
 *  Input     : station_restart - procedure of type 'MA_STATION_RESTART',
 *                                which the Agent will call when the
 *                                reservation time-out elapses or when
 *                                a reset command has been received
 *
 *  Return    : any MM_RESULT
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MM_RESULT
ma_subscribe_restart
(
    MA_STATION_RESTART  station_restart
);


#endif /* #ifndef TCN_MA_H */
