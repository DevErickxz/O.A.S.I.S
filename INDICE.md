# INDICE DO PROJETO - SISTEMA DE IRRIGACAO AUTOMATICO (ESP-NOW)

## Estrutura Completa

### Mestre (pasta raiz - 14 arquivos)

| Arquivo | Tipo | Descrição |
|---------|------|-----------|
| **Teste.ino** | Sketch | Mestre - setup() e loop() |
| **config.h** | Header | Configurações e constantes |
| **sensors.h/cpp** | Par | WaterLevelSensor + soilAnalogToPercent() |
| **espnow_manager.h/cpp** | Par | Recepção ESP-NOW, média dos sensores |
| **system.h/cpp** | Par | Bomba, buzzer, modos manual/auto |
| **data_logger.h/cpp** | Par | Buffer circular 150 amostras |
| **web_server.h/cpp** | Par | Servidor HTTP, páginas, JSON API |
| **wifi_manager.h/cpp** | Par | WiFi AP+STA, canal fixo |

### Sensor Node (SensorNode/ - 2 arquivos)

| Arquivo | Descrição |
|---------|-----------|
| **SensorNode.ino** | Sketch do sensor (DHT + solo analógico) |
| **sensor_config.h** | Config: ID=1 ou 2, pinos, canais |

### Documentação

| Arquivo | Descrição |
|---------|-----------|
| **README.md** | Visão geral |
| **COMPILACAO.md** | Instruções de compilação |
| **ARQUITETURA.md** | Diagramas, fluxos, API |
| **INDICE.md** | Este arquivo |

---

## Visão Geral da Arquitetura

```
┌─────────────┐     ESP-NOW     ┌──────────────┐
│ Sensor Node ├────────────────→│    Mestre    │
│ (ID=1,2)    │   (broadcast)   │              │
│             │                 │ Web Server   │
│ DHT22 +     │                 │ LCD + Bomba  │
│ Solo Analog │                 │ Ultrassônico │
└─────────────┘                 └──────────────┘
```

## O Que Cada Arquivo Faz

### Mestre

**Teste.ino** - Orquestra tudo
- Cria instâncias para 2 áreas
- processArea() para cada área com histerese (40%-70%)
- Buzzer global (OR entre áreas)

**config.h** - Configurações centralizadas
- GPIO, WiFi, calibração do solo, ESP-NOW

**espnow_manager.h/cpp** - NOVO!
- Recebe dados de 2 sensores via ESP-NOW
- Calcula média percentual do solo
- Gerencia timeout de sensores online

**sensors.h/cpp** - Modificado
- SoilMoistureSensor removido (era digital local)
- soilAnalogToPercent() → converte 3600..1900 para 0..100%

**web_server.h/cpp** - Modificado
- Landing page com escolha de área (/)
- Dashboard por área (/area1, /area2)
- Gráficos com 2 séries sobrepostas
- JSON por área (/data?area=N)

### Sensor Node

**SensorNode.ino** - Cada ESP sensor
- Lê solo analógico (média 5 amostras)
- Lê DHT22 (temperatura + umidade)
- Envia pacote 15 bytes via ESP-NOW (broadcast)
- LED pisca no envio

**sensor_config.h** - Config do sensor
- `SENSOR_ID` (1 ou 2), pinos, canal, intervalo

---

## Config para Sensor 2

Antes de compilar para o segundo sensor, mude no `sensor_config.h`:
```cpp
#define SENSOR_ID 1   →   #define SENSOR_ID 2
```

## Endpoints JSON

**GET /data?area=N** (por área):
```json
{
  "soloPercent": 65,
  "online": "SIM",
  "temperatura": 28.5,
  "umidade": 72,
  "nivel": 42.3,
  "tempo": "há 5min",
  "manual": "NAO",
  "irrigando": "SIM"
}
```

**GET /data** (geral - ambas áreas):
```json
{
  "temperatura1": 28.5, "umidade1": 72, "online1": "SIM",
  "temperatura2": 27.8, "umidade2": 70, "online2": "NAO",
  "nivel1": 42.3, "nivel2": 38.1,
  "tempo1": "há 5min", "tempo2": "Nunca",
  "manual1": "NAO", "manual2": "SIM",
  "irrigando1": "SIM", "irrigando2": "NAO"
}
```

## Estatísticas

| Métrica | Valor |
|---------|-------|
| Arquivos (mestre + sensor) | 16 |
| ESP32s na rede | 3 (1 mestre + 2 sensores) |
| Linhas de código | ~2200 |
| Classes | 7 |
| Endpoints web | 14 |
| Comunicação | ESP-NOW broadcast |
| Dados por pacote | 15 bytes |

---

## Troubleshooting

| Problema | Solução |
|----------|---------|
| Sensor offline no dashboard | Verificar se sensor está ligado e no mesmo canal |
| Apenas 1 sensor aparece online | Verificar SENSOR_ID (ID 1 e ID 2, não ambos 1) |
| ESP-NOW não inicializa | WiFi deve estar em modo AP_STA |
| Solo mostra --% | Aguardar primeiro pacote ESP-NOW (até 600ms) |
| DHT mostra -- | Verificar pinagem 22, DHT pode estar falhando |
| Dashboard não carrega | Conectar em ROBOCORE FARM, abrir 192.168.4.1 |
