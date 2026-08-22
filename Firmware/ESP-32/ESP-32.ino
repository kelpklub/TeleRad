#include "config.h"
#include "Motion.h"
#include "Safety.h"
#include "Homing.h"
#include "Logger.h"
#include "SerialProtocol.h"
#include "ConfigStorage.h"

void setup()
{
    Serial.begin(BAUD_RATE);
    initLogger();
    initConfigStorage();

    loadConfig();

    initMotion();

    setSpeed(getStoredSpeed());
    setAcceleration(getStoredAcceleration());
    
    initSafety();
    initHoming();
    initSerialProtocol();
}

void loop()
{
    updateMotion();
    updateHoming();
    updateSafety();
    updateSerialProtocol();
}