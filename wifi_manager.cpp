/**
 * @file wifi_manager.cpp
 * @brief Implementação do WiFi manager
 * @author RoboCore
 * @version 1.0
 */

#include "wifi_manager.h"
#include "config.h"

WiFiManager::WiFiManager(const char* stationSSID, const char* stationPass,
                         const char* apSSID, const char* apPass, unsigned long timeoutMs)
    : ssidStation(stationSSID), passStation(stationPass),
      ssidAP(apSSID), passAP(apPass), connectionTimeout(timeoutMs) {
}

void WiFiManager::init() {
  // Configura modo simultâneo (AP + Station)
  WiFi.mode(WIFI_AP_STA);

  // Inicia conexão com a rede home
  Serial.print("[WiFi] Conectando ao SSID: ");
  Serial.println(ssidStation);
  WiFi.begin(ssidStation.c_str(), passStation.c_str());

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < connectionTimeout) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[WiFi] Conexão estabelecida!");
    Serial.print("[WiFi] IP da estação: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[WiFi] Falha na conexão. Mantendo AP ativo.");
  }

  // Cria o Access Point
  WiFi.softAP(ssidAP.c_str(), passAP.c_str(), WIFI_AP_CHANNEL);
  Serial.print("[WiFi] AP SSID: ");
  Serial.println(ssidAP);
  Serial.print("[WiFi] IP do AP: ");
  Serial.println(WiFi.softAPIP());
}

bool WiFiManager::isStationConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getStationIP() {
  return WiFi.localIP().toString();
}

String WiFiManager::getAPIP() {
  return WiFi.softAPIP().toString();
}
