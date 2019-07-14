#ifndef MVB_TYPESET_TEST
#define MVB_TYPESET_TEST
#include <stdint.h>
#include <stdlib.h>

#define ZSG_DEV_ADDR		(230)
#define OBJ_DEV_ADDR		(3360)

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

#define OBJ_REP_PORT_3360    (3360) 
#define OBJ_REP_PORT_3376    (3376) 
#define OBJ_REP_PORT_3377    (3377) 
#define OBJ_REP_PORT_3378    (3378) 
#define OBJ_REP_PORT_3379    (3379) 
#define OBJ_REP_PORT_3380    (3380) 

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
#define PORTCYC_3368_3360 (16)
struct 3368_msg_t {
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
} 3368_msg;

struct 3360_msg_t {
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
	uint8_t  2xVALID:4;
	uint8_t	 ENUM4:4;
	uint8_t	 BITSET8;
}3360_msg;

/*3390<-->3376*/
#define PORTCYC_3390_3376	(64)

struct 3390_msg_t {
	uint16_t INT16_1;
	uint8_t BITSET8;
	uint8_t UINT8_1;
	uint8_t ENUM8;
	int8_t INT8_1;
	char CHAR1;
	uint8_t BCD4:4;
	uint8_t ENUM4:4;
}3390_msg;

struct 3376_msg_t {
	uint16_t INT16_1;
	uint8_t BITSET8;
	uint8_t UINT8_1;
	uint8_t ENUM8;
	int8_t INT8_1;
	char CHAR1;
	uint8_t BCD4:4;
	uint8_t ENUM4:4;
}3376_msg;

/*3391 <---> 3377*/
#define PORTCYC_3391_3377	(128)

struct 3391_msg_t {
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
	uint8_t ENUM4:4;
	uint8_t UNINT8_1;
}3391_msg;

struct 3377_msg_t {
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
	uint8_t ENUM4:4;
	uint8_t UNINT8_1;
}3377_msg;


/*3392 <---> 3378*/
#define PORTCYC_3392_3378	(256)

struct 3392_msg_t {
	int16_t INT16_1;
	uint8_t ANTIVALENT2:2;
	uint8_t 3xVALID:6;
	uint8_t BITSET8;
}3392_msg;

struct 3378_msg_t {
	int16_t INT16_1;
	uint8_t ANTIVALENT2:2;
	uint8_t 3xVALID:6;
	uint8_t BITSET8;
}3378_msg;

/*3393 <---> 3379*/
#define PORTCYC_3393_3379	(512)

struct 3393_msg_t {
	int16_t INT16_1;
	uint16_t BITSET16;
	char STR3[4];
	char STR4[4];
	int16_t INT16_2;
	uint8_t BCD4:4;
	uint8_t 2xVALID:4;
	uint8_t 4xVALID;
	char STR16[16];
}3393_msg;

struct 3379_msg_t {
	int16_t INT16_1;
	uint16_t BITSET16;
	char STR3[4];
	char STR4[4];
	int16_t INT16_2;
	uint8_t BCD4:4;
	uint8_t 2xVALID:4;
	uint8_t 4xVALID;
	char STR16[16];
}3379_msg;

/*3394 <---> 3380*/
#define PORTCYC_3394_3380	(1024)

struct 3394_msg_t {
	int16_t INT16_1;
	uint8_t 4xVALID;
	char CHAR1;
	uint32_t BITSET32;
	uint32_t UINT32_1;
	uint32_t UINT32_2;
}3394_msg;

struct 3380_msg_t {
	int16_t INT16_1;
	uint8_t 4xVALID;
	char CHAR1;
	uint32_t BITSET32;
	uint32_t UINT32_1;
	uint32_t UINT32_2;
}3380_msg;
#endif

