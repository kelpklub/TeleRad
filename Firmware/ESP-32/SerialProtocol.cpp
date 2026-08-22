#include "SerialProtocol.h"

#include "config.h"
#include "Motion.h"
#include "Safety.h"
#include "Homing.h"
#include "ConfigStorage.h"

#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>


// -------------------------------------------------
// Internal serial buffer
// -------------------------------------------------

static String serialBuffer;

constexpr size_t MAX_COMMAND_LENGTH = 96;

enum class PendingOperation
{
    NONE,
    HOME,
    MOVE,
    REL,
    PARK
};

static PendingOperation pendingOperation =
    PendingOperation::NONE;

// -------------------------------------------------
// Emergency stop protection
// -------------------------------------------------

static bool rejectIfEmergencyStopped()
{
    if (isEmergencyStopped())
    {
        Serial.println("ERROR EMERGENCY_STOP");
        return true;
    }

    return false;
}


// -------------------------------------------------
// Numeric parsing
// -------------------------------------------------

static bool parseLongArgument(
    const String& argument,
    long& value
)
{
    if (argument.length() == 0)
    {
        return false;
    }

    char* endPointer = nullptr;

    errno = 0;

    long parsedValue = strtol(
        argument.c_str(),
        &endPointer,
        10
    );

    if (endPointer == argument.c_str())
    {
        return false;
    }

    if (*endPointer != '\0')
    {
        return false;
    }

    if (errno == ERANGE)
    {
        return false;
    }

    value = parsedValue;

    return true;
}


static bool parseFloatArgument(
    const String& argument,
    float& value
)
{
    if (argument.length() == 0)
    {
        return false;
    }

    char* endPointer = nullptr;

    errno = 0;

    float parsedValue = strtof(
        argument.c_str(),
        &endPointer
    );

    if (endPointer == argument.c_str())
    {
        return false;
    }

    if (*endPointer != '\0')
    {
        return false;
    }

    if (errno == ERANGE)
    {
        return false;
    }

    if (!isfinite(parsedValue))
    {
        return false;
    }

    value = parsedValue;

    return true;
}

static void updateCommandCompletion()
{
    switch (pendingOperation)
    {
        case PendingOperation::HOME:

            if (isHomed())
            {
                Serial.println("HOMED");

                pendingOperation =
                    PendingOperation::NONE;
            }
            else if (
                getHomingState() == HomingState::ERROR ||
                isEmergencyStopped())
            {
                if (isEmergencyStopped())
                {
                    Serial.println(
                        "ERROR EMERGENCY_STOP"
                    );
                }
                else
                {
                    Serial.println(
                        "ERROR HOMING_FAILED"
                    );
                }

                pendingOperation =
                    PendingOperation::NONE;
            }

            break;


        case PendingOperation::MOVE:
        case PendingOperation::REL:
        case PendingOperation::PARK:

            if (
                getMotionState() == MotionState::ERROR ||
                isEmergencyStopped())
            {
                if (isEmergencyStopped())
                {
                    Serial.println(
                        "ERROR EMERGENCY_STOP"
                    );
                }
                else
                {
                    Serial.println(
                        "ERROR MOTION"
                    );
                }

                pendingOperation =
                    PendingOperation::NONE;

                break;
            }

            if (
                getMotionState() == MotionState::IDLE &&
                !isBusy())
            {
                Serial.println("DONE");

                pendingOperation =
                    PendingOperation::NONE;
            }

            break;


        case PendingOperation::NONE:
            break;
    }
}

// Initialization

void initSerialProtocol()
{
    serialBuffer.reserve(MAX_COMMAND_LENGTH);

    serialBuffer = "";

    Serial.println("READY");
}


// Serial update

