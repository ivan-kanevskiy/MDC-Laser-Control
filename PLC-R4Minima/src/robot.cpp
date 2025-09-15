//------------------------------------------------------------------------------
// Included files to resolve specific definitions in this file
//------------------------------------------------------------------------------

#include "robot.h"
#include "main.h"

//------------------------------------------------------------------------------
// Local constants
//------------------------------------------------------------------------------
#define RobotStatusPin A2

#define KEY_F 0x46
#define KEY_O 0x4F

#define KboardShortCutTime 1000 // time to wait between key presses to open "File Open" menu in ms
#define FileMenuOpenTime 4000   // time to wait for "File Open" menu to be opened in ms
#define LoadFileWaitTime 5000   // time to wait for file to be loaded in ms 

#define debounceTime 1000
//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#define GetHMIRobotMenuButtonsStatus() Register(ReadHoldingRegisters, HMIRobotMenuControl) // returns   buttons pressed status  : 1 == Start, 2 == Pause, 3 == Stop
#define GetHMINumOfSteps() Register(ReadHoldingRegisters, HMIRobotMenuNumOfSteps) // returns number of steps to engrave. Get this value from HMI screen

#define ClearHMIStatus() Register(WriteHoldingRegisters, HMIRobotMenuControl, eHMIButtonNoPressed) // clear   buttons status
#define SetHMIRobotStatus(x) Register(WriteHoldingRegisters, HMIRobotMenuControlReturn, x) // used to return current status of robot program to HMI . This is status is visualized on HMI screen


#define IsSignalFromRobotDetected() (SignalFromRobot == true)
//------------------------------------------------------------------------------
// Local types
//------------------------------------------------------------------------------

typedef enum tHMIRobotButtonsState
{
    eHMIButtonNoPressed = 0,
    eHMIButtonStartPressed = 1,
    eHMIButtonPausePressed = 2,
    eHMIButtonStopPressed = 3
} eHMIRobotButtonsState;

typedef enum tRobotTaskState
{
    eInitState = 0,
    eOpeningFileSelectMenuState,
    eAfterKeyPressedDelayState,
    eWaitForFileMenuToBeLoadedState,
    eWritingFileNameState,
    eWaitingToLoadFileState,
    eExecFileCheckState,
    eExecFileState,
    eExecFileDelayState,
    eWaitForRobotImpulse,
    eExecDoneState,
    eExecStopState,
    eExecPausedState
} eRobotTaskState;

//------------------------------------------------------------------------------
// Local data
//------------------------------------------------------------------------------

static eRobotTaskState RobotCurtState;
static eHMIRobotButtonsState eHMIRobotProgramStatus = eHMIButtonNoPressed;

static String fileNameList[5] = {"STEP-1.edz", "STEP-2.edz", "STEP-3.edz", "STEP-4.edz", "STEP-5.edz"};

static int numberOfSteps = 0;
static int currentStepsCnt = 0;

static int KeyboardShorcutStep = 0;
static unsigned long Timer = 0;

static bool SignalFromRobot = false;


static unsigned long RobotLastImpulse = millis();
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
void robot_interupt_first();
void robot_interupt_sec();

void robot_setup()
{
    RobotCurtState = eExecStopState;
    eHMIRobotProgramStatus = eHMIButtonNoPressed;
    pinMode(RobotStatusPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RobotStatusPin), robot_interupt_first, RISING);
    attachInterrupt(digitalPinToInterrupt(RobotStatusPin), robot_interupt_sec, FALLING);
}

void robot_interupt_first()
{ 
    if (RobotCurtState == eWaitForRobotImpulse)
    {
        Serial.println("interupted pt.1");
        RobotLastImpulse = millis();
    }
}
void robot_interupt_sec()
{ 
    if ( (millis() - RobotLastImpulse > 200   ) && 
         (millis() - RobotLastImpulse <= 1000 ) &&
         (RobotCurtState == eWaitForRobotImpulse) 
        )
    {
        SignalFromRobot = true;
        Serial.println("interupted pt.2");
        RobotLastImpulse = millis();
    }
}

