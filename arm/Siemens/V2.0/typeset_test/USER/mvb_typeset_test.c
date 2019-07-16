#include "mvb_typeset_test.h"
struct port_group 3368_3360_group = {
	.3368;
	.3360;
	.16;
	.16;
	.64;
	.false;
	.false;
}; 

struct port_group 3390_3376_group = {
	.3390;
	.3376;
	.32;
	.8;
	.128;
	.false;
	.false;
};

struct port_group 3391_3377_group = {
	.3391;
	.3377;
	.16;
	.32;
	.64;
	.false;
	.false;
};

struct port_group 3392_3378_group = {
	.3392;
	.3379;
	.64;
	.4;
	.256;
	.false;
	.false;
};

struct port_group 3393_3379_group = {
	.3393;
	.3379;
	.128;
	.32;
	.512;
	.false;
	.false;
};

struct port_group 3394_3380_group = {
	.3394;
	.3380;
	.512;
	.32;
	.2048;
	.false;
	.false;
};

struct port_group * 3368_3360_port;
struct port_group * 3390_3376_port;
struct port_group * 3391_3377_port;
struct port_group * 3392_3378_port;
struct port_group * 3393_3379_port;
struct port_group * 3394_3380_port;
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
	event_check(3368_3360_port); 
	event_check(3390_3376_port); 
	event_check(3391_3377_port); 
	event_check(3392_3378_port); 
	event_check(3393_3379_port); 
	event_check(3394_3380_port); 
}

static void test_host_failure(void)
{
	/*need fire watchdog*/
	wdt_fired();
}

int process_msg_from_3368(struct 3368_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(msg, &3360_msg, sizeof(3360_msg));
	/*reset err/warning*/
	if (msg->INT16_2) {
		3360_msg.UINT16_2 = 0;
		3360_msg.UINT16_3 = 0;
	}
	/*Host failure*/
	if(!msg->BOOLEAN1)
		test_host_failure();
	else {
		3360_msg.INT16_1++;
		3360_msg.INT16_2++;
		3360_msg.UINT16_1++;
		3360_msg.BOOLEAN_1++;
		3360_msg.BOOLEAN_2++;
		reply_msg_to_zsg(3368_3360_port, &3360_msg, sizeof(3360_msg));
	}
	return 0;
}

int process_msg_from_3390(struct 3390_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(msg, &3376_msg, sizeof(3376_msg));
	3376_msg.INT16_1++;
	3376_msg.UINT8_1++;
	3376_msg.INT8_1++;
	3376_msg.CHAR1++;
	reply_msg_to_zsg(3390_3376_port, &3376_msg, sizeof(3376_msg));
	return 0;
}

int process_msg_from_3391(struct 3391_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(msg, &3377_msg, sizeof(3377_msg));
	3377_msg.INT16_1++;
	3377_msg.UINT8_1++;
	3377_msg.UINT32_1++;
	3377_msg.INT32_1++;
	3377_msg.CHAR1++;
	3377_msg.BOOLEAN_1++;
	3377_msg.BOOLEAN_2++;
	reply_msg_to_zsg(3391_3377_port, &3377_msg, sizeof(3377_msg));
	return 0;

}

int process_msg_from_3392(struct 3392_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(msg, &3378_msg, sizeof(3378_msg));
	3378_msg.INT16_1++;
	reply_msg_to_zsg(3392_3378_port, &3378_msg, sizeof(3378_msg));
	return 0;
}

int process_msg_from_3393(struct 3393_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(msg, &3379_msg, sizeof(3379_msg));
	3379_msg.INT16_1++;
	reply_msg_to_zsg(3393_3379_port, &3379_msg, sizeof(3379_msg));
	return 0;
}

int process_msg_from_3394(struct 3394_msg_t * msg)
{
	if (msg == NULL) return -1;
	memcpy(msg, &3380_msg, sizeof(3380_msg));
	3380_msg.INT16_1++;
	3380_msg.CHAR1++;
	3380_msg.UINT32_1++;
	3380_msg.UINT32_2++;
	reply_msg_to_zsg(3394_3380_port, &3380_msg, sizeof(3380_msg));
	return 0;
}

void init_typeset_service(void) 
{
	3368_3360_port = &3368_3360_group;
	3390_3376_port = &3390_3376_group;
	3391_3377_port = &3391_3377_group;
	3392_3378_port = &3392_3378_group;
	3393_3379_port = &3393_3379_group;
	3394_3380_port = &3394_3380_group;
}

void mvb_typetest_service(void) 
{
	if(3368_3360_port->is_req_event) {
		copy_req_msg(3368_3360_port,
					&3368_msg,
					sizeof(3368_msg));
		process_msg_from_3368(&3368_msg);
	}
	
	if(3390_3376_port->is_req_event) {
		copy_req_msg(3390_3376_port,
					&3390_msg,
					sizeof(3390_msg));
		process_msg_from_3390(&3390_msg);
	}
	
	if(3391_3377_port->is_req_event) {
		copy_req_msg(3391_3377_port,
					&3391_msg,
					sizeof(3391_msg));
		process_msg_from_3391(&3391_msg);
	}
	
	if(3392_3378_port->is_req_event) {
		copy_req_msg(3392_3378_port,
					&3392_msg,
					sizeof(3392_msg));
		process_msg_from_3392(&3392_msg);
	}
	
	if(3393_3379_port->is_req_event) {
		copy_req_msg(3393_3379_port,
					&3393_msg,
					sizeof(3393_msg));
		process_msg_from_3393(&3393_msg);
	}
	if(3394_3380_port->is_req_event) {
		copy_req_msg(3394_3380_port,
					&3394_msg,
					sizeof(3394_msg));
		process_msg_from_3394(&3394_msg);
	}
}
