#ifndef calibration_h
#define calibration_h
#include "AccelStepper.h"
#define StepperFullRotation 64000
extern void calibration_setup();
extern void motor_loop();
extern void calibration_loop();
extern void moveMottorSteps(long steps);
extern bool checkForErrorAndReturnTrueIfNeedToBeCorrected();
extern long readMottorSteps();
extern float readMottorSpeed();
#endif