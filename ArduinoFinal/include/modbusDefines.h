#ifndef modbusDefines_h
#define modbusDefines_h

#define ReadCoils 0x01
#define ReadDiscreteInputs 0x02
#define ReadHoldingRegisters 0x03
#define ReadInputRegisters 0x04

#define WriteCoils 0x05
#define WriteDiscreteInputs 0x06
#define WriteHoldingRegisters 0x07
#define WriteInputRegisters 0x08

#define windowRegistery 99
#define SendDataButton 4
#define FileNameWriteRegister 39
#define FileNameReadRegister 39
#define ProgramRepeatRegister 100
#define ProgramNameRegister 110
#define NumberOfProgramsRepetitions 10
#define NumberOfProgramsNames 10
#define ProgramNameStaringRegister 110
#define ProgramNameLength 20
#define EmulatorControlAdrees 434
#endif