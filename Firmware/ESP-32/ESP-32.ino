#include <WiFi.h>
#include <WebServer.h>
#include <AccelStepper.h>

//====================== WiFi ======================

const char* ssid = "TeleRad-ESP32";
const char* password = "rootaboot";

WebServer server(80);

//====================== Pins ======================

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

//====================== Motors ======================

AccelStepper azMotor(AccelStepper::DRIVER, AZ_STEP, AZ_DIR);
AccelStepper altMotor(AccelStepper::DRIVER, ALT_STEP, ALT_DIR);

//====================== Mechanics ======================

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

// Travel limits
const float MAX_AZ = 360.0;
const float MAX_ALT = 90.0;

//====================== Coordinate ======================

struct Coordinate
{
    float azimuth;
    float altitude;
};

Coordinate startPoint;
Coordinate endPoint;

//====================== States ======================

enum SystemState
{
    HOMING,
    IDLE,
    MOVING,
    SCANNING,
    COMPLETE
};

SystemState state = HOMING;

bool scanRequested = false;

//====================== Function Prototypes ======================

void setupWiFi();
void setupWebServer();
void setupMotors();

void homeMount();
void homeAxis(AccelStepper&, uint8_t);

void pointTo(float az, float alt);
void executeScan();

void handleRoot();
void handleSubmit();

String webpage();

//====================== Setup ======================

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

    azMotor.setMaxSpeed(1200);
    azMotor.setAcceleration(600);

    altMotor.setMaxSpeed(1200);
    altMotor.setAcceleration(600);
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

    server.begin();

    Serial.println("Web Server Started");
}

void handleRoot()
{
    server.send(200, "text/html", webpage());
}

void handleSubmit()
{
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
//==================================================
// Utility Functions
//==================================================

long azDegreesToSteps(float degrees)
{
    return (long)(degrees * STEPS_PER_DEG_AZ);
}

long altDegreesToSteps(float degrees)
{
    return (long)(degrees * STEPS_PER_DEG_ALT);
}

//==================================================
// Home One Axis
//==================================================

void homeAxis(AccelStepper &motor, uint8_t limitPin)
{
    Serial.println("Searching for limit switch...");

    motor.setMaxSpeed(400);
    motor.setSpeed(-200);

    // Move until switch is pressed
    while (digitalRead(limitPin) == HIGH)
    {
        motor.runSpeed();
    }

    delay(100);

    // Move away
    motor.move(100);

    while (motor.distanceToGo() != 0)
    {
        motor.run();
    }

    delay(100);

    // Slowly approach again
    motor.setSpeed(-60);

    while (digitalRead(limitPin) == HIGH)
    {
        motor.runSpeed();
    }

    motor.stop();

    while (motor.isRunning())
    {
        motor.run();
    }

    motor.setCurrentPosition(0);

    Serial.println("Axis Homed");
}

//==================================================
// Home Mount
//==================================================

void homeMount()
{
    Serial.println();
    Serial.println("========== HOMING ==========");

    Serial.println("Azimuth...");
    homeAxis(azMotor, AZ_LIMIT);

    Serial.println("Altitude...");
    homeAxis(altMotor, ALT_LIMIT);

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

        // Safety: stop if either limit switch is hit
        if (digitalRead(AZ_LIMIT) == LOW &&
            azMotor.currentPosition() < 0)
        {
            azMotor.stop();
        }

        if (digitalRead(ALT_LIMIT) == LOW &&
            altMotor.currentPosition() < 0)
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
            state = IDLE;
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

<p id="status"></p>

</div>

<script>

function sendData()
{

let C1_Azi=document.getElementById("C1_Azi").value;
let C1_Alt=document.getElementById("C1_Alt").value;
let C2_Azi=document.getElementById("C2_Azi").value;
let C2_Alt=document.getElementById("C2_Alt").value;

fetch(`/submit?C1_Azi=${C1_Azi}&C1_Alt=${C1_Alt}&C2_Azi=${C2_Azi}&C2_Alt=${C2_Alt}`)
.then(response=>response.json())
.then(data=>{
document.getElementById("status").innerHTML="Scan Started";
});

}

</script>

</body>

</html>
)rawliteral";
}