#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <NewPing.h>

class WaterLevelSensor {
private:
  NewPing sonar;
  int readingsBuffer[5];
  uint8_t readingIndex;
  unsigned int lastDistanceMM;

public:
  WaterLevelSensor(uint8_t trigPin, uint8_t echoPin, unsigned int maxDistance);
  void init();
  float getWaterLevel();
  unsigned int getDistanceMM();
  void update();
};

float soilAnalogToPercent(uint16_t analogValue, uint16_t dryValue, uint16_t wetValue);

#endif
