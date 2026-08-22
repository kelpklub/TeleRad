#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

// Initialization
void initLogger();

// Logging
void logInfo(const String& message);
void logWarning(const String& message);
void logError(const String& message);

// Enable / disable logging
void setLoggingEnabled(bool enabled);
bool loggingEnabled();

#endif