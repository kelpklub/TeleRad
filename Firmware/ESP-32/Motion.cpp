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
static Position currentPosition = {0, 0};
static Position targetPosition={0,0};
static MotionState motionState =MotionState::IDLE;
static bool motorsEnabled = false;
static float currentSpeed= DEFAULT_MAX_SPEED;
static float currentAcceleration=DEFAULT_ACCELERATION;

//INITIALIZATION
void initMotion()
{
    pinMode(AZ_ENABLE_PIN,OUTPUT);
    pinMode(ALT_ENABLE_PIN,OUTPUT);

    disableDrivers();

    azMotor.setMaxSpeed(currentSpeed);
    azMotor.setAcceleration(currentAcceleration);

    altMotor.setMaxSpeed(currentSpeed);
    altMotor.setAcceleration(currentAcceleration);

    azMotor.setCurrentPosition(0);
    altMotor.setCurrentPosition(0);

    currentPosition.azimuth=0;
    currentPosition.altitude=0;

    targetPosition=currentPosition;
    motionState=MotionState::IDLE;
}

//driver control 
void enableDrivers()
{
    digitalWrite(AZ_ENABLE_PIN,LOW);
    digitalWrite(ALT_ENABLE_PIN,LOW);
    motorsEnabled=true;
}

void disableDrivers()
{
    digitalWrite(AZ_ENABLE_PIN,HIGH);
    digitalWrite(ALT_ENABLE_PIN,HIGH);
    motorsEnabled=false;
}

bool driversEnabled()
{
    return motorsEnabled;
}

//motor settings
void setSpeed(float speed)
{
    currentSpeed=speed;
    azMotor.setMaxSpeed(speed);
    altMotor.setMaxSpeed(speed);
}

void setAcceleration(float acceleration)
{
    currentAcceleration=acceleration;
    azMotor.setAcceleration(acceleration);
    altMotor.setAcceleration(acceleration);
}

float getSpeed()
{
    return currentSpeed;
}

float getAcceleration()
{
    return currentAcceleration;
}