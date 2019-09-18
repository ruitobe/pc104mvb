#ifndef MVB_TYPESET_TEST_H
#define MVB_TYPESET_TEST_H

#include "sys.h"
#include "timer.h"

#define ZSG_DEV_ADDR		(230)
#define OBJ_DEV_ADDR		(3360)
#define bool uint8_t
#define false            0
#define true             1 

/*
 * 3390<->3376
 * 3391<->3377
 * 3392<->3378
 * 3393<->3379
 * 3394<->3380
 * 3368<->3360
 * */
#define ZSG_REQ_PORT_3390	 (3390)
#define ZSG_REQ_PORT_3391	 (3391)
#define ZSG_REQ_PORT_3392	 (3392)
#define ZSG_REQ_PORT_3393	 (3393)
#define ZSG_REQ_PORT_3394	 (3394)
#define ZSG_REQ_PORT_3368	 (3368)

#define OBJ_REP_PORT_3360  (3360) 
#define OBJ_REP_PORT_3376  (3376) 
#define OBJ_REP_PORT_3377  (3377) 
#define OBJ_REP_PORT_3378  (3378) 
#define OBJ_REP_PORT_3379  (3379) 
#define OBJ_REP_PORT_3380  (3380) 

struct port_group {
	uint16_t port_req;
	uint16_t port_rpl;
	uint16_t port_cyc; /*unit: ms*/
	uint16_t port_size; 
	uint16_t port_timeout; /*supervision time, unit: ms*/
	bool is_req_event;
	bool is_need_reply;
};
/*3368 <-> 3360*/
#define PORTCYC_3368_3360 (32)
struct g_3368_msg_t {
	int16_t INT16_1;
	int16_t INT16_2;
	uint16_t UINT16_1;
	uint8_t  BOOLEAN_1:1;
	uint8_t  BOOLEAN_2:1;
	uint8_t	 ANTIVALENT2:2;
	uint8_t	 BCD4:4;
	uint8_t	 ENUM8;
	uint16_t UINT16_2;
	uint16_t UINT16_3;
	uint16_t BITSET16;
	uint8_t  RESEVED1:4;
	uint8_t	 ENUM4:4;
	uint8_t	 BITSET8;
};
// After power up, 
// INT16_2 must be set to 10 (0x0a)
// When the first packet sent, 
// UINT16_2 must be 0 if no error
#define DEFAULT_3360_MSG \
{ 0X7f7f,\
	0,\
	0xA1A1,\
	0xA,\
	0x1,\
	0x1,\
	0x3,\
	0xA1,\
	0xFFFF,\
	0xFFFF,\
	0xA2,\
	0xf,\
	0xB,\
	0X7f,\
}
struct g_3360_msg_t {
	int16_t INT16_1;
	int16_t INT16_2;
	uint16_t UINT16_1;
	uint8_t  BOOLEAN_1:1;
	uint8_t  BOOLEAN_2:1;
	uint8_t	 ANTIVALENT2:2;
	uint8_t	 BCD4:4;
	uint8_t	 ENUM8;
	uint16_t UINT16_2;
	uint16_t UINT16_3;
	uint16_t BITSET16;
	uint8_t  VALID:4;
	uint8_t	 ENUM4:4;
	uint8_t	 BITSET8;
};

/*3390<-->3376*/
#define PORTCYC_3390_3376	(64)

struct g_3390_msg_t {
	uint16_t INT16_1;
	uint8_t BITSET8;
	uint8_t UINT8_1;
	uint8_t ENUM8;
	int8_t INT8_1;
	char CHAR1;
	uint8_t BCD4:4;
	uint8_t ENUM4:4;
};

#define DEFAULT_3376_MSG \
{	0x7F7F,\
	0xA1,\
	0xA2,\
	0xA3,\
	0x7F,\
	0x21,\
	0xB,\
	0xA,\
}

struct g_3376_msg_t {
	uint16_t INT16_1;
	uint8_t BITSET8;
	uint8_t UINT8_1;
	uint8_t ENUM8;
	int8_t INT8_1;
	char CHAR1;
	uint8_t BCD4:4;
	uint8_t ENUM4:4;
};

