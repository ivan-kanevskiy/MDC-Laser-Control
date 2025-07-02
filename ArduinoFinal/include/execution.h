#ifndef execution_h
#define execution_h
#include "Keyboard.h"

extern void Keyboard_setup();
extern void Keyboard_loop();

#define StartStatus 1
#define PauseStatus 2
#define StopStatus 3
#define KboardElementEngraveTime 500 
#define KboardShortCutTime 100 
#define KboardTickTime 100
#define FileMenuOpenTime 5000 
#define FileOpenTime 250 
#define KEY_F 0x46
#define KEY_O 0x4F
//#define OpenignFileSelectMenu 0
//#define WritingAndSelectiongFile 1
//#define FileSelectionDone 2
//#define ExecutionCompleted 3
typedef enum {
    OpeningFileSelectMenuState = 0,
    OpeningFileSelectMenuDelay,
    WaitForFileMenuToLoadState,
    WritingFileName,
    WaitingToLoadFileState,
    ExeFileCheckState,
    ExeFileState,
    ExeFileDelay,
    ExecutionCompleted
} ExeCurtState_typedef;
#endif