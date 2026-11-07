#pragma once

// WiFi
#define WIFI_SSID "your-ssid"
#define WIFI_PASS "your-password"

// MQTT
#define MQTT_HOST "data-track.duckdns.org"
#define MQTT_PORT 8883
#define MQTT_USER "device"
#define MQTT_PASS "CHECAR-COM-O-RAFA"
#define MQTT_CLIENT_ID "data-track-esp32-01"
#define MQTT_TOPIC "data-track/esp32-01/temperature"

#define PUBLISH_INTERVAL_MS 5000

// MAX31865 / PT100
#define RTD_NOMINAL 100.0   // PT100
#define RTD_REF 430.0       // resistor de referência da placa MAX31865 (430 para PT100)
#define RTD_WIRES MAX31865_3WIRE  // MAX31865_2WIRE / _3WIRE / _4WIRE

// Pinos SPI (VSPI do ESP32)
#define PIN_RTD_CS 5
#define PIN_RTD_MOSI 23
#define PIN_RTD_MISO 19
#define PIN_RTD_SCK 18

// NTP
#define NTP_SERVER "pool.ntp.org"
#define NTP_TIMEOUT_MS 15000
