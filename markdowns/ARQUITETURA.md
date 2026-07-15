# ARQUITETURA DO SISTEMA (ESP-NOW)

## Diagrama da Rede

```
┌──────────────────────────────────────────────────────────────────────┐
│                      REDE ESP-NOW (Canal 1)                          │
└──────────────────────────────────────────────────────────────────────┘
          ▲                         ▲
          │ ESP-NOW (broadcast)     │ ESP-NOW (broadcast)
          │                         │
┌─────────┴───────────────┐  ┌──────┴───────────────┐
│   ESP32 MESTRE           │  │  ESP32 SENSOR 1      │
│                          │  │                      │
│ ├─ Web Server (HTTP)     │  │ ├─ DHT22 (Temp/Ar)   │
│ ├─ LCD 16x2 I2C          │  │ └─ Sensor Solo       │
│ ├─ Bomba Área 1 (GPIO17) │  │   (Analógico GPIO32) │
│ ├─ Bomba Área 2 (GPIO16) │  │                      │
│ ├─ Ultrassônico 1 (27/14)│  │   ID = 1 (Área 1)    │
│ ├─ Ultrassônico 2 (26/25)│  └──────────────────────┘
│ ├─ Buzzer (GPIO33, OR)   │  ┌──────────────────────┐
│ └─ WiFi AP+STA           │  │  ESP32 SENSOR 2      │
└───────────────────────────┘  │                      │
                               │ ├─ DHT22 (Temp/Ar)   │
                               │ └─ Sensor Solo       │
                               │   (Analógico GPIO32) │
                               │                      │
                               │   ID = 2 (Área 2)    │
                               └──────────────────────┘
```

## Diagrama de Dependências (Mestre)

```
┌─────────────────────────────────────────────────────────────────┐
│                     Teste.ino                                    │
│                     (Master Sketch)                              │
└──────────┬──────────┬──────────┬──────────┬──────────┬──────────┘
           │          │          │          │          │
           v          v          v          v          v
┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐
│ SENSORS  │ │ SYSTEM   │ │ESPNOWMAN │ │ WEB_SERV │ │WIFI_MAN  │
│          │ │          │ │          │ │          │ │          │
│• Water   │ │• Pump    │ │• Receive │ │• HTTP    │ │• AP+STA  │
│• 2 inst  │ │• Modes   │ │• Store   │ │• Pages   │ │• Connec  │
│          │ │• 2 areas │ │• Per ID  │ │• LCD     │ │• Status  │
└──────────┘ └──────────┘ └──────────┘ └──────────┘ └──────────┘
     │            │            │            │            │
     └────────────┴────────────┴────────────┴────────────┘
                          │
                          v
                     CONFIG.H
```

## Fluxo de Dados

```
ESP32 SENSOR 1 e 2 (a cada 600ms):
┌──────────────────────────────────────────────┐
│ 1. readSoilAverage() → média de 5 leituras   │
│ 2. readDHT() → temperatura + umidade         │
│ 3. sendViaESPNOW() → pacote 15 bytes         │
│    ┌──────────────────────────────┐          │
│    │ sensorId | soilAnalog | temp │          │
│    │ hum | timestamp              │          │
│    └──────────────────────────────┘          │
└──────────────────────────────────────────────┘

ESP32 MESTRE (loop infinito):
┌──────────────────────────────────────────────┐
│ 1. waterSensor1.update() (Área 1)            │
│    waterSensor2.update() (Área 2)            │
│ 2. soilPercent1 = getSoilPercent(ID 1)       │
│    soilPercent2 = getSoilPercent(ID 2)       │
│ 3. processArea() para cada área:             │
│    ├─ Água ≤ 25% → Para bomba da área       │
│    ├─ Sensor offline → Bomba bloqueada       │
│    ├─ Solo < 40% → Liga bomba da área       │
│    ├─ Solo > 70% → Desliga bomba da área    │
│    └─ 40-70% → Mantém estado (histerese)    │
│ 4. updateBuzzer(OR entre áreas)              │
│ 5. LCD alterna entre áreas a cada 3s         │
│ 6. dataLogger1/2.recordSample()              │
│ 7. webServer.handleClient()                  │
│ 8. Log serial a cada 10s                     │
└──────────────────────────────────────────────┘
```

## Classes e Métodos

### Mestre

#### 1. WaterLevelSensor (sensors.h/.cpp)
```cpp
void init();
void update();
float getWaterLevel();       // 0-100% (ultrassônico)
unsigned int getDistanceMM(); // 20-60mm
```

#### 2. ESPNOWManager (espnow_manager.h/.cpp)
```cpp
void init();
SensorDataPacket getData(uint8_t sensorId);
bool isSensorOnline(uint8_t sensorId, timeout=10s);
float getAverageSoilPercent(dryValue, wetValue); // 0-100% ou -1
float getSoilPercent(sensorId, dryValue, wetValue); // por sensor
int getOnlineSensorCount(timeout=10s);
```

#### 3. soilAnalogToPercent() (sensors.cpp)
```cpp
float soilAnalogToPercent(uint16_t analog, dry, wet);
// 3600 → 0%, 1900 → 100%
```

