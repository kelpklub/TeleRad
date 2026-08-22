#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

#include <Arduino.h>

// Initialization
void initConfigStorage();

// Load saved settings
bool loadConfig();

// Save current settings
bool saveConfig();

// Restore default settings
bool resetConfig();

// Stored motion settings
float getStoredSpeed();
float getStoredAcceleration();

void setStoredSpeed(float speed);
void setStoredAcceleration(float acceleration);

#endif