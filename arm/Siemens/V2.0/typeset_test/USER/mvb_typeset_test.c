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

void test_host_failure(void)
{
	/*need fire watchdog*/
	wdt_fired();
}

int process_msg_from_3368(struct 3368_msg_t * msg)
{
	if (msg == NULL) return -1;
	if(!msg->BOOLEAN1)
		test_host_failure();

}

void typeset_test_service(void) 
{

}
