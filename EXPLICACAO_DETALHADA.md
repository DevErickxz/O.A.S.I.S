# 📚 EXPLICAÇÃO DETALHADA - ARQUIVO POR ARQUIVO

> Este documento explica CADA ARQUIVO da refatoração com explicações linha por linha e exemplos práticos.

---

## 📖 ÍNDICE

1. **config.h** - Configurações
2. **sensors.h** - Definição dos sensores
3. **sensors.cpp** - Como os sensores funcionam
4. **system.h** - Definição do sistema
5. **system.cpp** - Como o sistema funciona
6. **data_logger.h** - Definição do histórico
7. **data_logger.cpp** - Como o histórico funciona
8. **web_server.h** - Definição web
9. **web_server.cpp** - Como a web funciona (o MAIOR arquivo!)
10. **wifi_manager.h** - Definição do WiFi
11. **wifi_manager.cpp** - Como o WiFi funciona
12. **irrigacao_PB.ino** - Arquivo mestre

---

# 1️⃣ config.h - CONFIGURAÇÕES

Este arquivo é como um "controle remoto" - aqui estão TODOS os valores que você pode querer mudar.

```cpp
#ifndef CONFIG_H
#define CONFIG_H
```
**O que é:** Proteção contra incluir o arquivo 2 vezes.
**Por quê:** Se incluir 2 vezes, dá erro de "redefinição". Isto evita.

---

```cpp
// GPIO DEFINITIONS (QUAL PINO ESTÁ CONECTADO NO QUÊ)
#define GPIO_SDA 21          // I2C data (LCD conecta aqui)
#define GPIO_SCL 22          // I2C clock (LCD conecta aqui)
#define GPIO_SOIL_SENSOR 18  // Sensor de solo (digital)
#define GPIO_RELAY_PUMP 17   // Relé da bomba
#define GPIO_BUZZER 33       // Buzzer (campainha)
#define GPIO_ULTRASONIC_TRIG 27  // Ultrassônico - TRIGGER
#define GPIO_ULTRASONIC_ECHO 14  // Ultrassônico - ECHO
```

**Tradução simples:**
- Se você conectar o sensor em outro pino, muda aqui!
- Exemplo: Se conectar a bomba no GPIO 16 em vez de 17:
  ```cpp
  #define GPIO_RELAY_PUMP 16  // Mudou!
  ```

---

```cpp
// I2C CONFIGURATION
#define I2C_ADDR_LCD 0x27      // Endereço da LCD (pode ser 0x27 ou 0x3F)
#define LCD_COLS 16            // LCD tem 16 colunas
#define LCD_ROWS 2             // LCD tem 2 linhas
```

**Entender:**
- LCD "endereço" é tipo um CPF - cada LCD tem um único
- Se sua LCD não funcionar, tenta 0x3F em vez de 0x27

---

```cpp
// RESERVOIR SPECIFICATIONS (TAMANHO DO RESERVATÓRIO)
#define RESERVOIR_FULL_MM 20      // Quando está CHEIO, sensor marca 20mm
#define RESERVOIR_EMPTY_MM 60     // Quando está VAZIO, sensor marca 60mm
#define WATER_LEVEL_MIN_ALERT 25  // Se cair abaixo de 25%, liga alerta
#define ULTRASONIC_MAX_DISTANCE_CM 200  // Alcance máximo do sensor
```

**Exemplo prático:**
```
Reservatório seu:
├─ CHEIO (20mm) = 100%
├─ MEIO (40mm) = 50%
└─ VAZIO (60mm) = 0%

Se nível < 25% → BUZZER LIGA E BOMBA PARA
```

---

```cpp
// TIMING CONSTANTS (TEMPOS)
#define DEBOUNCE_SOIL_MS 700        // Espera 700ms para confirmar mudança
#define BUZZER_BEEP_MS 150          // Cada beep dura 150ms
#define HIST_SAMPLE_INTERVAL_MS 5000 // Guarda um dado a cada 5 segundos
```

**Tradução:**
- **DEBOUNCE**: Se sensor fica oscilando, espera 700ms pra confirmar
- **BUZZER**: Beep curto de 150ms
- **HISTÓRICO**: Cada 5 segundos tira uma foto do sistema

---

```cpp
// WIFI CONFIGURATION
#define WIFI_SSID "ALAN - SMART.INTERNET"   // WiFi de casa
#define WIFI_PASS "20212021"                // Senha
#define WIFI_AP_SSID "ROBOCORE FARM"        // WiFi local do ESP32
#define WIFI_AP_PASS "12345678"             // Senha local
#define WIFI_TIMEOUT_MS 20000               // 20 segundos pra conectar
```

