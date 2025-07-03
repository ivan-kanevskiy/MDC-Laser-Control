#include "main.h"
#include <cmath>
// defines

#define StepperSpeed 500 // move half a degre
#define none 0
#define Left 1
#define Right 2
#define OneMMInSteps 212
// varialbles

const int STEP_PIN = SCK;
const int DIR_PIN = MISO;

uint16_t dir = 0;
long rotationLength = 0;

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

bool checkForErrorAndReturnTrueIfNeedToBeCorrected()
{
    static uint16_t plateNumber = Register(ReadHoldingRegisters, HMIExecMenuNumberOfPlatesButton); 
    static int RoudDown = StepperFullRotation / plateNumber;
    static int ErrorWhenRoundDown = StepperFullRotation - (plateNumber * RoudDown); 

    static int errorAccumulator = 0;

    errorAccumulator += ErrorWhenRoundDown;

    if (errorAccumulator >= plateNumber) {
        errorAccumulator -= plateNumber;
        return true; 
    }
    if (currentPlate >= plateNumber) {
        currentPlate = 0;
        errorAccumulator = 0;
    }
    return false;
}

void calibration_setup()
{
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    stepper.setMaxSpeed(StepperSpeed);         // steps/second
    stepper.setAcceleration(StepperSpeed * 2); // steps/second^2
    stepper.setCurrentPosition(0);
}
long readMottorSteps()
{
    return stepper.currentPosition();
}
float readMottorSpeed()
{
    return stepper.speed();
}
void moveMottorSteps(long steps)
{
    stepper.setSpeed(StepperSpeed);
    stepper.move(steps);
}
void motor_loop()
{
    stepper.run();
}
void calibration_loop()
{
    dir = Register(ReadHoldingRegisters, HMICalibDirectionButton);
    rotationLength = Register(ReadHoldingRegisters, HMICalibButtonForRotationLength) * OneMMInSteps;

    switch (dir)
    {
    case Left:
        if (readMottorSpeed() == 0)
        {
            stepper.move(rotationLength);
            Register(WriteHoldingRegisters, HMICalibDirectionButton, none);
            dir = none;
        }
        break;
    case Right:
        if (readMottorSpeed() == 0)
        {
            stepper.setCurrentPosition(0);
            stepper.move(rotationLength * -1);
            Register(WriteHoldingRegisters, HMICalibDirectionButton, none);
            dir = none;
        }
    default:
        if (readMottorSpeed() == 0)
        {
            stepper.setCurrentPosition(0);
        }
        break;
    }
}
