#ifndef ESPNOW_MANAGER_H
#define ESPNOW_MANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#define MAX_SENSOR_NODES 2

typedef struct {
  uint8_t sensorId;
  uint16_t soilAnalog;
  float temperature;
  float humidity;
  uint32_t timestamp;
} __attribute__((packed)) SensorDataPacket;

class ESPNOWManager {
private:
  SensorDataPacket m_sensorData[MAX_SENSOR_NODES];
  bool m_hasNewData[MAX_SENSOR_NODES];
  unsigned long m_lastReceiveTime[MAX_SENSOR_NODES];

  unsigned long m_lastAcceptTime[MAX_SENSOR_NODES];

  static ESPNOWManager* s_instance;

  static void onDataRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len);

public:
  ESPNOWManager();
  void init();
  bool hasNewData(uint8_t sensorId);
  SensorDataPacket getData(uint8_t sensorId);
  bool isSensorOnline(uint8_t sensorId, unsigned long timeoutMs = 10000);
  float getAverageSoilPercent(uint16_t dryValue, uint16_t wetValue);
  float getSoilPercent(uint8_t sensorId, uint16_t dryValue, uint16_t wetValue);
  int getOnlineSensorCount(unsigned long timeoutMs = 10000);
};

#endif
