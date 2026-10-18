```markdown
# NeuroSpine — Smart Posture Coach

This project uses an ESP32 microcontroller programmed in C++ (Arduino IDE) to process motion data from an MPU6050 sensor and stream it directly to a serverless HTML local dashboard.

## Hardware Setup

Here is the physical setup for the project:

![Neurospine Setup](setup.jpg)

* **Microcontroller:** ESP32 DevKit V1 (30-pin)[cite: 1]
* **Motion Sensor:** MPU6050 connected via I2C (SDA $\rightarrow$ D21, SCL $\rightarrow$ D22)[cite: 1]
* **Haptic Feedback:** 1027 coin vibration motor controlled via Pin D19 using a transistor[cite: 1]

## How to Run
1. Open your Arduino IDE, load the firmware sketch, and upload it to your ESP32 board.
2. Disconnect your computer from your regular Wi-Fi and connect to the ESP32 local access point network: **`IoT_Aura_Network`** (Password: `password123`).
3. Open `dashboard.html` in any web browser, type `192.168.4.1` into the IP address field, and click **Connect** to view live posture metrics, deviation gauges, and history charts.

```