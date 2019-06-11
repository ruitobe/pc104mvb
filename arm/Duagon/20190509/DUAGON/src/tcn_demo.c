/* ==========================================================================
 *
 *  File      : TCN_DEMO.C
 *
 *  Purpose   : TCN Demo Application
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
 *  - TCN_DEMO_BA_TCN     - enable MVB bus administrator:
 *                          - use BA configuration from TCN compatible
 *                            'MVB_Administrator' object (see IEC 61375-1)
 *  - TCN_DEMO_BA_D2000   - enable MVB bus administrator:
 *                          - use BA configuration from D2000 configuration
 *                            file (use file name 'ba_d2000.bin')
 *  - TCN_DEMO_MD         - enable MVB message data communication
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
#include "delay.h"
#include "localbus.h"
#include "modbus.h"

/* ==========================================================================
 *
 *  Conditional Compiling
 *
 * ==========================================================================
 */
 

#ifdef TCN_DEMO


/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - common
 * --------------------------------------------------------------------------
 */
#include <tcn_def.h>
#include <tcn_osl.h>

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - supervision
 * --------------------------------------------------------------------------
 */
#include <tcn_sv.h>
#include <tcn_sv_m.h>
#include <tcn_as.h>

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - process data
 * --------------------------------------------------------------------------
 */
#include <tcn_pd.h>
#include <tcn_ap.h>

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - message data
 * --------------------------------------------------------------------------
 */
#ifdef TCN_MD
#ifdef TCN_DEMO_MD
#   include <tcn_am.h>
#   include <tcn_am.h>
#   include <tcn_tnm.h>
#   include <tcn_ma.h>
#   include <tcn_mm.h>
#endif /* #ifdef TCN_DEMO_MD */
#endif /* #ifdef TCN_MD */

/* --------------------------------------------------------------------------
 *  TCN Software Architecture - extra (configuration manager)
 * --------------------------------------------------------------------------
 */
#ifdef TCN_DEMO_BA_D2000
#   include <tcn_cm.h>
#endif /* #ifdef TCN_DEMO_BA_D2000 */


/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
#define TCN_DEMO_ON_ERROR(_result_)


/* ==========================================================================
 *
 *  Local Variables
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Message Data
 * --------------------------------------------------------------------------
 */
#ifdef TCN_MD
#ifdef TCN_DEMO_MD
TCN_DECL_LOCAL  TNM_MSG_Reply_Read_Mvb_Status   msg_reply_read_mvb_status;
#endif /* #ifdef TCN_DEMO_MD */
#endif /* #ifdef TCN_MD */

TYPE_MD_PRT_CFG m_md_prt_cfg[32];
MD_MVB_CONTROL  m_md_mvb_control;
UNSIGNED16 pc104_data_buff[17 * 32];
UNSIGNED16 pc104_data_old[17 * 32];
UNSIGNED8 MVB_Initial_Regs_Read = 0;
UNSIGNED16 temp_buff[17];
UNSIGNED8 reg_num[32] = {0};
/* ==========================================================================
 *
 *  Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : am_call_confirm
 * --------------------------------------------------------------------------
 */
