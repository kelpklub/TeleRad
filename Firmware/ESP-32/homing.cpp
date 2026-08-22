#include "Homing.h"

#include "config.h"
#include "Motion.h"
#include "Safety.h"
#include "Pins.h"

static HomingState homingState = HomingState::IDLE;

static bool homing = false;
static bool homed = false;

static unsigned long homingStartTime = 0;


// -------------------------------------------------
// Internal failure cleanup
// -------------------------------------------------

static void failHoming()
{
    stopMotion();

    setLimitSwitchesEnabled(true);

    setSpeed(DEFAULT_MAX_SPEED);
    setAcceleration(DEFAULT_ACCELERATION);

    homing = false;
    homed = false;

    homingState = HomingState::ERROR;

    setMotionState(MotionState::ERROR);
}


// -------------------------------------------------
// Initialization
// -------------------------------------------------

void initHoming()
{
    homingState = HomingState::IDLE;

    homing = false;
    homed = false;

    homingStartTime = 0;
}


// -------------------------------------------------
// Start homing
// -------------------------------------------------

void startHoming()
{
    if (homing)
    {
        return;
    }

    enableDrivers();

    // Physical limit switches are intentionally triggered
    // during homing.
    setLimitSwitchesEnabled(false);

    setSpeed(HOMING_SPEED_FAST);
    setAcceleration(HOMING_ACCELERATION);

    homing = true;
    homed = false;

    setMotionState(MotionState::HOMING);

    homingStartTime = millis();

    // Start azimuth fast search.
    moveRelative(
        -HOMING_SEARCH_DISTANCE,
        0
    );

    setMotionState(MotionState::HOMING);

    homingState = HomingState::AZ_FAST;
}


// -------------------------------------------------
// Stop homing
// -------------------------------------------------

void stopHoming()
{
    if (!homing)
    {
        return;
    }

    stopMotion();

    setLimitSwitchesEnabled(true);

    homing = false;
    homed = false;

    homingState = HomingState::IDLE;

    setSpeed(DEFAULT_MAX_SPEED);
    setAcceleration(DEFAULT_ACCELERATION);

    setMotionState(MotionState::IDLE);
}


// -------------------------------------------------
// Homing state machine
// -------------------------------------------------