void updateSerialProtocol()
{
    while (Serial.available() > 0)
    {
        char incoming = Serial.read();

        if (incoming == '\n')
        {
            if (serialBuffer.length() > 0)
            {
                Command command;

                command.command = "";
                command.argumentCount = 0;

                for (uint8_t i = 0; i < 4; i++)
                {
                    command.arguments[i] = "";
                }

                // Parse command name
                int start = 0;

                int space = serialBuffer.indexOf(' ');

                if (space == -1)
                {
                    command.command = serialBuffer;
                }
                else
                {
                    command.command =
                        serialBuffer.substring(0, space);

                    start = space + 1;

                    // Parse arguments
                    while (
                        start < serialBuffer.length() &&
                        command.argumentCount < 4
                    )
                    {
                        int nextSpace =
                            serialBuffer.indexOf(' ', start);

                        if (nextSpace == -1)
                        {
                            command.arguments[
                                command.argumentCount
                            ] = serialBuffer.substring(start);

                            command.argumentCount++;

                            break;
                        }

                        if (nextSpace > start)
                        {
                            command.arguments[
                                command.argumentCount
                            ] = serialBuffer.substring(
                                start,
                                nextSpace
                            );

                            command.argumentCount++;
                        }

                        start = nextSpace + 1;
                    }
                }

                command.command.trim();
                command.command.toUpperCase();

                for (
                    uint8_t i = 0;
                    i < command.argumentCount;
                    i++
                )
                {
                    command.arguments[i].trim();
                }

                processCommand(command);

                serialBuffer = "";
            }
        }

        else if (incoming != '\r')
        {
            if (serialBuffer.length() < MAX_COMMAND_LENGTH)
            {
                serialBuffer += incoming;
            }
            else
            {
                serialBuffer = "";

                Serial.println("ERROR COMMAND_TOO_LONG");
            }
        }
    }
    updateCommandCompletion();
}


// Command processing