#ifdef TCN_MD
#ifdef TCN_DEMO_MD
void
am_call_confirm
(
    UNSIGNED8           caller_function,
    void                *am_caller_ref,
    const AM_ADDRESS    *replier,
    void                *in_msg_adr,
    UNSIGNED32          in_msg_size,
    AM_RESULT           status
)
{
TNM_MSG_Reply_Read_Mvb_Status   *p;

    p = (TNM_MSG_Reply_Read_Mvb_Status*)in_msg_adr;
    TCN_MACRO_CONVERT_BE16_TO_CPU(&p->device_address);
    TCN_MACRO_CONVERT_BE16_TO_CPU(&p->device_status);
    TCN_MACRO_CONVERT_BE32_TO_CPU(&p->lineA_errors);
    TCN_MACRO_CONVERT_BE32_TO_CPU(&p->lineB_errors);

    TCN_OSL_PRINTF("- am_call_confirm()\n");
    TCN_OSL_PRINTF("  -> caller_function     =%d\n", caller_function);
    TCN_OSL_PRINTF("  -> replier.sg_node     =%d\n", replier->sg_node);
    TCN_OSL_PRINTF("  -> replier.func_or_stat=%d\n", replier->func_or_stat);
    TCN_OSL_PRINTF("  -> replier.next_station=%d\n", replier->next_station);
    TCN_OSL_PRINTF("  -> replier.topo_counter=%d\n", replier->topo_counter);
    TCN_OSL_PRINTF("  -> in_msg_size         =%ld\n", in_msg_size);
    TCN_OSL_PRINTF("  -> status              =%d\n", status);
    TCN_OSL_PRINTF("  TNM_MSG_Reply_Read_Mvb_Status:\n");
    TCN_OSL_PRINTF("  -> tnm_key          =0x%02X\n", p->tnm_key);
    TCN_OSL_PRINTF("  -> sif_code         =%d\n", p->sif_code);
    TCN_OSL_PRINTF("  -> bus_id           =%d\n", p->bus_id);
    TCN_OSL_PRINTF("  -> reserved1        =0x%02X\n", p->reserved1);
    TCN_OSL_PRINTF("  -> reserved2        =0x%04X\n", p->reserved2);
    TCN_OSL_PRINTF("  -> device_address   =0x%04X\n", p->device_address);
    TCN_OSL_PRINTF("  -> mvb_hardware_name='%s'\n", \
        (char*)&p->mvb_hardware_name);
    TCN_OSL_PRINTF("  -> mvb_software_name='%s'\n", \
        (char*)&p->mvb_software_name);
    TCN_OSL_PRINTF("  -> device_status    =0x%04X\n", p->device_status);
    TCN_OSL_PRINTF("  -> reserved3        =0x%02X\n", p->reserved3);
    TCN_OSL_PRINTF("  -> t_ignore         =%d\n", p->t_ignore);
    TCN_OSL_PRINTF("  -> lineA_errors     =0x%08lX\n", p->lineA_errors);
    TCN_OSL_PRINTF("  -> lineB_errors     =0x%08lX\n", p->lineB_errors);

} /* am_call_confirm */
#endif /* #ifdef TCN_DEMO_MD */
#endif /* #ifdef TCN_MD */

UNSIGNED16 tcn_reinit(void) {
  /* miscellaneous */
  UNSIGNED16          result;
  UNSIGNED32          iter = 0;
  /* supervision */
  AS_RESULT           as_result;
  SV_MVB_CONTROL      mvb_control;
	/* process variables */
  AP_RESULT           ap_result;
  WORD16              pd_port_address;
  PD_PRT_ATTR         pd_prt_attr;
  DS_NAME             ds_name;
  WORD8               pd_port_data[32];
  UNSIGNED16          pd_port_freshness;
	result = 0;
	as_result = as_init();
  if (as_result != AS_OK)
  {
    TCN_OSL_PRINTF("ERROR: as_result=%u\n", as_result);
    TCN_DEMO_ON_ERROR(as_result);
    result = (UNSIGNED16)as_result;
  }
	if (0 == result)
  {
    TCN_OSL_PRINTF("- ap_init()\n");
    ap_result = ap_init();
    if (ap_result != AP_OK)
    {
      TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
      TCN_DEMO_ON_ERROR(ap_result);
      result = (UNSIGNED16)ap_result;
    } /* if (ap_result != AP_OK) */
  } /* if (0 == result) */

  if (0 == result)
  {
    TCN_OSL_PRINTF("- ap_ts_config()\n");
    ap_result = ap_ts_config(0, 16);
    if (ap_result != AP_OK)
    {
      TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
      TCN_DEMO_ON_ERROR(ap_result);
      result = (UNSIGNED16)ap_result;
    } /* if (ap_result != AP_OK) */
  } /* if (0 == result) */
	
	/* ----------------------------------------------------------------------
   *  ap_port_manage(PD_PRT_CMD_CONFIG)
   * ----------------------------------------------------------------------
   */
  if (0 == result)
  {
    TCN_OSL_PRINTF("- ap_port_manage(PD_PRT_CMD_CONFIG)\n");
    /* configure port 0x010 as sink with size of 2 bytes */
    pd_port_address            = 0x010;
    pd_prt_attr.port_size      = 2;
    pd_prt_attr.port_config    = PD_SINK_PORT;
    pd_prt_attr.p_bus_specific = NULL;
    ap_result =                 \
      ap_port_manage          \
        (                       \
          0,                  \
          pd_port_address,    \
          PD_PRT_CMD_CONFIG,  \
          &pd_prt_attr        \
        );
    if (ap_result != AP_OK)
    {
			TCN_OSL_PRINTF("ERROR1!!!\n");
      TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
      TCN_DEMO_ON_ERROR(ap_result);
      result = (UNSIGNED16)ap_result;
    } /* if (ap_result != AP_OK) */
  } /* if (0 == result) */
  if (0 == result)
  {
    /* configure port 0x018 as source with size of 2 bytes */
    pd_port_address            = 0x018;
    pd_prt_attr.port_size      = 2;
    pd_prt_attr.port_config    = PD_SOURCE_PORT;
    pd_prt_attr.p_bus_specific = NULL;
    ap_result =                 \
        ap_port_manage          \
          (                       \
            0,                  \
            pd_port_address,    \
            PD_PRT_CMD_CONFIG,  \
            &pd_prt_attr        \
          );
    if (ap_result != AP_OK)
    {
      TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
      TCN_DEMO_ON_ERROR(ap_result);
      result = (UNSIGNED16)ap_result;
    } /* if (ap_result != AP_OK) */
  } /* if (0 == result) */
}


