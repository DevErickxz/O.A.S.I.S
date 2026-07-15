#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "sensors.h"
#include "system.h"
#include "data_logger.h"
#include "espnow_manager.h"

class WebServerManager {
private:
  WebServer server;
  LiquidCrystal_I2C lcd;
  WaterLevelSensor* waterSensor1;
  WaterLevelSensor* waterSensor2;
  ESPNOWManager* espnowManager;
  IrrigationSystem* system1;
  IrrigationSystem* system2;
  DataLogger* dataLogger1;
  DataLogger* dataLogger2;
  String lastLcdLine1;
  String lastLcdLine2;

public:
  void handleRoot();
  void handleArea(int num);
  void handleInfo();
  void handleCharts();
  void handleForecast();
  void handleOperation();
  void handleStyle();
  void handleScript();
  void handleData();
  void handleChartData();
  void handleSetManualMode();
  void handleSetAutoMode();
  void handlePumpStart();
  void handlePumpStop();
  void handleTestBuzzer();

  WebServerManager(WaterLevelSensor* w1, WaterLevelSensor* w2,
                   ESPNOWManager* espnow,
                   IrrigationSystem* s1, IrrigationSystem* s2,
                   DataLogger* d1, DataLogger* d2);
  void init();
  void handleClient();
  void displayLcd(const String& line1, const String& line2);
};

#endif