/*3391 <---> 3377*/
#define PORTCYC_3391_3377	(32)

struct g_3391_msg_t {
	int16_t INT16_1;
	char CHAR1;
	bool BOOLEAN1:1;
	bool BOOLEAN2:1;
	uint8_t ANTIVALENT2:2;
	uint8_t ENUM4:4;
	uint32_t UNINT32_1;
	uint16_t BITSET16;
	char STR[10];
	int32_t INT32_1;
	uint16_t MSECOND;
	uint16_t LSECOND;
	uint16_t TICKS;
	uint8_t BCD4:4;
	uint8_t ENUM5:4;
	uint8_t UINT8_1;
};

#define DEFAULT_3377_MSG \
{	0x7f7f,\
	0x21,\
	0x1,\
	0x1,\
	0x3,\
	0xA,\
	0xA1A1A1,\
	0xA1A1,\
	"ABCDEFGHIJ",\
	0x7FFFFFFF,\
	0xA2A2,\
	0xA3A3,\
	0,\
	0XC,\
	0xB,\
	0xA1,\
}

struct g_3377_msg_t {
	int16_t INT16_1;
	char CHAR1;
	bool BOOLEAN_1:1;
	bool BOOLEAN_2:1;
	uint8_t ANTIVALENT2:2;
	uint8_t ENUM4:4;
	uint32_t UINT32_1;
	uint16_t BITSET16;
	char STR[10];
	int32_t INT32_1;
	uint16_t MSECOND;
	uint16_t LSECOND;
	uint16_t TICKS;
	uint8_t BCD4:4;
	uint8_t ENUM5:4;
	uint8_t UINT8_1;
};


/*3392 <---> 3378*/
#define PORTCYC_3392_3378	(128)

struct g_3392_msg_t {
	int16_t INT16_1;
	uint8_t ANTIVALENT2:2;
	uint8_t VALID:6;
	uint8_t BITSET8;
};

#define DEFAULT_3378_MSG \
{	0x7f7f,\
	0x3f,\
	0x3,\
	0xa1,\
} 

struct g_3378_msg_t {
	int16_t INT16_1;
	uint8_t ANTIVALENT2:2;
	uint8_t VALID:6;
	uint8_t BITSET8;
};

/*3393 <---> 3379*/
#define PORTCYC_3393_3379	(256)

struct g_3393_msg_t {
	int16_t INT16_1;
	uint16_t BITSET16;
	char STR3[4];
	char STR4[4];
	int16_t INT16_2;
	uint8_t BCD4:4;
	uint8_t VALID:4;
	uint8_t VALID1;
	char STR16[16];
};

#define DEFAULT_3379_MSG \
{	0x7f7f,\
	0xa1a1,\
	"ABC0",\
	"ABCD",\
	0x7FFF,\
	0x5,\
	0xa,\
	0x55,\
	"ABCDEFGHIJKLMNOP",\
}
struct g_3379_msg_t {
	int16_t INT16_1;
	uint16_t BITSET16;
	char STR3[4];
	char STR4[4];
	int16_t INT16_2;
	uint8_t BCD4:4;
	uint8_t VALID:4;
	uint8_t VALID1;
	char STR16[16];
};

/*3394 <---> 3380*/
#define PORTCYC_3394_3380	(1024)

struct g_3394_msg_t {
	int16_t INT16_1;
	uint8_t VALID;
	char CHAR1;
	uint32_t BITSET32;
	uint32_t UINT32_1;
	uint32_t UINT32_2;
};

#define DEFAULT_3380_MSG \
{	0x7f7f,\
	0x55,\
	0x21,\
	0xA1A1A1A1,\
	0xA2A2A2A2,\
	0xA3A3A3A3,\
}
struct g_3380_msg_t {
	int16_t INT16_1;
	uint8_t VALID;
	char CHAR1;
	uint32_t BITSET32;
	uint32_t UINT32_1;
	uint32_t UINT32_2;
};
#endif

