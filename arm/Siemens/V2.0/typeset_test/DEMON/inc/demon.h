#ifndef DEMON_H_
#define DEMON_H_

#include "config.h"
#include "host_dep.h"
#include "dpr_dep.h"
#include "cl1_ret.h"
#include "cl1_1.h"

// MVB Port 配置结构体
extern TYPE_LP_PRT_CFG mvb_ts_cfg[32];
// MVB TS memory 结构体
extern TYPE_LP_TS_CFG mvb;
// PC104 数据缓存
extern UNSIGNED16 pc104_data_buff[17 * 32];

void MVB_Service(void);
void MVB_Parameter_Init(void);

int test_main (void);

#endif