/* --------------------------------------------------------------------------
 *  Procedure : tcn_demo
 * --------------------------------------------------------------------------
 */

void MVB_Service(void) {
	UNSIGNED16 age;
	UNSIGNED16 sink_port_count = 0;
	UNSIGNED8 pis = 0;
	UNSIGNED8 i = 0;
	DS_NAME   ds_name;
	UNSIGNED16  pd_port_freshness;

	UNSIGNED8 freshness = 0;
  
	if (MVB_START_FLAG == 0)
		return;
	if (MVB_Initial_Regs_Read == 0) {
		
	  for (pis = 0; pis < m_md_mvb_control.port_number; pis ++) {
			if (m_md_prt_cfg[pis].pd_prt_attr.port_config == PD_SINK_PORT) {
			  memset(temp_buff, 0, sizeof(temp_buff));
				ds_name.port_address = m_md_prt_cfg[pis].pd_port_address;
				ds_name.traffic_store_id = 0;
				ap_get_dataset(&ds_name, temp_buff, &pd_port_freshness);
				memcpy(&pc104_data_buff[pis*17], &temp_buff[1],m_md_prt_cfg[pis].pd_prt_attr.port_size);
				//delay_ms(1);
				//delay_us(250);
			}
	    reg_num[pis] = (m_md_prt_cfg[pis].pd_prt_attr.port_size) / 2;
		}
		MVB_Initial_Regs_Read = 1;
		
		return;
	}
	
  for (pis = 0; pis < m_md_mvb_control.port_number; pis ++) {
		if (m_md_prt_cfg[pis].pd_prt_attr.port_config == PD_SINK_PORT) {
		  memset(temp_buff, 0, sizeof(temp_buff));
		  ds_name.port_address = m_md_prt_cfg[pis].pd_port_address;
			ds_name.traffic_store_id = 0;
			ap_get_dataset(&ds_name, temp_buff, &pd_port_freshness);
			memcpy(&pc104_data_buff[pis*17], temp_buff, m_md_prt_cfg[pis].pd_prt_attr.port_size);
			for (i = 0; i < reg_num[pis]; i++) {
			  if (pc104_data_old[pis * 17 + i] != pc104_data_buff[pis * 17 + i]) {
					// Data is different, freshness is true
					freshness = 1;
					// Update pc104_data_old
					pc104_data_old[pis * 17 + i] = pc104_data_buff[pis * 17 + i];
				}
			}
			freshness = 0;
			
			if (freshness == 1) {
			  pc104_data_buff[(pis + 1)* (reg_num[pis]) + pis * 17] = 0x0001;
				freshness = 0;
			} else {
			  // Data is same, freshness is false
			  pc104_data_buff[(pis + 1)* (reg_num[pis]) + pis * 17] = 0x0000;
			}
		} else {
			memcpy(&temp_buff[1], &pc104_data_buff[pis*17], m_md_prt_cfg[pis].pd_prt_attr.port_size);
			//printf("Data buff addr: %x\r\n", mvb_ts_cfg[pis].prt_addr);
		  //MVBCPutPort(mvb_ts_cfg[pis].prt_addr, &pc104_data_buff[pis * 17], 0);
			ds_name.port_address = m_md_prt_cfg[pis].pd_port_address;
			ds_name.traffic_store_id = 0;
			ap_put_dataset(&ds_name, &temp_buff[1]);
		}
		//delay_ms(1);
		//delay_us(250);
  }
	// Set MVB watchdog 16384ms
	//MVBCRetriggerWd(0, 0x8200);
}


