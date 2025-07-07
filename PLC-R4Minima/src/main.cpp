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
  switch (Register(ReadHoldingRegisters, windowRegistery)) // read current windows on HMI
  {
    case windowMain:
      //rtt.println("MAIN PROGRAM");
      break;
    case windowLoadProgram:
      loadprogram_loop();
      //rtt.println("LOAD PROGRAM");
      break;
    case windowExecution:
      execution_loop();
      execution_program_loop();
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

