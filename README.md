# esp32-autonomous-robot-arm

4-DOF robotic arm featuring an ESP32 Access Point web controller for manual joint calibration and an IR-triggered autonomous pick-and-place routine.

## 🛠️ Hardware Architecture

* **Microcontroller:** ESP32 DevKit V1
* **Actuators:** 4x Servo motors (Base, Shoulder, Elbow, Gripper)
* **Sensor:** Digital IR obstacle sensor for autonomous object detection

## 💻 Software Stack

* **Firmware & Control Logic:** C++ / Arduino (`robot_arm.ino`)
* **Web Dashboard:** Embedded HTML/CSS/JS hosted locally in ESP32 AP Mode (`192.168.4.1`)
* **Motion Control:** Non-blocking stepped angle interpolation with software joint limits

