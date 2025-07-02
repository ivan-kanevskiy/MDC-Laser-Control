#include "execution.h"
#include "modbusHandler.h"
#include "modbusFiles.h"
#include "main.h"

//defines

typedef enum {
    OpeningFileSelectMenuState = 0,
    AfterKeyPressedDelayState,
    WaitForFileMenuToBeLoadedState,
    WritingFileName,
    WaitingToLoadFileState,
    ExeFileCheckState,
    ExeFileState,
    ExeFileDelay,
    ExecutionCompleted
} eExecutionTaskState;

//constants 


//varialbles 


eExecutionTaskState ExeCurtState;
static int KeyboardActionWaitTime = 0;
static int KeyboardElementEngraveTime = 0;
static int fileOpenTime = 0;
static int ShortcutTime = 0;
static int currentFileIndex = 0;
static int fileRepetition = 0;
static int Status = false;
static int KeyboardShorcutStep = 0;
// Time to wait before sending next file
void Keyboard_setup()
{
    Keyboard.begin();
    ExeCurtState = OpeningFileSelectMenuState;
}

void Keyboard_loop()
{
    Status = Register(ReadHoldingRegisters, HMIExecMenuControlButtons);
    switch (Status)
    {
    case btnStartPressed :
        if (millis() - KeyboardActionWaitTime > KboardTickTime)
        {
            if (numFiles != 0)
            {
                switch (ExeCurtState)
                {
                    
                case OpeningFileSelectMenuState:
                   
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
                        fileOpenTime = millis();
                        break;
                    }
                    ShortcutTime = millis();
                    break;
                case AfterKeyPressedDelayState:
                    if (millis() - ShortcutTime > KboardShortCutTime + KboardTickTime)
                    {
                        KeyboardShorcutStep++;
                        ExeCurtState = OpeningFileSelectMenuState;
                    }
                    break;

                case WaitForFileMenuToBeLoadedState:
                    if (millis() - fileOpenTime > FileMenuOpenTime + KboardTickTime)
                    {
                        ExeCurtState = WritingFileName;
                    }
                    break;

                case WritingFileName:
                    Keyboard.print(ReadFileName(currentFileIndex));
                    Keyboard.write(KEY_RETURN);
                    ExeCurtState = WaitingToLoadFileState;
                    fileOpenTime = millis();
                    break;
                case WaitingToLoadFileState:
                    if (millis() - fileOpenTime > FileOpenTime + KboardTickTime)
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
                    Keyboard.println("F2");
                    fileRepetition++;
                    KeyboardElementEngraveTime = millis();
                    ExeCurtState = ExeFileDelay;
                    break;
                case ExeFileDelay:
                    if (millis() - KeyboardElementEngraveTime > KboardElementEngraveTime + KboardTickTime)
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
                KeyboardActionWaitTime = millis();
            }
        }
        break;
    case btnPausePressed:
        // do nothing 
        break;
    case btnStopPressed:
        currentFileIndex = 0;
        KeyboardShorcutStep = 0;
        fileRepetition = 0;
        ExeCurtState = OpeningFileSelectMenuState;
        break;

    default:
        break;
    }
}
