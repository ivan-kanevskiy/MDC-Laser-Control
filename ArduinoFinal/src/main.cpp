#include "main.h"
#include "modbusHandler.h"
#include "modbusFiles.h"
#include "execution.h"
RTTStream rtt;
long lastTick = 0;
void setup()
{
  modbus_setup();
  Keyboard_setup();
}


void loop()
{
  modbus_loop();
  if (millis() - lastTick > tickTime)
  {
    switch (Register(ReadHoldingRegisters, windowRegistery)) // Example register read
    {
    case windowMain:
      rtt.println("MAIN PROGRAM");
      break;
    case windowLoadProgram:
      loopFilesFromDisp();
      rtt.println("LOAD PROGRAM");
      break;
    case windowExecution:
      Keyboard_loop();
      rtt.println("EXECUTION");
      /* code */
      break;
    case windowCalibration:
      rtt.println("CALIBRATION");
      /* code */
      break;
    case windowSetup:
      rtt.println("SETUP");
      /* code */
      break;
    default:
      break;
    }
    lastTick = millis();
  }
}

// Your main function that takes a boolean statement and a callback function
// void executeIfConditionIsTrue(int statement, int value, CallbackFunction func)
// {
//   if (statement == value)
//   {
//     func();
//   }
// }