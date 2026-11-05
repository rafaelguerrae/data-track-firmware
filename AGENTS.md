# AGENTS.md

Firmware do Data Track para ESP32 (PlatformIO + Arduino, C++). Lê a temperatura de um PT100 via módulo MAX31865 (SPI) e publica por MQTT usando WiFi.

## Estrutura

- `platformio.ini`: ambiente `ttgo-t-call` (placa `esp32dev`) e dependências (`lib_deps`).
- `src/main.cpp`: programa principal (conexão WiFi/MQTT, leitura do sensor, publicação).
- `src/config.example.h`: modelo de configuração, versionado.
- `src/config.h`: configuração local com segredos. Ignorado pelo git; nunca versionar nem repetir seus valores em respostas, commits ou logs.

## Comandos

```bash
pio run                  # compila
pio run -t upload        # grava na placa
pio device monitor       # monitor serial (115200)
pio run -t clean         # limpa o build
```

O `pio` fica em `~/.platformio/penv/bin`. Se não estiver no PATH, use o caminho completo.

## Convenções

- Escrever comentários, documentação e mensagens de log em português (pt-br).
- Escrever sempre `data-track` (com hífen), nunca `datatrack`.
- Tópicos MQTT no formato `data-track/<id-do-dispositivo>/...`.
- Não usar `delay()` bloqueante no `loop()`; usar temporização com `millis()`.
- Ao adicionar uma opção em `config.h`, adicionar também em `config.example.h`.
- Não há testes automatizados; valide com `pio run` (compila) e, com a placa, pelo monitor serial e `mosquitto_sub -h <broker> -t 'data-track/#' -v`.
