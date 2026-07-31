#ifndef PINS_H
#define PINS_H

//azimuth stepper driver 
constexpr uint8_t AZ_STEP_PIN =23;
constexpr uint8_t AZ_DIR_PIN = 22;
constexpr uint8_t AZ_ENABLE_PIN = 25;

//altitude stepper driver 
constexpr uint8_t ALT_STEP_PIN = 21;
constexpr uint8_t ALT_DIR_PIN = 19;
constexpr uint8_t ALT_ENABLE_PIN = 26;

//Limit switches
constexpr uint8_t AZ_LIMIT_PIN = 5;
constexpr uint8_t ALT_LIMIT_PIN = 17;

#endif