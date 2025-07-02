#include "execution.h"
#include "modbusHandler.h"
#include "modbusFiles.h"
#include "main.h"
int KeyboardActionWaitTime = 0;
int KeyboardElementEngraveTime = 0;
int fileOpenTime = 0;
int ShortCutTime = 0;
ExeCurtState_typedef ExeCurtState;
int currentFileIndex = 0;
int fileRepetition = 0;
int Status = false;
int KeyboardShorcutStep = 0;
// Time to wait before sending next file
void Keyboard_setup()
{
    Keyboard.begin();
    ExeCurtState = OpeningFileSelectMenuState;
}

void Keyboard_loop()
{
    Status = Register(ReadHoldingRegisters, EmulatorControlAdrees);
    switch (Status)
    {
    case StartStatus:
        if (millis() - KeyboardActionWaitTime > KboardTickTime)
        {
            if (numFiles != 0)
            {
                switch (ExeCurtState)
                {
                case OpeningFileSelectMenuState:
                    if (KeyboardShorcutStep == 0)
                    {
                        Keyboard.write(KEY_ESC);
                        Keyboard.write(KEY_ESC);
                        Keyboard.write(KEY_ESC);
                    }
                    switch (KeyboardShorcutStep)
                    {
                    case 1:
                        Keyboard.write(KEY_LEFT_ALT);
                        ExeCurtState = OpeningFileSelectMenuState;
                        break;
                    case 2:
                        Keyboard.write(KEY_F);
                        
                        ExeCurtState = OpeningFileSelectMenuState;
                        break;
                    case 3:
                        Keyboard.write(KEY_O);
                        ExeCurtState = OpeningFileSelectMenuState;
                        break;
                    default:
                        KeyboardShorcutStep = 0;
                        ExeCurtState = WaitForFileMenuToLoadState;
                        fileOpenTime = millis();
                        break;
                    }
                    ShortCutTime = millis();
                    break;
                case OpeningFileSelectMenuDelay:
                    if (millis() - ShortCutTime > KboardShortCutTime + KboardTickTime)
                    {
                        KeyboardShorcutStep++;
                        ExeCurtState = OpeningFileSelectMenuState;
                    }

                    break;
                case WaitForFileMenuToLoadState:
                    if (millis() - fileOpenTime > FileMenuOpenTime + KboardTickTime)
                    {
                        ExeCurtState = WritingFileName;
                    }
                    break;
                case WritingFileName:
                    Keyboard.print(ReadFileName(currentFileIndex) + ".txt");
                    Keyboard.write(KEY_RETURN);
                    ExeCurtState = WaitingToLoadFileState;
                    fileOpenTime = millis();
                    break;
                case WaitingToLoadFileState:
                    if (millis() - fileOpenTime > FileOpenTime + KboardTickTime)
                    {
                        ExeCurtState = ExeFileState;
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
                    Register(WriteHoldingRegisters, EmulatorControlAdrees, StopStatus);
                    Status = StopStatus;
                }
                KeyboardActionWaitTime = millis();
            }
        }
        break;
    case PauseStatus:
        KeyboardShorcutStep = 0;
        break;
    case StopStatus:
        currentFileIndex = 0;
        KeyboardShorcutStep = 0;
        fileRepetition = 0;
        ExeCurtState = OpeningFileSelectMenuState;
        break;

    default:
        break;
    }
}
