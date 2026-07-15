#include "sensors.h"
#include "config.h"

WaterLevelSensor::WaterLevelSensor(uint8_t trigPin, uint8_t echoPin, unsigned int maxDistance)
    : sonar(trigPin, echoPin, maxDistance), readingIndex(0), lastDistanceMM(0) {
  for (int i = 0; i < ULTRASONIC_NUM_READINGS; i++) {
    readingsBuffer[i] = 40;
  }
}

void WaterLevelSensor::init() {
}

void WaterLevelSensor::update() {
  int reading = sonar.ping_cm() * 10;

  if (reading == 0) {
    reading = lastDistanceMM;
  }

  readingsBuffer[readingIndex] = reading;
  readingIndex = (readingIndex + 1) % ULTRASONIC_NUM_READINGS;

  int sum = 0;
  for (int i = 0; i < ULTRASONIC_NUM_READINGS; i++) {
    sum += readingsBuffer[i];
  }
  lastDistanceMM = sum / ULTRASONIC_NUM_READINGS;
}

float WaterLevelSensor::getWaterLevel() {
  if (lastDistanceMM <= RESERVOIR_FULL_MM) {
    return 100.0f;
  }
  if (lastDistanceMM >= RESERVOIR_EMPTY_MM) {
    return 0.0f;
  }

  float level = ((float)(RESERVOIR_EMPTY_MM - lastDistanceMM) * 100.0f) /
                (RESERVOIR_EMPTY_MM - RESERVOIR_FULL_MM);

  if (level < 0.0f) level = 0.0f;
  if (level > 100.0f) level = 100.0f;

  return level;
}

unsigned int WaterLevelSensor::getDistanceMM() {
  return lastDistanceMM;
}

float soilAnalogToPercent(uint16_t analogValue, uint16_t dryValue, uint16_t wetValue) {
  if (analogValue >= dryValue) return 0.0f;
  if (analogValue <= wetValue) return 100.0f;

  float range = (float)(dryValue - wetValue);
  float percent = ((float)(dryValue - analogValue) * 100.0f) / range;

  if (percent < 0.0f) percent = 0.0f;
  if (percent > 100.0f) percent = 100.0f;

  return percent;
}