void processCommand(const Command& command)
{
    // PING

    if (command.command == "PING")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        Serial.println("PONG");
        return;
    }


    // HOME

    if (command.command == "HOME")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (isHoming())
        {
            Serial.println("BUSY");
            return;
        }

        if (rejectIfEmergencyStopped())
        {
            return;
        }

        startHoming();

        pendingOperation =
            PendingOperation::HOME;

        Serial.println("OK");
        return;
    }

    // ESTOP

    if (command.command == "ESTOP")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        emergencyStop();
        pendingOperation =
            PendingOperation::NONE;
        Serial.println("OK");
        return;
    }
    // CLEAR_ESTOP

    if (command.command == "CLEAR_ESTOP")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (!isEmergencyStopped())
        {
            Serial.println("OK");
            return;
        }

        clearEmergencyStop();

        Serial.println("OK");
        return;
    }
    // STOP

    if (command.command == "STOP")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (isHoming())
        {
            stopHoming();
        }
        else
        {
            stopMotion();
        }
        pendingOperation =
            PendingOperation::NONE;

        Serial.println("OK");
        return;
    }


    // ENABLE

    if (command.command == "ENABLE")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (rejectIfEmergencyStopped())
        {
            return;
        }

        enableDrivers();

        Serial.println("OK");
        return;
    }


    // DISABLE

    if (command.command == "DISABLE")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (isBusy())
        {
            Serial.println("BUSY");
            return;
        }

        disableDrivers();

        Serial.println("OK");
        return;
    }


    // POSITION

    if (command.command == "POSITION")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        Position position = getCurrentPosition();

        Serial.print("AZ=");
        Serial.print(position.azimuth);

        Serial.print(" ALT=");
        Serial.println(position.altitude);

        return;
    }


    // STATUS

    if (command.command == "STATUS")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        Position position = getCurrentPosition();

        Serial.print("STATE=");

        switch (getMotionState())
        {
            case MotionState::IDLE:
                Serial.print("IDLE");
                break;

            case MotionState::MOVING:
                Serial.print("MOVING");
                break;

            case MotionState::HOMING:
                Serial.print("HOMING");
                break;

            case MotionState::PARKING:
                Serial.print("PARKING");
                break;

            case MotionState::STOPPED:
                Serial.print("STOPPED");
                break;

            case MotionState::ERROR:
                Serial.print("ERROR");
                break;
        }

        Serial.print(" AZ=");
        Serial.print(position.azimuth);

        Serial.print(" ALT=");
        Serial.print(position.altitude);

        Serial.print(" HOMED=");
        Serial.print(isHomed() ? 1 : 0);

        Serial.print(" ENABLED=");
        Serial.print(driversEnabled() ? 1 : 0);

        Serial.print(" LIMIT=");
        Serial.println(
            limitsTriggered() ? 1 : 0
        );

        return;
    }


    // MOVE

    if (command.command == "MOVE")
    {
        if (command.argumentCount != 2)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (isHoming())
        {
            Serial.println("BUSY");
            return;
        }

        if (!isHomed())
        {
            Serial.println("ERROR NOT_HOMED");
            return;
        }

        if (rejectIfEmergencyStopped())
        {
            return;
        }

        long azimuth;
        long altitude;

        if (!parseLongArgument(
                command.arguments[0],
                azimuth))
        {
            Serial.println("ERROR INVALID_AZIMUTH");
            return;
        }

        if (!parseLongArgument(
                command.arguments[1],
                altitude))
        {
            Serial.println("ERROR INVALID_ALTITUDE");
            return;
        }

        if (!withinSoftLimits(
                azimuth,
                altitude))
        {
            Serial.println("ERROR SOFT_LIMIT");
            return;
        }

        moveTo(
            azimuth,
            altitude
        );

        pendingOperation =
            PendingOperation::MOVE;

        Serial.println("OK");
        return;
    }


    // REL

    if (command.command == "REL")
    {
        if (command.argumentCount != 2)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (isHoming())
        {
            Serial.println("BUSY");
            return;
        }

        if (!isHomed())
        {   
            Serial.println("ERROR NOT_HOMED");
            return;
        } 

        if (rejectIfEmergencyStopped())
        {
            return;
        }

        long azimuthOffset;
        long altitudeOffset;

        if (!parseLongArgument(
                command.arguments[0],
                azimuthOffset))
        {
            Serial.println("ERROR INVALID_AZIMUTH");
            return;
        }

        if (!parseLongArgument(
                command.arguments[1],
                altitudeOffset))
        {
            Serial.println("ERROR INVALID_ALTITUDE");
            return;
        }

        Position position = getCurrentPosition();

        long targetAzimuth =
            position.azimuth + azimuthOffset;

        long targetAltitude =
            position.altitude + altitudeOffset;

        if (!withinSoftLimits(
                targetAzimuth,
                targetAltitude))
        {
            Serial.println("ERROR SOFT_LIMIT");
            return;
        }

        moveRelative(
            azimuthOffset,
            altitudeOffset
        );

        pendingOperation =
            PendingOperation::REL;

        Serial.println("OK");
        return;
    }


    // PARK

    if (command.command == "PARK")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (isHoming())
        {
            Serial.println("BUSY");
            return;
        }

        if (rejectIfEmergencyStopped())
        {
            return;
        }

        if (!isHomed())
        {
            Serial.println("ERROR NOT_HOMED");
            return;
        }

        if (!withinSoftLimits(
                PARK_AZIMUTH_STEPS,
                PARK_ALTITUDE_STEPS))
        {
            Serial.println("ERROR SOFT_LIMIT");
            return;
        }

        park();

        pendingOperation =
            PendingOperation::PARK;

        Serial.println("OK");
        return;
    }


    // SPEED

    if (command.command == "SPEED")
    {
        if (command.argumentCount != 1)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        float speed;

        if (!parseFloatArgument(
                command.arguments[0],
                speed))
        {
            Serial.println("ERROR INVALID_SPEED");
            return;
        }

        if (speed <= 0.0f)
        {
            Serial.println("ERROR INVALID_SPEED");
            return;
        }

        setSpeed(speed);
        setStoredSpeed(speed);

        Serial.println("OK");
        return;
    }


    // ACCEL

    if (command.command == "ACCEL")
    {
        if (command.argumentCount != 1)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        float acceleration;

        if (!parseFloatArgument(
                command.arguments[0],
                acceleration))
        {
            Serial.println("ERROR INVALID_ACCELERATION");
            return;
        }

        if (acceleration <= 0.0f)
        {
            Serial.println("ERROR INVALID_ACCELERATION");
            return;
        }

        setAcceleration(acceleration);
        setStoredAcceleration(acceleration);

        Serial.println("OK");
        return;
    }
    // SAVE

    if (command.command == "SAVE")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (saveConfig())
        {
            Serial.println("OK");
        }
        else
        {
            Serial.println("ERROR SAVE_FAILED");
        }

        return;
    }

    // LOAD

    if (command.command == "LOAD")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (!loadConfig())
        {
            Serial.println("ERROR LOAD_FAILED");
            return;
        }

        setSpeed(getStoredSpeed());
        setAcceleration(getStoredAcceleration());

        Serial.println("OK");
        return;
    }

    // RESET_CONFIG

    if (command.command == "RESET_CONFIG")
    {
        if (command.argumentCount != 0)
        {
            Serial.println("ERROR INVALID_ARGUMENTS");
            return;
        }

        if (!resetConfig())
        {
            Serial.println("ERROR RESET_FAILED");
            return;
        }

        setSpeed(getStoredSpeed());
        setAcceleration(getStoredAcceleration());

        Serial.println("OK");
        return;
    }

    // Unknown command

    Serial.println("ERROR UNKNOWN_COMMAND");
}