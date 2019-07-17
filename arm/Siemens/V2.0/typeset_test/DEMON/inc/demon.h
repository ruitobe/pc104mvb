#ifndef DEMON_H_
#define DEMON_H_

#include "config.h"
#include "host_dep.h"
#include "dpr_dep.h"
#include "cl1_ret.h"
#include "cl1_1.h"
#include "mvb_typeset_test.h"

// MVB Port 配置结构体
extern TYPE_LP_PRT_CFG mvb_ts_cfg[32];
// MVB TS memory 结构体
extern TYPE_LP_TS_CFG mvb;
// PC104 数据缓存
extern UNSIGNED16 pc104_data_buff[17 * 32];

void MVB_Service(void);
void MVB_Parameter_Init(void);

int test_main (void);
void test_mvb_init(void);

bool check_port_event(struct port_group * port);
bool reply_msg_to_zsg(struct port_group * port, void * msg, int msg_size);
int copy_req_msg(struct port_group * port, void * buffer, int msg_size);

#endif
