#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <WebServer.h>

Adafruit_MPU6050 mpu;
WebServer server(80);

// Hardware Pin Configurations (Breadboard Setup)
const int motorPin = 19;   // Transistor pin controlling vibration motor
const int blueLED = 2;     // Built-in Blue LED

// Wireless Network Access Point Settings
const char* ssid = "IoT_Aura_Network";
const char* password = "password123";

float straightAngle = 0;
float currentDeviation = 0.0;

// This API endpoint feeds your custom dashboard.html structure
void handleData() {
  // CRITICAL CORS HEADERS: Prevents the web browser from blocking the incoming data
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET");
  
  // Sends the current deviation to the dashboard chart
  server.send(200, "text/plain", String(currentDeviation, 2));
}

void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.send(204);
}

void setup(void) {
  Serial.begin(115200); 
  
  // Force Motor Pin 19 to turn OFF instantly on bootup for safety
  digitalWrite(motorPin, LOW); 
  pinMode(motorPin, OUTPUT);
  pinMode(blueLED, OUTPUT);

  // Initialize the MPU6050
  if (!mpu.begin()) {
    Serial.println("❌ ERROR: MPU6050 not found! Check your D21 (SDA) and D22 (SCL) wires.");
    while (1) { 
      digitalWrite(blueLED, HIGH); delay(100);
      digitalWrite(blueLED, LOW);  delay(100);
    }
  }

  // --- STARTUP HARDWARE TEST ---
  digitalWrite(motorPin, HIGH); 
  digitalWrite(blueLED, HIGH);
  delay(800); 
  digitalWrite(motorPin, LOW);
  digitalWrite(blueLED, LOW);

  // --- CALIBRATION ROUTINE ---
  Serial.println("\n=============================================");
  Serial.println("         NEURO SPINE: INITIALIZATION         ");
  Serial.println("=============================================");
  Serial.println("STEP 1: Sit up completely straight and hold the sensor upright.");
  Serial.println("Calibrating baseline position in 3 seconds...");
  delay(3000); 
  
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  straightAngle = a.acceleration.y; 
  
  Serial.print("✅ Calibration Complete! Baseline Set to: ");
  Serial.print(straightAngle);
  Serial.println(" m/s²");
  Serial.println("=============================================\n");

  // --- LOCAL NETWORK INITIALIZATION ---
  WiFi.softAP(ssid, password);
  Serial.print("📡 Wi-Fi Hotspot Broadcaster Started: ");
  Serial.println(ssid);
  Serial.print("🌐 Dashboard Target IP: http://");
  Serial.println(WiFi.softAPIP()); // This defaults to 192.168.4.1

  // Bind Web server connection hooks
  server.on("/data", HTTP_GET, handleData);
  server.on("/data", HTTP_OPTIONS, handleOptions);
  
  server.begin();
  Serial.println("🚀 Web API Server Live. Awaiting dashboard pairing...\n");
}

void loop() {
  // Constantly listen for browser endpoint calls from dashboard.html
  server.handleClient(); 

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float currentPos = a.acceleration.y;
  
  // Calculate dynamic change from your calibrated posture baseline
  currentDeviation = abs(currentPos - straightAngle);

  // Print raw debugging logs directly to the Arduino IDE Serial Monitor
  Serial.print("Target: "); Serial.print(straightAngle, 2);
  Serial.print(" | Current: "); Serial.print(currentPos, 2);
  Serial.print(" | Deviation: "); Serial.print(currentDeviation, 2);

  // Posture Evaluation Check
  if (currentDeviation > 1.4) { 
    Serial.println(" ❌ [ !!! SLOUCHING DETECTED !!! - Sit back straight ]");
    digitalWrite(motorPin, HIGH);  // Fire haptic vibration loop
    digitalWrite(blueLED, HIGH);   // Visual verification lamp ON
  } else {
    Serial.println("  Correct Position");
    digitalWrite(motorPin, LOW);   // Kill haptic feedback loop
    digitalWrite(blueLED, LOW);    // Status confirmation light OFF
  }

  delay(100); // 10Hz execution rate for smooth browser integration
}