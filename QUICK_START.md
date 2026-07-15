# ⚡ QUICK START - Começar em 5 Minutos

## 🎯 Objetivo: Fazer Upload e Testar

---

## ✅ CHECKLIST PRÉ-UPLOAD

```
☐ Arduino IDE instalado
☐ ESP32 board support instalado
☐ Bibliotecas: LiquidCrystal_I2C, NewPing instaladas
☐ Todos os 17 arquivos na pasta
☐ Cabo USB conectado ao ESP32
```

---

## 🚀 PASSO 1: Abrir o Projeto

### No Arduino IDE:
1. **Arquivo → Abrir** (Ctrl+O)
2. Navegar para: `C:\Users\Usuario\Documents\teste_ardo\`
3. Selecionar: **irrigacao_PB.ino**
4. ✅ Pronto! IDE carrega todos os .h e .cpp automaticamente

---

## ⚙️ PASSO 2: Configurar Placa

### Board:
```
Tools → Board → esp32 → ESP32 Dev Module
```

### Porta COM:
```
Tools → Port → COM3 (ou qual seu ESP32 está conectado)
```

### Upload Speed:
```
Tools → Upload Speed → 115200 baud
```

### Partition:
```
Tools → Partition Scheme → Default 4MB with spiffs
```

---

## 📥 PASSO 3: Upload

### Opção 1 (Rápido):
```
Clique no botão: → (Carregar/Upload)
OU pressione: Ctrl+U
```

### Opção 2 (Com Verificação):
```
Sketch → Verify (Ctrl+R)  [Verifica erros]
Sketch → Upload (Ctrl+U)   [Faz upload]
```

### ⏳ Aguarde:
- ~30 segundos compilando
- "Hard resetting via RTS pin..." = sucesso!

---

## 🔍 PASSO 4: Verificar Serial

1. **Abrir Serial Monitor**: Ctrl+Shift+M
2. **Selecionar velocidade**: 115200 baud (canto inferior direito)
3. **Resetar ESP32**: Pressionar botão RESET

### Deve aparecer:
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
[WiFi] IP da estação: 192.168.1.XXX
[WiFi] AP SSID: ROBOCORE FARM
[WiFi] IP do AP: 192.168.4.1
[INIT] Inicializando servidor web...

[OK] Sistema pronto para operação!
```

✅ Se vir isto = **SUCESSO!**

---

## 🌐 PASSO 5: Acessar Interface Web

### Opção A: Rede Local (Casa)
1. Computador deve estar na mesma rede do ESP32
2. Abrir navegador
3. Digite: `192.168.1.XXX` (veja o IP no Serial)

### Opção B: Access Point
1. **Conectar WiFi**: `ROBOCORE FARM`
2. **Senha**: `12345678`
3. Abrir navegador: `http://192.168.4.1`

### Deve aparecer:
```
DASHBOARD - Sistema de Irrigação
├─ Status: PRONTO
├─ Nível de Água: XX%
├─ Solo: MOLHADO/SECO
├─ Bomba: DESLIGADA
└─ [Botão Ligar] [Botão Desligar]
```

---

## 🧪 PASSO 6: Testar Funções

### Teste 1: Ligar Bomba
- Clicar em **[Ligar]** no dashboard
- Deve ver: "Bomba LIGADA" 🔴
- Serial mostra: `Bomba: LIGADA`

### Teste 2: Buzzer
- Clicar em **[Testar Buzzer]**
- Deve ouvir 3 beeps curtos
- Serial mostra: `Buzzer ON` → `Buzzer OFF`

### Teste 3: Gráficos
- Clicar em **[Gráficos]**
- Aguardar 5 minutos para histórico se preencher
- Deve aparecer gráficos de água, solo e bomba

---

## 🔧 TROUBLESHOOTING RÁPIDO

### ❌ Erro: "Board not found"
```
→ Tools → Board Manager → Instalar "esp32"
```

### ❌ Erro: "Port not found"
```
→ Verificar se USB está conectado
→ Tentar outro port em Tools → Port
```

### ❌ Erro: "LiquidCrystal_I2C not found"
```
→ Tools → Manage Libraries → Instalar "LiquidCrystal I2C"
```

### ❌ Serial Monitor vazio
```
→ Selecionar velocidade 115200 baud
→ Pressionar botão RESET do ESP32
```

### ❌ WiFi "Falha na conexão"
```
→ Verificar SSID/senha em config.h
→ Serial mostre "Mantendo AP ativo" = AP funcionando
```

### ❌ Dashboard não carrega
```
→ Verificar IP: Serial Monitor mostra qual IP está usando
→ Tentar 192.168.4.1 se tudo falhar (AP local)
```

---

## 📊 STATUS ESPERADO

| Item | OK? |
|------|-----|
| Arduino IDE abre arquivo | ✅ |
| Compila sem erros | ✅ |
| Upload sucede | ✅ |
| Serial mostra logs | ✅ |
| LCD liga (se conectado) | ✅ |
| WiFi se conecta | ✅ |
| Dashboard abre | ✅ |
| Botões funcionam | ✅ |
| Sensor água lê 0-100% | ✅ |
| Sensor solo detecta | ✅ |
| Bomba pode ligar/desligar | ✅ |

Se todos ✅ = **SISTEMA FUNCIONANDO!** 🎉

---

## 🎮 CONTROLES DO DASHBOARD

| Botão | Função |
|-------|--------|
| **Ligar** | Liga bomba manualmente |
| **Desligar** | Desliga bomba manualmente |
| **Modo Manual** | Controle total via web |
| **Modo Automático** | Controle baseado em sensores |
| **Testar Buzzer** | Toca 3 beeps |

---

## 📈 PRÓXIMOS PASSOS

1. ✅ **Verificar compilação** ← Você está aqui
2. ⏭️ **Explorar interface** - Clique em cada página
3. ⏭️ **Aguardar 5 minutos** - Para gráficos se preencherem
4. ⏭️ **Testar em modo automático** - Ajustar sensor de solo
5. ⏭️ **Ler README.md** - Para entender configurações

---

## 💾 CONFIGURAÇÕES (Se Precisar Ajustar)

### Editar **config.h**:

```cpp
// Mudar WiFi
#define WIFI_SSID "SUA_REDE"
#define WIFI_PASS "SUA_SENHA"

// Mudar nível crítico de água
#define WATER_LEVEL_MIN_ALERT 25  // % mínimo

// Mudar tempo de debounce
#define DEBOUNCE_SOIL_MS 700  // milissegundos
```

Depois: **Sketch → Upload** novamente

---

## ✨ AGORA VOCÊ ESTÁ PRONTO!

```
┌─────────────────────────────────┐
│  🎉 SISTEMA FUNCIONANDO 🎉      │
│                                 │
│  ✅ Código compilado            │
│  ✅ Interface web ativa         │
│  ✅ Sensores lendo              │
│  ✅ Controles funcionando       │
│                                 │
│  Próximo: Ler README.md         │
│           para detalhes técnicos│
└─────────────────────────────────┘
```

---

## 📞 REFERÊNCIA DE CONTATO

- **Navegador em caso de erro**: Ctrl+Shift+I (abrir console)
- **Serial Monitor**: Ctrl+Shift+M (abrir logs)
- **Reset ESP32**: Botão no microcontrolador

---

**5 minutos e você está online! 🚀**

Divirta-se com seu sistema de irrigação! 🌾
