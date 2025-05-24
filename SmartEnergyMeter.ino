
// #define BLYNK_TEMPLATE_ID "TMPL6sMWFEFEk"
#define BLYNK_TEMPLATE_ID "TMPL6neA4lOpu"
#define BLYNK_TEMPLATE_NAME "SmartEnergyConsumption"
#define BLYNK_DEVICE_NAME "SmartEnergyConsumption"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <cmath>
#include <BlynkSimpleEsp32.h>
#include <FirebaseESP32.h>
#include <iostream> 
#include <iomanip> 
#include <sstream> 
#include <ctime>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3D
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Constants
#define V_REF 3.3
#define ADC_RESOLUTION 4095.0

#define CURRENT_CALIBRATION 5.0
#define VOLTAGE_DIVIDER_RATIO 5.56

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

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 OLED allocation failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Smart Energy Monitor");
  display.display();
  delay(2000);

  WiFi.begin("IOT", "12345678");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  timeClient.begin();
  Blynk.begin(auth, "IOT", "12345678"); 

  fbConfig.database_url = FIREBASE_DATABASE_URL;  // full URL, no trailing slash
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

// Only current needs RMS
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
// RMS voltage from AC input via divider
float readVoltageRMS() {
  int voltageRawValue = analogRead(voltagePin); 
  float voltageValue = voltageRawValue * (V_REF / ADC_RESOLUTION) * VOLTAGE_DIVIDER_RATIO;
  return voltageValue;
}

BLYNK_WRITE(V4) {
  int buttonState = param.asInt();
  Serial.print("Blynk V4 = ");
  Serial.println(buttonState);
  if (buttonState == 0) {
    digitalWrite(relayPin, LOW);
    Serial.println("Relay OFF");
  } else {
    digitalWrite(relayPin, HIGH);
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
   
  if (voltage > 18.0 && !notificationSent) { 
    Serial.println("High voltage detected!"); 
    Blynk.logEvent("high_voltage", "Alert! Critical Voltage level"); 
    notificationSent = true; 
  } else if (voltage <= 18.0) { 
    notificationSent = false; 
  }


  storeDataToFirebase(current, voltage, power); 
  Blynk.virtualWrite(V1, current);
  Blynk.virtualWrite(V2, voltage);
  Blynk.virtualWrite(V3, power);

    
  // OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Smart Energy Monitor");

  display.print("Current: ");
  display.print(current, 2);
  display.println(" A");

  display.print("Voltage: ");
  display.print(voltage, 2);
  display.println(" V");

  display.print("Power: ");
  display.print(power, 2);
  display.println(" W");

  display.display();
  Blynk.run();
  delay(3000);

}
