/**
 * @file wifi_manager.h
 * @brief Gerenciamento de conexão WiFi
 * @author RoboCore
 * @version 1.0
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

/**
 * @class WiFiManager
 * @brief Gerencia conexão WiFi (AP + Station simultâneos)
 */
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

#endif  // WIFI_MANAGER_H
