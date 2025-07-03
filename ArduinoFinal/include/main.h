#ifndef main_h
#define main_h
#include "rttstream.h"
#include "Arduino.h"

#include "loadProgram.h"
#include "modbusDefines.h"
#include "modbusHandler.h"
#include "execution.h"
#include "calibration.h"

extern RTTStream rtt;
#define tickTime 100


#define windowMain 1
#define windowLoadProgram 2
#define windowExecution 3
#define windowCalibration 4
#define windowSetup 5
typedef void (*CallbackFunction)();
extern void executeIfConditionIsTrue(int statement, int value, CallbackFunction func);
#endif