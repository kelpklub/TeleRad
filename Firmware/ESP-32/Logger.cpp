#include "Logger.h"

static bool loggerEnabled = true;


// Initialization

void initLogger()
{
    loggerEnabled = true;
}


// Enable / disable logging

void setLoggingEnabled(bool enabled)
{
    loggerEnabled = enabled;
}

bool loggingEnabled()
{
    return loggerEnabled;
}


// Logging

void logInfo(const String& message)
{
    if (!loggerEnabled)
    {
        return;
    }

    Serial.print("[INFO] ");
    Serial.println(message);
}


void logWarning(const String& message)
{
    if (!loggerEnabled)
    {
        return;
    }

    Serial.print("[WARNING] ");
    Serial.println(message);
}


void logError(const String& message)
{
    if (!loggerEnabled)
    {
        return;
    }

    Serial.print("[ERROR] ");
    Serial.println(message);
}