UNSIGNED8 MVB_init(void) {
	UNSIGNED8           pis;
  UNSIGNED16          result;
  //SV_MVB_CONTROL      mvb_control;
  AP_RESULT           ap_result;
  AS_RESULT           as_result;
	
  if (as_init() != AS_OK) 
	  return -1;
	if (ap_init() != AP_OK)
		return -1;
	if (ap_ts_config(0, 16) != AP_OK)
		return -1;
	for(pis = 0; pis < m_md_mvb_control.port_number; pis++) {
		ap_result = ap_port_manage(0, m_md_prt_cfg[pis].pd_port_address, PD_PRT_CMD_CONFIG, &m_md_prt_cfg[pis].pd_prt_attr);
	  if (ap_result != AP_OK)
			return -1;
	}
	
	m_md_mvb_control.sv_control.reserved1       = 0x00;
  m_md_mvb_control.sv_control.t_ignore        = 0x00;
  m_md_mvb_control.sv_control.reserved2       = 0x00;
  m_md_mvb_control.sv_control.command         = SV_MVB_CTRL_CMD_BIT_SLA;
  m_md_mvb_control.sv_control.command        |= SV_MVB_CTRL_CMD_BIT_SLB;
  m_md_mvb_control.sv_control.reserved3       = 0x0000;
	
	as_result = as_service_handler(0, SV_MVB_SERVICE_WRITE_CONTROL, (void*)&m_md_mvb_control.sv_control);
	
	if (as_result != AS_OK)
		return -1;
	return 0;
}


