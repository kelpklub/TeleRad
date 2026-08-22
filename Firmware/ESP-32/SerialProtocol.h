#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#include <Arduino.h>
#include "Types.h"

// Initialization
void initSerialProtocol();

// Main update
void updateSerialProtocol();

// Command handling
void processCommand(const Command& command);

#endif