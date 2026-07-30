#include <WiFi.h>
#include <WebServer.h>
#include <AccelStepper.h>

//WiFi 
const char* ssid = "TeleRad-ESP32";
const char* password = "rootaboot";

WebServer server(80);

//Pins

// Azimuth Driver
#define AZ_STEP 23
#define AZ_DIR 22
#define AZ_EN 25

// Altitude Driver
#define ALT_STEP 21
#define ALT_DIR 19
#define ALT_EN 26

// Shared Sleep
#define DRIVER_SLEEP 27

// Limit Switches
#define AZ_LIMIT 16
#define ALT_LIMIT 17

//Motors

AccelStepper azMotor(AccelStepper::DRIVER, AZ_STEP, AZ_DIR);
AccelStepper altMotor(AccelStepper::DRIVER, ALT_STEP, ALT_DIR);

//Mechanics

const float AZ_GEAR_RATIO = 3.0;
const float ALT_GEAR_RATIO = 6.0;

const int MOTOR_STEPS = 200;
const int MICROSTEPS = 16;

const float STEPS_PER_DEG_AZ =
(MOTOR_STEPS * MICROSTEPS * AZ_GEAR_RATIO) / 360.0;

const float STEPS_PER_DEG_ALT =
(MOTOR_STEPS * MICROSTEPS * ALT_GEAR_RATIO) / 360.0;

// Scan spacing
const float SCAN_STEP = 1.0;

// Travel limits software; 
const float MAX_AZ = 360.0;
const float MAX_ALT = 90.0;

// Normal run speed
const float RUN_MAX_SPEED = 1200;
const float RUN_ACCEL = 600;

// Homing speeds
const float HOMING_SEEK_SPEED = 400;   // fast search toward switch
const float HOMING_APPROACH_SPEED = 80; // slow, repeatable re-approach
const float HOMING_ACCEL = 400;
const long HOMING_SEEK_DISTANCE = 1000000; 
const unsigned long HOMING_TIMEOUT_MS = 20000; 

//Coordinate 

struct Coordinate
{
    float azimuth;
    float altitude;
};

Coordinate startPoint;
Coordinate endPoint;

//States

enum SystemState
{
    HOMING,
    IDLE,
    MOVING,
    SCANNING,
    COMPLETE,
    FAULT 
};

SystemState state = HOMING;

bool scanRequested = false;

//Function 

void setupWiFi();
void setupWebServer();
void setupMotors();

void homeMount();
bool homeAxis(AccelStepper&, uint8_t); 

void pointTo(float az, float alt);
void executeScan();

void handleRoot();
void handleSubmit();
void handleStatus(); 

String webpage();

//setup 

void setup()
{
    Serial.begin(115200);

    setupMotors();
    setupWiFi();
    setupWebServer();

    Serial.println();
    Serial.println("TeleRad Starting...");
}
void setupMotors()
{
    pinMode(AZ_EN, OUTPUT);
    pinMode(ALT_EN, OUTPUT);
    pinMode(DRIVER_SLEEP, OUTPUT);

    pinMode(AZ_LIMIT, INPUT_PULLUP);
    pinMode(ALT_LIMIT, INPUT_PULLUP);

    digitalWrite(DRIVER_SLEEP, HIGH);

    digitalWrite(AZ_EN, LOW);
    digitalWrite(ALT_EN, LOW);

    azMotor.setMaxSpeed(RUN_MAX_SPEED);
    azMotor.setAcceleration(RUN_ACCEL);

    altMotor.setMaxSpeed(RUN_MAX_SPEED);
    altMotor.setAcceleration(RUN_ACCEL);
}

void setupWiFi()
{
    WiFi.softAP(ssid, password);

    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
}

void setupWebServer()
{
    server.on("/", handleRoot);
    server.on("/submit", handleSubmit);
    server.on("/status", handleStatus); 

    server.begin();

    Serial.println("Web Server Started");
}

void handleRoot()
{
    server.send(200, "text/html", webpage());
}

void handleSubmit()
{
    if (state != IDLE)
    {
        server.send(409, "application/json",
            "{\"status\":\"busy\"}");
        return;
    }

    startPoint.azimuth =
        server.arg("C1_Azi").toFloat();

    startPoint.altitude =
        server.arg("C1_Alt").toFloat();

    endPoint.azimuth =
        server.arg("C2_Azi").toFloat();

    endPoint.altitude =
        server.arg("C2_Alt").toFloat();

    scanRequested = true;

    server.send(
        200,
        "application/json",
        "{\"status\":\"accepted\"}"
    );

    Serial.println("Coordinates Received");
}

