#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "RobotArm";
const char* password = "khalil123";

WebServer server(80);

const int basePin = 13;
const int shoulderPin = 12;
const int elbowPin = 14;
const int gripperPin = 27;
const int irPin = 26;

Servo sBase;
Servo sShoulder;
Servo sElbow;
Servo sGripper;

int baseMin = 0, baseMax = 180;
int shoulderMin = 20, shoulderMax = 160;
int elbowMin = 20, elbowMax = 160;
int gripOpen = 30, gripClosed = 110;

int curBase = 90, targetBase = 90;
int curShoulder = 90, targetShoulder = 90;
int curElbow = 90, targetElbow = 90;
int curGrip = 30, targetGrip = 30;

bool autoMode = false;
unsigned long lastStep = 0;
int stepSpeed = 15;

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Robot Arm Control</title>
  <style>
    body { font-family: sans-serif; background: #181818; color: #f0f0f0; text-align: center; padding: 15px; }
    .box { background: #242424; padding: 20px; border-radius: 10px; max-width: 380px; margin: auto; }
    .ctrl { margin: 16px 0; text-align: left; }
    input[type=range] { width: 100%; }
    button { background: #4caf50; color: white; border: none; padding: 12px; font-size: 15px; border-radius: 6px; width: 100%; margin-top: 10px; cursor: pointer; }
    .auto-on { background: #e65100; }
    .home-btn { background: #1976d2; }
  </style>
</head>
<body>
  <div class="box">
    <h3>Arm Controller</h3>
    <button id="mBtn" onclick="switchMode()">Mode: Manual</button>

    <div class="ctrl">
      <label>Base: <span id="v0">90</span></label>
      <input type="range" min="0" max="180" value="90" oninput="upd(0, this.value)">
    </div>
    <div class="ctrl">
      <label>Shoulder: <span id="v1">90</span></label>
      <input type="range" min="20" max="160" value="90" oninput="upd(1, this.value)">
    </div>
    <div class="ctrl">
      <label>Elbow: <span id="v2">90</span></label>
      <input type="range" min="20" max="160" value="90" oninput="upd(2, this.value)">
    </div>
    <div class="ctrl">
      <label>Gripper: <span id="v3">30</span></label>
      <input type="range" min="30" max="110" value="30" oninput="upd(3, this.value)">
    </div>
    <button class="home-btn" onclick="resetHome()">Reset Home</button>
  </div>

  <script>
    let autoState = false;
    function upd(id, val) {
      document.getElementById('v' + id).innerText = val;
      fetch('/set?s=' + id + '&v=' + val);
    }
    function switchMode() {
      autoState = !autoState;
      let b = document.getElementById('mBtn');
      b.innerText = autoState ? "Mode: Auto (IR)" : "Mode: Manual";
      b.className = autoState ? "auto-on" : "";
      fetch('/mode?a=' + (autoState ? 1 : 0));
    }
    function resetHome() {
      fetch('/home');
    }
  </script>
</body>
</html>
)rawliteral";

void stepServos() {
  if (millis() - lastStep < stepSpeed) return;
  lastStep = millis();

  if (curBase < targetBase) curBase++;
  else if (curBase > targetBase) curBase--;

  if (curShoulder < targetShoulder) curShoulder++;
  else if (curShoulder > targetShoulder) curShoulder--;

  if (curElbow < targetElbow) curElbow++;
  else if (curElbow > targetElbow) curElbow--;

  if (curGrip < targetGrip) curGrip++;
  else if (curGrip > targetGrip) curGrip--;

  sBase.write(curBase);
  sShoulder.write(curShoulder);
  sElbow.write(curElbow);
  sGripper.write(curGrip);
}

void moveArm(int b, int s, int e, int g) {
  targetBase = constrain(b, baseMin, baseMax);
  targetShoulder = constrain(s, shoulderMin, shoulderMax);
  targetElbow = constrain(e, elbowMin, elbowMax);
  targetGrip = constrain(g, gripOpen, gripClosed);

  while (curBase != targetBase || curShoulder != targetShoulder || curElbow != targetElbow || curGrip != targetGrip) {
    stepServos();
    server.handleClient();
    delay(1);
  }
}

void pickAndPlace() {
  moveArm(45, 90, 90, gripOpen);
  delay(200);
  moveArm(45, 130, 120, gripOpen);
  delay(300);
  moveArm(45, 130, 120, gripClosed);
  delay(400);
  moveArm(45, 80, 90, gripClosed);
  delay(200);
  moveArm(140, 80, 90, gripClosed);
  delay(200);
  moveArm(140, 125, 115, gripClosed);
  delay(300);
  moveArm(140, 125, 115, gripOpen);
  delay(300);
  moveArm(90, 90, 90, gripOpen);
}

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleSet() {
  if (autoMode) {
    server.send(200, "text/plain", "busy");
    return;
  }
  if (server.hasArg("s") && server.hasArg("v")) {
    int id = server.arg("s").toInt();
    int val = server.arg("v").toInt();
    if (id == 0) targetBase = constrain(val, baseMin, baseMax);
    if (id == 1) targetShoulder = constrain(val, shoulderMin, shoulderMax);
    if (id == 2) targetElbow = constrain(val, elbowMin, elbowMax);
    if (id == 3) targetGrip = constrain(val, gripOpen, gripClosed);
  }
  server.send(200, "text/plain", "ok");
}

void handleMode() {
  if (server.hasArg("a")) {
    autoMode = server.arg("a").toInt() == 1;
  }
  server.send(200, "text/plain", "ok");
}

void handleHome() {
  autoMode = false;
  targetBase = 90;
  targetShoulder = 90;
  targetElbow = 90;
  targetGrip = gripOpen;
  server.send(200, "text/plain", "ok");
}

void setup() {
  Serial.begin(115200);
  pinMode(irPin, INPUT);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  sBase.setPeriodHertz(50);
  sShoulder.setPeriodHertz(50);
  sElbow.setPeriodHertz(50);
  sGripper.setPeriodHertz(50);

  sBase.attach(basePin, 500, 2400);
  sShoulder.attach(shoulderPin, 500, 2400);
  sElbow.attach(elbowPin, 500, 2400);
  sGripper.attach(gripperPin, 500, 2400);

  sBase.write(curBase);
  sShoulder.write(curShoulder);
  sElbow.write(curElbow);
  sGripper.write(curGrip);

  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/mode", handleMode);
  server.on("/home", handleHome);
  server.begin();
}

void loop() {
  server.handleClient();

  if (autoMode) {
    if (digitalRead(irPin) == LOW) {
      delay(300);
      if (digitalRead(irPin) == LOW) {
        pickAndPlace();
      }
    }
  } else {
    stepServos();
  }
}