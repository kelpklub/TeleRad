#include "Safety.h"
#include "config.h"
#include "Pins.h"
#include "Motion.h"

//state
static SafetyState safetyState =SafetyState::NORMAL;
static bool emergencyStopped=false;

//init
void initSaftey()
{
    pinMode(AZ_LIMIT_PIN,INPUT_PULLUP);
    pinMode(ALT_LIMIT_PIN,INPUT_PULLUP);

    safetyState =SafetyState::NORMAL;
    emergencyStopped=false;
}

//update 
void updateSaftey()
{
    if(emergencyStopped)
    {
        return;
    }
    if (ENABLE_LIMIT_SWITCHES && limitsTriggered())
    {
        stopMotion();
        safetyState =SafetyState::LIMIT_TRIGGERED;
        return;
    }
    safetyState=SafetyState::NORMAL;
}

void emergencyStop()
{
    stopMotion();
    emergencyStopped=true;
    safetyState=SafetyState::EMERGENCY_STOP;
}

void clearEmergencyStop()
{
    emergencyStopped=false;
    safetyState=SafetyState::NORMAL;
}
bool isEmergencyStopped()
{
    return emergencyStopped;
}

//limit switches
 bool limitsTriggered()
 {
    if(!ENABLE_LIMIT_SWITCHES)
    {
        return false;
    }
    return digitalRead(AZ_LIMIT_PIN)==LOW ||
     digitalRead(ALT_LIMIT_PIN)==LOW;
 }

 //SOft LImits
 bool withinSoftLimits(long azimuth,long altitude)
 {
    if(!ENABLE_SOFT_LIMITS)
    {
        return true;
    }
    return 
    azimuth >=AZ_MIN_STEPS && 
    azimuth <=AZ_MAX_STEPS &&
    altitude >=ALT_MIN_STEPS &&
    altitude <=ALT_MAX_STEPS;
 }

 //State
 SafetyState getSafetyState()
 {
    return safetyState;
 }

