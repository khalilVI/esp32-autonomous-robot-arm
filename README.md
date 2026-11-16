# ESP32 4-DOF Autonomous & Web-Controlled Robotic Arm

A 4-Degree-of-Freedom (4-DOF) robotic arm powered by an ESP32 microcontroller. It operates in **ESP32 Access Point (AP) mode**, hosting a lightweight web interface for real-time manual servo control alongside an IR-triggered autonomous pick-and-place routine.

## Features
- **Local Web Control Dashboard:** Connects directly to the ESP32's Wi-Fi hotspot (`192.168.4.1`) with responsive sliders for individual joint control and calibration.
- **Autonomous Pick-and-Place Mode:** Uses an infrared (IR) obstacle sensor to detect an object, execute a multi-step pickup sequence, transfer it to a drop-off zone, and return to its home position.
- **Non-Blocking Smooth Motion:** Stepped angle interpolation prevents sudden mechanical jerks, reducing stress on gears and joints.
- **Hardware Safety Limits:** Software-enforced angle constraints prevent servo binding and mechanical collisions.

## Hardware Components
- **Microcontroller:** ESP32 DevKit V1
- **Actuators:** 4x Servo Motors (SG90 / MG996R) for Base, Shoulder, Elbow, and Gripper
- **Sensor:** Digital IR Obstacle Avoidance Sensor
- **Power Supply:** Dedicated external 5V–6V DC power supply for the servos (sharing a common ground with the ESP32)

## Pinout Configuration

| Component | ESP32 GPIO Pin | Notes |
| :--- | :--- | :--- |
| **Base Servo** | `GPIO 13` | PWM Signal (50Hz) |
| **Shoulder Servo** | `GPIO 12` | PWM Signal (50Hz) |
| **Elbow Servo** | `GPIO 14` | PWM Signal (50Hz) |
| **Gripper Servo** | `GPIO 27` | PWM Signal (50Hz) |
| **IR Sensor** | `GPIO 26` | Digital Output (`LOW` = Object Detected) |

## Setup & Usage
1. **Install Dependencies:** Open the Arduino IDE and install the **ESP32Servo** library via the Library Manager.
2. **Flash Firmware:** Open `robot_arm.ino`, select your ESP32 board, and upload the code.
3. **Connect to UI:** 
   - Connect your phone or PC to the Wi-Fi network **`RobotArm_AP`** (Password: **`password123`**).
   - Open a browser and navigate to **`http://192.168.4.1`**.
4. **Operate:** Use **Manual Mode** to test and calibrate joint angles, or toggle **Auto (IR) Mode** to trigger the automated pick-and-place sequence whenever an object is placed in front of the sensor.
