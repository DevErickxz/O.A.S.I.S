# Sistema de Irrigação Automático - RoboCore (ESP-NOW)

Projeto de irrigação automatizada da **ECIT Profª Auricélia Maria Da Costa** (João Pessoa - PB). Rede com **3 ESP32s** em ESP-NOW **+ 2 áreas independentes**.

**Equipe:** Erick Marques, Abner Felipe, Bruno Silva *(RoboCore)*

---

## Arquitetura da Rede

```
┌─────────────────┐    ESP-NOW     ┌──────────────────────────┐
│ ESP32 SENSOR 1  │───────────────→│ ESP32 MESTRE             │
│ (ID=1, Área 1)  │  (broadcast)   │                          │
│ ├─ DHT22        │               │ ├─ Web Server            │
│ └─ Solo Analógico│               │ ├─ LCD 16x2 I2C          │
└─────────────────┘               │ ├─ Bomba 1 (GPIO17)      │
┌─────────────────┐               │ ├─ Bomba 2 (GPIO16)      │
│ ESP32 SENSOR 2  │───────────────→│ ├─ Ultrassônico 1 (27/14)│
│ (ID=2, Área 2)  │               │ ├─ Ultrassônico 2 (26/25)│
│ ├─ DHT22        │               │ ├─ Buzzer (GPIO33, OR)   │
│ └─ Solo Analógico│               │ └─ WiFi AP+STA           │
└─────────────────┘               └──────────────────────────┘
```

## Hardware

### Mestre
- **ESP32**: Web server, LCD, 2 bombas, buzzer, 2 ultrassônicos
- **Ultrassônico 1 (HC-SR04)**: Nível água Área 1 (GPIO27/14)
- **Ultrassônico 2 (HC-SR04)**: Nível água Área 2 (GPIO26/25)
- **Relé 1**: Bomba Área 1 (GPIO17, ativo LOW)
- **Relé 2**: Bomba Área 2 (GPIO16, ativo LOW)
- **Buzzer**: Alerta compartilhado (GPIO33, OR entre áreas)
- **LCD 16x2 I2C**: Display alterna entre áreas (GPIO21/22)

### Sensores Remotos (x2)
- **ESP32**: Envio via ESP-NOW a cada 600ms
- **DHT22**: Temperatura + umidade do ar (GPIO22)
- **Sensor Solo Analógico**: 0-100% (GPIO32)

## Funcionalidades

- **2 áreas independentes** com lógica de irrigação própria
- **Solo em percentual** (0-100%) com barra de progresso
- **Dois sensores remotos** com temperatura e umidade por área
- **Indicador online/offline** de cada sensor
- **ESP-NOW** broadcast sem necessidade de configurar MACs
- **Histerese**: Liga em 40%, desliga em 70%
- **Proteção**: Água baixa bloqueia bomba + buzzer
- **Buzzer compartilhado**: alerta se qualquer área com água baixa
- **Web responsivo**: Landing page → escolha de área → dashboard
- **Gráficos Canvas**: séries das 2 áreas sobrepostas

## Acesso

1. Conecte no WiFi `ROBOCORE FARM` (senha: `12345678`)
2. Abra `http://192.168.4.1`
3. Selecione **Área 1** ou **Área 2** para monitorar

## Setup Rápido

### Mestre (1 ESP32)
1. Abra `Teste.ino` na Arduino IDE (pasta raiz)
2. Placa: **ESP32 Dev Module**, Porta: **COM[X]**
3. Bibliotecas: `LiquidCrystal I2C`, `NewPing`
4. Faça upload

### Sensores (2 ESP32s)
1. Abra `SensorNode/SensorNode.ino`
2. Em `sensor_config.h`, ajuste `SENSOR_ID`:
   - **Sensor da Área 1**: `#define SENSOR_ID 1`
   - **Sensor da Área 2**: `#define SENSOR_ID 2`
3. Placa: **ESP32 Dev Module**
4. Bibliotecas: `DHT sensor library`
5. Faça upload em cada placa