void updateHoming()
{
    if (!homing)
    {
        return;
    }


    // -------------------------------------------------
    // Emergency stop
    // -------------------------------------------------

    if (isEmergencyStopped())
    {
        homing = false;
        homed = false;

        homingState = HomingState::ERROR;

        setLimitSwitchesEnabled(true);

        setSpeed(DEFAULT_MAX_SPEED);
        setAcceleration(DEFAULT_ACCELERATION);

        setMotionState(MotionState::ERROR);

        return;
    }


    // -------------------------------------------------
    // Global homing timeout
    // -------------------------------------------------

    if (millis() - homingStartTime > HOMING_TIMEOUT)
    {
        failHoming();
        return;
    }


    switch (homingState)
    {
        // -------------------------------------------------
        // AZIMUTH FAST SEARCH
        // -------------------------------------------------

        case HomingState::AZ_FAST:

            if (digitalRead(AZ_LIMIT_PIN) == LOW)
            {
                stopAzimuth();

                // Wait for the controlled stop before
                // starting the backoff movement.
                homingState = HomingState::AZ_BACKOFF;
            }

            break;


        // -------------------------------------------------
        // AZIMUTH BACKOFF
        // -------------------------------------------------

        case HomingState::AZ_BACKOFF:

            if (!isBusy())
            {
                setSpeed(HOMING_SPEED_FAST);

                // Move away from the switch.
                moveRelative(
                    HOME_OFFSET_STEPS,
                    0
                );

                setMotionState(MotionState::HOMING);

                homingState = HomingState::AZ_SLOW;
            }

            break;


        // -------------------------------------------------
        // AZIMUTH SLOW APPROACH
        // -------------------------------------------------

        case HomingState::AZ_SLOW:

            if (!isBusy())
            {
                setSpeed(HOMING_SPEED_SLOW);

                // Move back toward the switch.
                moveRelative(
                    -HOME_OFFSET_STEPS * 2,
                    0
                );

                setMotionState(MotionState::HOMING);

                homingState = HomingState::AZ_ZERO;
            }

            break;


        // -------------------------------------------------
        // AZIMUTH ZERO
        // -------------------------------------------------

        case HomingState::AZ_ZERO:

            if (digitalRead(AZ_LIMIT_PIN) == LOW)
            {
                stopAzimuth();

                if (!isBusy())
                {
                    Position position = getCurrentPosition();

                    setCurrentPosition(
                        0,
                        position.altitude
                    );

                    setSpeed(HOMING_SPEED_FAST);

                    // Start altitude fast search.
                    moveRelative(
                        0,
                        -HOMING_SEARCH_DISTANCE
                    );

                    setMotionState(MotionState::HOMING);

                    homingState = HomingState::ALT_FAST;
                }
            }
            else if (!isBusy())
            {
                // Slow approach finished without reaching
                // the physical switch.
                failHoming();
            }

            break;


        // -------------------------------------------------
        // ALTITUDE FAST SEARCH
        // -------------------------------------------------

        case HomingState::ALT_FAST:

            if (digitalRead(ALT_LIMIT_PIN) == LOW)
            {
                stopAltitude();

                // Wait for controlled stop.
                homingState = HomingState::ALT_BACKOFF;
            }

            break;


        // -------------------------------------------------
        // ALTITUDE BACKOFF
        // -------------------------------------------------

        case HomingState::ALT_BACKOFF:

            if (!isBusy())
            {
                setSpeed(HOMING_SPEED_FAST);

                // Move away from the switch.
                moveRelative(
                    0,
                    HOME_OFFSET_STEPS
                );

                setMotionState(MotionState::HOMING);

                homingState = HomingState::ALT_SLOW;
            }

            break;


        // -------------------------------------------------
        // ALTITUDE SLOW APPROACH
        // -------------------------------------------------

        case HomingState::ALT_SLOW:

            if (!isBusy())
            {
                setSpeed(HOMING_SPEED_SLOW);

                // Move back toward the switch.
                moveRelative(
                    0,
                    -HOME_OFFSET_STEPS * 2
                );

                setMotionState(MotionState::HOMING);

                homingState = HomingState::ALT_ZERO;
            }

            break;


        // -------------------------------------------------
        // ALTITUDE ZERO
        // -------------------------------------------------

        case HomingState::ALT_ZERO:

            if (digitalRead(ALT_LIMIT_PIN) == LOW)
            {
                stopAltitude();

                if (!isBusy())
                {
                    Position position = getCurrentPosition();

                    setCurrentPosition(
                        position.azimuth,
                        0
                    );

                    setSpeed(HOMING_SPEED_FAST);

                    // Move away from both home switches.
                    moveRelative(
                        HOME_OFFSET_STEPS,
                        HOME_OFFSET_STEPS
                    );

                    setMotionState(MotionState::HOMING);

                    homingState =
                        HomingState::MOVE_HOME_OFFSET;
                }
            }
            else if (!isBusy())
            {
                // Slow approach finished without reaching
                // the physical switch.
                failHoming();
            }

            break;


        // -------------------------------------------------
        // MOVE AWAY FROM HOME SWITCHES
        // -------------------------------------------------

        case HomingState::MOVE_HOME_OFFSET:

            if (!isBusy())
            {
                setSpeed(DEFAULT_MAX_SPEED);
                setAcceleration(DEFAULT_ACCELERATION);

                // Normal safety operation resumes.
                setLimitSwitchesEnabled(true);

                homingState = HomingState::COMPLETE;
            }

            break;


        // -------------------------------------------------
        // COMPLETE
        // -------------------------------------------------

        case HomingState::COMPLETE:

            homed = true;
            homing = false;

            setMotionState(MotionState::IDLE);

            break;


        // -------------------------------------------------
        // ERROR
        // -------------------------------------------------

        case HomingState::ERROR:
            break;


        // -------------------------------------------------
        // IDLE
        // -------------------------------------------------

        case HomingState::IDLE:
            break;
    }
}


// -------------------------------------------------
// Status
// -------------------------------------------------

bool isHoming()
{
    return homing;
}


bool isHomed()
{
    return homed;
}


HomingState getHomingState()
{
    return homingState;
}