**Mudança comum:**
Se você quer conectar em outro WiFi:
```cpp
#define WIFI_SSID "Minha_Casa"
#define WIFI_PASS "minha_senha"
```

---

```cpp
// SERIAL CONFIGURATION
#define SERIAL_BAUD_RATE 115200  // Velocidade de comunicação
```

**Nota:** Sempre 115200 pro ESP32.

---

```cpp
// HISTORY CONFIGURATION
#define HISTORY_SIZE 60  // Guarda 60 medições
// 60 medições × 5 segundos = 300 segundos = 5 MINUTOS
```

---

```cpp
#endif  // CONFIG_H
```
**Fecha a proteção.**

---

## 🎯 RESUMO config.h:

**Para quê serve?** Centralizar TODOS os números do projeto em um só lugar.

**Quando mudar?**
- Conectar sensor em outro pino? Muda aqui.
- Mudar WiFi? Muda aqui.
- Aumentar tamanho do histórico? Muda aqui.

**O que NÃO mudar:** Não mexe se não souber o que está fazendo!

---

---

# 2️⃣ sensors.h - DEFINIÇÃO DOS SENSORES

Este arquivo é como um "molde" - define a FORMA de uma classe, não a função dela.

```cpp
#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <NewPing.h>
#include "config.h"
```

**Tradução:**
- `#include <Arduino.h>` - Inclui funções básicas do Arduino
- `#include <NewPing.h>` - Biblioteca do ultrassônico
- `#include "config.h"` - Nossos valores (GPIO, etc)

---

```cpp
/**
 * @class WaterLevelSensor
 * @brief Controla o sensor ultrassônico de nível de água
 */
class WaterLevelSensor {
private:
  // VARIÁVEIS PRIVADAS (só a classe vê)
  NewPing sonar;           // Objeto do ultrassônico
  int readingsBuffer[5];   // Guarda 5 leituras
  int readingIndex;        // Qual das 5 estou usando
  float lastDistanceMM;    // Última distância em MM

public:
  // FUNÇÕES PÚBLICAS (qualquer um pode chamar)
  WaterLevelSensor(int trigPin, int echoPin, int maxDist);
  void init();
  void update();
  float getWaterLevel();   // Retorna 0-100%
  int getDistanceMM();     // Retorna distância em MM
};
```

**O que isto significa?**

```
PRIVADO (private):
  - readingsBuffer, readingIndex, lastDistanceMM
  - Só a classe usa, ninguém de fora mexe

PÚBLICO (public):
  - init(), update(), getWaterLevel(), getDistanceMM()
  - Qualquer um chama essas funções
```

**Analogy:** Classe é como um carro:
- PRIVADO = motor (você não mexe)
- PÚBLICO = volante (você muda a direção)

---

```cpp
/**
 * @class SoilMoistureSensor
 * @brief Sensor digital de umidade do solo
 */
class SoilMoistureSensor {
private:
  int sensorPin;                    // Qual pino está conectado
  int lastReading;                  // Última leitura (HIGH/LOW)
  int stableReading;                // Valor confirmado (sem oscilação)
  unsigned long lastChangeTime;     // Quando mudou por último
  unsigned long debounceDelay;      // Quanto tempo espera pra confirmar

public:
  SoilMoistureSensor(int pin, unsigned long debounceMs);
  void init();
  void update();
  String getStatus();     // Retorna "SECO" ou "MOLHADO"
  bool isDry();           // Retorna true se seco
  bool isWet();           // Retorna true se molhado
};
```

**Novidade:** `debounce`
```
Sem debounce:
  Sensor lê: DRY → WET → DRY → WET (oscila muito!)

Com debounce (700ms):
  Sensor lê: DRY → WET → WET → WET (espera confirmar)
  Resultado: "OK, é WET mesmo"
```

---

```cpp
#endif  // SENSORS_H
```

---

## 🎯 RESUMO sensors.h:

**Para quê?** Define COMO são os sensores (que variáveis têm, que funções podem usar).

**Lembrete:** Isto é só o "molde", a função real está em `sensors.cpp`.

---

---

# 3️⃣ sensors.cpp - IMPLEMENTAÇÃO DOS SENSORES

Agora sim, aqui está a MÁGICA - como os sensores funcionam de verdade.

```cpp
#include "sensors.h"

// CONSTRUTOR (executado uma vez quando cria o objeto)
WaterLevelSensor::WaterLevelSensor(int trigPin, int echoPin, int maxDist)
  : sonar(trigPin, echoPin, maxDist),  // Inicializa ultrassônico
    readingIndex(0),                    // Começa no índice 0
    lastDistanceMM(0) {                 // Distância inicial 0
}
```

