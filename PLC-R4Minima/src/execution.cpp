//------------------------------------------------------------------------------
// Included files to resolve specific definitions in this file
//------------------------------------------------------------------------------

#include "main.h"

//------------------------------------------------------------------------------
// Local constants
//------------------------------------------------------------------------------
#define KEY_F 0x46
#define KEY_O 0x4F

#define NumberOfDetailsOnPlateDefaultValue  60     // default number of details on plate
#define KboardShortCutTime                  1000   // time to wait between key presses to open "File Open" menu
#define FileMenuOpenTime                    4000   // time to wait for "File Open" menu to be opened
#define LoadFileWaitTime                    5000   // time to wait for file to be loaded


//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------
#define GetHMIStatus()              Register(ReadHoldingRegisters, HMIExecMenuControlButtons) 
#define ClearHMIStatus()            Register(ReadHoldingRegisters, HMIExecMenuControlButtons, eHMIButtonNoPressed) 
#define SetHMIStatus(x)             Register(WriteHoldingRegisters, HMIxecMenuControlReturn, x)
#define GetNumberOfDetailOnPlate()  Register(ReadHoldingRegisters, HMIExecMenuNumberOfPlatesButton)
#define GetEngraveTime()            Register(ReadHoldingRegisters, HMIExecMenuEngraveTimeButton)

//------------------------------------------------------------------------------
// Local types
//------------------------------------------------------------------------------
typedef enum tHMIExecButtonsState
{
    eHMIButtonNoPressed = 0,
    eHMIButtonStartPressed = 1,
    eHMIButtonPausePressed = 2,
    eHMIButtonStopPressed = 3
} eHMIExecButtonsState;

typedef enum tExecutionTaskState
{
    eInitState = 0 ,
    eOpeningFileSelectMenuState ,
    eAfterKeyPressedDelayState,
    eWaitForFileMenuToBeLoadedState,
    eWritingFileNameState,
    eWaitingToLoadFileState,
    eExecFileCheckState,
    eExecFileState,
    eExecFileDelayState,
    eWaitForMottorMovementFinishState,
    eExecDoneState,
    eExecStopState,
    eExecPausedState
} eExecutionTaskState;

//------------------------------------------------------------------------------
// Local data
//------------------------------------------------------------------------------



static eExecutionTaskState ExeCurtState;   //
static eHMIExecButtonsState eHMIExecProgramStatus= eHMIButtonNoPressed;

static unsigned long StepsForOneDetail = NumberOfDetailsOnPlateDefaultValue;
static int ErrorWhenRoundDown = 0;       // missed steps for one full plate rotation. 
static unsigned long Timer = 0;

static uint16_t KeyboardElementEngraveTime = 0;

static int KeyboardActionWaitTime = 0;
static int currentFileCnt = 0;
static int fileExecutionCnt = 0;
static int KeyboardShorcutStep = 0;
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


// Time to wait before sending next file
void execution_setup()
{
    Keyboard.begin();
    ExeCurtState = eExecPausedState;
    KeyboardShorcutStep = 0;
    StepsForOneDetail = NumberOfDetailsOnPlateDefaultValue;
    
}


/**
 * @brief Main execution loop
 * 
 * This function is called in the main loop to handle the execution laser programs
 * based on the HMI button states and other parameters.
 */