void handleStatus()
{
    String stateStr;
    switch (state)
    {
        case HOMING:    stateStr = "Homing";   break;
        case IDLE:      stateStr = "Idle";     break;
        case MOVING:    stateStr = "Moving";   break;
        case SCANNING:  stateStr = "Scanning"; break;
        case COMPLETE:  stateStr = "Complete"; break;
        case FAULT:     stateStr = "Fault";    break;
        default:        stateStr = "Unknown";  break;
    }

    String json = "{";
    json += "\"state\":\"" + stateStr + "\",";
    json += "\"azimuth\":" + String(getCurrentAzimuth(), 2) + ",";
    json += "\"altitude\":" + String(getCurrentAltitude(), 2);
    json += "}";

    server.send(200, "application/json", json);
}
// Utility Functions

long azDegreesToSteps(float degrees)
{
    return (long)(degrees * STEPS_PER_DEG_AZ);
}

long altDegreesToSteps(float degrees)
{
    return (long)(degrees * STEPS_PER_DEG_ALT);
}

// Home One Axis

bool homeAxis(AccelStepper &motor, uint8_t limitPin)
{
    Serial.println("Searching for limit switch...");

    motor.setAcceleration(HOMING_ACCEL);
    motor.setMaxSpeed(HOMING_SEEK_SPEED);

    // ---- Fast search phase ----
    motor.moveTo(-HOMING_SEEK_DISTANCE);
    unsigned long startTime = millis();

    while (digitalRead(limitPin) == HIGH)
    {
        motor.run();
        server.handleClient();

        if (millis() - startTime > HOMING_TIMEOUT_MS)
        {
            motor.stop();
            while (motor.isRunning()) motor.run();
            Serial.println("HOMING TIMEOUT - limit switch not found (search phase)");
            return false;
        }
    }

    motor.stop();
    while (motor.isRunning())
    {
        motor.run();
        server.handleClient();
    }

    delay(100);

    // Move away
    motor.move(100);
    while (motor.distanceToGo() != 0)
    {
        motor.run();
        server.handleClient();
    }

    delay(100);

    // Slow re-approach 
    motor.setMaxSpeed(HOMING_APPROACH_SPEED);
    motor.moveTo(-HOMING_SEEK_DISTANCE);
    startTime = millis();

    while (digitalRead(limitPin) == HIGH)
    {
        motor.run();
        server.handleClient();

        if (millis() - startTime > HOMING_TIMEOUT_MS)
        {
            motor.stop();
            while (motor.isRunning()) motor.run();
            Serial.println("HOMING TIMEOUT - limit switch not found (re-approach phase)");
            return false;
        }
    }

    motor.stop();
    while (motor.isRunning())
    {
        motor.run();
        server.handleClient();
    }

    motor.setCurrentPosition(0);

    Serial.println("Axis Homed");
    return true;
}

// Home Mount

void homeMount()
{
    Serial.println();
    Serial.println(" HOMING ");

    Serial.println("Azimuth...");
    if (!homeAxis(azMotor, AZ_LIMIT))
    {
        state = FAULT;
        return;
    }

    Serial.println("Altitude...");
    if (!homeAxis(altMotor, ALT_LIMIT))
    {
        state = FAULT;
        return;
    }

    // FIX: restore normal run speed/accel now that homing (which uses its
    // own slower speeds) is done. This was previously never restored, so
    // every scan ran at homing speed (400) instead of the configured 1200.
    azMotor.setMaxSpeed(RUN_MAX_SPEED);
    azMotor.setAcceleration(RUN_ACCEL);
    altMotor.setMaxSpeed(RUN_MAX_SPEED);
    altMotor.setAcceleration(RUN_ACCEL);

    Serial.println("Homing Complete");
    Serial.println();
}

//==================================================
// Move to Coordinate
//==================================================

void pointTo(float azimuth, float altitude)
{
    azimuth = constrain(azimuth, 0.0f, MAX_AZ);
    altitude = constrain(altitude, 0.0f, MAX_ALT);

    long azTarget = azDegreesToSteps(azimuth);
    long altTarget = altDegreesToSteps(altitude);

    azMotor.moveTo(azTarget);
    altMotor.moveTo(altTarget);

    Serial.print("Moving to ");
    Serial.print(azimuth);
    Serial.print("° , ");
    Serial.print(altitude);
    Serial.println("°");

    while (azMotor.distanceToGo() != 0 ||
           altMotor.distanceToGo() != 0)
    {
        azMotor.run();
        altMotor.run();

        // FIX: real safety check. The previous version gated this on
        // currentPosition() < 0, which can't happen since targets are
        // constrained to >= 0 above -- so it never actually fired.
        // This checks the physical switch state directly instead.
        if (digitalRead(AZ_LIMIT) == LOW)
        {
            azMotor.stop();
        }

        if (digitalRead(ALT_LIMIT) == LOW)
        {
            altMotor.stop();
        }

        server.handleClient();
    }

    Serial.println("Target Reached");
}