UNSIGNED16
tcn_demo (void)
{
    /* miscellaneous */
    UNSIGNED16          result;
    UNSIGNED32          iter = 0;
    /* supervision */
    AS_RESULT           as_result;
    SV_MVB_CONTROL      mvb_control;
#ifdef TCN_DEMO_BA_TCN
    WORD8               *mvb_administrator;
#endif
#ifdef TCN_DEMO_BA_D2000
    CM_RESULT           cm_result;
    CM_D2000_PARAMETER  cm_d2000_parameter;
#endif /* #ifdef TCN_DEMO_BA_D2000 */

    /* process variables */
    AP_RESULT           ap_result;
    WORD16              pd_port_address;
    PD_PRT_ATTR         pd_prt_attr;
    DS_NAME             ds_name;
    WORD8               pd_port_data[32];
    UNSIGNED16          pd_port_freshness;

    /* message data */
#ifdef TCN_MD
#ifdef TCN_DEMO_MD
    AM_RESULT           am_result;
    MM_RESULT           mm_result;
    AM_ADDRESS          am_replier;
    UNSIGNED16          am_caller_ref;
    TNM_MSG_Call_Read_Mvb_Status \
                        msg_call_read_mvb_status;
#endif /* #ifdef TCN_DEMO_MD */
#endif /* #ifdef TCN_MD */


    result = 0;

    TCN_OSL_PRINTF("\n");
    TCN_OSL_PRINTF("Demo of TCN Driver API\n");
    TCN_OSL_PRINTF("======================\n");


    /* ======================================================================
     *
     *  general supervision initialisation
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  as_init()
     * ----------------------------------------------------------------------
     */
    TCN_OSL_PRINTF("- as_init()\n");
    as_result = as_init();
    if (as_result != AS_OK)
    {
        TCN_OSL_PRINTF("ERROR: as_result=%u\n", as_result);
        TCN_DEMO_ON_ERROR(as_result);
        result = (UNSIGNED16)as_result;
    } /* if (as_result != AS_OK) */


    /* ======================================================================
     *
     *  process data port initialisation
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  ap_init()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- ap_init()\n");
        ap_result = ap_init();
        if (ap_result != AP_OK)
        {
            TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
            TCN_DEMO_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (ap_result != AP_OK) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  ap_ts_config()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- ap_ts_config()\n");
        ap_result = ap_ts_config(0, 16);
        if (ap_result != AP_OK)
        {
            TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
            TCN_DEMO_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (ap_result != AP_OK) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  ap_port_manage(PD_PRT_CMD_CONFIG)
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- ap_port_manage(PD_PRT_CMD_CONFIG)\n");
        /* configure port 0x010 as sink with size of 2 bytes */
        pd_port_address            = 0x010;
        pd_prt_attr.port_size      = 2;
        pd_prt_attr.port_config    = PD_SINK_PORT;
        pd_prt_attr.p_bus_specific = NULL;
        ap_result =                 \
            ap_port_manage          \
            (                       \
                0,                  \
                pd_port_address,    \
                PD_PRT_CMD_CONFIG,  \
                &pd_prt_attr        \
            );
        if (ap_result != AP_OK)
        {
					  TCN_OSL_PRINTF("ERROR1!!!\n");
            TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
            TCN_DEMO_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (ap_result != AP_OK) */
    } /* if (0 == result) */
    if (0 == result)
    {
        /* configure port 0x018 as source with size of 2 bytes */
        pd_port_address            = 0x018;
        pd_prt_attr.port_size      = 2;
        pd_prt_attr.port_config    = PD_SOURCE_PORT;
        pd_prt_attr.p_bus_specific = NULL;
        ap_result =                 \
            ap_port_manage          \
            (                       \
                0,                  \
                pd_port_address,    \
                PD_PRT_CMD_CONFIG,  \
                &pd_prt_attr        \
            );
        if (ap_result != AP_OK)
        {
            TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
            TCN_DEMO_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (ap_result != AP_OK) */
    } /* if (0 == result) */


#ifdef TCN_MD
#ifdef TCN_DEMO_MD
    /* ======================================================================
     *
     *  message data initialisation
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  am_init()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- am_init()\n");
        am_result = am_init();
        if (am_result != AM_OK)
        {
            TCN_OSL_PRINTF("ERROR: am_result=%u\n", am_result);
            TCN_DEMO_ON_ERROR(am_result);
            result = (UNSIGNED16)am_result;
        } /* if (am_result != AM_OK) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  am_announce_device()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
#ifdef MUELL_MD16
        TCN_OSL_PRINTF("- am_announce_device(5, 10, 400, 1)\n");
        am_result = am_announce_device(5, 10, 400, 1);
#else /* #ifdef MUELL_MD16 */
        TCN_OSL_PRINTF("- am_announce_device(5, 10, 400, 7)\n");
        am_result = am_announce_device(5, 10, 400, 7);
#endif /* #else */
        if (am_result != AM_OK)
        {
            TCN_OSL_PRINTF("ERROR: am_result=%u\n", am_result);
            TCN_DEMO_ON_ERROR(am_result);
            result = (UNSIGNED16)am_result;
        } /* if (am_result != AM_OK) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  ma_init()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF("- ma_init()\n");
        mm_result = ma_init();
        if (mm_result != MM_OK)
        {
            TCN_OSL_PRINTF("ERROR: mm_result=%u\n", mm_result);
            TCN_DEMO_ON_ERROR(mm_result);
            result = (UNSIGNED16)mm_result;
        } /* if (mm_result != MM_OK) */
    } /* if (0 == result) */
#endif /* #ifdef TCN_DEMO_MD */
#endif /* #ifdef TCN_MD */


    /* ======================================================================
     *
     *  advanced supervision initialisation
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF \
            ("- as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)\n");
        /* set device address = 0x001, enable MVB line A & B */
        mvb_control.device_address  = 0x001;
        mvb_control.reserved1       = 0x00;
        mvb_control.t_ignore        = 0x00;
        mvb_control.reserved2       = 0x00;
        mvb_control.command         = SV_MVB_CTRL_CMD_BIT_SLA;
        mvb_control.command        |= SV_MVB_CTRL_CMD_BIT_SLB;
        mvb_control.reserved3       = 0x0000;
        as_result =                             \
            as_service_handler                  \
            (                                   \
                0,                              \
                SV_MVB_SERVICE_WRITE_CONTROL,   \
                (void*)&mvb_control             \
            );
        if (as_result != AS_OK)
        {
            TCN_OSL_PRINTF("ERROR: as_result=%u\n", as_result);
            TCN_DEMO_ON_ERROR(as_result);
            result = (UNSIGNED16)as_result;
        } /* if (as_result != AS_OK) */
    } /* if (0 == result) */

