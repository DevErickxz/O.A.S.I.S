/**
 * @file data_logger.cpp
 * @brief Implementação do data logger
 * @author RoboCore
 * @version 1.0
 */

#include "data_logger.h"
#include "config.h"

DataLogger::DataLogger(unsigned long sampleIntervalMs)
    : currentIndex(0), totalSamples(0), lastSampleTime(0), 
      sampleInterval(sampleIntervalMs) {
  memset(soilHistory, 0, HISTORY_SIZE);
  memset(waterHistory, 0, HISTORY_SIZE);
  memset(pumpHistory, 0, HISTORY_SIZE);
}

void DataLogger::init(uint8_t initialSoil, uint8_t initialWater) {
  for (int i = 0; i < HISTORY_SIZE; i++) {
    soilHistory[i] = initialSoil;
    waterHistory[i] = initialWater;
    pumpHistory[i] = 0;
  }
  currentIndex = 0;
  totalSamples = HISTORY_SIZE;
  lastSampleTime = millis();
}

bool DataLogger::shouldRecord() {
  return (millis() - lastSampleTime) >= sampleInterval;
}

void DataLogger::recordSample(uint8_t soilValue, uint8_t waterValue, uint8_t pumpValue) {
  if (!shouldRecord()) {
    return;
  }

  lastSampleTime = millis();

  soilHistory[currentIndex] = soilValue;
  waterHistory[currentIndex] = waterValue;
  pumpHistory[currentIndex] = pumpValue;

  currentIndex = (currentIndex + 1) % HISTORY_SIZE;
  
  if (totalSamples < HISTORY_SIZE) {
    totalSamples++;
  }
}

const uint8_t* DataLogger::getSoilHistory() const {
  return soilHistory;
}

const uint8_t* DataLogger::getWaterHistory() const {
  return waterHistory;
}

const uint8_t* DataLogger::getPumpHistory() const {
  return pumpHistory;
}

uint8_t DataLogger::getTotalSamples() const {
  return totalSamples;
}

uint8_t DataLogger::getHistorySize() const {
  return HISTORY_SIZE;
}

String DataLogger::getSoilHistoryJson() const {
  String json = "[";
  uint8_t startIdx = (currentIndex + HISTORY_SIZE - totalSamples) % HISTORY_SIZE;

  for (uint8_t i = 0; i < totalSamples; i++) {
    uint8_t idx = (startIdx + i) % HISTORY_SIZE;
    json += String(soilHistory[idx]);
    if (i + 1 < totalSamples) json += ",";
  }

  json += "]";
  return json;
}

String DataLogger::getWaterHistoryJson() const {
  String json = "[";
  uint8_t startIdx = (currentIndex + HISTORY_SIZE - totalSamples) % HISTORY_SIZE;

  for (uint8_t i = 0; i < totalSamples; i++) {
    uint8_t idx = (startIdx + i) % HISTORY_SIZE;
    json += String(waterHistory[idx]);
    if (i + 1 < totalSamples) json += ",";
  }

  json += "]";
  return json;
}

String DataLogger::getPumpHistoryJson() const {
  String json = "[";
  uint8_t startIdx = (currentIndex + HISTORY_SIZE - totalSamples) % HISTORY_SIZE;

  for (uint8_t i = 0; i < totalSamples; i++) {
    uint8_t idx = (startIdx + i) % HISTORY_SIZE;
    json += String(pumpHistory[idx]);
    if (i + 1 < totalSamples) json += ",";
  }

  json += "]";
  return json;
}
