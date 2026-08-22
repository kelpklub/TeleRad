#ifndef HOMING_H
#define HOMING_H

#include <Arduino.h>
#include "Types.h"

// init
void initHoming();
void updateHoming();

void startHoming();
void stopHoming();

bool isHoming();
bool isHomed();
HomingState getHomingState();

#endif