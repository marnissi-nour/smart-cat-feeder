#include <ESP32Servo.h>
#include <WiFi.h>
#include <AsyncMqttClient.h>

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

const char* MQTT_HOST = "192.168.1.10";
const uint16_t MQTT_PORT = 1883;

const char* TOPIC_RESERVOIR = "petfeeder/reservoir";
const char* TOPIC_BOL = "petfeeder/bol";
const char* TOPIC_CONSO = "petfeeder/consommation";
const char* TOPIC_CMD = "petfeeder/commande";
const char* TOPIC_HEARTBEAT = "petfeeder/heartbeat";

AsyncMqttClient mqtt;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

#define TRIG_PIN1 5
#define ECHO_PIN1 18
#define LIMITE_RESERVOIR 26

#define TRIG_PIN2 27
#define ECHO_PIN2 14
#define LIMITE_BOL 10

#define LED_PIN 2
#define SERVO_PIN 4

Servo moteur;

unsigned long previousHeartbeat = 0;
const unsigned long heartbeatInterval = 60000; // 60s

// ms per gram (calibrate to your mechanism)
const float MS_PER_GRAM = 600.0 / 5.0; // example: 120 ms/g

float getDistanceCM(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000L); // timeout 30ms
  if (duration == 0) return -1; // error
  return (duration * 0.034) / 2.0;
}

void doFeedDuration(unsigned long ms) {
  Serial.printf("Feeding for %lu ms\n", ms);
  moteur.write(90);
  delay(ms);
  moteur.write(0);
  float grams = ms / MS_PER_GRAM;
  char buf[16];
  dtostrf(grams, 0, 1, buf);
  mqtt.publish(TOPIC_CONSO, 1, false, buf);
  Serial.printf("Published consumption: %s g\n", buf);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  String t = String(topic);
  String msg = String(payload).substring(0, len);
  Serial.printf("MQTT recv %s : %s\n", t.c_str(), msg.c_str());

  if (t == TOPIC_CMD) {
    if (msg == "feed") {
      doFeedDuration(3000); // default duration
    } else if (msg.startsWith("feed_for:")) {
      String v = msg.substring(strlen("feed_for:"));
      int g = v.toInt();
      if (g > 0) {
        unsigned long ms = (unsigned long)(g * MS_PER_GRAM);
        doFeedDuration(ms);
      }
    }
  }
}

void connectToMqtt() {
  if (WiFi.isConnected()) {
    mqtt.connect(MQTT_HOST, MQTT_PORT);
  }
}

void onWifiConnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi connected");
  connectToMqtt();
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN1, OUTPUT); pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT); pinMode(ECHO_PIN2, INPUT);
  pinMode(LED_PIN, OUTPUT);

  moteur.attach(SERVO_PIN);
  moteur.write(0);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  wifiConnectHandler = WiFi.onEvent(onWifiConnect, SYSTEM_EVENT_STA_GOT_IP);

  mqtt.onMessage(onMqttMessage);
  mqtt.onConnect([](bool sessionPresent){
    Serial.println("MQTT connected");
    mqtt.subscribe(TOPIC_CMD, 1);
  });
  mqtt.onDisconnect([](AsyncMqttClientDisconnectReason reason){
    Serial.println("MQTT disconnected");
  });
}

void loop() {
  unsigned long now = millis();

  float distReservoir = getDistanceCM(TRIG_PIN1, ECHO_PIN1);
  float distBol = getDistanceCM(TRIG_PIN2, ECHO_PIN2);

  if (distReservoir >= 0) {
    char buf[16];
    dtostrf(distReservoir, 0, 2, buf);
    mqtt.publish(TOPIC_RESERVOIR, 1, false, buf);
    digitalWrite(LED_PIN, distReservoir > LIMITE_RESERVOIR ? HIGH : LOW);
  }

  if (distBol >= 0) {
    char buf2[16];
    dtostrf(distBol, 0, 2, buf2);
    mqtt.publish(TOPIC_BOL, 1, false, buf2);
  }

  if (now - previousHeartbeat >= heartbeatInterval) {
    previousHeartbeat = now;
    mqtt.publish(TOPIC_HEARTBEAT, 1, false, "online");
  }

  delay(1000);
}