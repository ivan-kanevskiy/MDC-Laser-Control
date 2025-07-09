//------------------------------------------------------------------------------
// Included files to resolve specific definitions in this file
//------------------------------------------------------------------------------
#include "main.h"
#include <cmath>
#include "AccelStepper.h"
#include "calibration.h"

//------------------------------------------------------------------------------
// Local constants
//------------------------------------------------------------------------------

#define StepperSpeed 500 // move half a degree
#define ZeroPointOneMMInSteps 2 // 0.1 mm in steps for plate with radius 480
#define STEP_PIN  SCK
#define DIR_PIN  MISO

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#define GetHMICalibrationState()  Register(ReadHoldingRegisters, HMICalibDirectionButton)
#define SetHMICalibrationState(x) Register(WriteHoldingRegisters, HMICalibDirectionButton, x)

#define GetHMICalibrationRotationLength()  Register(ReadHoldingRegisters, HMICalibButtonForRotationLength)

//------------------------------------------------------------------------------
// Local types
//------------------------------------------------------------------------------

typedef enum tHMICalibrationButtonState
{
    eHMIButtonNoPressed    = 0,
    eHMIButtonLeftPressed  = 1,
    eHMIButtonRightPressed = 2

} eHMICalibrationButtonState;
 
//------------------------------------------------------------------------------
// Local data
//------------------------------------------------------------------------------

static int errorAccumulator = 0;

static eHMICalibrationButtonState HMICalibrationDirState = eHMIButtonNoPressed;
static long rotationLength = 0;

static AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

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


//------------------------------------------------------------------------------
// Local function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// LOCAL FUNCTIONS  
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EXPORTED FUNCTIONS  
//------------------------------------------------------------------------------

bool checkForErrorAndReturnTrueIfNeedToBeCorrected()
{
     int DetailNumOnPlate = 0;    // number on details on plate;   
     // int RoudDown = 0;                 // steps for one detail on plate rounded down   
     int ErrorWhenRoundDown = 0;       // missed steps for one full plate rotation. 
     // Read number of detail on a plate from HMI
    DetailNumOnPlate = Register(ReadHoldingRegisters, HMIExecMenuNumberOfPlatesButton); 
    // calculate missed steps for one revolution of plate
    ErrorWhenRoundDown = PlateFullRotationSteps % DetailNumOnPlate;

    errorAccumulator += ErrorWhenRoundDown;

    if (errorAccumulator >= DetailNumOnPlate) {
        errorAccumulator -= DetailNumOnPlate;
        return true; 
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

/**
 * @brief 
 * 
 */
void calibration_loop()
{
    HMICalibrationDirState = (eHMICalibrationButtonState)GetHMICalibrationState();
    rotationLength = Register(ReadHoldingRegisters, HMICalibButtonForRotationLength) * ZeroPointOneMMInSteps;
    switch (HMICalibrationDirState)
    {
    case eHMIButtonLeftPressed:
        if (readMottorSpeed() == 0)
        {
            stepper.move(rotationLength);
            SetHMICalibrationState(eHMIButtonNoPressed);
            HMICalibrationDirState = eHMIButtonNoPressed;
        }
        break;
    case eHMIButtonRightPressed:
        if (readMottorSpeed() == 0)
        {
            
            stepper.move(rotationLength * -1);
            SetHMICalibrationState(eHMIButtonNoPressed);
            HMICalibrationDirState = eHMIButtonNoPressed;
        }
    default:
        if (readMottorSpeed() == 0)
        {
            stepper.setCurrentPosition(0);
        }
        break;
    }
}
