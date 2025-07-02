#ifndef modbushandler_h
#define modbushandler_h
#include "ModbusRTUSlave.h"
#include "modbusDefines.h"

#define numCoils 200            // 0x1/2
#define numDiscreteInputs 200   // 0x2/1
#define numHoldingRegisters 500 // 0x3/4
#define numInputRegisters 500   // 0x4/3

extern void modbus_setup();
extern void modbus_loop();
extern uint16_t Register(int regType, int regNumber, uint16_t value = 0);

#endif