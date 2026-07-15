#include "espnow_manager.h"
#include "config.h"

ESPNOWManager* ESPNOWManager::s_instance = nullptr;

ESPNOWManager::ESPNOWManager() {
  for (int i = 0; i < MAX_SENSOR_NODES; i++) {
    m_sensorData[i].sensorId = i + 1;
    m_sensorData[i].soilAnalog = 0;
    m_sensorData[i].temperature = NAN;
    m_sensorData[i].humidity = NAN;
    m_sensorData[i].timestamp = 0;
    m_hasNewData[i] = false;
    m_lastReceiveTime[i] = 0;
    m_lastAcceptTime[i] = 0;
  }

  s_instance = this;
}

void ESPNOWManager::init() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("[ESPNOW] Falha na inicializacao");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
  Serial.println("[ESPNOW] Inicializado. MAC do mestre:");
  Serial.print("[ESPNOW] ");
  Serial.println(WiFi.macAddress());
}

void ESPNOWManager::onDataRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  if (!s_instance || len != sizeof(SensorDataPacket)) {
    return;
  }

  SensorDataPacket packet;
  memcpy(&packet, data, sizeof(SensorDataPacket));

  uint8_t id = packet.sensorId;
  if (id < 1 || id > MAX_SENSOR_NODES) {
    return; // evite Serial aqui também
  }

  uint8_t index = id - 1;
  s_instance->m_hasNewData[index] = true;
  s_instance->m_lastReceiveTime[index] = millis();

  unsigned long now = millis();
  if (now - s_instance->m_lastAcceptTime[index] >= 300) {
    s_instance->m_lastAcceptTime[index] = now;
    s_instance->m_sensorData[index].soilAnalog = packet.soilAnalog;
    s_instance->m_sensorData[index].temperature = packet.temperature;
    s_instance->m_sensorData[index].humidity = packet.humidity;
    s_instance->m_sensorData[index].timestamp = now;
  }
}

bool ESPNOWManager::hasNewData(uint8_t sensorId) {
  if (sensorId < 1 || sensorId > MAX_SENSOR_NODES) return false;
  bool data = m_hasNewData[sensorId - 1];
  m_hasNewData[sensorId - 1] = false;
  return data;
}

SensorDataPacket ESPNOWManager::getData(uint8_t sensorId) {
  if (sensorId < 1 || sensorId > MAX_SENSOR_NODES) {
    SensorDataPacket empty = {0, 0, NAN, NAN, 0};
    return empty;
  }
  return m_sensorData[sensorId - 1];
}

bool ESPNOWManager::isSensorOnline(uint8_t sensorId, unsigned long timeoutMs) {
  if (sensorId < 1 || sensorId > MAX_SENSOR_NODES) return false;
  return (millis() - m_lastReceiveTime[sensorId - 1]) < timeoutMs;
}

float ESPNOWManager::getAverageSoilPercent(uint16_t dryValue, uint16_t wetValue) {
  int count = 0;
  float sum = 0;

  for (int i = 0; i < MAX_SENSOR_NODES; i++) {
    uint8_t id = i + 1;
    if (isSensorOnline(id)) {
      float percent = 100.0f * (float)(dryValue - m_sensorData[i].soilAnalog) /
                      (float)(dryValue - wetValue);

      if (percent < 0.0f) percent = 0.0f;
      if (percent > 100.0f) percent = 100.0f;

      sum += percent;
      count++;
    }
  }

  if (count == 0) return -1.0f;

  return sum / (float)count;
}

float ESPNOWManager::getSoilPercent(uint8_t sensorId, uint16_t dryValue, uint16_t wetValue) {
  if (sensorId < 1 || sensorId > MAX_SENSOR_NODES) return -1.0f;
  if (!isSensorOnline(sensorId)) return -1.0f;

  uint8_t index = sensorId - 1;
  float percent = 100.0f * (float)(dryValue - m_sensorData[index].soilAnalog) /
                  (float)(dryValue - wetValue);

  if (percent < 0.0f) percent = 0.0f;
  if (percent > 100.0f) percent = 100.0f;

  return percent;
}

int ESPNOWManager::getOnlineSensorCount(unsigned long timeoutMs) {
  int count = 0;
  for (int i = 0; i < MAX_SENSOR_NODES; i++) {
    if ((millis() - m_lastReceiveTime[i]) < timeoutMs) {
      count++;
    }
  }
  return count;
}
