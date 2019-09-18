#include "mvb_typeset_test.h"
#include "demon.h"
#include "iwdg.h"

bool is_sys_act = true;

// Rui: The value testint_2 in port 3360 is used to check the power-up behavior. 
// It must be set to 10 when the test object is started,
// and then incremented every time a new telegram is received at port 3368.
bool is_sys_up = false;
int16_t g_count = 10;
uint16_t g_error = 0;
uint16_t g_warning = 0xffff;
uint32_t g_cycle_cnt[32] = {0};

struct port_group g_3368_3360_group = {
	.port_req = 3368,
	.port_rpl = 3360,
	//.port_cyc = 16,
	.port_cyc = 32,
	.port_size = 16,
	.port_timeout = 64,
	.is_req_event = 0,
	.is_need_reply = 0
}; 

struct port_group g_3390_3376_group = {
	.port_req = 3390,
	.port_rpl = 3376,
	//.port_cyc = 32,
	.port_cyc = 64,
	.port_size = 8,
	.port_timeout = 128,
	.is_req_event = 0,
	.is_need_reply = 0
};

struct port_group g_3391_3377_group = {
	.port_req = 3391,
	.port_rpl = 3377,
	//.port_cyc = 16,
	.port_cyc = 32,
	.port_size = 32,
	.port_timeout = 64,
	.is_req_event = 0,
	.is_need_reply = 0
};

struct port_group g_3392_3378_group = {
	.port_req = 3392,
	.port_rpl = 3378,
	//.port_cyc = 64,
	.port_cyc = 128,
	.port_size = 4,
	.port_timeout = 256,
	.is_req_event = 0,
	.is_need_reply = 0
};

struct port_group g_3393_3379_group = {
	.port_req = 3393,
	.port_rpl = 3379,
	//.port_cyc = 128,
	.port_cyc = 256,
	.port_size = 32,
	.port_timeout = 512,
	.is_req_event = 0,
	.is_need_reply = 0
};

struct port_group g_3394_3380_group = {
	.port_req = 3394,
	.port_rpl = 3380,
	.port_cyc = 1024,
	//.port_size = 32,
	.port_size = 16,
	.port_timeout = 2048,
	.is_req_event = 0,
	.is_need_reply = 0
};
struct g_3368_msg_t g_3368_msg;
struct g_3360_msg_t g_3360_msg = DEFAULT_3360_MSG;
static struct g_3360_msg_t g_3360_default_msg = DEFAULT_3360_MSG;

struct g_3390_msg_t g_3390_msg;
struct g_3376_msg_t g_3376_msg = DEFAULT_3376_MSG;
static struct g_3376_msg_t g_3376_default_msg = DEFAULT_3376_MSG;

struct g_3391_msg_t g_3391_msg;
struct g_3377_msg_t g_3377_msg = DEFAULT_3377_MSG;
static struct g_3377_msg_t g_3377_default_msg = DEFAULT_3377_MSG;

struct g_3392_msg_t g_3392_msg;
struct g_3378_msg_t g_3378_msg = DEFAULT_3378_MSG;
static struct g_3378_msg_t g_3378_default_msg = DEFAULT_3378_MSG;


struct g_3393_msg_t g_3393_msg;
struct g_3379_msg_t g_3379_msg = DEFAULT_3379_MSG;
static struct g_3379_msg_t g_3379_default_msg = DEFAULT_3379_MSG;

struct g_3394_msg_t g_3394_msg;
struct g_3380_msg_t g_3380_msg = DEFAULT_3380_MSG;
static struct g_3380_msg_t g_3380_default_msg = DEFAULT_3380_MSG;
 
struct port_group *g_3368_3360_port;
struct port_group *g_3390_3376_port;
struct port_group *g_3391_3377_port;
struct port_group *g_3392_3378_port;
struct port_group *g_3393_3379_port;
struct port_group *g_3394_3380_port;
/*
 *There is a sys timer, the period is 4ms.
 */
