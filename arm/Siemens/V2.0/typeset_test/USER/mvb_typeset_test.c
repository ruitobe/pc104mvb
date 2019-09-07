#include "mvb_typeset_test.h"
#include "demon.h"
#include "iwdg.h"

bool is_sys_act = true;

struct port_group g_3368_3360_group = {
	.port_req = 3368,
	.port_rpl = 3360,
	.port_cyc = CYC_TIME_3360_3368,
	.port_size = 16,
	.port_timeout = 0,
	.is_req_event = 0,
	.is_need_reply = 0
}; 

struct port_group g_3390_3376_group = {
	.port_req = 3390,
	.port_rpl = 3376,
	.port_cyc = CYC_TIME_3376_3390,
	.port_size = 8,
	.port_timeout = 0,
	.is_req_event = 0,
	.is_need_reply = 0
};

struct port_group g_3391_3377_group = {
	.port_req = 3391,
	.port_rpl = 3377,
	.port_cyc = CYC_TIME_3377_3391,
	.port_size = 32,
	.port_timeout = 0,
	.is_req_event = 0,
	.is_need_reply = 0
};

struct port_group g_3392_3378_group = {
	.port_req = 3392,
	.port_rpl = 3378,
	.port_cyc = CYC_TIME_3378_3392,
	.port_size = 4,
	.port_timeout = 0,
	.is_req_event = 0,
	.is_need_reply = 0
};

struct port_group g_3393_3379_group = {
	.port_req = 3393,
	.port_rpl = 3379,
	.port_cyc = CYC_TIME_3379_3393,
	.port_size = 32,
	.port_timeout = 0,
	.is_req_event = 0,
	.is_need_reply = 0
};

struct port_group g_3394_3380_group = {
	.port_req = 3394,
	.port_rpl = 3380,
	.port_cyc = CYC_TIME_3380_3394,
	.port_size = 32,
	.port_timeout = 0,
	.is_req_event = 0,
	.is_need_reply = 0
};
struct g_3368_msg_t g_3368_msg;
struct g_3360_msg_t g_3360_msg = DEFAULT_3360_MSG;
const struct g_3360_msg_t g_3360_msg_default = DEFAULT_3360_MSG;
struct g_3390_msg_t g_3390_msg;
struct g_3376_msg_t g_3376_msg = DEFAULT_3376_MSG;
const struct g_3376_msg_t g_3376_msg_default = DEFAULT_3376_MSG;
struct g_3391_msg_t g_3391_msg;
struct g_3377_msg_t g_3377_msg = DEFAULT_3377_MSG;
const struct g_3377_msg_t g_3377_msg_default = DEFAULT_3377_MSG;
struct g_3392_msg_t g_3392_msg;
struct g_3378_msg_t g_3378_msg = DEFAULT_3378_MSG;
const struct g_3378_msg_t g_3378_msg_default = DEFAULT_3378_MSG;
struct g_3393_msg_t g_3393_msg;
struct g_3379_msg_t g_3379_msg = DEFAULT_3379_MSG;
const struct g_3379_msg_t g_3379_msg_default = DEFAULT_3379_MSG;
struct g_3394_msg_t g_3394_msg;
struct g_3380_msg_t g_3380_msg = DEFAULT_3380_MSG;
const struct g_3380_msg_t g_3380_msg_default = DEFAULT_3380_MSG;
 
struct port_group * g_3368_3360_port;
struct port_group * g_3390_3376_port;
struct port_group * g_3391_3377_port;
struct port_group * g_3392_3378_port;
struct port_group * g_3393_3379_port;
struct port_group * g_3394_3380_port;
/*
 *There is a sys timer, the period is 4ms.
 */
#define TIMER_PERIOD 4 
static uint32_t sys_tick = 0;

/* These api provided by driver
extern bool check_port_event(struct port_group * port);
extern bool reply_msg_to_zsg(struct port_group * port, 
							void * msg,
							int msg_size);
extern int copy_req_msg(struct port_group * port, 
						void * buffer,
						int msg_size);*/

void event_check(struct port_group * port)
{
	if(port != NULL && check_port_event(port)) {
		port->is_req_event = true;
		port->to_event = false;
		port->port_timeout = 0;
	} else {
		port->port_timeout += TIMER_PERIOD;
		if (port->port_timeout >= (port->port_cyc << 2)) {
			/*port time out fired*/
			port->to_event = true;
			port->port_timeout = 0;
		}
	}
}


void test_event_check(void) 
{
	event_check(g_3368_3360_port); 
	event_check(g_3390_3376_port); 
	event_check(g_3391_3377_port); 
	event_check(g_3392_3378_port); 
	event_check(g_3393_3379_port); 
	event_check(g_3394_3380_port); 
}

static void test_host_failure(void)
{
	/*need fire watchdog*/
	wdt_fired();
}

int process_msg_from_3368(struct g_3368_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(&g_3360_msg, (uint8_t *)msg, sizeof(g_3360_msg));
	/*reset err/warning*/
	if (msg->INT16_2) {
		g_3360_msg.UINT16_2 = 0;
		g_3360_msg.UINT16_3 = 0;
	}
	/*Host failure*/
	if(!msg->BOOLEAN_1) {
		is_sys_act = false;
		test_host_failure();
	} else {
		g_3360_msg.INT16_1++;
		g_3360_msg.INT16_2++;
		g_3360_msg.UINT16_1++;
		g_3360_msg.BOOLEAN_1++;
		g_3360_msg.BOOLEAN_2++;
		reply_msg_to_zsg(g_3368_3360_port, &g_3360_msg, sizeof(g_3360_msg));
	}
	return 0;
}

