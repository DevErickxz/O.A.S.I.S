#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

#include "sensor_config.h"

DHT g_dht(DHTPIN, DHTTYPE);

float g_lastTemperature = NAN;
float g_lastHumidity = NAN;
uint16_t g_lastSoilAnalog = 0;
unsigned long g_lastSendTime = 0;

typedef struct {
  uint8_t sensorId;
  uint16_t soilAnalog;
  float temperature;
  float humidity;
  uint32_t timestamp;
} __attribute__((packed)) SensorDataPacket;

uint8_t g_broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void initSerial() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(500);
  Serial.print("\n[SENSOR] ESP32 Sensor ");
  Serial.print(SENSOR_ID);
  Serial.println(" iniciado");
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("[SENSOR] WiFi modo STA (ESP-NOW)");
}

void initESPNOW() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERRO] Falha ao inicializar ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, g_broadcastMac, 6);
  peerInfo.channel = ESPNOW_CHANNEL;
  peerInfo.ifidx = WIFI_IF_STA;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("[ERRO] Falha ao adicionar peer broadcast");
    return;
  }

  Serial.println("[SENSOR] ESP-NOW inicializado (broadcast)");
}

void initSensors() {
  g_dht.begin();
  pinMode(SOIL_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  Serial.println("[SENSOR] DHT + sensor solo iniciados");
  delay(1000);
}

uint16_t readSoilAverage() {
  uint32_t sum = 0;

  for (int i = 0; i < SOIL_NUM_SAMPLES; i++) {
    sum += analogRead(SOIL_PIN);
    delay(5);
  }

  return (uint16_t)(sum / SOIL_NUM_SAMPLES);
}

void readDHT() {
  float temp = g_dht.readTemperature();
  float hum = g_dht.readHumidity();

  if (!isnan(temp)) {
    g_lastTemperature = temp;
  }
  if (!isnan(hum)) {
    g_lastHumidity = hum;
  }
}

void printReadings(uint16_t soilAnalog, float temp, float hum) {
  Serial.print("[SOLO] Analog: ");
  Serial.print(soilAnalog);
  Serial.print(" | Temp: ");
  Serial.print(temp, 1);
  Serial.print("°C | Umidade: ");
  Serial.print(hum, 1);
  Serial.println("%");
}

void sendViaESPNOW() {
  SensorDataPacket data;
  data.sensorId = SENSOR_ID;
  data.soilAnalog = g_lastSoilAnalog;
  data.temperature = g_lastTemperature;
  data.humidity = g_lastHumidity;
  data.timestamp = millis();

  esp_err_t result = esp_now_send(g_broadcastMac, (uint8_t*)&data, sizeof(data));

  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(10);
  digitalWrite(STATUS_LED_PIN, LOW);

  if (result == ESP_OK) {
    Serial.print("[ENVIO] Pacote ");
    Serial.print(sizeof(data));
    Serial.println(" bytes enviado via ESP-NOW");
  } else {
    Serial.println("[ERRO] Falha ao enviar pacote ESP-NOW");
  }
}

void collectAndSend() {
  g_lastSoilAnalog = readSoilAverage();
  readDHT();

  printReadings(g_lastSoilAnalog, g_lastTemperature, g_lastHumidity);
  sendViaESPNOW();
}

void setup() {
  initSerial();
  initWiFi();
  initESPNOW();
  initSensors();

  g_lastSendTime = millis();
  Serial.println("[SENSOR] Pronto! Enviando dados a cada ");
  Serial.print(SEND_INTERVAL_MS);
  Serial.println("ms");
  Serial.print("[SENSOR] Sensor ID: ");
  Serial.println(SENSOR_ID);

  collectAndSend();
}

void loop() {
  if (millis() - g_lastSendTime >= SEND_INTERVAL_MS) {
    g_lastSendTime = millis();
    collectAndSend();
  }

  yield();
}
