# 🐾 Smart Cat Feeder (ESP32 + MQTT + Node-RED)
Effortlessly keep your feline friends fed with this automated cat feeder powered by ESP32, MQTT, and Node-RED. Revolutionize your pet care with real-time monitoring, customization, and automated feeding on schedule! 

![Smart Cat Feeder Setup](https://via.placeholder.com/800x400?text=Smart+Cat+Feeder+Visual+Demo)  
*Visual representation of the Smart Cat Feeder in action*

---

## 🚀 Table of Contents
- [Features](#features)
- [Hardware](#hardware-example)
- [Wiring](#wiring-pins)
- [MQTT Topics](#mqtt)
- [Setup Guide](#setup)
- [Calibration](#calibration)
- [Using Commands](#using-the-commands)
- [Safety](#safety)
- [License](#license)

## ✨ Features
- ✅ ESP32-based servo-controlled feeder
- ✅ MQTT telemetry & commands:
  - Bowl distance (`petfeeder/bol`)
  - Reservoir distance (`petfeeder/reservoir`)
  - Consumption grams (`petfeeder/consommation`)
  - Heartbeat (`petfeeder/heartbeat`)
  - Commands: `petfeeder/commande` (`feed`, `feed_for:<grams>`)
- ✅ Node-RED dashboard with:
  - Live gauges & charts 📊
  - Scheduling UI 🕒
  - Telegram alerts 📩
- ✅ SQLite database for distance & consumption logging

---

## 🛠️ Hardware (example)
| Component         | Details                                     |
|-------------------|---------------------------------------------|
| **Controller**    | ESP32                                      |
| **Sensors**       | 2x Ultrasonic (HC-SR04) for Bowl & Reservoir|
| **Servo Motor**   | Powered via 5V with control on pin 4       |
| **LED**           | Indicator at pin 2                         |
| **Power Supply**  | 5V shared supply for sensor & actuator     |

---

## 🌀 Wiring (Pins)
- **Reservoir Sensor**: TRIG1 5, ECHO1 18 (threshold: 26 cm)  
- **Bowl Sensor**: TRIG2 27, ECHO2 14 (threshold: 10 cm)  
- **Actuator**: Servo on pin 4  
- **Status Indicator**: LED on pin 2

---

## 🌐 MQTT
### Server
Host your MQTT broker (e.g., Mosquitto) at `192.168.1.10:1883`.

### Topics Overview
| Direction   | Topic | Purpose |
|-------------|-------|---------|
| **Publish** | `petfeeder/reservoir`  | Reservoir Distance Data |
|             | `petfeeder/bol`       | Bowl Distance Data      |
|             | `petfeeder/consommation` | Consumption (grams) |
|             | `petfeeder/heartbeat` | ESP32 Status           |
| **Subscribe** | `petfeeder/commande`  | Feed Commands (`feed`, `feed_for:<grams>`)

---

## 🔧 Calibration
Adjust `MS_PER_GRAM` in code for accuracy.  Example:  
If `600 ms = 5 g`, then `MS_PER_GRAM = 120` (adjust according to your servo mechanism).

---

## 🛠️ Setup Guide
### Flash ESP32
1. Update credentials for Wi-Fi and MQTT broker in `src/feeder/feeder.ino`.  
2. Build and upload using Arduino IDE or PlatformIO.  

### Run MQTT Broker
Example: Mosquitto at `192.168.1.10:1883`

### Import Node-RED Flow
1. In Node-RED, navigate to Import > Paste the feeder flow (`nodered/flow.json`).
2. Ensure necessary modules (`node-red-dashboard`, `sqlite`, etc.) are installed.

### Prepare SQLite
Create `petfeeder.db` with tables for:
- **Bowl Distance** (`bol` - distance in cm)
- **Reservoir Distance** (`reservoir` - distance in cm)
- **Consumption** (`consommation` - grams consumed)

### Set Telegram Alerts
Use `PetFeederBot` credentials in the Telegram Sender Node of Node-RED.

---

## 💡 Using the Commands
| Command                     | Effect                                  |
|-----------------------------|-----------------------------------------|
| `petfeeder/commande:feed`   | Dispense default feed amount           |
| `petfeeder/commande:feed_for:<grams>` | Dispense specified grams |

---

## ⚠️ Safety Recommendations
- Always test the servo first **without load**.
- Validate that the servo has adequate **5V power**.
- Include failsafes to handle potential mechanical issues (e.g., jamming).

---

## 📜 License
Licensed under the [MIT License](./LICENSE).

---

*Made with ❤️ for happy and healthy pets!*