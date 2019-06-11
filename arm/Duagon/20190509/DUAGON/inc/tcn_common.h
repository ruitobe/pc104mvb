#ifndef TCN_COMMON_H
#define TCN_COMMON_H


#include <tcn_def.h>
#include <mue_def.h>
#include "tcn_pd.h"
#include "tcn_sv_m.h"

//#define TCN_DEMO_MD 1
//#define TCN_MD 1
//#define TCN_CHECK_PARAMETER  1
//#define TCN_TEST 1
//#define TCN_STR  1
//#define MUE_PD_FULL  1
//#define DMUE_TEST_MDFL 1
//#define MUELL_MDFL 1
#define TCN_PRJ
/*
typedef struct
{
    UNSIGNED8   port_size;
    BITSET8     port_config;
    WORD16      reserved;
    void        *p_bus_specific;
}   PD_PRT_ATTR;
*/
typedef struct
{

	  UNSIGNED16  pd_port_address;
    PD_PRT_ATTR pd_prt_attr;
}   TYPE_MD_PRT_CFG;

typedef struct
{ 
    UNSIGNED8        port_number;
	  SV_MVB_CONTROL   sv_control;
}   MD_MVB_CONTROL;


extern TYPE_MD_PRT_CFG m_md_prt_cfg[32];
extern MD_MVB_CONTROL  m_md_mvb_control;
UNSIGNED16 tcn_demo (void);
UNSIGNED32 mue_calibrate (void);
UNSIGNED8 MVB_init(void);
//TCN_DECL_PUBLIC MUE_RESULT mue_test(void);
//UNSIGNED16 tcn_test(void);
#endif