#define TIMER_PERIOD 4 
static uint32_t sys_tick = 0;
/*extern bool check_port_event(struct port_group * port);
extern bool reply_msg_to_zsg(struct port_group * port, 
							void * msg,
							int msg_size);
extern int copy_req_msg(struct port_group * port, 
						void * buffer,
						int msg_size);*/

void event_check(struct port_group * port)
{
	if(port != NULL && (true == check_port_event(port))) {
		port->is_req_event = true;
	} else {
	  // Rui: need to update the status
		port->is_req_event = false;
	}
}

void test_event_check(void) 
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
	//memcpy((uint8_t *)msg, &g_3360_msg, sizeof(g_3360_msg));
	memcpy(&g_3360_msg, (uint8_t *)msg, sizeof(g_3360_msg));
	if (!is_sys_up) {
	  is_sys_up = true;
		g_3360_msg.INT16_2 = g_count;
	} else {
	  g_3360_msg.INT16_2 = g_count;
		g_count++;
	}
	
	if(!msg->BOOLEAN_1) {
		/*Host failure*/
		is_sys_act = false;
		test_host_failure();
	}	else {
		// Reset errors/warnings if = 1
	  if (msg->INT16_2 == 0x0001) {
			/*reset err/warning*/
		  g_3360_msg.UINT16_2 = 0;
		  g_3360_msg.UINT16_3 = 0;
	  } else {
		  // The value testfehler 
		  // Before any errors have occurred, e.g. directly after startup, the position of the error counter should be 0. 
			// In each cycle during which at least one error has occurred, the error counter is incremented by 1.
			// An error is recognized when the sink time supervision of the test object
		  g_3360_msg.UINT16_2 = g_error;
			// The value testwarnung
			// in port 3360 should be incremented if the signal age is higher than the port cycle time + the processing time.
			g_3360_msg.UINT16_3 = g_warning;
		}
		
		if(msg->BOOLEAN_2) {
			// Host sick
		  g_3360_msg.BITSET16 = 0;
	  }
		
		g_3360_msg.INT16_1++;
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
	//memcpy(msg, &g_3376_msg, sizeof(g_3376_msg));
	memcpy(&g_3376_msg, (uint8_t *)msg, sizeof(g_3376_msg));
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
	//memcpy(msg, &g_3377_msg, sizeof(g_3377_msg));
	memcpy(&g_3377_msg, (uint8_t *)msg, sizeof(g_3377_msg));
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
	//memcpy(msg, &g_3378_msg, sizeof(g_3378_msg));
	memcpy(&g_3378_msg, (uint8_t *)msg, sizeof(g_3378_msg));
	g_3378_msg.INT16_1++;
	reply_msg_to_zsg(g_3392_3378_port, &g_3378_msg, sizeof(g_3378_msg));
	return 0;
}

int process_msg_from_3393(struct g_3393_msg_t * msg)
{
	if (msg == NULL) return -1;
	//memcpy(msg, &g_3379_msg, sizeof(g_3379_msg));
	memcpy(&g_3379_msg, (uint8_t *)msg, sizeof(g_3379_msg));
	g_3379_msg.INT16_1++;
	reply_msg_to_zsg(g_3393_3379_port, &g_3379_msg, sizeof(g_3379_msg));
	return 0;
}

