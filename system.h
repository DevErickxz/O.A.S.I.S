#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>

class IrrigationSystem {
private:
  uint8_t relayPin;
  bool isPumpRunning;
  bool isManualMode;
  unsigned long lastIrrigationTime;

public:
  IrrigationSystem(uint8_t relayPin);
  void init();
  void pumpStart();
  void pumpStop();
  bool getPumpStatus();
  void setManualMode(bool enable);
  bool getManualMode();
  String getLastIrrigationTime();
};

#endif