void robot_program_loop()
{
    if (GetHMINumOfSteps() > 0)
    {
        unsigned long currentMillis = millis();
        switch (RobotCurtState)
        {
        case eInitState:
            // Engrave wait time
            numberOfSteps = GetHMINumOfSteps(); //get how many steps to engrave from HMI
            RobotCurtState = eWaitForRobotImpulse; 
            break;

        case eWaitForRobotImpulse:
            // wainting for signal from robot. After recieving of the signal program for execution should be sent to PC
            if (SignalFromRobot == true)
            {
                SignalFromRobot = false;
                RobotCurtState = eExecFileCheckState;
            }
            break;
        
        case eExecFileCheckState:
            if (currentStepsCnt <= numberOfSteps -1 )
            {
                RobotCurtState = eOpeningFileSelectMenuState;
            }
            else
            {
                RobotCurtState = eExecDoneState;
            }
            break;
        case eOpeningFileSelectMenuState:

            KeyboardShorcutStep++;
            switch (KeyboardShorcutStep)
            {
            case 1:
                // emulate left "ALT" key press
                Keyboard.write(KEY_LEFT_ALT);
                RobotCurtState = eAfterKeyPressedDelayState;
                break;

            case 2:
                // emulate "F" key  pressFOFO
                Keyboard.write(KEY_F);
                RobotCurtState = eAfterKeyPressedDelayState;
                break;
            case 3:
                // emulate "O" key press
                Keyboard.write(KEY_O);
                RobotCurtState = eAfterKeyPressedDelayState;
                break;
            default:
                // there is nothing to sent. Go to WaitForFileMenuToLoadState to give
                // enough time program to open " File Open " menu
                KeyboardShorcutStep = 0;
                RobotCurtState = eWaitForFileMenuToBeLoadedState;
                break;
            }
            Timer = millis();
            break;
        case eAfterKeyPressedDelayState:
            if (currentMillis - Timer > KboardShortCutTime)
            {
                RobotCurtState = eOpeningFileSelectMenuState;
            }
            break;

        case eWaitForFileMenuToBeLoadedState:
            if (currentMillis - Timer > FileMenuOpenTime)
            {
                RobotCurtState = eWritingFileNameState;
            }
            break;
        case eWritingFileNameState:
            // write file name in the "File Open" menu
            Keyboard.print(fileNameList[currentStepsCnt]);
            // press ENTER key to select file
            Keyboard.write(KEY_RETURN);
            // go to wait state because computer takes some time to open file
            RobotCurtState = eWaitingToLoadFileState;
            // get current timer value
            Timer = millis();
            break;

        case eWaitingToLoadFileState:
            if (currentMillis - Timer >= LoadFileWaitTime)
            {
                RobotCurtState = eExecFileState;
            }
            break;
        case eExecFileState:
            // Keyboard.println(String(readMottorSteps()));
            Keyboard.write(KEY_F2); // send F2 key to start engraving
            currentStepsCnt++;
            Timer = millis();
            RobotCurtState = eWaitForRobotImpulse;
            break;
        case eExecDoneState:
            // execution of all files is done
            // reset all variables to initial state
            RobotCurtState = eExecStopState;

            // set HMI to button stop state
            SetHMIRobotStatus(eHMIButtonStopPressed);
            break;
        case eExecStopState:
            currentStepsCnt = 0;
            KeyboardShorcutStep = 0;
            numberOfSteps = 0;

            break;
            // eHMIExecProgramStatus = eHMIButtonStopPressed;

        case eExecPausedState:
            // do nothing, just wait for HMI to change state
            break;
        default:
            break;
        }
    }
    else
    {
        RobotCurtState = eExecStopState;
    }
}

void robot_loop()
{
    eHMIRobotProgramStatus = (eHMIRobotButtonsState)GetHMIRobotMenuButtonsStatus(); // make sure that HMI send valid values for this enum

    switch (eHMIRobotProgramStatus)
    {
    case eHMIButtonStartPressed:

        RobotCurtState = eInitState;               // set execution state to initial state
        SetHMIRobotStatus(eHMIButtonStartPressed); // set HMI status to start pressed
        ClearHMIStatus();
        break;

    case eHMIButtonPausePressed:
        RobotCurtState = eExecPausedState;         // set execution state to paused
        SetHMIRobotStatus(eHMIButtonPausePressed); // set HMI status to pause pressed
        ClearHMIStatus();
        break;

    case eHMIButtonStopPressed:
        RobotCurtState = eExecStopState;
        SetHMIRobotStatus(eHMIButtonStopPressed); // set HMI status to stop pressed
        ClearHMIStatus();
        break;
    default:
        break;
    }
    robot_program_loop();
}