**Tradução:**
```cpp
// Quando você escreve no main:
WaterLevelSensor meuSensor(27, 14, 200);

// Isto chama o construtor acima, que:
// 1. Cria um ultrassônico nos pinos 27 (TRIG) e 14 (ECHO)
// 2. Define alcance máximo de 200cm
// 3. Zera o índice
// 4. Zera a distância
```

---

```cpp
void WaterLevelSensor::init() {
  // Não faz nada especial pro ultrassônico
  // Mas poderia inicializar pinos se necessário
  Serial.println("[Sensor] Agua inicializado");
}
```

**Simples:** Só avisa que inicializou.

---

```cpp
void WaterLevelSensor::update() {
  // Faz uma leitura do ultrassônico
  int distanceCM = sonar.ping_cm();
  
  // Converte pra milímetros (multiplica por 10)
  // Exemplo: 2cm → 20mm, 6cm → 60mm
  int distanceMM = distanceCM * 10;
  
  // GUARDA na lista de leituras
  readingsBuffer[readingIndex] = distanceMM;
  
  // Move pro próximo índice
  readingIndex = (readingIndex + 1) % 5;
  // Explica: Se era 4, vira 0 (volta pro começo)
  
  // CALCULA MÉDIA das 5 leituras
  int soma = 0;
  for (int i = 0; i < 5; i++) {
    soma += readingsBuffer[i];
  }
  lastDistanceMM = soma / 5;  // Média
}
```

**Visualizar o processo:**

```
PRIMEIRO UPDATE:
  readingsBuffer = [20, ?, ?, ?, ?]   (Primeira leitura)
  readingIndex = 1

SEGUNDO UPDATE:
  readingsBuffer = [20, 21, ?, ?, ?]  (Segunda leitura)
  readingIndex = 2

TERCEIRO UPDATE:
  readingsBuffer = [20, 21, 19, ?, ?] (Terceira leitura)
  readingIndex = 3

...

SEXTO UPDATE:
  readingsBuffer = [18, 21, 19, 20, 22]  (Todas cheias)
  readingIndex = 0 (volta pro começo)
  
  MÉDIA = (18+21+19+20+22) / 5 = 20

PRÓXIMO UPDATE:
  readingsBuffer = [20, 21, 19, 20, 22]  (substitui o primeiro)
```

---

```cpp
float WaterLevelSensor::getWaterLevel() {
  // Converte distância em MM para percentual
  
  // Mapeamento:
  // 20mm (cheio) = 100%
  // 60mm (vazio) = 0%
  
  float percentage = 100.0 - ((lastDistanceMM - RESERVOIR_FULL_MM) / 
                              (RESERVOIR_EMPTY_MM - RESERVOIR_FULL_MM)) * 100.0;
  
  // Garante que fica entre 0-100
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;
  
  return percentage;
}
```

**Fórmula prática:**

```
Se mede 20mm → 100%
Se mede 40mm → 50%
Se mede 60mm → 0%

Fórmula: 100 - ((medida - 20) / (60-20)) * 100
         100 - ((40 - 20) / 40) * 100
         100 - (20/40) * 100
         100 - 50
         = 50% ✓
```

---

```cpp
int WaterLevelSensor::getDistanceMM() {
  return lastDistanceMM;
}
```

**Simples:** Retorna a distância em milímetros.

---

```cpp
// CONSTRUTOR DO SENSOR DE SOLO
SoilMoistureSensor::SoilMoistureSensor(int pin, unsigned long debounceMs)
  : sensorPin(pin),
    lastReading(HIGH),        // Começa "seco"
    stableReading(HIGH),      // Confirmado "seco"
    lastChangeTime(0),        // Nunca mudou
    debounceDelay(debounceMs) {
}
```

---

```cpp
void SoilMoistureSensor::init() {
  pinMode(sensorPin, INPUT);  // Configura pino como ENTRADA
  Serial.println("[Sensor] Solo inicializado");
}
```

**Tradução:** Diz ao ESP32 "este pino é entrada de dados".

---

```cpp
void SoilMoistureSensor::update() {
  int reading = digitalRead(sensorPin);  // Lê o pino (HIGH ou LOW)
  
  // Se mudou da última leitura...
  if (reading != lastReading) {
    // ... resetar o timer de debounce
    lastChangeTime = millis();
    lastReading = reading;
  }
  
  // Se passou o tempo de debounce sem mudar...
  if ((millis() - lastChangeTime) >= debounceDelay) {
    // ... confirmar que é verdade
    stableReading = reading;
  }
}
```

**Visualizar:**

