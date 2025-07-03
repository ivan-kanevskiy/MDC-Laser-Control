#include "main.h"

bool Coils[numCoils];
bool discreteInputs[numDiscreteInputs];
uint16_t HoldingRegisters[numHoldingRegisters];
uint16_t InputRegisters[numInputRegisters];

ModbusRTUSlave modbus(Serial1, A1, A0);

const uint32_t baudrate = 115200UL;

uint16_t Register(int regType, int regNumber, uint16_t value)
{
    switch (regType)
    {
    case ReadCoils: // Coils
        if (regNumber < numCoils)
        {
            return Coils[regNumber] ? 1 : 0;
        }
        break;
    case ReadDiscreteInputs: // Discrete Inputs
        if (regNumber < numDiscreteInputs)
        {
            return discreteInputs[regNumber] ? 1 : 0;
        }
        break;
    case ReadHoldingRegisters: // Holding Registers
        if (regNumber < numHoldingRegisters)
        {
            return HoldingRegisters[regNumber];
        }
        break;
    case ReadInputRegisters: // Input Registers
        if (regNumber < numInputRegisters)
        {
            return InputRegisters[regNumber];
        }
        break;
    case WriteCoils: // Write Coils
        if (regNumber < numCoils)
        {
            Coils[regNumber] = value != 0;
            return 1; // Success
        }
    case WriteDiscreteInputs: // Write Coils
        if (regNumber < numDiscreteInputs)
        {
            discreteInputs[regNumber] = value != 0;
            return 1; // Success
        }
        break;
    case WriteHoldingRegisters: // Holding Registers
        if (regNumber < numHoldingRegisters)
        {
            HoldingRegisters[regNumber] = value;
            return 1;
        }
        break;
    case WriteInputRegisters: // Input Registers
        if (regNumber < numInputRegisters)
        {
            InputRegisters[regNumber] = value;
            return 1;
        }
        break;
    }

    return 0; // Invalid register type or number
}
void modbus_setup()
{
    Serial1.begin(baudrate, SERIAL_8N1);
    modbus.begin(4, baudrate, SERIAL_8N1);
    modbus.configureCoils(Coils, numCoils);
    modbus.configureDiscreteInputs(discreteInputs, numDiscreteInputs);
    modbus.configureHoldingRegisters(HoldingRegisters, numHoldingRegisters);
    modbus.configureInputRegisters(InputRegisters, numInputRegisters);
    Register(WriteHoldingRegisters, HMIExecMenuDelayButton, 20);
    Register(WriteHoldingRegisters, HMIExecMenuNumberOfPlatesButton, 60);
}
void modbus_loop()
{
    if (modbus.poll())
    {
        // Process the Modbus PDU
        // The ModbusRTUSlave class handles the PDU processing internally
        // No additional action needed here unless you want to handle specific cases
    }
}