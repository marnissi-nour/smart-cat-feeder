# Smart Cat Feeder (ESP32 + MQTT + Node-RED)
Automated food dispenser for cats using ESP32, MQTT, and a Node-RED dashboard.

## Authors
- marnissi-nour
- Emna Merdessi
- Adem Fraj

## Features
- ESP32-based servo-controlled feeder
- MQTT telemetry:
  - Bowl distance (`petfeeder/bol`)
  - Reservoir distance (`petfeeder/reservoir`)
  - Consumption grams (`petfeeder/consommation`)
  - Heartbeat (`petfeeder/heartbeat`)
- MQTT commands:
  - `petfeeder/commande`: `feed` or `feed_for:<grams>`
- Node-RED dashboard with live gauges, charts, scheduling UI, and Telegram alerts
- SQLite logging for distances and consumption

## Hardware (example)
- ESP32
- Ultrasonic sensors (HC-SR04) x2
  - Reservoir: TRIG 5, ECHO 18
  - Bowl: TRIG 27, ECHO 14
- Servo on pin 4
- Status LED on pin 2
- 5V supply for servo (with common ground)

## Wiring (pins)
- TRIG1 5, ECHO1 18 (reservoir), threshold: 26 cm
- TRIG2 27, ECHO2 14 (bowl), threshold: 10 cm
- SERVO 4, LED 2

## MQTT
- Host: `MQTT_HOST` (e.g., `192.168.1.10`)
- Port: `1883`
- Topics:
  - Publish: `petfeeder/reservoir`, `petfeeder/bol`, `petfeeder/consommation`, `petfeeder/heartbeat`
  - Subscribe: `petfeeder/commande`

## Calibration
Set `MS_PER_GRAM` in code. Example: `600 ms / 5 g` → `120 ms/g` (adjust to your mechanism).

## Setup
1) Flash ESP32
- Update Wi-Fi and MQTT host in `src/feeder/feeder.ino`.
- Use Arduino IDE or PlatformIO to build & upload.

2) Run MQTT broker
- Example: Mosquitto on `192.168.1.10:1883`.

3) Import Node-RED flow
- Open Node-RED → Import → paste `nodered/flow.json`.
- Ensure dashboard (node-red-dashboard), sqlite, and telegrambot nodes installed.

4) SQLite
- The flow uses `petfeeder.db` with tables: `bol(distance_cm)`, `reservoir(distance_cm)`, `consommation(quantite_g)` (create before running or let you manage schema manually).

5) Telegram alerts
- Configure `PetFeederBot` credentials in the Telegram sender node.

## Using the commands
- `petfeeder/commande`: `feed`
- `petfeeder/commande`: `feed_for:<grams>` (e.g., `feed_for:10`)

## Safety
- Test with the servo unloaded first.
- Verify 5V power for servo; keep grounds common.
- Add mechanical failsafes to avoid jamming.

## License
MIT (see LICENSE).