```
Sensor oscila:
  T0:    lê HIGH
  T10ms: lê LOW         ← Mudou!
  T20ms: lê HIGH        ← Mudou de novo!
  T30ms: lê LOW         ← Ainda oscilando...

Com debounce de 700ms:
  T0-T700ms: aguarda (não confirma nada)
  T700ms:    se ainda for LOW, confirma
  
  Resultado: ignora as oscilações, pega só o valor estável
```

---

```cpp
String SoilMoistureSensor::getStatus() {
  return stableReading == LOW ? "MOLHADO" : "SECO";
}

bool SoilMoistureSensor::isDry() {
  return stableReading == HIGH;
}

bool SoilMoistureSensor::isWet() {
  return stableReading == LOW;
}
```

**Lógica:**
```
Sensor = HIGH (não tem água) → "SECO"
Sensor = LOW (tem água) → "MOLHADO"
```

---

## 🎯 RESUMO sensors.cpp:

**Para quê?** Implementação de como ler e processar os sensores.

**Principais funções:**
1. `update()` - Lê o sensor e filtra o ruído
2. `getWaterLevel()` - Converte distância em percentual
3. `getStatus()` - Retorna "SECO" ou "MOLHADO"

---

---

# 4️⃣ system.h - DEFINIÇÃO DO SISTEMA

Aqui definimos como controlar a bomba e o buzzer.

```cpp
/**
 * @class IrrigationSystem
 * @brief Controla bomba, buzzer e modos
 */
class IrrigationSystem {
private:
  int relayPin;      // Pino do relé
  int buzzerPin;     // Pino do buzzer
  bool manualMode;   // Modo manual ou automático?
  bool pumpRunning;  // Bomba está ligada?
  unsigned long lastIrrigationTime;  // Quando irrigou por último
  unsigned long buzzerStartTime;     // Quando o buzzer ligou
  bool buzzerActive;                 // Buzzer está tocando?

public:
  IrrigationSystem(int relayPin, int buzzerPin);
  void init();
  void pumpStart();
  void pumpStop();
  bool getPumpStatus();
  void setManualMode(bool enable);
  bool getManualMode();
  void updateBuzzerAlert(bool active);
  String getLastIrrigationTime();
};
```

**Lógica:**
- Variáveis PRIVADAS guardam estado
- Funções PÚBLICAS são "controles remotos"

---

# 5️⃣ system.cpp - IMPLEMENTAÇÃO DO SISTEMA

```cpp
#include "system.h"

IrrigationSystem::IrrigationSystem(int relay, int buzzer)
  : relayPin(relay),
    buzzerPin(buzzer),
    manualMode(true),           // Começa em manual
    pumpRunning(false),         // Bomba desligada
    lastIrrigationTime(0),      // Nunca irrigou
    buzzerActive(false) {
}
```

---

```cpp
void IrrigationSystem::init() {
  pinMode(relayPin, OUTPUT);    // Relé é saída
  pinMode(buzzerPin, OUTPUT);   // Buzzer é saída
  digitalWrite(relayPin, HIGH); // Relé começa DESLIGADO (HIGH = OFF)
  digitalWrite(buzzerPin, LOW); // Buzzer começa MUDO
  Serial.println("[Sistema] Irrigacao inicializado");
}
```

**Importante:** Relé funciona de TRÁS PARA FRENTE:
- `HIGH` = Bomba DESLIGADA
- `LOW` = Bomba LIGADA

(Isto é "lógica invertida" do relé)

---

```cpp
void IrrigationSystem::pumpStart() {
  digitalWrite(relayPin, LOW);   // LOW = LIGADA
  pumpRunning = true;
  lastIrrigationTime = millis(); // Guarda quando ligou
  Serial.println("[Bomba] LIGADA");
}

void IrrigationSystem::pumpStop() {
  digitalWrite(relayPin, HIGH);  // HIGH = DESLIGADA
  pumpRunning = false;
  Serial.println("[Bomba] DESLIGADA");
}
```

---

```cpp
void IrrigationSystem::updateBuzzerAlert(bool active) {
  if (active) {
    // Se foi ativado agora, guarda o tempo
    if (!buzzerActive) {
      buzzerStartTime = millis();
      buzzerActive = true;
    }
    
    // Alterna HIGH-LOW a cada 1 segundo (beep-silêncio)
    unsigned long elapsed = millis() - buzzerStartTime;
    if ((elapsed / 1000) % 2 == 0) {
      digitalWrite(buzzerPin, HIGH);  // TOCA
    } else {
      digitalWrite(buzzerPin, LOW);   // SILENCIO
    }
  } else {
    // Desativa buzzer
    digitalWrite(buzzerPin, LOW);
    buzzerActive = false;
  }
}
```

