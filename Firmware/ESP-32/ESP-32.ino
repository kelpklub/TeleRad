
//AIM-1. Make a webserver that i can access from any device and send two coordinates C1(x,y):C1 is the lower-left vertex of a rectangle
// and C2(x,y):C2 is the upper-right vertex of a rectangle. x is its azimuth angle and y is its altitude angle.
//2.the system should know where it is currently pointing in alt-azi and move to C1. From C1 the system moves to C2 in a scanning pattern.
#include <WiFi.h>
#include <WebServer.h>

// the wifi name and password the esp32 will create
const char* ssid = "TeleRad-ESP32";
const char* password = "rootaboot";

// Creates server on Port 80(Default port)
WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Create a ESP-32 wifi 
  WiFi.softAP(ssid, password);

  Serial.println("WiFi Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Website route
  server.on("/", handleRoot);

  // begin server
  server.begin();

  Serial.println("Server Started");
}

void loop() {
  server.handleClient();
}

// Calls webpage,sends code for success->200
void handleRoot() {
  server.send(200, "text/html", webpage());
}

// HTML 
String webpage() {

  String html = R"rawliteral(
<!DOCTYPE html>
<html>

<head>

  <title>ESP32 Web Server</title>

</head>
<body>

  <h1>ESP32 Web Server</h1>

  <p>Hello World</p>

</body>
</html>
)rawliteral";

  return html;
}