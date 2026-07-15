#include "system.h"
#include "config.h"

IrrigationSystem::IrrigationSystem(uint8_t relayPin)
    : relayPin(relayPin), isPumpRunning(false),
      isManualMode(true), lastIrrigationTime(0) {
}

void IrrigationSystem::init() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
}

void IrrigationSystem::pumpStart() {
  isPumpRunning = true;
  lastIrrigationTime = millis();
  digitalWrite(relayPin, LOW);
}

void IrrigationSystem::pumpStop() {
  isPumpRunning = false;
  digitalWrite(relayPin, HIGH);
}

bool IrrigationSystem::getPumpStatus() {
  return isPumpRunning;
}

void IrrigationSystem::setManualMode(bool enable) {
  isManualMode = enable;
  if (enable) {
    pumpStop();
  }
}

bool IrrigationSystem::getManualMode() {
  return isManualMode;
}

String IrrigationSystem::getLastIrrigationTime() {
  if (lastIrrigationTime == 0) {
    return "Nunca";
  }

  unsigned long seconds = (millis() - lastIrrigationTime) / 1000;

  if (seconds < 60) {
    return "há " + String(seconds) + "s";
  }

  unsigned long minutes = seconds / 60;
  if (minutes < 60) {
    return "há " + String(minutes) + "min";
  }

  unsigned long hours = minutes / 60;
  return "há " + String(hours) + "h";
}