**Explica o alerta:**
```
Tempo:  0-1s:  HIGH (toca)
        1-2s:  LOW (silência)
        2-3s:  HIGH (toca)
        ...
        
Resultado: beep-beep-beep em intervalo de 1 segundo
```

---

```cpp
String IrrigationSystem::getLastIrrigationTime() {
  if (lastIrrigationTime == 0) {
    return "Nunca";  // Nunca irrigou
  }
  
  unsigned long elapsed = millis() - lastIrrigationTime;
  
  if (elapsed < 60000) {
    // Menos de 1 minuto: "há Xs"
    return "há " + String(elapsed / 1000) + "s";
  } else if (elapsed < 3600000) {
    // Menos de 1 hora: "há Xmin"
    return "há " + String(elapsed / 60000) + "min";
  } else {
    // Mais de 1 hora: "há Xh"
    return "há " + String(elapsed / 3600000) + "h";
  }
}
```

**Exemplo:**
```
Se irrigou 5 segundos atrás → "há 5s"
Se irrigou 2 minutos atrás → "há 2min"
Se irrigou 1 hora atrás → "há 1h"
```

---

## 🎯 RESUMO system.h/cpp:

**Para quê?** Controlar bomba e buzzer.

**Funções principais:**
- `pumpStart()` / `pumpStop()` - Liga e desliga
- `updateBuzzerAlert()` - Faz beep intermitente
- `getLastIrrigationTime()` - Quanto tempo faz que irrigou

---

---

# 6️⃣ data_logger.h/cpp - HISTÓRICO

Aqui guardamos dados em um BUFFER CIRCULAR (tipo uma fita que volta ao começo).

```cpp
class DataLogger {
private:
  uint8_t soilHistory[60];      // Guarda 60 valores de solo
  uint8_t waterHistory[60];     // Guarda 60 valores de água
  uint8_t pumpHistory[60];      // Guarda 60 valores de bomba
  int currentIndex;             // Qual posição estou usando
  unsigned long lastSampleTime; // Quando foi a última amostra
  unsigned long sampleInterval; // Intervalo entre amostras (5000ms)

public:
  DataLogger(unsigned long intervalMs);
  void init(uint8_t initialSoil, uint8_t initialWater);
  void recordSample(uint8_t soil, uint8_t water, uint8_t pump);
  bool shouldRecord();
  // ... mais funções para pegar JSON
};
```

---

```cpp
void DataLogger::recordSample(uint8_t soil, uint8_t water, uint8_t pump) {
  // Guarda na posição atual
  soilHistory[currentIndex] = soil;
  waterHistory[currentIndex] = water;
  pumpHistory[currentIndex] = pump;
  
  // Move pro próximo índice (circular)
  currentIndex = (currentIndex + 1) % HISTORY_SIZE;
  // Se era 59, vira 0 (volta pro começo)
}
```

**Buffer Circular visualizado:**

```
Primeira hora (60 amostras guardadas):
  [0]=10%, [1]=11%, [2]=12%, ... [59]=15%

Hora 61 (substitui o [0]):
  [0]=14% (sobrescreveu o 10%)
  [1]=11%, [2]=12%, ... [59]=15%

Resultado: Sempre tem 5 minutos mais recentes!
```

---

```cpp
String DataLogger::getWaterHistoryJson() {
  // Cria string JSON com todos os dados
  String json = "[";
  
  for (int i = 0; i < 60; i++) {
    // Pega na ordem correta (do mais antigo pro mais novo)
    int index = (currentIndex + i) % 60;
    json += waterHistory[index];
    
    if (i < 59) {
      json += ",";  // Adiciona vírgula entre valores
    }
  }
  
  json += "]";
  return json;  // Retorna: [45,46,47,48,49,...]
}
```

**Resultado:**
```json
[45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,...]
```

---

## 🎯 RESUMO data_logger.h/cpp:

**Para quê?** Guardar histórico de 5 minutos em um buffer circular.

**Conceito:**
- Buffer circular = fita que volta ao começo
- Sempre atualizado com dados recentes
- Sem limite de memória (sempre sobrescreve)

---

---

# 7️⃣ web_server.h/cpp - SERVIDOR WEB (O MAIOR!)

Este é o arquivo mais complexo (~28KB). Aqui está TODA a interface web.

```cpp
class WebServerManager {
private:
  WaterLevelSensor* waterSensor;
  SoilMoistureSensor* soilSensor;
  IrrigationSystem* irrigation;
  DataLogger* logger;
  
  WebServer server;                    // Servidor HTTP
  LiquidCrystal_I2C lcd;              // LCD
  String lcd_line1, lcd_line2;        // Linhas do LCD em cache
  
  // Funções dos handlers (páginas)
  void handleRoot();
  void handleData();
  void handleChartData();
  // ... mais

public:
  WebServerManager(...);
  void init();
  void handleClient();
  void displayLcd(String line1, String line2);
};
```

