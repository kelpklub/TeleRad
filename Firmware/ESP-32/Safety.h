#ifndef SAFETY_H
#define SAFETY_H

#include <Arduino.h>
#include "Types.h"

void initSafety();
void updateSafety();
void emergencyStop();
void clearEmergencyStop();

bool isEmergencyStopped();
bool limitsTriggered();
bool withinSoftLimits(long azimuth, long altitude);

SafetyState getSafetyState();

#endif