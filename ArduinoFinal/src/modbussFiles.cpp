#include "modbusHandler.h"
#include "modbusFiles.h"
#include "main.h"

#define MaxRecCnt 10


uint16_t numFiles = 0;      // 
// int numRepetition = 0; // 

uint16_t allRepetition[MaxRecCnt];
String allFilenames[MaxRecCnt];

String hexToAscii(String hexString);
String decimalToHex(unsigned long decValue);

static void LoadFilesFromDisp();


void loopFilesFromDisp()
{
    // check  "Send" button state
    if(Register(ReadCoils, SendDataButton) == 1 )
    {  // Pressed
        // load data 
        LoadFilesFromDisp();

        Register(WriteCoils, SendDataButton, 0); // clear "Send" button state
    }

    // executeIfConditionIsTrue(Register(ReadCoils, SendDataButton), 1, LoadFilesFromDisp);
}

String ReadFileName(int FileIndex){
    return allFilenames[FileIndex];
}

int ReadFileRepetition(int FileIndex){
    return allRepetition[FileIndex];
}

static void LoadFilesFromDisp()
{
    int numberOfdata = 0;
    numFiles = 0;
    //numRepetition = 0;

    //check how many rows are received from HMI 
    // the value 0 means that an empy row is reached 
    for (int i = 0; i < MaxRecCnt; i++)
    {

        if (Register(ReadHoldingRegisters, ProgramRepeatRegister + i) != 0)
        {
            numberOfdata++; // number of rows with  valid data 
        }
    }

    if (numberOfdata > 0)
    {
        // extract data from each row 
        for (int i = 0; i < numberOfdata; i++)
        {
            allRepetition[i] = Register(ReadHoldingRegisters, ProgramRepeatRegister + i);
            String buf;
            // for (int j = ProgramNameStaringRegister + i * ProgramNameLength; j < (ProgramNameStaringRegister + ProgramNameLength) + i * ProgramNameLength; j++)
            //{
            //     if (Register(ReadHoldingRegisters, j) == 0)
            //         break;
            //     buf += String(hexToAscii(decimalToHex(Register(ReadHoldingRegisters, j))));
            // }
            
            for (int j = 0; j < ProgramNameLength; j++)
            {
                
                if (Register(ReadHoldingRegisters, ProgramNameRegister + j + ProgramNameLength * i) == 0)
                    break;
                buf += String(hexToAscii(decimalToHex(Register(ReadHoldingRegisters, ProgramNameRegister + j + ProgramNameLength * i))));
            }
            allFilenames[numFiles] = buf;
            numFiles++;
            //numRepetition++;
            rtt.println(buf);
        }
        for (int i = 0; i < 20; i++)
        {
            Register(WriteInputRegisters, FileNameWriteRegister + i, Register(ReadHoldingRegisters, FileNameReadRegister + i));
        }
        for (int i = 0; i < NumberOfProgramsRepetitions; i++)
        {
            Register(WriteHoldingRegisters, ProgramRepeatRegister + i, 0);
            for (int j = 0; j < ProgramNameLength; j++)
            {
                Register(WriteHoldingRegisters, ProgramNameStaringRegister + j + i * ProgramNameLength, 0);
            }
        }
    }
    
}


String decimalToHex(unsigned long decValue)
{
    String hexString = "";
    char hexDigits[] = "0123456789ABCDEF";

    if (decValue == 0)
    {
        return "0";
    }

    while (decValue > 0)
    {
        hexString = hexDigits[decValue % 16] + hexString;
        decValue /= 16;
    }
    return hexString;
}

// Function to convert a hex string to an ASCII string, reversing each byte pair
String hexToAscii(String hexString)
{
    String ascii = "";
    for (int i = 0; i < hexString.length(); i += 4)
    {
        String pair = hexString.substring(i, i + 4);
        if (pair.length() < 4)
            break;
        String lowByte = pair.substring(2, 4);
        String highByte = pair.substring(0, 2);
        // Always check and append low byte first, then high byte
        if (lowByte != "00")
            ascii += (char)strtol(lowByte.c_str(), NULL, 16);
        if (highByte != "00")
            ascii += (char)strtol(highByte.c_str(), NULL, 16);
    }
    // Handle any remaining bytes (if the last register is not a full 4 hex digits)
    int rem = hexString.length() % 4;
    if (rem == 2)
    {
        String lastByte = hexString.substring(hexString.length() - 2);
        if (lastByte != "00")
            ascii += (char)strtol(lastByte.c_str(), NULL, 16);
    }
    return ascii;
}