---

### A ESTRUTURA DO HTML:

```cpp
void WebServerManager::handleRoot() {
  // Envia uma página HTML completa para o navegador
  String html = R"HTMLEOF(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Sistema Irrigacao</title>
      <meta charset="UTF-8">
      // ...CSS aqui...
    </head>
    <body>
      // ...HTML aqui...
      // ...JavaScript aqui...
    </body>
    </html>
  )HTMLEOF";
  
  server.send(200, "text/html", html);
}
```

**O que é `R"HTMLEOF( ... )HTMLEOF"`?**

É uma forma de guardar string muito grande sem escapar as aspas:

```cpp
// SEM isto: precisa escapar tudo
String html = "<html><head><title>\"Titulo\"</title></head>";

// COM isto: digita normal
String html = R"EOF(
  <html>
    <head>
      <title>"Titulo"</title>
    </head>
  </html>
)EOF";
```

---

### JSON ENDPOINT:

```cpp
void WebServerManager::handleData() {
  // Cria um JSON com os dados atuais
  float nivel = waterSensor->getWaterLevel();
  String solo = soilSensor->getStatus();
  bool bomba = irrigation->getPumpStatus();
  
  String json = "{";
  json += "\"status\":\"" + solo + "\",";
  json += "\"tempo\":\"" + irrigation->getLastIrrigationTime() + "\",";
  json += "\"nivel\":\"" + String(nivel, 1) + "\",";
  json += "\"manual\":\"" + (irrigation->getManualMode() ? "SIM" : "NAO") + "\",";
  json += "\"irrigando\":\"" + (bomba ? "SIM" : "NAO") + "\"";
  json += "}";
  
  server.send(200, "application/json", json);
}
```

**Resultado JSON:**
```json
{
  "status": "MOLHADO",
  "tempo": "há 5min",
  "nivel": "42.3",
  "manual": "NAO",
  "irrigando": "SIM"
}
```

---

### JAVASCRIPT PARA ATUALIZAR:

```javascript
// A cada 500ms, chama /data e atualiza a página
setInterval(() => {
  fetch('/data')
    .then(response => response.json())
    .then(data => {
      // Atualiza elementos HTML
      document.getElementById('status').textContent = data.status;
      document.getElementById('nivel').textContent = data.nivel + '%';
      document.getElementById('tempo').textContent = data.tempo;
    });
}, 500);  // A cada 500ms
```

**O que acontece:**
```
T0:    Chama /data
       Recebe: {"status":"MOLHADO",...}
       Atualiza HTML com esses valores

T500ms: Chama /data novamente
        ... (repete para sempre)
```

---

### GRÁFICOS COM CANVAS:

```javascript
// HTML:
<canvas id="chartWater" width="400" height="200"></canvas>

// JavaScript:
function drawChart() {
  const canvas = document.getElementById('chartWater');
  const ctx = canvas.getContext('2d');
  
  // Limpa canvas
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  
  // Desenha eixos
  ctx.strokeStyle = '#000';
  ctx.beginPath();
  ctx.moveTo(50, 10);
  ctx.lineTo(50, 180);
  ctx.lineTo(400, 180);
  ctx.stroke();
  
  // Desenha dados como linhas
  ctx.strokeStyle = '#0077be';
  ctx.beginPath();
  for (let i = 0; i < data.length; i++) {
    const x = 50 + (i * (canvas.width - 50) / data.length);
    const y = 180 - (data[i] * 170 / 100);
    
    if (i === 0) ctx.moveTo(x, y);
    else ctx.lineTo(x, y);
  }
  ctx.stroke();
}
```

---

## 🎯 RESUMO web_server.h/cpp:

**Para quê?** Toda a interface web, HTML, CSS, JavaScript e API JSON.

**Principais endpoints:**
- `/` - Dashboard (página principal)
- `/data` - JSON com dados atuais
- `/grafdata` - JSON com histórico
- `/ligar`, `/desligar` - Controlar bomba

---

---

# 8️⃣ wifi_manager.h/cpp - WIFI

```cpp
class WiFiManager {
private:
  String ssidStation;
  String passStation;
  String ssidAP;
  String passAP;
  unsigned long connectionTimeout;

public:
  WiFiManager(const char* stationSSID, const char* stationPass,
              const char* apSSID, const char* apPass, unsigned long timeoutMs);
  void init();
  bool isStationConnected();
  String getStationIP();
  String getAPIP();
};
```

---