#ifdef TCN_DEMO_BA_TCN
    /* ----------------------------------------------------------------------
     *  as_service_handler(SV_MVB_SERVICE_WRITE_ADMINISTRATOR)
     * ----------------------------------------------------------------------
     *  NOTE:
     *  (1) Allocate a memory buffer for the BA configuration.
     *      See 'MVB_Administrator' object in IEC 61375-1.
     *      Pointer to the memory buffer: mvb_administrator
     *  (2) Fill the memory buffer with the data of the
     *      'MVB_Administrator' object.
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF \
            ("- as_service_handler(SV_MVB_SERVICE_WRITE_ADMINISTRATOR)\n");
        mvb_administrator = /* todo */
        as_result =                                 \
            as_service_handler                      \
            (                                       \
                0,                                  \
                SV_MVB_SERVICE_WRITE_ADMINISTRATOR, \
                (void*)mvb_administrator            \
            );
        if (as_result != AS_OK)
        {
            TCN_OSL_PRINTF("ERROR: as_result=%u\n", as_result);
            TCN_DEMO_ON_ERROR(as_result);
            result = (UNSIGNED16)as_result;
        } /* if (as_result != AS_OK) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF \
            ("- as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)\n");
        /* enable BA */
        mvb_control.command |= SV_MVB_CTRL_CMD_BIT_AON;
        as_result =                             \
            as_service_handler                  \
            (                                   \
                0,                              \
                SV_MVB_SERVICE_WRITE_CONTROL,   \
                (void*)&mvb_control             \
            );
        if (as_result != AS_OK)
        {
            TCN_OSL_PRINTF("ERROR: as_result=%u\n", as_result);
            TCN_DEMO_ON_ERROR(as_result);
            result = (UNSIGNED16)as_result;
        } /* if (as_result != AS_OK) */
    } /* if (0 == result) */
#endif /* #ifdef TCN_DEMO_BA_TCN */


#ifdef TCN_DEMO_BA_D2000
    if (0 == result)
    {
        memset((void*)&cm_d2000_parameter, 0, sizeof(cm_d2000_parameter));

        cm_d2000_parameter.nr_entries = 1;
        cm_d2000_parameter.entries_list[0].entry  = \
            CM_D2000_ENTRY_MVB_ADMINISTRATOR;
        cm_d2000_parameter.entries_list[0].bus_id = 0;

#ifdef TCN_CM_PRINT
        TCN_OSL_PRINTF("- cm_config_device()\n");
#endif /* #ifdef TCN_CM_PRINT */
        cm_result =                     \
            cm_config_device            \
            (                           \
                CM_CONFIG_TYPE_D2000,   \
                "ba_d2000.bin",         \
                &cm_d2000_parameter     \
            );
        if (cm_result != CM_OK)
        {
            TCN_OSL_PRINTF("ERROR: cm_result=%u\n", cm_result);
            TCN_DEMO_ON_ERROR(cm_result);
            result = (UNSIGNED16)cm_result;
        } /* if (cm_result != CM_OK) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        TCN_OSL_PRINTF \
            ("- as_service_handler(SV_MVB_SERVICE_WRITE_CONTROL)\n");
        /* enable BA */
        mvb_control.command |= SV_MVB_CTRL_CMD_BIT_AON;
        as_result =                             \
            as_service_handler                  \
            (                                   \
                0,                              \
                SV_MVB_SERVICE_WRITE_CONTROL,   \
                (void*)&mvb_control             \
            );
        if (as_result != AS_OK)
        {
            TCN_OSL_PRINTF("ERROR: as_result=%u\n", as_result);
            TCN_DEMO_ON_ERROR(as_result);
            result = (UNSIGNED16)as_result;
        } /* if (as_result != AS_OK) */
    } /* if (0 == result) */
