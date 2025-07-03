#include "main.h"

RTTStream rtt;
long lastTick = 0;
void setup()
{
  modbus_setup();
  execution_setup();
  calibration_setup();
}

void loop()
{
  modbus_loop();
  motor_loop();
  // if (millis() - lastTick > tickTime)
  //{
  switch (Register(ReadHoldingRegisters, windowRegistery)) // Example register read
  {
  case windowMain:
    //rtt.println("MAIN PROGRAM");
    break;
  case windowLoadProgram:
    loopFilesFromDisp();
    //rtt.println("LOAD PROGRAM");
    break;
  case windowExecution:
    execution_loop();
    //rtt.println("EXECUTION");
    /* code */
    break;
  case windowCalibration:
    calibration_loop();
    //rtt.println("CALIBRATION");
    /* code */
    break;
  case windowSetup:
    //rtt.println("SETUP");
    /* code */
    break;
  default:
    break;
  }
  // lastTick = millis();
  // }
}

// Your main function that takes a boolean statement and a callback function
// void executeIfConditionIsTrue(int statement, int value, CallbackFunction func)
// {
//   if (statement == value)
//   {
//     func();
//   }
// }
// void executeIfConditionIsTrue(int statement, int value, CallbackFunction func)
// {
//   if (statement == value)
//   {
//     func();
//   }
// }