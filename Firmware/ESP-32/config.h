#ifndef CONFIG_H
#define CONFIG_H

// Serial

constexpr unsigned long BAUD_RATE = 115200;

// Motion

constexpr float DEFAULT_MAX_SPEED = 1000.0f;
constexpr float DEFAULT_ACCELERATION = 500.0f;

//homing
constexpr float HOMING_ACCELERATION=200.0f;
constexpr long HOMING_SEARCH_DISTANCE=50000;
constexpr unsigned long HOMING_TIMEOUT=60000;

constexpr float HOMING_SPEED_FAST = 400.0f;
constexpr float HOMING_SPEED_SLOW = 100.0f;
constexpr long HOME_OFFSET_STEPS = 200;

// Step Conversion

constexpr float AZ_STEPS_PER_DEGREE = 53.333f;
constexpr float ALT_STEPS_PER_DEGREE = 106.667f;

// Limits

constexpr long AZ_MIN_STEPS = 0;
constexpr long AZ_MAX_STEPS = 19200;

constexpr long ALT_MIN_STEPS = 0;
constexpr long ALT_MAX_STEPS = 9600;



//parking pos
constexpr long PARK_AZIMUTH_STEPS=0;
constexpr long PARK_ALTITUDE_STEPS=0;

// Timeouts (ms)

constexpr unsigned long COMMAND_TIMEOUT = 3000;
constexpr unsigned long STATUS_INTERVAL = 500;

// Safety

constexpr bool ENABLE_SOFT_LIMITS = true;
constexpr bool ENABLE_LIMIT_SWITCHES = true;

#endif