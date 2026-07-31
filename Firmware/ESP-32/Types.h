#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

//axis
enum class Axis 
{
    AZIMUTH,
    ALTITUDE

};

//Motion state
enum class MotionState
{
    IDLE,
    MOVING,
    HOMING,
    PARKING,
    STOPPED,
    ERROR
};

//errors 
enum class ErrorCode
{
    NONE,
    INVALID_COMMAND,
    HOMING_FAILED,
    LIMIT_SWITCH_TRIGGERED,
    SOFT_LIMIT_REACHED,
    MOTOR_FAULT,
    SERIAL_TIMEOUT,
    UNKNOWN
};

//POsition
struct Position
{
    long azimuth;
    long altitude;
};

//telescope status
struct TelescopeStatus
{
    MotionState state;
    Position position;
    bool homed;
    bool motorsenabled;
    bool limitTriggered;
    ErrorCode error;
};

//commands 
struct Command
{
    String command;
    String arguments[4];
    uint8_t argumentCount;
};

#endif