#endif /* #ifdef TCN_DEMO_BA_D2000 */


#ifdef TCN_MD
#ifdef TCN_DEMO_MD
    /* ======================================================================
     *
     *  message data communication
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  TNM service 'Read_Mvb_Status'
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        msg_call_read_mvb_status.tnm_key   = (ENUM8)TNM_KEY_DEFAULT_CALL;
        msg_call_read_mvb_status.sif_code  = (ENUM8)READ_MVB_STATUS;
        msg_call_read_mvb_status.bus_id    = (UNSIGNED8)0;
        msg_call_read_mvb_status.reserved1 = (WORD8)0;

        am_replier.sg_node      = AM_SAME_NODE;
        am_replier.func_or_stat = AM_AGENT_FCT;
        am_replier.next_station = AM_SAME_STATION;
        am_replier.topo_counter = 0;

        am_call_request                                         \
        (                                                       \
            (UNSIGNED8)AM_MANAGER_FCT,                          \
            (AM_ADDRESS*)&am_replier,                           \
            (void*)&msg_call_read_mvb_status,                   \
            (UNSIGNED32)sizeof(TNM_MSG_Call_Read_Mvb_Status),   \
            (void*)&msg_reply_read_mvb_status,                  \
            (UNSIGNED32)sizeof(TNM_MSG_Reply_Read_Mvb_Status),  \
            (UNSIGNED16)100,                                    \
            (AM_CALL_CONFIRM)am_call_confirm,                   \
            (void*)&am_caller_ref                               \
        );
    } /* if (0 == result) */
