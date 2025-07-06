//------------------------------------------------------------------------------
// Included files to resolve specific definitions in this file
//------------------------------------------------------------------------------
#include "main.h"

//------------------------------------------------------------------------------
// Local constants
//------------------------------------------------------------------------------

#define EngraveTimeDefaultValue 20 // default engraving time in seconds
#define NumberOfDetailsOnPlateDefaultValue 60 // default number of details on plate

#define modbusBaudrate  ((uint32_t)115200UL) // Modbus baud rate

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local data
//------------------------------------------------------------------------------

bool Coils[numCoils];
bool discreteInputs[numDiscreteInputs];
uint16_t HoldingRegisters[numHoldingRegisters];
uint16_t InputRegisters[numInputRegisters];

ModbusRTUSlave modbus(Serial1, A1, A0);


//------------------------------------------------------------------------------
// Constant local data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Exported data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Constant exported data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local function prototypes
//------------------------------------------------------------------------------



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
    Serial1.begin(modbusBaudrate, SERIAL_8N1);
    modbus.begin(4, modbusBaudrate, SERIAL_8N1);
    modbus.configureCoils(Coils, numCoils);
    modbus.configureDiscreteInputs(discreteInputs, numDiscreteInputs);
    modbus.configureHoldingRegisters(HoldingRegisters, numHoldingRegisters);
    modbus.configureInputRegisters(InputRegisters, numInputRegisters);
    Register(WriteHoldingRegisters, HMIExecMenuEngraveTimeButton, EngraveTimeDefaultValue);
    Register(WriteHoldingRegisters, HMIExecMenuNumberOfPlatesButton, NumberOfDetailsOnPlateDefaultValue);
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