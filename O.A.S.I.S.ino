#include "config.h"
#include "sensors.h"
#include "system.h"
#include "data_logger.h"
#include "espnow_manager.h"
#include "web_server.h"
#include "wifi_manager.h"

WaterLevelSensor* g_waterSensor1 = nullptr;
WaterLevelSensor* g_waterSensor2 = nullptr;
ESPNOWManager* g_espnowManager = nullptr;
IrrigationSystem* g_system1 = nullptr;
IrrigationSystem* g_system2 = nullptr;
DataLogger* g_dataLogger1 = nullptr;
DataLogger* g_dataLogger2 = nullptr;
WebServerManager* g_webServer = nullptr;
WiFiManager* g_wifiManager = nullptr;

unsigned long g_lastLcdToggle = 0;
uint8_t g_lcdArea = 0;

void updateBuzzer(bool anyAlert) {
  static unsigned long lastBeep = 0;
  static bool state = false;

  if (!anyAlert) {
    digitalWrite(GPIO_BUZZER, LOW);
    state = false;
    return;
  }

  if (millis() - lastBeep >= 1000) {
    lastBeep = millis();
    state = !state;
    digitalWrite(GPIO_BUZZER, state ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(500);

  Serial.println("\n\n");
  Serial.println("========================================================");
  Serial.println("  SISTEMA DE IRRIGACAO - 2 AREAS (ESP-NOW)");
  Serial.println("  ECIT Profª Auricelia Maria Da Costa - Caapora, PB");
  Serial.println("  Equipe RoboCore");
  Serial.println("========================================================");
  Serial.println();

  pinMode(GPIO_BUZZER, OUTPUT);
  digitalWrite(GPIO_BUZZER, LOW);

  Serial.println("[INIT] Inicializando sensores ultrassonicos...");
  g_waterSensor1 = new WaterLevelSensor(GPIO_ULTRASONIC1_TRIG, GPIO_ULTRASONIC1_ECHO,
                                        ULTRASONIC_MAX_DISTANCE_CM);
  g_waterSensor1->init();
  g_waterSensor2 = new WaterLevelSensor(GPIO_ULTRASONIC2_TRIG, GPIO_ULTRASONIC2_ECHO,
                                        ULTRASONIC_MAX_DISTANCE_CM);
  g_waterSensor2->init();

  Serial.println("[INIT] Inicializando sistemas de irrigacao...");
  g_system1 = new IrrigationSystem(GPIO_RELAY_PUMP1);
  g_system1->init();
  g_system2 = new IrrigationSystem(GPIO_RELAY_PUMP2);
  g_system2->init();

  Serial.println("[INIT] Inicializando data loggers...");
  g_waterSensor1->update();
  g_waterSensor2->update();
  g_dataLogger1 = new DataLogger(HIST_SAMPLE_INTERVAL_MS);
  g_dataLogger1->init(50, (uint8_t)round(g_waterSensor1->getWaterLevel()));
  g_dataLogger2 = new DataLogger(HIST_SAMPLE_INTERVAL_MS);
  g_dataLogger2->init(50, (uint8_t)round(g_waterSensor2->getWaterLevel()));

  Serial.println("[INIT] Inicializando WiFi (AP+STA)...");
  g_wifiManager = new WiFiManager(WIFI_SSID, WIFI_PASS,
                                  WIFI_AP_SSID, WIFI_AP_PASS, WIFI_TIMEOUT_MS);
  g_wifiManager->init();

  Serial.println("[INIT] Inicializando ESP-NOW...");
  g_espnowManager = new ESPNOWManager();
  g_espnowManager->init();

  Serial.println("[INIT] Inicializando servidor web...");
  g_webServer = new WebServerManager(
      g_waterSensor1, g_waterSensor2, g_espnowManager,
      g_system1, g_system2,
      g_dataLogger1, g_dataLogger2);
  g_webServer->init();

  Serial.println("[OK] Sistema pronto!");
  Serial.print("[ESPNOW] MAC do mestre: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  g_waterSensor1->update();
  g_waterSensor2->update();

  float waterLevel1 = g_waterSensor1->getWaterLevel();
  float waterLevel2 = g_waterSensor2->getWaterLevel();
  float soilPercent1 = g_espnowManager->getSoilPercent(1, SOIL_DRY_VALUE, SOIL_WET_VALUE);
  float soilPercent2 = g_espnowManager->getSoilPercent(2, SOIL_DRY_VALUE, SOIL_WET_VALUE);
  bool online1 = g_espnowManager->isSensorOnline(1);
  bool online2 = g_espnowManager->isSensorOnline(2);

  bool alert1 = !g_system1->getManualMode() && (waterLevel1 <= WATER_LEVEL_MIN_ALERT);
  bool alert2 = !g_system2->getManualMode() && (waterLevel2 <= WATER_LEVEL_MIN_ALERT);

  processArea(g_system1, waterLevel1, soilPercent1, online1, 1);
  processArea(g_system2, waterLevel2, soilPercent2, online2, 2);

  updateBuzzer(alert1 || alert2);

  if (millis() - g_lastLcdToggle >= 3000) {
    g_lastLcdToggle = millis();
    g_lcdArea = (g_lcdArea + 1) % NUM_AREAS;
  }

  if (alert1 || alert2) {
    g_webServer->displayLcd("AGUA BAIXA!", "BLOQUEIO TOTAL");
  } else if (g_lcdArea == 0) {
    String l1 = "A1: " + String((int)round(soilPercent1 >= 0 ? soilPercent1 : 0)) + "% ";
    l1 += String(waterLevel1, 0) + "%";
    String l2 = g_system1->getPumpStatus() ? "IRRIGANDO    " : "BOMBA OFF    ";
    g_webServer->displayLcd(l1, l2);
  } else {
    String l1 = "A2: " + String((int)round(soilPercent2 >= 0 ? soilPercent2 : 0)) + "% ";
    l1 += String(waterLevel2, 0) + "%";
    String l2 = g_system2->getPumpStatus() ? "IRRIGANDO    " : "BOMBA OFF    ";
    g_webServer->displayLcd(l1, l2);
  }

  g_dataLogger1->recordSample(
    (uint8_t)(soilPercent1 >= 0 ? (uint8_t)round(soilPercent1) : 0),
    (uint8_t)round(waterLevel1),
    (uint8_t)(g_system1->getPumpStatus() ? 1 : 0)
  );
  g_dataLogger2->recordSample(
    (uint8_t)(soilPercent2 >= 0 ? (uint8_t)round(soilPercent2) : 0),
    (uint8_t)round(waterLevel2),
    (uint8_t)(g_system2->getPumpStatus() ? 1 : 0)
  );

  g_webServer->handleClient();

  static unsigned long lastLog = 0;
  if (millis() - lastLog >= 10000) {
    lastLog = millis();
    Serial.print("[LOG] A1 agua:");
    Serial.print(waterLevel1, 0);
    Serial.print("% solo:");
    Serial.print(soilPercent1 >= 0 ? (int)round(soilPercent1) : -1);
    Serial.print("% bomba:");
    Serial.print(g_system1->getPumpStatus() ? "ON" : "OFF");
    Serial.print(" | A2 agua:");
    Serial.print(waterLevel2, 0);
    Serial.print("% solo:");
    Serial.print(soilPercent2 >= 0 ? (int)round(soilPercent2) : -1);
    Serial.print("% bomba:");
    Serial.print(g_system2->getPumpStatus() ? "ON" : "OFF");
    Serial.print(" | online:");
    Serial.print(online1 ? "1" : "0");
    Serial.println(online2 ? "1" : "0");
  }

  yield();
}

void processArea(IrrigationSystem* sys, float waterLevel, float soilPercent, bool online, int area) {
  if (sys->getManualMode()) return;

  if (waterLevel <= WATER_LEVEL_MIN_ALERT) {
    if (sys->getPumpStatus()) sys->pumpStop();
    return;
  }

  if (!online) {
    if (sys->getPumpStatus()) sys->pumpStop();
    return;
  }

  if (soilPercent < 0) {
    if (sys->getPumpStatus()) sys->pumpStop();
    return;
  }

  if (soilPercent < SOIL_DRY_PERCENT) {
    if (!sys->getPumpStatus()) sys->pumpStart();
  } else if (soilPercent > SOIL_WET_PERCENT) {
    if (sys->getPumpStatus()) sys->pumpStop();
  }
}
