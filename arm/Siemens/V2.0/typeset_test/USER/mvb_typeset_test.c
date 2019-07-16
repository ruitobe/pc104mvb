#include "mvb_typeset_test.h"
struct port_group g_3368_3360_group = {
	.3368;
	.3360;
	.16;
	.16;
	.64;
	.false;
	.false;
}; 

struct port_group g_3390_3376_group = {
	.3390;
	.3376;
	.32;
	.8;
	.128;
	.false;
	.false;
};

struct port_group g_3391_3377_group = {
	.3391;
	.3377;
	.16;
	.32;
	.64;
	.false;
	.false;
};

struct port_group g_3392_3378_group = {
	.3392;
	.3379;
	.64;
	.4;
	.256;
	.false;
	.false;
};

struct port_group g_3393_3379_group = {
	.3393;
	.3379;
	.128;
	.32;
	.512;
	.false;
	.false;
};

struct port_group g_3394_3380_group = {
	.3394;
	.3380;
	.512;
	.32;
	.2048;
	.false;
	.false;
};

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
extern bool check_port_event(struct port_group * port);
extern bool reply_msg_to_zsg(struct port_group * port, 
							void * msg,
							int msg_size);
extern int copy_req_msg(struct port_group * port, 
						void * buffer,
						int msg_size);

void event_check(struct port_group * port)
{
	if(port != NULL && check_port_event(port)) {
		port->is_req_event = true;
	}
}


void timer_isr(void) 
{
	sys_tick += TIMER_PERIOD;
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
	memcpy(msg, &g_3360_msg, sizeof(g_3360_msg));
	/*reset err/warning*/
	if (msg->INT16_2) {
		g_3360_msg.UINT16_2 = 0;
		g_3360_msg.UINT16_3 = 0;
	}
	/*Host failure*/
	if(!msg->BOOLEAN1)
		test_host_failure();
	else {
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
	memcpy(msg, &g_3376_msg, sizeof(g_3376_msg));
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
	memcpy(msg, &g_3377_msg, sizeof(g_3377_msg));
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
	memcpy(msg, &g_3378_msg, sizeof(g_3378_msg));
	g_3378_msg.INT16_1++;
	reply_msg_to_zsg(g_3392_3378_port, &g_3378_msg, sizeof(g_3378_msg));
	return 0;
}

int process_msg_from_3393(struct g_3393_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(msg, &g_3379_msg, sizeof(g_3379_msg));
	g_3379_msg.INT16_1++;
	reply_msg_to_zsg(3393_3379_port, &g_3379_msg, sizeof(g_3379_msg));
	return 0;
}

int process_msg_from_3394(struct g_3394_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(msg, &g_3380_msg, sizeof(g_3380_msg));
	g_3380_msg.INT16_1++;
	g_3380_msg.CHAR1++;
	g_3380_msg.UINT32_1++;
	g_3380_msg.UINT32_2++;
	reply_msg_to_zsg(g_3394_3380_port, &g_3380_msg, sizeof(g_3380_msg));
	return 0;
}

void init_typeset_service(void) 
{
	g_3368_3360_port = &3368_3360_group;
	g_3390_3376_port = &3390_3376_group;
	g_3391_3377_port = &3391_3377_group;
	g_3392_3378_port = &3392_3378_group;
	g_3393_3379_port = &3393_3379_group;
	g_3394_3380_port = &3394_3380_group;
}

void mvb_typetest_service(void) 
{
	if(g_3368_3360_port->is_req_event) {
		copy_req_msg(g_3368_3360_port,
					&g_3368_msg,
					sizeof(g_3368_msg));
		process_msg_from_3368(&g_3368_msg);
	}
	
	if(g_3390_3376_port->is_req_event) {
		copy_req_msg(g_3390_3376_port,
					&g_3390_msg,
					sizeof(g_3390_msg));
		process_msg_from_3390(&g_3390_msg);
	}
	
	if(g_3391_3377_port->is_req_event) {
		copy_req_msg(g_3391_3377_port,
					&g_3391_msg,
					sizeof(g_3391_msg));
		process_msg_from_3391(&g_3391_msg);
	}
	
	if(g_3392_3378_port->is_req_event) {
		copy_req_msg(g_3392_3378_port,
					&g_3392_msg,
					sizeof(g_3392_msg));
		process_msg_from_3392(&g_3392_msg);
	}
	
	if(g_3393_3379_port->is_req_event) {
		copy_req_msg(g_3393_3379_port,
					&g_3393_msg,
					sizeof(g_3393_msg));
		process_msg_from_3393(&g_3393_msg);
	}
	if(g_3394_3380_port->is_req_event) {
		copy_req_msg(g_3394_3380_port,
					&g_3394_msg,
					sizeof(g_3394_msg));
		process_msg_from_3394(&g_3394_msg);
	}
}