```cpp
void WiFiManager::init() {
  // Ativa AMBOS os modos ao mesmo tempo
  WiFi.mode(WIFI_AP_STA);
  
  // Conecta à rede casa
  Serial.print("[WiFi] Conectando ao SSID: ");
  Serial.println(ssidStation);
  WiFi.begin(ssidStation.c_str(), passStation.c_str());
  
  // Espera até 20 segundos pela conexão
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && 
         millis() - startTime < connectionTimeout) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Se conseguiu, mostra IP
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[WiFi] Conectado!");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[WiFi] Falha, mas AP está ativo.");
  }
  
  // SEMPRE cria o Access Point
  WiFi.softAP(ssidAP.c_str(), passAP.c_str());
  Serial.print("[WiFi] AP SSID: ");
  Serial.println(ssidAP);
  Serial.print("[WiFi] AP IP: ");
  Serial.println(WiFi.softAPIP());
}
```

**O que acontece:**

```
CENÁRIO 1 (WiFi de casa disponível):
  └─ Se conecta → Acessa internet
  └─ AP também ativo → 192.168.4.1 funciona

CENÁRIO 2 (WiFi de casa indisponível):
  └─ Falha em conectar
  └─ Mas AP continua ligado! → 192.168.4.1 funciona
  └─ Sem internet, mas sistema funcionando

Resultado: NUNCA fica sem acesso!
```

---

## 🎯 RESUMO wifi_manager.h/cpp:

**Para quê?** Conectar a WiFi de forma robusta com dual mode.

**Estratégia:**
- Tenta conectar à rede casa
- Se falhar, não importa - AP local sempre funciona
- Acesso 100% garantido em 192.168.4.1

---

---

# 9️⃣ irrigacao_PB.ino - ARQUIVO MESTRE

Este é o "maestro" que coordena tudo.

```cpp
#include "config.h"
#include "sensors.h"
#include "system.h"
#include "data_logger.h"
#include "web_server.h"
#include "wifi_manager.h"

// INSTÂNCIAS GLOBAIS (ponteiros)
WaterLevelSensor* g_waterSensor = nullptr;
SoilMoistureSensor* g_soilSensor = nullptr;
IrrigationSystem* g_irrigationSystem = nullptr;
DataLogger* g_dataLogger = nullptr;
WebServerManager* g_webServer = nullptr;
WiFiManager* g_wifiManager = nullptr;
```

**Por que ponteiros?** Porque assim podem ser NULL no início e criados depois.

---

```cpp
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(500);
  
  // Mostra mensagem de boas-vindas
  Serial.println("\n\nSISTEMA DE IRRIGAÇÃO - RoboCore");
  
  // INICIALIZA SENSORES
  Serial.println("[INIT] Inicializando sensores...");
  g_waterSensor = new WaterLevelSensor(GPIO_ULTRASONIC_TRIG, GPIO_ULTRASONIC_ECHO, 
                                       ULTRASONIC_MAX_DISTANCE_CM);
  g_waterSensor->init();
  
  g_soilSensor = new SoilMoistureSensor(GPIO_SOIL_SENSOR, DEBOUNCE_SOIL_MS);
  g_soilSensor->init();
  
  // INICIALIZA SISTEMA
  Serial.println("[INIT] Inicializando bomba e buzzer...");
  g_irrigationSystem = new IrrigationSystem(GPIO_RELAY_PUMP, GPIO_BUZZER);
  g_irrigationSystem->init();
  
  // INICIALIZA HISTÓRICO
  Serial.println("[INIT] Inicializando data logger...");
  g_waterSensor->update();
  g_dataLogger = new DataLogger(HIST_SAMPLE_INTERVAL_MS);
  g_dataLogger->init((uint8_t)(g_soilSensor->isDry() ? 1 : 0), 
                     (uint8_t)round(g_waterSensor->getWaterLevel()));
  
  // INICIALIZA WiFi
  Serial.println("[INIT] Inicializando WiFi...");
  g_wifiManager = new WiFiManager(WIFI_SSID, WIFI_PASS, 
                                  WIFI_AP_SSID, WIFI_AP_PASS, WIFI_TIMEOUT_MS);
  g_wifiManager->init();
  
  // INICIALIZA WEB
  Serial.println("[INIT] Inicializando servidor web...");
  g_webServer = new WebServerManager(g_waterSensor, g_soilSensor, 
                                    g_irrigationSystem, g_dataLogger);
  g_webServer->init();
  
  Serial.println("[OK] Sistema pronto!\n");
}
```

**O que `setup()` faz:**
1. Cria todas as instâncias das classes
2. Passa os ponteiros para quem precisa
3. Inicializa tudo
4. Pronto pra operar!

