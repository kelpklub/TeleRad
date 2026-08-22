#ifndef MOTION_H
#define MOTION_H

#include <Arduino.h>
#include <AccelStepper.h>
#include "config.h"
#include "Pins.h"
#include "Types.h"

//init
void initMotion();
void updateMotion();

//movement
void moveTo(long azimuthSteps,long altitudeSteps);
void moveRelative(long azimuthOffset,long altitudeOffset);
void park();
void stopMotion();
void stopAzimuth();
void stopAltitude();

//driveer control 
void enableDrivers();
void disableDrivers();
bool driversEnabled();

//motionstate
bool isBusy();
void setMotionState(MotionState state);
MotionState getMotionState();

//position 
Position getCurrentPosition();
Position getTargetPosition();
void setCurrentPosition(long azimuth,long altitude);
void zeroPosition();

//motor settings 
void setSpeed(float speed);
void setAcceleration(float acceleration);
float getSpeed();
float getAcceleration();

//coordinate conversion
long azimuthDegreesToSteps(float degrees);
long altitudeDegreesToSteps(float degrees);
float azimuthStepsToDegrees(long steps);
float altitudeStepsToDegrees(long steps);

#endif
