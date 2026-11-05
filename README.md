# data-track-firmware

Firmware do Data Track para ESP32: lê a temperatura de um sensor PT100 (via módulo MAX31865) e publica por MQTT usando WiFi.

## Configuração

```bash
cp src/config.example.h src/config.h
```

Edite `src/config.h` com os dados de WiFi, broker MQTT, tópico e pinos. O arquivo é ignorado pelo git.

## Comandos

```bash
pio run                  # compila
pio run -t upload        # grava na placa
pio device monitor       # monitor serial (115200)
pio run -t clean         # limpa a build
```

## Verificar as mensagens

```bash
mosquitto_sub -h <broker> -t 'data-track/#' -v
```
