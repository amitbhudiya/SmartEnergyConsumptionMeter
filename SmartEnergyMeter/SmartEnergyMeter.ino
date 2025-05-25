
// Smart Energy Consumption Monitoring System
// Monitors current, voltage, and power using sensors, displays data on an OLED,
// sends data to Blynk for remote monitoring, and stores it in Firebase.

// Blynk template and authentication details
#define BLYNK_TEMPLATE_ID "TMPL6neA4lOpu"
#define BLYNK_TEMPLATE_NAME "SmartEnergyConsumption"
#define BLYNK_DEVICE_NAME "SmartEnergyConsumption"

// Required libraries
#include <Arduino.h>              // Core Arduino library
#include <Wire.h>                 // I2C communication for OLED
#include <Adafruit_GFX.h>         // Graphics library for OLED
#include <Adafruit_SSD1306.h>     // OLED display driver
#include <WiFiUdp.h>              // UDP for NTP time sync
#include <WiFi.h>                 // WiFi connectivity
#include <NTPClient.h>            // Network Time Protocol client
#include <cmath>                  // Math functions for RMS calculations
#include <BlynkSimpleEsp32.h>     // Blynk library for IoT integration
#include <FirebaseESP32.h>        // Firebase library for cloud storage
#include <iostream>               // Standard I/O (for Serial)
#include <iomanip>                // Formatting for timestamps
#include <sstream>                // String stream for formatting
#include <ctime>                  // Time functions for timestamps

// OLED setup
#define SCREEN_WIDTH 128        // OLED display width in pixels
#define SCREEN_HEIGHT 64      // OLED display height in pixels
#define OLED_RESET -1           // Reset pin (-1 if not used)
#define SCREEN_ADDRESS 0x3D      // I2C address of the OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Constants
#define V_REF 3.3 // Reference voltage for ADC (3.3V for ESP32)
#define ADC_RESOLUTION 4095.0 // 12-bit ADC resolution

#define CURRENT_CALIBRATION 5.0 // Calibration factor for current sensor
#define VOLTAGE_DIVIDER_RATIO 5.56  // Voltage divider ratio for voltage sensor

#define FIREBASE_DATABASE_URL "smartenergyconsumption-3dc05-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_API_KEY      "AIzaSyDGW5CICbfSdJkM29ZHWjkIQDPjyDeplqk"
#define FIREBASE_DB_SECRET    "neYhwvFAEyrZGDXqqrmeaYRrV49Pc7EJmpnLwbaF" 

FirebaseData   fbdo;
FirebaseConfig fbConfig;
FirebaseAuth   fbAuth;
// Pins
const int currentSensorPin = A0;
const int voltagePin = A2;
const int relayPin = D1;
char auth[] = "GV0kpml3IxynIRgnujcOJmySpNediOB3";
bool notificationSent = false;
bool isInWarningRange = false;
unsigned long warningStartTime = 0;

// NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800);

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 OLED allocation failed");
    while (true);
  }

  // Display startup message on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Smart Energy Monitor");
  display.display();
  delay(2000);
  // Connect to WiFi
  WiFi.begin("IOT", "12345678");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); // Print dots until connected
  }
  Serial.println("\nWiFi connected.");

  timeClient.begin(); // Start NTP client

  Blynk.begin(auth, "IOT", "12345678");  // Blynk authentication

  // Initialize Firebase
  fbConfig.database_url = FIREBASE_DATABASE_URL;
  fbConfig.api_key      = FIREBASE_API_KEY;
  fbConfig.signer.tokens.legacy_token = FIREBASE_DB_SECRET;
  Firebase.begin(&fbConfig, nullptr);
  Firebase.reconnectWiFi(true);
}

void storeDataToFirebase(float current, float voltage, float power) {
  // Update time client
  timeClient.update();
  time_t rawTime = timeClient.getEpochTime();
  struct tm *timeInfo = localtime(&rawTime);

  // Format timestamp key as dd-MM-yyyy_HH:mm:ss
  char timestamp[25];
  strftime(timestamp, sizeof(timestamp), "%d-%m-%Y_%H:%M:%S", timeInfo);

  // Create JSON object
  FirebaseJson json;
  json.set("current", current);
  json.set("voltage", voltage);
  json.set("power", power);

  // Use timestamp as key
  String path = "/SmartEnergy/" + String(timestamp);

  if (!Firebase.setJSON(fbdo, path.c_str(), json)) {
    Serial.printf("Firebase set failed: %s\n", fbdo.errorReason().c_str());
  }
}

// Read and Calculate RMS current in Amps
float readCurrentRMS() {
  const int samples = 1000;
  float sum = 0;
  int count=0;
  for (int i = 0; i < samples; i++) {
    int raw = analogRead(currentSensorPin);
    if (raw != 0) {
      sum += raw;
      count+=1;
    }
    delayMicroseconds(100);  // ~10kHz sample rate
  }

  float mean = sum / samples;
  float rmsCurrent = mean / sqrt(2);
  rmsCurrent = rmsCurrent * (V_REF /ADC_RESOLUTION );
  return rmsCurrent * CURRENT_CALIBRATION;
}

// Voltage is direct-read with scaling (from divider)
float readVoltageRMS() {
  int voltageRawValue = analogRead(voltagePin); 
  float voltageValue = voltageRawValue * (V_REF / ADC_RESOLUTION) * VOLTAGE_DIVIDER_RATIO;
  return voltageValue;
}
// Handles Blynk virtual pin V4 to control relay
BLYNK_WRITE(V4) {
  int buttonState = param.asInt(); // Get the state of the button from Blynk app
  Serial.print("Blynk V4 = "); 
  Serial.println(buttonState);  
  if (buttonState == 0) {
    digitalWrite(relayPin, LOW); // Turn off relay
    Serial.println("Relay OFF");
  } else {
    digitalWrite(relayPin, HIGH);   // Turn on relay
    Serial.println("Relay ON");
  }
}

void loop() {

  timeClient.update();

  float current = readCurrentRMS();   // in Amps
  float voltage = readVoltageRMS();      // in Volts
  float power = voltage * current;    // in Watts

  // Serial
  Serial.print("Current: ");
  Serial.print(current, 2);
  Serial.print(" A | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V | Power: ");
  Serial.print(power, 2);
  Serial.println(" W");
  
  // Blynk Notification for high voltage
  if (voltage > 18.0 && !notificationSent) { 
    Serial.println("High voltage detected!"); 
    Blynk.logEvent("high_voltage", "Alert! Critical Voltage level"); 
    notificationSent = true; 
  } else if (voltage <= 18.0) { 
    notificationSent = false; 
  }

  // Store data to Firebase
  storeDataToFirebase(current, voltage, power);
  // Send data to Blynk 
  Blynk.virtualWrite(V1, current);
  Blynk.virtualWrite(V2, voltage);
  Blynk.virtualWrite(V3, power);

    
  // OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Smart Energy Monitor");

  // Display current
  display.print("Current: ");
  display.print(current, 2);
  display.println(" A");

  // Display voltage
  display.print("Voltage: ");
  display.print(voltage, 2);
  display.println(" V");

  // Display power
  display.print("Power: ");
  display.print(power, 2);
  display.println(" W");

  display.display();
  Blynk.run(); // Run Blynk event loop
  delay(3000); // Update every 3 seconds
}
