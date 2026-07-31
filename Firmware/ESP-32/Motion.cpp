#include "Motion.h"
#include <AccelStepper.h>

//motor obj
AccelStepper azMotor(
    AccelStepper::DRIVER,
    AZ_STEP_PIN,
    AZ_DIR_PIN
);
AccelStepper altMotor(
    AccelStepper::DRIVER,
    ALT_STEP_PIN,
    ALT_DIR_PIN
);

//state
statis Position currentPosition = {0, 0};
statis Position targetPosition={0,0};
statis MotionState motionState =MotionState::IDLE;
static bool motorsEnabled = false;