---

```cpp
void loop() {
  // PASSO 1: Atualiza sensores
  g_waterSensor->update();
  g_soilSensor->update();
  
  // PASSO 2: Pega valores atuais
  float waterLevel = g_waterSensor->getWaterLevel();
  bool isDry = g_soilSensor->isDry();
  
  // PASSO 3: Lógica automática (se não estiver em modo manual)
  if (!g_irrigationSystem->getManualMode()) {
    
    // Proteção: água baixa?
    if (waterLevel <= WATER_LEVEL_MIN_ALERT) {
      if (g_irrigationSystem->getPumpStatus()) {
        g_irrigationSystem->pumpStop();
      }
      g_irrigationSystem->updateBuzzerAlert(true);  // Liga buzzer
      g_webServer->displayLcd("AGUA BAIXA", "BOMBA BLOQ");
    } 
    else {
      g_irrigationSystem->updateBuzzerAlert(false); // Desliga buzzer
      
      // Solo seco? Liga bomba
      if (isDry) {
        if (!g_irrigationSystem->getPumpStatus()) {
          g_irrigationSystem->pumpStart();
        }
        g_webServer->displayLcd("SOLO SECO", "IRRIGANDO");
      } 
      // Solo molhado? Desliga bomba
      else {
        if (g_irrigationSystem->getPumpStatus()) {
          g_irrigationSystem->pumpStop();
        }
        g_webServer->displayLcd("SOLO UMIDO", "BOMBA OFF");
      }
    }
  }
  
  // PASSO 4: Registra dados no histórico
  g_dataLogger->recordSample(
    (uint8_t)(isDry ? 1 : 0),
    (uint8_t)round(waterLevel),
    (uint8_t)(g_irrigationSystem->getPumpStatus() ? 1 : 0)
  );
  
  // PASSO 5: Processa requisições web
  g_webServer->handleClient();
  
  // PASSO 6: Log periódico
  static unsigned long lastLogTime = 0;
  if (millis() - lastLogTime >= 10000) {
    lastLogTime = millis();
    Serial.print("[LOG] Nível: ");
    Serial.print(waterLevel, 1);
    Serial.print("% | Solo: ");
    Serial.print(g_soilSensor->getStatus());
    Serial.print(" | Bomba: ");
    Serial.print(g_irrigationSystem->getPumpStatus() ? "LIGADA" : "DESLIGADA");
    Serial.print(" | Modo: ");
    Serial.println(g_irrigationSystem->getManualMode() ? "MANUAL" : "AUTOMÁTICO");
  }
  
  // PASSO 7: Dá tempo para o watchdog
  yield();
}
```

**Fluxo do loop:**

```
INFINITO:
  1. Lê sensores
  2. Pega valores
  3. Decide se liga/desliga bomba (lógica automática)
  4. Guarda no histórico
  5. Processa web (requisições HTTP)
  6. Mostra log a cada 10 segundos
  7. Dá tempo ao ESP32 respirar
  8. Volta ao 1
```

---

## 🎯 RESUMO irrigacao_PB.ino:

**Para quê?** Orquestar todo o sistema.

**Responsabilidades:**
- `setup()` - Cria tudo e inicializa
- `loop()` - Coordena a operação contínua

---

---

# 🎓 RESUMÃO DE TUDO:

| Arquivo | O QUE FEZ | LINHAS |
|---------|----------|--------|
| config.h | Valores e configurações | 80 |
| sensors.h/cpp | Ler temperatura e solo | 270 |
| system.h/cpp | Controlar bomba e buzzer | 170 |
| data_logger.h/cpp | Guardar histórico em buffer circular | 190 |
| web_server.h/cpp | Servidor web, HTML, CSS, JS, API JSON | 860 |
| wifi_manager.h/cpp | Conectar WiFi (AP+STA) | 90 |
| irrigacao_PB.ino | Maestro que coordena tudo | 150 |

**TOTAL: ~1.800 linhas de código profissional!**

---

## 💡 CONCEITOS PRINCIPAIS:

1. **OOP (Orientação a Objetos)** - Classes organizam dados e funções
2. **Headers (.h)** - Define a forma
3. **Implementation (.cpp)** - Define o funcionamento
4. **Ponteiros** - Passam endereços, não cópias
5. **Buffer Circular** - Guarda últimos N dados sem limite
6. **Debounce** - Ignora oscilações do sensor
7. **Averaging** - Filtra ruído lendo várias vezes
8. **JSON** - Formato padrão para dados web
9. **Dual WiFi** - AP local + internet simultâneos
10. **Logs estruturados** - [TAG] mensagem para debug

---

**Agora entende todo o projeto!** 🚀
