# 📋 INSTRUÇÕES DE COMPILAÇÃO E UPLOAD

## ✅ Pré-requisitos

1. **Arduino IDE** (versão 2.0+)
2. **ESP32 Board Support Package** instalado
3. **Bibliotecas necessárias**:
   - Wire.h (incluída no ESP32)
   - WiFi.h (incluída no ESP32)
   - WebServer.h (incluída no ESP32)
   - LiquidCrystal_I2C.h (instalar via Gerenciador de Bibliotecas)
   - NewPing.h (instalar via Gerenciador de Bibliotecas)

---

## 📥 Instalação de Bibliotecas

### Via Arduino IDE:

1. Abrir Arduino IDE
2. Ir para: **Sketch → Include Library → Manage Libraries**
3. Procurar e instalar:
   - **LiquidCrystal I2C** (por Frank de Brabander)
   - **NewPing** (por Tim Eckel)

### Via Terminal (alternativo):

```bash
# Não é necessário, use o gerenciador da IDE
```

---

## ⚙️ Configuração da Placa

1. **Selecionar Board**:
   - Abrir: **Tools → Board → esp32 → "ESP32 Dev Module"**

2. **Configurar Porta**:
   - Abrir: **Tools → Port → COM** (escolher porta do ESP32)

3. **Configurar Velocidade**:
   - Abrir: **Tools → Upload Speed → 115200**

4. **Configurar Partição**:
   - Abrir: **Tools → Partition Scheme → "Default 4MB with spiffs"**

---

## 🔍 Verificação Antes do Upload

- ✅ Todos os 13 arquivos (.ino, .h, .cpp) na mesma pasta
- ✅ Placa selecionada: ESP32 Dev Module
- ✅ Porta COM correta
- ✅ Cabo USB conectado

---

## 🚀 Upload do Código

### Opção 1: Upload Simples
1. Clicar no botão **"Carregar" (→)** na barra de ferramentas
2. Aguardar mensagem: `"Hard resetting via RTS pin..."`

### Opção 2: Verificar antes de fazer upload
1. Clicar em **Sketch → Verificar** (ou Ctrl+R)
2. Se compilar sem erros, clicar em **Carregar**

### Opção 3: Ambiente Customizado
Se quiser usar VS Code com PlatformIO:

#### Criar arquivo `platformio.ini`:
```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200

lib_deps =
    Wire
    WiFi
    WebServer
    LiquidCrystal I2C
    NewPing
```

---

## 📊 Verificação de Upload

Após o upload, abrir **Serial Monitor** (Ctrl+Shift+M):
- Velocidade: **115200 baud**
- Deve aparecer:

```
╔════════════════════════════════════════════════════════════════╗
║         SISTEMA DE IRRIGAÇÃO AUTOMÁTICO - RoboCore            ║
║   ECIT Profª Auricélia Maria Da Costa - João Pessoa, PB       ║
╚════════════════════════════════════════════════════════════════╝

[INIT] Inicializando sensores...
[INIT] Inicializando sistema de bomba e buzzer...
[INIT] Inicializando data logger...
[INIT] Inicializando WiFi (AP+STA)...
[WiFi] Conectando ao SSID: ALAN - SMART.INTERNET
[WiFi] Conexão estabelecida!
[WiFi] IP da estação: 192.168.X.X
[WiFi] AP SSID: ROBOCORE FARM
[WiFi] IP do AP: 192.168.4.1
[INIT] Inicializando servidor web...

[OK] Sistema pronto para operação!
```

---

## 🌐 Teste da Interface Web

1. Conectar a um computador/smartphone à rede: **ROBOCORE FARM**
2. Abrir navegador e acessar: `http://192.168.4.1`
3. Deve aparecer o dashboard com:
   - Status da água
   - Status do solo
   - Controles da bomba
   - Links para outras páginas

---

## 🔧 Troubleshooting de Compilação

### Erro: "LiquidCrystal_I2C not found"
```
Solução: Tools → Manage Libraries → Buscar e instalar "LiquidCrystal I2C"
```

### Erro: "NewPing not found"
```
Solução: Tools → Manage Libraries → Buscar e instalar "NewPing"
```

### Erro: "board not found"
```
Solução: Preferences → URLs de Placas Adicionais →
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
Depois: Tools → Board Manager → Instalar "esp32"
```

### Erro: "Port not found"
```
Solução:
1. Verificar se USB está conectado
2. Instalar driver CH340 (para alguns modelos de ESP32)
3. Tentar porta diferente
```

### Erro: "timeout while receiving packet"
```
Solução:
1. Desligar e religar ESP32
2. Pressionar botão BOOT enquanto carrega
3. Trocar de porta USB
```

---

## 📝 Estrutura dos Arquivos para Compilação

```
teste_ardo/
├── irrigacao_PB.ino ..................... Arquivo principal
├── config.h ............................ Configurações
├── sensors.h ........................... Declaração sensores
├── sensors.cpp ......................... Implementação sensores
├── system.h ............................ Declaração sistema
├── system.cpp .......................... Implementação sistema
├── data_logger.h ....................... Declaração histórico
├── data_logger.cpp ..................... Implementação histórico
├── web_server.h ........................ Declaração web
├── web_server.cpp ...................... Implementação web
├── wifi_manager.h ...................... Declaração WiFi
├── wifi_manager.cpp .................... Implementação WiFi
├── README.md ........................... Documentação
└── COMPILACAO.md ....................... Este arquivo
```

---

## ✨ Dicas Importantes

1. **Salvar todos os arquivos** antes de fazer upload
2. **Fechar Serial Monitor** antes de fazer upload
3. **Esperar até 30 segundos** durante a compilação inicial
4. **Se der erro**, copiar mensagem de erro e buscar na documentação
5. **Usar Serial Monitor** para debugging

---

## 🎯 Checklist Final

- [ ] Arduino IDE instalado
- [ ] ESP32 board support instalado
- [ ] Bibliotecas LiquidCrystal_I2C e NewPing instaladas
- [ ] Todos os 13 arquivos na pasta
- [ ] Placa ESP32 selecionada
- [ ] Porta COM selecionada
- [ ] Velocidade 115200
- [ ] Código compilado sem erros
- [ ] Upload realizado com sucesso
- [ ] Serial Monitor mostra mensagens de inicialização
- [ ] Interface web acessível em 192.168.4.1

---

**Se tudo estiver OK, o sistema está pronto para uso! 🎉**
