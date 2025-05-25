# 🔌 Smart Energy Consumption Meter

An IoT-based system designed to monitor household energy usage in real time using the Seeed Studio XIAO ESP32S3 microcontroller. It tracks current, voltage, and power, displaying the values locally on an OLED screen and pushing the data to Firebase for storage and to Blynk for remote monitoring and control.

## 📦 Features

- Real-time monitoring of:
  - 📈 Current (Amps)
  - 🔋 Voltage (Volts)
  - ⚡ Power (Watts)
- OLED display for local visualization
- Firebase Realtime Database integration for logging
- Blynk app integration for remote monitoring and control
- Relay-controlled load toggling
- Timestamped data logging with NTP synchronization
- Python script for exporting data to Excel for analysis

## 🧰 Hardware Components

- Seeed Studio XIAO ESP32S3 (Microcontroller)
- SSD1306 OLED Display (I2C)
- AC Current Sensor
- Voltage Divider (Resistors + Capacitor)
- Relay Module
- Voltage divider(220 Ohm + 1k ohm)
- 10k Ohm Resistor
- Capacitor (10uF)
- 12V Bulbs for load simulation
- Breadboard & jumper wires

## 🧰 Hardware Design

## 💻 Software & Tools

- Arduino IDE (C++)
- Firebase Realtime Database
- Blynk (Web and Mobile)
- Python (for data processing)
- Libraries used:
  - `WiFi.h`, `Wire.h`, `Adafruit_SSD1306.h`, `FirebaseESP32.h`, `BlynkSimpleEsp32.h`, `NTPClient.h`

## 🛠️ Setup & Deployment

### Arduino Code

1. Clone the repo and open the `.ino` file in Arduino IDE.
2. Update the following with your credentials:
    ```cpp
    char auth[] = "YOUR_BLYNK_AUTH_TOKEN";
    WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
    fbConfig.database_url = "YOUR_FIREBASE_URL";
    fbConfig.api_key = "YOUR_FIREBASE_API_KEY";
    fbConfig.signer.tokens.legacy_token = "YOUR_FIREBASE_DB_SECRET";
    ```
3. Connect the hardware and upload the code.

### Firebase Database Structure

