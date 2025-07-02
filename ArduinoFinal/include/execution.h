#ifndef execution_h
#define execution_h
#include "Keyboard.h"

extern void Keyboard_setup();
extern void Keyboard_loop();

#define btnStartPressed 1
#define btnPausePressed 2
#define btnStopPressed 3
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

#endif