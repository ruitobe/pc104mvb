#ifndef _MODBUS_H
#define _MODBUS_H
#include "sys.h"
#include "rs485.h"
#include "demon.h"

extern uint8_t MVB_START_FLAG;

void Modbus_Init(void);
void Timer7_Init(void);
void Modbus_Service(void);
void Heartbeat_Service(void);
void Modbus_03_Solve(void);
void Modbus_06_Solve(void);
void Modbus_16_Solve(void);

#endif
