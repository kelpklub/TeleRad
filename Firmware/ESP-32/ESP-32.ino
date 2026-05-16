#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <AccelStepper.h>
#include <MPU9250_asukiaaa.h>

const char* ssid = "TeleRad-ESP32";
const char* password = "rootaboot";

WebServer server(80);
//init mpu9250
MPU9250_asukiaaa mpu;

//motor controlling the azimuth rotation
#define AZ_STEP 23
#define AZ_DIR 22
AccelStepper az_motor(AccelStepper::DRIVER, AZ_STEP, AZ_DIR);

// moto rrotating the altitude morto
#define ALT_STEP 21
#define ALT_DIR 19
AccelStepper alt_motor(AccelStepper::DRIVER, ALT_STEP, ALT_DIR);

const float azimuth_gearratio = 3.0;
const float altitude_gearratio = 6.0;
const int motorsteps = 200;
//for 16th micro steps, wire m1,m0=low;m2=high
const int microsteps = 16;
//finding the steps per degree for both altitude and azimuth

const float steps_per_degree_az = (motorsteps * microsteps * azimuth_gearratio) / 360.0;
const float steps_per_degree_alt = (motorsteps * microsteps * altitude_gearratio) / 360.0;

struct coordinate {
  float x;  //azimuth
  float y;  // attitude
};

coordinate target1;
coordinate target2;
coordinate currentposition;
//checks
bool coordinatesreceived = false;
bool started = false;

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.begin();
  Wire.begin();  //i2c Protocol
  mpu.setWire(&Wire);
  mpu.beginAccel();
  mpu.beginGyro();
  mpu.beginMag();
  //setup motor
  az_motor.setMaxSpeed(1000);
  az_motor.setAcceleration(500);
  alt_motor.setMaxSpeed(1000);
  alt_motor.setAcceleration(500);
  //set x,y of current pos
  currentposition = get_current_position();
  Serial.println("READY");
}

void loop() {
  server.handleClient();
  az_motor.run();
  alt_motor.run();
  if (coordinatesreceived && !started) {
    started = true;
    //move to targer1
    point_to(target1);
    //start scan
    scanArea(target1, target2);
  }
}

void handleRoot() {
  server.send(200, "text/html", webpage());
}

void handleSubmit() {
  target1.x = server.arg("C1_Azi").toFloat();
  target1.y = server.arg("C1_Alt").toFloat();
  target2.x = server.arg("C2_Azi").toFloat();
  target2.y = server.arg("C2_Alt").toFloat();
  coordinatesreceived = true;
  server.send(200, "text/plain", "OK");
}

coordinate get_current_position() {
  //the get current pos  func will be done after i get the module
  return pos;
}

void point_to(coordinate target) {
  Serial.println("MOVING TO TARGET");
  float deltaAz = target.x - currentposition.x;
  float deltaAlt = target.y - currentposition.y;
  long azSteps = deltaAz * steps_per_degree_az;
  long altSteps = deltaAlt * steps_per_degree_alt;

  az_motor.move(azSteps);
  alt_motor.move(altSteps);

  while (az_motor.distanceToGo() != 0) { az_motor.run(); }
  while (alt_motor.distanceToGo() != 0) { alt_motor.run(); }
  Serial.println("MOVED TO TARGEt ");
}

void scanArea(coordinate start, coordinate end) {
  float Step = 1.0;
  //snake scan SSsssssssssssssssssssssssssssssssssssssssssssssssssss
  coordinate target;
  target.x = start.x - Step;
  for (coordinate Currentpos = start; Currentpos.x <= end.x && Currentpos.y <= end.y;;)
  //moves in y till end.y ,then moves in x by 1 Step ,then moves to start.y
  {
    target.x = target.x + Step;
    tagret.y = end.y;
    point_to(target);
    target.x = target.x + Step;
    point_to(target);
    target.y = start.y;
    point_to(target);
  }

  Serial.println("SCAN COMPLETE");
}

String webpage() {
  return R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <style>
        html,
        body,
        p {}
        
        body {
            overflow: hidden;
            background: rgba(255, 0, 0, 0.67);
            border-radius: 16px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 1);
            backdrop-filter: blur(5px);
            -webkit-backdrop-filter: blur(5px);
            border: 1px solid rgba(255, 255, 255, 0.3);
            text-align: center;
            display: inline-block;
            margin-top: 5vw;
            margin-bottom: 2vw;
            padding: 1vw;
        }
        
        h1 {
            color: #07beb8;
            font-family: Arial, Helvetica, sans-serif;
            font-weight: bold;
        }
    </style>
    <title>ESP32 Web Server</title>
</head>

<body>
    <h1>ESP32 Web Server</h1>
    <h2>coordinate 1</h2>
    <label>Altitude</label>
    <input type="text" id="C1_Alt">
    <br><br>
    <label>Azimuth</label>
    <input type="text" id="C1_Azi">
    <h2>coordinate 2</h2>
    <label>Altitude</label>
    <input type="text" id="C2_Alt">
    <br><br>
    <label>Azimuth</label>
    <input type="text" id="C2_Azi">
    <br><br><br>
    <button onclick="sendData()">
    Submit
  </button>

    <script>
        function sendData() {
            let C1_Alt = document.getElementById("C1_Alt").value;
            let C1_Azi = document.getElementById("C1_Azi").value;
            let C2_Alt = document.getElementById("C2_Alt").value;
            let C2_Azi = document.getElementById("C2_Azi").value;
            fetch(`/submit?C1_Alt=${C1_Alt}&C1_Azi=${C1_Azi}&C2_Alt=${C2_Alt}&C2_Azi=${C2_Azi}`);
        }
    </script>
</body>

</html>
)rawliteral";
}