# 🔌 Smart Energy Consumption Meter

An IoT-based real-time monitoring system that helps households and small businesses track electricity usage (current, voltage, power) using the Seeed Studio XIAO ESP32S3 microcontroller. The system provides local display via OLED, cloud storage via Firebase, and remote control through Blynk.

---

## ⚙️ Key Features

* **Real-Time Monitoring**:

  * Current (Amps), Voltage (Volts), Power (Watts)
* **Live Display**:

  * 0.96" SSD1306 OLED screen shows values locally
* **Cloud Integration**:

  * Firebase Realtime Database stores time-series data
* **Remote Control & Alerts**:

  * Blynk App (Web & Mobile) allows relay toggling and high-voltage alerts
* **Offline Analysis**:

  * Python script exports Firebase data to Excel for further analysis

---

## 🧰 Hardware Components

* Seeed Studio XIAO ESP32S3
* AC Current Sensor
* Voltage Divider (220Ω + 1kΩ)
* Capacitor (10µF)
* 10kΩ Resistor
* SSD1306 OLED (0.96", I2C, 128x64, addr: 0x3D)
* Relay Module (5V)
* 12V AC Bulbs (0.3A x3)
* Breadboard + jumper wires
* 12V Power Supply

---

## 🛠️ System Architecture

### Hardware Flow

* Voltage from 12V AC is scaled down using a voltage divider.
* AC current sensor measures bulb load.
* ESP32 reads analog values, controls relay, and shows data on OLED.
* Circuit assembled on breadboard.

### Communication Flow

* **Firebase** stores timestamped data every 3 seconds.
* **Blynk** allows:

  * Live monitoring (current, voltage, power)
  * Remote relay toggling
  * High voltage alerts

---

## 💻 Software Stack

* Arduino IDE (C++) for device firmware
* Firebase (Realtime DB) for data storage
* Blynk (cloud + mobile app) for UI and control
* Python (pandas, json) for data processing

**Libraries Used**:

* `WiFi.h`, `FirebaseESP32.h`, `BlynkSimpleEsp32.h`, `Adafruit_SSD1306.h`, `Wire.h`, `NTPClient.h`

---

## 🚀 Setup & Deployment

1. Wire up the circuit (see How-To Guide)
2. Flash `SmartEnergyMeter.ino` with your Wi-Fi, Firebase, and Blynk credentials
3. Create Firebase DB and Blynk dashboard
4. Monitor and control your loads live via app or OLED screen
5. Run Python script to analyze data:

   ```bash
   python firebasejson_Excel.py
   ```

---

## 📊 Evaluation Summary

* Current Accuracy: 92.22%
* Voltage Accuracy: 97.17%
* Power Accuracy: 89.63%

System demonstrated reliable performance across various loads using 12V simulation.

---

## 🔮 Future Enhancements

* AI-based usage prediction and optimization
* AC mains voltage adaptation
* Deep sleep for battery efficiency

---

## 📂 Repository Structure

* `SmartEnergyMeter.ino`: Firmware source
* `firebasejson_Excel.py`: Python export script
* `README.md`: Project overview
* `How-To-Guide.md`: Setup instructions

---

## 📎 Useful Links

* [GitHub Repository](https://github.com/amitbhudiya/SmartEnergyConsumptionMeter)
* [Firebase Console](https://console.firebase.google.com/)
* [Blynk Cloud](https://blynk.cloud/)

---

> Built with ❤️ by Group 19 (CITS5506 - UWA)
