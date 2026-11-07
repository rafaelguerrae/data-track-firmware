#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_MAX31865.h>

#include "config.h"
#include "ca_cert.h"

static const char *TOPIC_STATUS = MQTT_TOPIC "/status";
static const char *TOPIC_FAULT = MQTT_TOPIC "/fault";

WiFiClientSecure wifiClient;
PubSubClient mqtt(wifiClient);
Adafruit_MAX31865 rtd(PIN_RTD_CS, PIN_RTD_MOSI, PIN_RTD_MISO, PIN_RTD_SCK);

unsigned long lastPublish = 0;
unsigned long lastMqttAttempt = 0;
unsigned long lastWifiAttempt = 0;
unsigned long ntpStart = 0;
bool ntpStarted = false;

// Data mínima aceita como "relógio sincronizado" (01/01/2024).
static const time_t MIN_VALID_EPOCH = 1704067200;

void connectWifi() {
    if (WiFi.status() == WL_CONNECTED) return;
    if (millis() - lastWifiAttempt < 10000 && lastWifiAttempt != 0) return;
    lastWifiAttempt = millis();
    Serial.printf("Conectando ao WiFi %s...\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
}

// Sincroniza o relógio via NTP sem bloquear o loop(). Retorna true quando a data é válida.
bool clockSynced() {
    if (time(nullptr) >= MIN_VALID_EPOCH) return true;
    if (WiFi.status() != WL_CONNECTED) return false;

    if (!ntpStarted) {
        ntpStarted = true;
        ntpStart = millis();
        Serial.printf("Sincronizando relógio via NTP (%s)...\n", NTP_SERVER);
        configTime(0, 0, NTP_SERVER);
    } else if (millis() - ntpStart >= NTP_TIMEOUT_MS) {
        Serial.println("NTP sem resposta, tentando novamente...");
        ntpStarted = false;
    }
    return false;
}

void connectMqtt() {
    if (WiFi.status() != WL_CONNECTED || mqtt.connected()) return;
    if (!clockSynced()) return;
    if (millis() - lastMqttAttempt < 5000 && lastMqttAttempt != 0) return;
    lastMqttAttempt = millis();

    Serial.println("Conectando ao MQTT...");
    bool ok = strlen(MQTT_USER)
                  ? mqtt.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS, TOPIC_STATUS, 1, true, "offline")
                  : mqtt.connect(MQTT_CLIENT_ID, nullptr, nullptr, TOPIC_STATUS, 1, true, "offline");
    if (ok) {
        mqtt.publish(TOPIC_STATUS, "online", true);
        Serial.println("MQTT conectado");
    } else {
        Serial.printf("Falha MQTT, rc=%d\n", mqtt.state());
    }
}

// Retorna true e preenche `out` em caso de sucesso; em caso de falha, publica o código e retorna false.
bool readTemperature(float &out) {
    uint16_t raw = rtd.readRTD();
    uint8_t fault = rtd.readFault();
    if (fault) {
        Serial.printf("Falha no MAX31865: 0x%02X\n", fault);
        rtd.clearFault();
        if (mqtt.connected()) {
            char buf[8];
            snprintf(buf, sizeof(buf), "0x%02X", fault);
            mqtt.publish(TOPIC_FAULT, buf);
        }
        return false;
    }
    (void)raw;
    out = rtd.temperature(RTD_NOMINAL, RTD_REF);
    return true;
}

void publishTemperature(float temp) {
    JsonDocument doc;
    doc["temp_c"] = serialized(String(temp, 2));
    doc["ts"] = millis();

    char payload[64];
    serializeJson(doc, payload, sizeof(payload));
    bool ok = mqtt.publish(MQTT_TOPIC, payload);
    Serial.printf("%s -> %s (%s)\n", MQTT_TOPIC, payload, ok ? "ok" : "falhou");
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("ESP32 inicializado!");

    rtd.begin(RTD_WIRES);
    WiFi.mode(WIFI_STA);
    wifiClient.setCACert(MQTT_CA_CERT);
    mqtt.setServer(MQTT_HOST, MQTT_PORT);
}

void loop() {
    connectWifi();
    connectMqtt();
    mqtt.loop();

    if (millis() - lastPublish >= PUBLISH_INTERVAL_MS) {
        lastPublish = millis();
        float temp;
        if (readTemperature(temp) && mqtt.connected()) {
            publishTemperature(temp);
        }
    }
}
