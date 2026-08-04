#include "Safety.h"
#include "config.h"
#include "Pins.h"
#include "Motion.h"

//state
static SafetyState safetyState =SafetyState::NORMAL;
static bool emergencyStopped=false;
static bool limitSwitchesEnabledFlag=ENABLE_LIMIT_SWITCHES;

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
    if (limitsTriggered())
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
void setLimitSwitchesEnabled(bool enabled)
{
    limitSwitchesEnabledFlag=enabled;
}

bool limitSwitchesEnabled()
{
    return limitSwitchesEnabledFlag;
}

bool limitsTriggered()
 {
    if(!limitSwitchesEnabledFlag)
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

