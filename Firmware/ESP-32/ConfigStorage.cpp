#include "ConfigStorage.h"

#include <Preferences.h>

#include "config.h"


// Internal storage

static Preferences preferences;

static constexpr const char* STORAGE_NAMESPACE = "telerad";

static float storedSpeed = DEFAULT_MAX_SPEED;
static float storedAcceleration = DEFAULT_ACCELERATION;


// Initialization

void initConfigStorage()
{
    preferences.begin(
        STORAGE_NAMESPACE,
        false
    );
}


// Load configuration

bool loadConfig()
{
    storedSpeed = preferences.getFloat(
        "speed",
        DEFAULT_MAX_SPEED
    );

    storedAcceleration = preferences.getFloat(
        "accel",
        DEFAULT_ACCELERATION
    );

    return true;
}


// Save configuration

bool saveConfig()
{
    size_t speedBytes = preferences.putFloat(
        "speed",
        storedSpeed
    );

    size_t accelerationBytes = preferences.putFloat(
        "accel",
        storedAcceleration
    );

    return (
        speedBytes == sizeof(float) &&
        accelerationBytes == sizeof(float)
    );
}


// Reset configuration

bool resetConfig()
{
    storedSpeed = DEFAULT_MAX_SPEED;
    storedAcceleration = DEFAULT_ACCELERATION;

    return saveConfig();
}


// Speed

float getStoredSpeed()
{
    return storedSpeed;
}


void setStoredSpeed(float speed)
{
    storedSpeed = speed;
}


// Acceleration

float getStoredAcceleration()
{
    return storedAcceleration;
}


void setStoredAcceleration(float acceleration)
{
    storedAcceleration = acceleration;
}