#ifndef calibration_h
#define calibration_h


#define StepMotorReductorRate 10   // 10:1 reductor rate for stepper motor
#define StepMotorDriverFullRotationSteps 6400 // 6400 steps for full rotation of stepper motor driver
#define PlateFullRotationSteps (StepMotorDriverFullRotationSteps * StepMotorReductorRate)


extern void calibration_setup();
extern void motor_loop();
extern void calibration_loop();
extern void moveMottorSteps(long steps);
extern bool checkForErrorAndReturnTrueIfNeedToBeCorrected();
extern long readMottorSteps();
extern float readMottorSpeed();

#endif