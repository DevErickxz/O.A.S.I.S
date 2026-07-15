/**
 * @file data_logger.h
 * @brief Gerenciamento do histórico e armazenamento de dados
 * @author RoboCore
 * @version 1.0
 */

#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <Arduino.h>

/**
 * @class DataLogger
 * @brief Armazena histórico de sensores para gráficos
 */
class DataLogger {
private:
  uint8_t soilHistory[150];
  uint8_t waterHistory[150];
  uint8_t pumpHistory[150];
  uint8_t currentIndex;
  uint8_t totalSamples;
  unsigned long lastSampleTime;
  unsigned long sampleInterval;

public:
  DataLogger(unsigned long sampleIntervalMs);
  void init(uint8_t initialSoil, uint8_t initialWater);
  void recordSample(uint8_t soilValue, uint8_t waterValue, uint8_t pumpValue);
  bool shouldRecord();
  
  // Getters para histórico
  const uint8_t* getSoilHistory() const;
  const uint8_t* getWaterHistory() const;
  const uint8_t* getPumpHistory() const;
  uint8_t getTotalSamples() const;
  uint8_t getHistorySize() const;
  
  // Gera JSON do histórico
  String getSoilHistoryJson() const;
  String getWaterHistoryJson() const;
  String getPumpHistoryJson() const;
};

#endif  // DATA_LOGGER_H
