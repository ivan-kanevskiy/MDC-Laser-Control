#ifndef execution_h
#define execution_h
#include "Keyboard.h"

extern void execution_setup();
extern void execution_loop();
extern int currentPlate;
#define btnStartPressed 1
#define btnPausePressed 2
#define btnStopPressed 3
#define KboardShortCutTime 1000 
#define FileMenuOpenTime 4000 
#define FileOpenTime 5000 
#define KEY_F 0x46
#define KEY_O 0x4F
//#define OpenignFileSelectMenu 0
//#define WritingAndSelectiongFile 1
//#define FileSelectionDone 2
//#define ExecutionCompleted 3

#endif