int process_msg_from_3390(struct g_3390_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(&g_3376_msg, msg, sizeof(g_3376_msg));
	g_3376_msg.INT16_1++;
	g_3376_msg.UINT8_1++;
	g_3376_msg.INT8_1++;
	g_3376_msg.CHAR1++;
	reply_msg_to_zsg(g_3390_3376_port, &g_3376_msg, sizeof(g_3376_msg));
	return 0;
}

int process_msg_from_3391(struct g_3391_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(&g_3377_msg, msg, sizeof(g_3377_msg));
	g_3377_msg.INT16_1++;
	g_3377_msg.UINT8_1++;
	g_3377_msg.UINT32_1++;
	g_3377_msg.INT32_1++;
	g_3377_msg.CHAR1++;
	g_3377_msg.BOOLEAN_1++;
	g_3377_msg.BOOLEAN_2++;
	reply_msg_to_zsg(g_3391_3377_port, &g_3377_msg, sizeof(g_3377_msg));
	return 0;

}

int process_msg_from_3392(struct g_3392_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(&g_3378_msg, msg, sizeof(g_3378_msg));
	g_3378_msg.INT16_1++;
	reply_msg_to_zsg(g_3392_3378_port, &g_3378_msg, sizeof(g_3378_msg));
	return 0;
}

int process_msg_from_3393(struct g_3393_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(&g_3379_msg, msg, sizeof(g_3379_msg));
	g_3379_msg.INT16_1++;
	reply_msg_to_zsg(g_3393_3379_port, &g_3379_msg, sizeof(g_3379_msg));
	return 0;
}

int process_msg_from_3394(struct g_3394_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(&g_3380_msg, msg, sizeof(g_3380_msg));
	g_3380_msg.INT16_1++;
	g_3380_msg.CHAR1++;
	g_3380_msg.UINT32_1++;
	g_3380_msg.UINT32_2++;
	reply_msg_to_zsg(g_3394_3380_port, &g_3380_msg, sizeof(g_3380_msg));
	return 0;
}

void init_typeset_service(void) 
{
	g_3368_3360_port = &g_3368_3360_group;
	g_3390_3376_port = &g_3390_3376_group;
	g_3391_3377_port = &g_3391_3377_group;
	g_3392_3378_port = &g_3392_3378_group;
	g_3393_3379_port = &g_3393_3379_group;
	g_3394_3380_port = &g_3394_3380_group;
}

void mvb_typetest_service(void) 
{
	if (!is_sys_act) return;
	if(g_3368_3360_port->is_req_event) {
		copy_req_msg(g_3368_3360_port,
					&g_3368_msg,
					sizeof(g_3368_msg));
		process_msg_from_3368(&g_3368_msg);
	} else if ( g_3368_3360_port->to_event) {
		g_3368_3360_port->to_event = false;
		reply_msg_to_zsg(g_3368_3360_port, 
							&g_3368_msg_default, 
							sizeof(g_3368_msg));
	}
	
	if(g_3390_3376_port->is_req_event) {
		copy_req_msg(g_3390_3376_port,
					&g_3390_msg,
					sizeof(g_3390_msg));
		process_msg_from_3390(&g_3390_msg);
	} else if ( g_3390_3376_port->to_event) {
		g_3390_3376_port->to_event = false;
		reply_msg_to_zsg(g_3390_3376_port, 
						&g_3376_msg_default, 
						sizeof(g_3376_msg));
	}
	
	if(g_3391_3377_port->is_req_event) {
		copy_req_msg(g_3391_3377_port,
					&g_3391_msg,
					sizeof(g_3391_msg));
		process_msg_from_3391(&g_3391_msg);
	} else if ( g_3391_3377_port->to_event) {
		g_3391_3377_port->to_event = false;
		reply_msg_to_zsg(g_3391_3377_port, 
						&g_3377_msg_default, 
						sizeof(g_3377_msg));
	}
	
	if(g_3392_3378_port->is_req_event) {
		copy_req_msg(g_3392_3378_port,
					&g_3392_msg,
					sizeof(g_3392_msg));
		process_msg_from_3392(&g_3392_msg);
	} else if ( g_3392_3378_port->to_event) {
		 g_3392_3378_port->to_event = false;
		reply_msg_to_zsg(g_3392_3378_port, 
						&g_3378_msg_default, 
						sizeof(g_3378_msg));
	}
	
	if(g_3393_3379_port->is_req_event) {
		copy_req_msg(g_3393_3379_port,
					&g_3393_msg,
					sizeof(g_3393_msg));
		process_msg_from_3393(&g_3393_msg);
	} else if ( g_3393_3379_port->to_event) {
		g_3393_3379_port->to_event = false;
		reply_msg_to_zsg(g_3393_3379_port, 
							&g_3379_msg_default, 
							sizeof(g_3379_msg));
	}

	if(g_3394_3380_port->is_req_event) {
		copy_req_msg(g_3394_3380_port,
					&g_3394_msg,
					sizeof(g_3394_msg));
		process_msg_from_3394(&g_3394_msg);
	} else if ( g_3394_3380_port->to_event) {
		g_3394_3380_port->to_event = false;
		reply_msg_to_zsg(g_3394_3380_port, 
						&g_3380_msg_default, 
						sizeof(g_3380_msg));
	}
}