#### 4. IrrigationSystem (pump-only, sem buzzer)
```cpp
IrrigationSystem(uint8_t relayPin);
void pumpStart() / pumpStop();
bool getPumpStatus();
void setManualMode(bool) / getManualMode();
String getLastIrrigationTime();
```

#### 5. DataLogger
```cpp
void init(initialSoil, initialWater);
void recordSample(soil%, água%, bomba);
String getSoilHistoryJson();
String getWaterHistoryJson();
String getPumpHistoryJson();
```

#### 6. WebServerManager
```cpp
void init();
void handleClient();
void displayLcd(String line1, String line2);
// Handlers: handleRoot(), handleData(), handleChartData(),
// handleForecast(), handlePumpStart/Stop(),
// handleSetManualMode/setAutoMode()
```

#### 7. WiFiManager
```cpp
void init();                    // AP+STA, canal = ESPNOW_CHANNEL
bool isStationConnected();
String getStationIP();
String getAPIP();
```

### Sensor Node (SensorNode/)

#### SensorNode.ino
```cpp
void initSerial() / initWiFi() / initESPNOW() / initSensors();
uint16_t readSoilAverage();     // Média de 5 leituras analógicas
void readDHT();                  // DHT11 com fallback NaN
void sendViaESPNOW();            // Broadcast 15 bytes
```

## Estrutura de Dados ESP-NOW (15 bytes)

| Campo | Tipo | Bytes | Descrição |
|-------|------|-------|-----------|
| sensorId | uint8_t | 1 | 1 ou 2 |
| soilAnalog | uint16_t | 2 | 0-4095 (ADC) |
| temperature | float | 4 | °C |
| humidity | float | 4 | % |
| timestamp | uint32_t | 4 | millis() |

## Conversão Solo (Analógico → Percentual)

```
% = 100 × (SOIL_DRY_VALUE - analog) / (SOIL_DRY_VALUE - SOIL_WET_VALUE)

Exemplos:
  3600 → 0%  (seco)
  2600 → 59% (úmido)
  1900 → 100% (molhado)
```

## Lógica de Irrigação com Histerese

```
Solo < 40%  → LIGA bomba
Solo > 70%  → DESLIGA bomba
40-70%      → Mantém estado atual
```

## Rotas HTTP

```
GET /                    → Landing page (escolha de área)
GET /area1               → Dashboard da Área 1
GET /area2               → Dashboard da Área 2
GET /info                → Informações da equipe
GET /previsao            → Previsão do tempo (Open-Meteo)
GET /operacao            → Manual de operação
GET /graficos            → Gráficos (2 áreas sobrepostos)
GET /data                → JSON geral (ambas áreas) ou /data?area=N
GET /grafdata            → JSON com histórico separado por área
GET /modoManual?area=N   → Ativa modo manual da área N
GET /modoAuto?area=N     → Ativa modo automático da área N
GET /ligar?area=N        → Liga bomba da área N
GET /desligar?area=N     → Desliga bomba da área N
GET /testarBuzzer        → Testa buzzer (3x)
```

## JSON /data

### ?area=N (por área)

```json
{
  "soloPercent": 65,
  "online": "SIM",
  "temperatura": 28.5,
  "umidade": 72.0,
  "nivel": 42.3,
  "tempo": "há 5min",
  "manual": "NAO",
  "irrigando": "SIM"
}
```

### Geral (sem parâmetro)

```json
{
  "temperatura1": 28.5,
  "umidade1": 72.0,
  "temperatura2": 27.8,
  "umidade2": 70.5,
  "solo1": 2200,
  "solo2": 1800,
  "online1": "SIM",
  "online2": "NAO",
  "nivel1": 42.3,
  "nivel2": 38.1,
  "tempo1": "há 5min",
  "tempo2": "Nunca",
  "manual1": "NAO",
  "manual2": "SIM",
  "irrigando1": "SIM",
  "irrigando2": "NAO"
}
```

## Logs Estruturados

```
[ESPNOW] Recebido Sensor 1 | Solo Analog: 2200 | Temp: 28.5C | Umidade: 72.0%
[LOG] Agua: 42.3% | Solo: 65% | Sensores Online: 2 | Bomba: DESLIGADA | Modo: AUTOMATICO
```

## GPIO Mestre

```
GPIO21(SDA)   LCD I2C
GPIO22(SCL)   LCD I2C
GPIO27(TRIG)  Ultrassônico 1 (Área 1)
GPIO14(ECHO)  Ultrassônico 1 (Área 1)
GPIO26(TRIG)  Ultrassônico 2 (Área 2)
GPIO25(ECHO)  Ultrassônico 2 (Área 2)
GPIO17(RELAY) Bomba 1 (Área 1, ativo LOW)
GPIO16(RELAY) Bomba 2 (Área 2, ativo LOW)
GPIO33(BUZZER) Alerta sonoro (OR entre áreas)
```

## GPIO Sensor Node

```
GPIO22(DHT) DHT11/DHT22
GPIO32(SOIL) Sensor solo analógico
GPIO2(STATUS) LED indicador de envio
```

---

**Arquitetura v4.0 - 2 Áreas Independentes + Buzzer Compartilhado**