//==================================================
// Current Position Helpers
//==================================================

float getCurrentAzimuth()
{
    return azMotor.currentPosition() / STEPS_PER_DEG_AZ;
}

float getCurrentAltitude()
{
    return altMotor.currentPosition() / STEPS_PER_DEG_ALT;
}
//==================================================
// Snake Scan
//==================================================

void executeScan()
{
    Serial.println();
    Serial.println("========== STARTING SCAN ==========");

    float startAz = min(startPoint.azimuth, endPoint.azimuth);
    float endAz = max(startPoint.azimuth, endPoint.azimuth);

    float startAlt = min(startPoint.altitude, endPoint.altitude);
    float endAlt = max(startPoint.altitude, endPoint.altitude);

    bool upwards = true;

    for (float az = startAz; az <= endAz; az += SCAN_STEP)
    {
        if (upwards)
        {
            pointTo(az, startAlt);
            pointTo(az, endAlt);
        }
        else
        {
            pointTo(az, endAlt);
            pointTo(az, startAlt);
        }

        upwards = !upwards;

        // NOTE: no sampling/logging call happens here yet. If you need a
        // sensor reading captured at each stop, that goes in this loop --
        // let me know what you're sampling and I'll wire it in.
    }

    Serial.println("========== SCAN COMPLETE ==========");
}

//==================================================
// Main Loop
//==================================================

void loop()
{
    server.handleClient();

    switch (state)
    {
        case HOMING:
            homeMount();
            if (state == HOMING) // only advance if homeMount didn't fault
            {
                state = IDLE;
            }
            break;

        case IDLE:
            if (scanRequested)
            {
                scanRequested = false;
                state = SCANNING;
            }
            break;

        case SCANNING:
            executeScan();
            state = COMPLETE;
            break;

        case COMPLETE:
            Serial.println("Scan Finished");
            state = IDLE;
            break;

        case FAULT:
            // FIX: stay here until power-cycled/reset rather than silently
            // looping back to IDLE and accepting further scan requests
            // with an un-homed or partially-homed mount.
            break;

        default:
            break;
    }

    azMotor.run();
    altMotor.run();
}

//==================================================
// HTML Page
//==================================================

String webpage()
{
return R"rawliteral(
<!DOCTYPE html>
<html>

<head>

<meta charset="UTF-8">

<title>TeleRad Controller</title>

<style>

body{

font-family:Arial;

background:#101010;

color:white;

text-align:center;

margin-top:40px;

}

input{

width:100px;

padding:8px;

margin:5px;

font-size:16px;

}

button{

padding:10px 25px;

font-size:18px;

cursor:pointer;

}

.card{

display:inline-block;

padding:25px;

border-radius:12px;

background:#202020;

}

#status{

font-family:monospace;

margin-top:15px;

}

</style>

</head>

<body>

<div class="card">

<h1>TeleRad</h1>

<h2>Start Coordinate</h2>

Azimuth<br>
<input id="C1_Azi" type="number"><br>

Altitude<br>
<input id="C1_Alt" type="number"><br><br>

<h2>End Coordinate</h2>

Azimuth<br>
<input id="C2_Azi" type="number"><br>

Altitude<br>
<input id="C2_Alt" type="number"><br><br>

<button onclick="sendData()">Start Scan</button>

<p id="status">Loading status...</p>

</div>

<script>

function sendData()
{

let C1_Azi=document.getElementById("C1_Azi").value;
let C1_Alt=document.getElementById("C1_Alt").value;
let C2_Azi=document.getElementById("C2_Azi").value;
let C2_Alt=document.getElementById("C2_Alt").value;

// FIX: basic validation -- empty/non-numeric fields previously sent
// silently as 0 with no feedback.
if ([C1_Azi,C1_Alt,C2_Azi,C2_Alt].some(v => v === "" || isNaN(v)))
{
document.getElementById("status").innerHTML = "Please fill in all four fields with numbers.";
return;
}

fetch(`/submit?C1_Azi=${C1_Azi}&C1_Alt=${C1_Alt}&C2_Azi=${C2_Azi}&C2_Alt=${C2_Alt}`)
.then(response=>response.json())
.then(data=>{
if (data.status === "busy")
{
document.getElementById("status").innerHTML = "Mount is busy -- try again once idle.";
}
});

}

// FIX: poll real status/position instead of a static "Scan Started" message
function pollStatus()
{
fetch('/status')
.then(r=>r.json())
.then(d=>{
document.getElementById("status").innerHTML =
`State: ${d.state} &nbsp; Az: ${d.azimuth}&deg; &nbsp; Alt: ${d.altitude}&deg;`;
})
.catch(()=>{});
}

setInterval(pollStatus, 1000);
pollStatus();

</script>

</body>

</html>
)rawliteral";
}