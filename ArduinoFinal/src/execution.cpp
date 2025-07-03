#include "main.h"

// defines

typedef enum
{
    OpeningFileSelectMenuState = 0,
    AfterKeyPressedDelayState,
    WaitForFileMenuToBeLoadedState,
    WritingFileName,
    WaitingToLoadFileState,
    ExeFileCheckState,
    ExeFileState,
    ExeFileDelay,
    WaitForMottorToFinishRotatinglPlateState,
    ExecutionCompleted
} eExecutionTaskState;

// constants

// varialbles

eExecutionTaskState ExeCurtState;
static int KeyboardActionWaitTime = 0;
static uint16_t KeyboardElementEngraveTime = 0;
static long numberOFStepsForOnePLateRotataion = 0;
static unsigned long Timer = 0;
static int currentFileIndex = 0;
static int fileRepetition = 0;
static int Status = false;
static int KeyboardShorcutStep = 0;
int currentPlate = 0;
// Time to wait before sending next file
void execution_setup()
{
    Keyboard.begin();
    ExeCurtState = OpeningFileSelectMenuState;
    KeyboardShorcutStep = 0;
}

void execution_loop()
{
    Status = Register(ReadHoldingRegisters, HMIExecMenuControlButtons);
    KeyboardElementEngraveTime = Register(ReadHoldingRegisters, HMIExecMenuDelayButton) * 1000;
    numberOFStepsForOnePLateRotataion = (StepperFullRotation / Register(ReadHoldingRegisters, HMIExecMenuNumberOfPlatesButton));

    switch (Status)
    {
    case btnStartPressed:
        if (numFiles != 0)
        {
            unsigned long currentMillis = millis();
            switch (ExeCurtState)
            {
            case OpeningFileSelectMenuState:
                currentPlate = 0;
                KeyboardShorcutStep++;
                switch (KeyboardShorcutStep)
                {
                case 1:
                    // emulate left "ALT" key press
                    Keyboard.write(KEY_LEFT_ALT);
                    ExeCurtState = AfterKeyPressedDelayState;
                    break;

                case 2:
                    // emulate "F" key  press
                    Keyboard.write(KEY_F);
                    ExeCurtState = AfterKeyPressedDelayState;
                    break;
                case 3:
                    // emulate "O" key press
                    Keyboard.write(KEY_O);
                    ExeCurtState = AfterKeyPressedDelayState;
                    break;
                default:
                    // there is nothing to sent.G o to WaitForFileMenuToLoadState to give
                    // enough time program to open " File Open " menu
                    KeyboardShorcutStep = 0;
                    ExeCurtState = WaitForFileMenuToBeLoadedState;
                    break;
                }
                Timer = millis();
                break;
            case AfterKeyPressedDelayState:
                if (currentMillis - Timer > KboardShortCutTime)
                {
                    ExeCurtState = OpeningFileSelectMenuState;
                }
                break;

            case WaitForFileMenuToBeLoadedState:
                if (currentMillis - Timer > FileMenuOpenTime)
                {
                    ExeCurtState = WritingFileName;
                }
                break;
            case WritingFileName:
                Keyboard.print(ReadFileName(currentFileIndex));
                Keyboard.write(KEY_RETURN);
                ExeCurtState = WaitingToLoadFileState;
                Timer = millis();
                break;
            case WaitingToLoadFileState:
                if (currentMillis - Timer >= FileOpenTime)
                {
                    ExeCurtState = ExeFileCheckState;
                }
                break;
            case ExeFileCheckState:
                if (fileRepetition >= ReadFileRepetition(currentFileIndex))
                {
                    fileRepetition = 0;
                    currentFileIndex++;
                    if (currentFileIndex >= numFiles)
                    {
                        rtt.println(readMottorSteps());
                        ExeCurtState = ExecutionCompleted;
                    }
                    else
                    {
                        ExeCurtState = OpeningFileSelectMenuState;
                    }
                }
                else
                {
                    ExeCurtState = ExeFileState;
                }
                break;
            case ExeFileState:
                Keyboard.println(String(readMottorSteps()));
                //Keyboard.println("F2");
                fileRepetition++;
                Timer = millis();
                ExeCurtState = ExeFileDelay;
                break;
            case ExeFileDelay:
                if (currentMillis - Timer >= KeyboardElementEngraveTime)
                {
                    // moveMottorSteps(1064);
                    currentPlate++;
                    if (checkForErrorAndReturnTrueIfNeedToBeCorrected() == true)
                    {
                        moveMottorSteps(numberOFStepsForOnePLateRotataion +1);
                    }
                    else
                    {
                        moveMottorSteps(numberOFStepsForOnePLateRotataion);
                    }

                    ExeCurtState = WaitForMottorToFinishRotatinglPlateState;
                }
                break;
            case WaitForMottorToFinishRotatinglPlateState:
                // check if mottor has stopped moving
                if (readMottorSpeed() == 0)
                {
                    ExeCurtState = ExeFileCheckState;
                }
                break;
            case ExecutionCompleted:
                break;
            default:
                break;
            }
            if (currentFileIndex >= numFiles)
            {
                currentFileIndex = 0;
                ExeCurtState = OpeningFileSelectMenuState;
                Register(WriteHoldingRegisters, HMIExecMenuControlButtons, btnStopPressed);
                Status = btnStopPressed;
            }
        }
        break;
    case btnPausePressed:
        // do nothing
        break;
    case btnStopPressed:
        currentPlate = 0;
        currentFileIndex = 0;
        KeyboardShorcutStep = 0;
        fileRepetition = 0;
        ExeCurtState = OpeningFileSelectMenuState;
        break;
    default:
        break;
    }
}