int process_msg_from_3394(struct g_3394_msg_t * msg)
{
	if (msg == NULL) return -1;
	//memcpy(msg, &g_3380_msg, sizeof(g_3380_msg));
	memcpy(&g_3380_msg, (uint8_t *)msg, sizeof(g_3380_msg));
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
	// Rui:
	if (is_sys_act) {
		if(!g_3368_3360_port->is_req_event) {
			// For test purposes, 3.1.4
			// If no data are received from a port, and sink timeout
			// the receiver responds, and operation is continued with the defined default values.
			//g_3360_msg = DEFAULT_3360_MSG;
			memcpy(&g_3360_msg, &g_3360_default_msg, sizeof(g_3360_msg));
			reply_msg_to_zsg(g_3368_3360_port, &g_3360_msg, sizeof(g_3360_msg));
		} else if(g_3368_3360_port->is_req_event && is_sys_act) {
			if ((millis1() - g_cycle_cnt[0]) > g_3368_3360_port->port_cyc) {
				g_cycle_cnt[0] = millis1();
				copy_req_msg(g_3368_3360_port,
					&g_3368_msg,
					sizeof(g_3368_msg));
		    process_msg_from_3368(&g_3368_msg);
			}
	  }
	
	  if(!g_3390_3376_port->is_req_event) {
		  //g_3376_msg = DEFAULT_3376_MSG;
			memcpy(&g_3376_msg, &g_3376_default_msg, sizeof(g_3376_msg));
		  reply_msg_to_zsg(g_3390_3376_port, &g_3376_msg, sizeof(g_3376_msg));
	  } else if(g_3390_3376_port->is_req_event && is_sys_act) {
			if ((millis1() - g_cycle_cnt[1]) > g_3390_3376_port->port_cyc) {
				g_cycle_cnt[1] = millis1();
				copy_req_msg(g_3390_3376_port,
			    &g_3390_msg,
			    sizeof(g_3390_msg));
		    process_msg_from_3390(&g_3390_msg);
			}
	  }
	
	  if(!g_3391_3377_port->is_req_event) {
		  //g_3377_msg = DEFAULT_3377_MSG;
			memcpy(&g_3377_msg, &g_3377_default_msg, sizeof(g_3377_msg));
		  reply_msg_to_zsg(g_3391_3377_port, &g_3377_msg, sizeof(g_3377_msg));
	  } else if(g_3391_3377_port->is_req_event && is_sys_act) {
			if ((millis1() - g_cycle_cnt[2]) > g_3391_3377_port->port_cyc) {
				g_cycle_cnt[2] = millis1();
				copy_req_msg(g_3391_3377_port,
			  &g_3391_msg,
			  sizeof(g_3391_msg));
		    process_msg_from_3391(&g_3391_msg);
			}
	  }
	
	  if(!g_3392_3378_port->is_req_event) {
		  //g_3378_msg = DEFAULT_3378_MSG;
			memcpy(&g_3378_msg, &g_3378_default_msg, sizeof(g_3378_msg));
		  reply_msg_to_zsg(g_3392_3378_port, &g_3378_msg, sizeof(g_3378_msg));
	  } else if(g_3392_3378_port->is_req_event && is_sys_act) {
			if ((millis1() - g_cycle_cnt[3]) > g_3392_3378_port->port_cyc) {
				g_cycle_cnt[3] = millis1();
				copy_req_msg(g_3392_3378_port,
			  &g_3392_msg,
			  sizeof(g_3392_msg));
		    process_msg_from_3392(&g_3392_msg);
			}
	  }

	  if(!g_3393_3379_port->is_req_event) {
		  //g_3379_msg = DEFAULT_3379_MSG;
			memcpy(&g_3379_msg, &g_3379_default_msg, sizeof(g_3379_msg));
		  reply_msg_to_zsg(g_3393_3379_port, &g_3379_msg, sizeof(g_3379_msg));
	  } else if(g_3393_3379_port->is_req_event && is_sys_act) {
			if ((millis1() - g_cycle_cnt[4]) > g_3393_3379_port->port_cyc) {
				g_cycle_cnt[4] = millis1();
				copy_req_msg(g_3393_3379_port,
			  &g_3393_msg,
			  sizeof(g_3393_msg));
		    process_msg_from_3393(&g_3393_msg);
			}
	  }
	
	  if(!g_3394_3380_port->is_req_event) {
		  //g_3380_msg = DEFAULT_3380_MSG;
			memcpy(&g_3380_msg, &g_3380_default_msg, sizeof(g_3380_msg));
		  reply_msg_to_zsg(g_3394_3380_port, &g_3380_msg, sizeof(g_3380_msg));
	  } else if(g_3394_3380_port->is_req_event && is_sys_act) {
			if ((millis1() - g_cycle_cnt[5]) > g_3394_3380_port->port_cyc) {
				g_cycle_cnt[5] = millis1();
				copy_req_msg(g_3394_3380_port,
			  &g_3394_msg,
			  sizeof(g_3394_msg));
		    process_msg_from_3394(&g_3394_msg);
			}
	  }
	}
}