void execution_program_loop()
{
    if (GetNumberOfFileToExec() != 0)
        {
            unsigned long currentMillis = millis();
            switch (ExeCurtState)
            {
            case eInitState:
                // Engrave wait time 
                KeyboardElementEngraveTime = GetEngraveTime() * 1000;  // convert seconds to milliseconds
                // calculate number of steps for one detail on plate
                StepsForOneDetail = (PlateFullRotationSteps / GetNumberOfDetailOnPlate());
                // calculate missed steps for one revolution of plate
                ErrorWhenRoundDown = PlateFullRotationSteps % GetNumberOfDetailOnPlate();
                
                ExeCurtState = eOpeningFileSelectMenuState;
                break;
            case eOpeningFileSelectMenuState:
              
                KeyboardShorcutStep++;
                switch (KeyboardShorcutStep)
                {
                case 1:
                    // emulate left "ALT" key press
                    Keyboard.write(KEY_LEFT_ALT);
                    ExeCurtState = eAfterKeyPressedDelayState;
                    break;

                case 2:
                    // emulate "F" key  press
                    Keyboard.write(KEY_F);
                    ExeCurtState = eAfterKeyPressedDelayState;
                    break;
                case 3:
                    // emulate "O" key press
                    Keyboard.write(KEY_O);
                    ExeCurtState = eAfterKeyPressedDelayState;
                    break;
                default:
                    // there is nothing to sent. Go to WaitForFileMenuToLoadState to give
                    // enough time program to open " File Open " menu
                    KeyboardShorcutStep = 0;
                    ExeCurtState = eWaitForFileMenuToBeLoadedState;
                    break;
                }
                Timer = millis();
                break;
            case eAfterKeyPressedDelayState:
                if (currentMillis - Timer > KboardShortCutTime)
                {
                    ExeCurtState = eOpeningFileSelectMenuState;
                }
                break;

            case eWaitForFileMenuToBeLoadedState:
                if (currentMillis - Timer > FileMenuOpenTime)
                {
                    ExeCurtState = eWritingFileNameState;
                }
                break;
            case eWritingFileNameState:
                // write file name in the "File Open" menu
                Keyboard.print(GetFileName(currentFileCnt));
                //press ENTER key to select file
                Keyboard.write(KEY_RETURN);
                //go to wait state because computer takes some time to open file
                ExeCurtState = eWaitingToLoadFileState;
                //get current timer value
                Timer = millis();
                break;
                
            case eWaitingToLoadFileState:
                if (currentMillis - Timer >= LoadFileWaitTime)
                {
                    ExeCurtState = eExecFileCheckState;
                }
                break;

            case eExecFileCheckState:
                if (fileExecutionCnt < GetFileRepeatNumber(currentFileCnt))
                {
                    ExeCurtState = eExecFileState;
                }
                else
                {
                    fileExecutionCnt = 0;
                    currentFileCnt++;

                    // check if we have more files to execute
                    if(currentFileCnt < GetNumberOfFileToExec() )
                    {  // load next file
                        ExeCurtState = eOpeningFileSelectMenuState;
                    }
                    else
                    { // there is no more files to execute
                         ExeCurtState = eExecDoneState;
                    }
                }
                
                break;
            case eExecFileState:
                //Keyboard.println(String(readMottorSteps()));
                Keyboard.println("F2"); // send F2 key to start engraving
                fileExecutionCnt++;
                Timer = millis();
                ExeCurtState = eExecFileDelayState;
                break;
            case eExecFileDelayState:
                if (currentMillis - Timer >= KeyboardElementEngraveTime)
                { // wait for engraving time to pass
                    
                    if (checkForErrorAndReturnTrueIfNeedToBeCorrected() == true)
                    {
                        moveMottorSteps(StepsForOneDetail +1);
                    }
                    else
                    {
                        moveMottorSteps(StepsForOneDetail);
                    }

                    ExeCurtState = eWaitForMottorMovementFinishState;
                }
                break;
            case eWaitForMottorMovementFinishState:
                // check if mottor has stopped moving
                if (readMottorSpeed() == 0)
                {
                    ExeCurtState = eExecFileCheckState;
                }
                break;
            case eExecDoneState:
                // execution of all files is done   
                // reset all variables to initial state
               
           
                ExeCurtState = eExecStopState;
            
                // set HMI to button stop state
                //SetHMIStatus(eHMIButtonStopPressed);
             case eExecStopState: 
                  currentFileCnt = 0;
                  KeyboardShorcutStep = 0;
                  fileExecutionCnt = 0;  

             break;
                // eHMIExecProgramStatus = eHMIButtonStopPressed;

                break;
            case eExecPausedState:
                // do nothing, just wait for HMI to change state
                break;
            default:
                break;
            }
             
        }

}
/**
 * @brief Execution loop to handle HMI button states and change execution state accordingly
 * 
 * This function checks the current HMI button state and changes the execution state
 * based on the button pressed.
 */
void execution_loop()
{
    eHMIExecProgramStatus = (eHMIExecButtonsState) GetHMIStatus();   //make sure that HMI send valid values for this enum
    
    switch (eHMIExecProgramStatus)
    {
    case eHMIButtonStartPressed:

        ExeCurtState = eInitState; // set execution state to initial state
        SetHMIStatus(eHMIButtonStartPressed); // set HMI status to start pressed
        ClearHMIStatus();

        break;

    case eHMIButtonPausePressed:
        ExeCurtState = eExecPausedState; // set execution state to paused
        SetHMIStatus(eHMIButtonPausePressed); // set HMI status to pause pressed
        ClearHMIStatus();

        break;

    case eHMIButtonStopPressed:
        ExeCurtState = eExecStopState;
        SetHMIStatus(eHMIButtonStopPressed);// set HMI status to stop pressed 
        ClearHMIStatus();

        break;
    default:
        break;
    }
}