#endif /* #ifdef TCN_DEMO_MD */
#endif /* #ifdef TCN_MD */


    /* ======================================================================
     *
     *  process data port communication
     *
     * ======================================================================
     */

    /* ----------------------------------------------------------------------
     *  ap_get_dataset()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        ds_name.traffic_store_id = 0;
        ds_name.port_address     = 0x010;
        TCN_OSL_PRINTF("- ap_get_dataset(0x010)\n");
        ap_result = ap_get_dataset(&ds_name, &pd_port_data[0], \
            &pd_port_freshness);
        TCN_OSL_PRINTF("  -> result    = %d\n", ap_result);
        TCN_OSL_PRINTF("  -> port_data = 0x%02X 0x%02X\n", pd_port_data[0], \
            pd_port_data[1]);
        if (ap_result != AP_OK)
        {
            TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
            TCN_DEMO_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (ap_result != AP_OK) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  ap_get_dataset()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        ds_name.traffic_store_id = 0;
        ds_name.port_address     = 0x018;
        TCN_OSL_PRINTF("- ap_get_dataset(0x018)\n");
        ap_result = ap_get_dataset(&ds_name, &pd_port_data[0], \
            &pd_port_freshness);
        TCN_OSL_PRINTF("  -> result    = %d\n", ap_result);
        TCN_OSL_PRINTF("  -> port_data = 0x%02X 0x%02X\n", pd_port_data[0], \
            pd_port_data[1]);
        if (ap_result != AP_OK)
        {
            TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
            TCN_DEMO_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (ap_result != AP_OK) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  ap_put_dataset()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        ds_name.traffic_store_id = 0;
        ds_name.port_address     = 0x018;
        pd_port_data[0]          = 0x12;
        pd_port_data[1]          = 0x34;
        TCN_OSL_PRINTF("- ap_put_dataset(0x018)\n");
        TCN_OSL_PRINTF("  -> port_data = 0x%02X 0x%02X\n", pd_port_data[0], \
            pd_port_data[1]);
        ap_result = ap_put_dataset(&ds_name, &pd_port_data[0]);
        if (ap_result != AP_OK)
        {
            TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
            TCN_DEMO_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (ap_result != AP_OK) */
    } /* if (0 == result) */

    /* ----------------------------------------------------------------------
     *  ap_get_dataset()
     * ----------------------------------------------------------------------
     */
    if (0 == result)
    {
        ds_name.traffic_store_id = 0;
        ds_name.port_address     = 0x018;
        TCN_OSL_PRINTF("- ap_get_dataset(0x018)\n");
        ap_result = ap_get_dataset(&ds_name, &pd_port_data[0], \
            &pd_port_freshness);
        TCN_OSL_PRINTF("  -> result    = %d\n", ap_result);
        TCN_OSL_PRINTF("  -> port_data = 0x%02X 0x%02X\n", pd_port_data[0], \
            pd_port_data[1]);
        if (ap_result != AP_OK)
        {
            TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
            TCN_DEMO_ON_ERROR(ap_result);
            result = (UNSIGNED16)ap_result;
        } /* if (ap_result != AP_OK) */
    } /* if (0 == result) */

    if (0 == result)
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF \
            ("*************************************************\n");
        TCN_OSL_PRINTF \
            ("* TCN demo application terminates successfully! *\n");
        TCN_OSL_PRINTF \
            ("*************************************************\n");
			  m_localbus = 0;
			  while(1) {
			    TCN_OSL_PRINTF("<-----------Iteration %ld----------->\r\n", iter++);
		      if (0 == result) {
            ds_name.traffic_store_id = 0;
            ds_name.port_address     = 0x018;
            pd_port_data[0]          = 0x33;
            pd_port_data[1]          = 0x35;
            //TCN_OSL_PRINTF("- ap_put_dataset(0x018)\n");
            // TCN_OSL_PRINTF("  -> port_data = 0x%02X 0x%02X\n", pd_port_data[0], \
              pd_port_data[1]);
            ap_result = ap_put_dataset(&ds_name, &pd_port_data[0]);
            if (ap_result != AP_OK) {
              TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
              //TCN_DEMO_ON_ERROR(ap_result);
              result = (UNSIGNED16)ap_result;
					    break;
							//continue;
            } /* if (ap_result != AP_OK) */
          } /* if (0 == result) */
          delay_ms(20);
          if (0 == result) {
            ds_name.traffic_store_id = 0;
            ds_name.port_address     = 0x010;
						pd_port_data[0]          = 0x0;
            pd_port_data[1]          = 0x0;
            //TCN_OSL_PRINTF("- ap_get_dataset(0x010)\n");
            ap_result = ap_get_dataset(&ds_name, &pd_port_data[0], \
              &pd_port_freshness);
            //TCN_OSL_PRINTF("  -> result    = %d\n", ap_result);
            //TCN_OSL_PRINTF("  -> port_data = 0x%02X 0x%02X\n", pd_port_data[0], \
              pd_port_data[1]);
            if (ap_result != AP_OK) {
              TCN_OSL_PRINTF("ERROR: ap_result=%u\n", ap_result);
              //TCN_DEMO_ON_ERROR(ap_result);
              //result = (UNSIGNED16)ap_result;
							result = 0;
							// Reset MVB and reinit
							MVBC_RESET = 1;
	            delay_ms(500);
	            MVBC_RESET = 0;
              delay_ms(5000);
							
							tcn_reinit();
							
							//while (1) {
							  //TCN_OSL_PRINTF("read 0x4fd = 0x%x\r\n", Localbus_ReadData(0x4fd));
								//delay_ms(512);
							//}
					    //break;
							continue;
            } /* if (ap_result != AP_OK) */
          } 
		    }
    } /* if (0 == result) */
    else
    {
        TCN_OSL_PRINTF("\n");
        TCN_OSL_PRINTF \
            ("****************************************************\n");
        TCN_OSL_PRINTF \
            ("* TCN demo application terminates with error (%2d). *\n", \
                result);
        TCN_OSL_PRINTF \
            ("****************************************************\n");
    } /* else */
		
		
		

    return(result);

} /* tcn_demo */


/* --------------------------------------------------------------------------
 *  Procedure : main
 * --------------------------------------------------------------------------
 */
/*int
main (void)
{
    UNSIGNED16  result;

    result = tcn_demo();

    TCN_MACRO_TERMINATE_MAIN(result);

    return((int)result);

}*/ /* main */


#endif /* #ifdef TCN_DEMO */
