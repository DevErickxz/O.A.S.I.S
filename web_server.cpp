#include "web_server.h"
#include "config.h"

WebServerManager* g_webServerInstance = nullptr;

void onRoot() { if (g_webServerInstance) g_webServerInstance->handleRoot(); }
void onArea1() { if (g_webServerInstance) g_webServerInstance->handleArea(1); }
void onArea2() { if (g_webServerInstance) g_webServerInstance->handleArea(2); }
void onInfo() { if (g_webServerInstance) g_webServerInstance->handleInfo(); }
void onCharts() { if (g_webServerInstance) g_webServerInstance->handleCharts(); }
void onForecast() { if (g_webServerInstance) g_webServerInstance->handleForecast(); }
void onOperation() { if (g_webServerInstance) g_webServerInstance->handleOperation(); }
void onStyle() { if (g_webServerInstance) g_webServerInstance->handleStyle(); }
void onScript() { if (g_webServerInstance) g_webServerInstance->handleScript(); }
void onData() { if (g_webServerInstance) g_webServerInstance->handleData(); }
void onChartData() { if (g_webServerInstance) g_webServerInstance->handleChartData(); }
void onManual() { if (g_webServerInstance) g_webServerInstance->handleSetManualMode(); }
void onAuto() { if (g_webServerInstance) g_webServerInstance->handleSetAutoMode(); }
void onLigar() { if (g_webServerInstance) g_webServerInstance->handlePumpStart(); }
void onDesligar() { if (g_webServerInstance) g_webServerInstance->handlePumpStop(); }
void onBuzzer() { if (g_webServerInstance) g_webServerInstance->handleTestBuzzer(); }

WebServerManager::WebServerManager(WaterLevelSensor* w1, WaterLevelSensor* w2,
                                   ESPNOWManager* espnow,
                                   IrrigationSystem* s1, IrrigationSystem* s2,
                                   DataLogger* d1, DataLogger* d2)
    : server(WEB_SERVER_PORT),
      lcd(I2C_ADDR_LCD, LCD_COLS, LCD_ROWS),
      waterSensor1(w1), waterSensor2(w2), espnowManager(espnow),
      system1(s1), system2(s2),
      dataLogger1(d1), dataLogger2(d2),
      lastLcdLine1(""), lastLcdLine2("") {
  g_webServerInstance = this;
}

void WebServerManager::init() {
  Wire.begin(GPIO_SDA, GPIO_SCL);
  lcd.init();
  lcd.backlight();
  displayLcd("INICIANDO...", "Servidor Web");

  server.on("/", onRoot);
  server.on("/area1", onArea1);
  server.on("/area2", onArea2);
  server.on("/info", onInfo);
  server.on("/graficos", onCharts);
  server.on("/previsao", onForecast);
  server.on("/operacao", onOperation);
  server.on("/style.css", onStyle);
  server.on("/script.js", onScript);
  server.on("/data", onData);
  server.on("/grafdata", onChartData);
  server.on("/modoManual", onManual);
  server.on("/modoAuto", onAuto);
  server.on("/ligar", onLigar);
  server.on("/desligar", onDesligar);
  server.on("/testarBuzzer", onBuzzer);

  server.begin();
  displayLcd("Servidor Web", "PRONTO");
}

void WebServerManager::handleClient() {
  server.handleClient();
}

void WebServerManager::displayLcd(const String& line1, const String& line2) {
  if (line1 == lastLcdLine1 && line2 == lastLcdLine2) return;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  lastLcdLine1 = line1;
  lastLcdLine2 = line2;
}

// ======================================================
// CSS compartilhado (cacheado pelo navegador)
// ======================================================

void WebServerManager::handleStyle() {
  server.send(200, "text/css",
    "*{box-sizing:border-box;margin:0;padding:0}"
    "body{font-family:Arial,Helvetica,sans-serif;background:#eef4ef;color:#1d2b1d;text-align:center;padding:0;min-height:100vh;padding-bottom:16px}"
    ".topbar{background:#1b5e20;color:#fff;padding:14px 16px;position:sticky;top:0;z-index:10}"
    ".topbar h1{margin:0;font-size:18px;color:#fff}"
    ".topbar .sub{font-size:11px;opacity:.7;margin-top:1px}"
    "h2{font-size:14px;font-weight:700;color:#1b5e20;margin:0 0 8px}"
    ".card{background:#fff;border-radius:14px;padding:14px;margin:10px auto;box-shadow:0 1px 6px rgba(0,0,0,.06);max-width:390px}"
    ".card-center{text-align:center}"
    "button{width:100%;max-width:290px;display:block;margin:8px auto;padding:12px 14px;border:none;border-radius:10px;font-size:14px;font-weight:700;cursor:pointer;color:#fff}"
    ".green{background:#2e7d32}.blue{background:#1976d2}.red{background:#c62828}.orange{background:#e65100}.gray{background:#607d8b}"
    ".sensor-row{display:flex;gap:8px;margin:6px 0}"
    ".sensor-card{flex:1;background:#f5faf6;border-radius:10px;padding:10px;text-align:center}"
    ".sensor-card .val{font-size:22px;font-weight:800;color:#1a2e1a}"
    ".sensor-card .lbl{font-size:10px;color:#64748b;text-transform:uppercase;margin-top:2px}"
    ".bar-wrap{margin:10px 0 2px}"
    ".bar-wrap .lbl-row{display:flex;justify-content:space-between;font-size:11px;font-weight:600;margin-bottom:3px;color:#475569}"
    ".bar{width:100%;height:26px;background:#e0e7e2;border-radius:13px;overflow:hidden}"
    ".bar-fill{height:100%;width:0%;border-radius:13px;display:flex;align-items:center;justify-content:center;font-size:11px;font-weight:700;color:#fff;transition:width .25s}"
    ".bar-fill.soil{background:#388e3c}.bar-fill.soil-low{background:#c62828}.bar-fill.soil-mid{background:#ef6c00}"
    ".bar-fill.water{background:#1565c0}.bar-fill.water-low{background:#c62828}"
    ".status-row{display:flex;align-items:center;justify-content:center;gap:6px;margin:4px 0;font-size:13px}"
    ".dot{width:8px;height:8px;border-radius:50%;display:inline-block}"
    ".dot.on{background:#2e7d32}.dot.off{background:#bdbdbd}"
    ".badge{display:inline-block;padding:3px 12px;border-radius:14px;font-size:11px;font-weight:700}"
    ".badge.on{background:#c8e6c9;color:#1b5e20}.badge.off{background:#f0f0f0;color:#9e9e9e}"
    ".state-box{text-align:center;padding:10px;border-radius:10px;margin:8px 0;font-size:17px;font-weight:800}"
    ".state-box.running{background:#e8f5e9;color:#1b5e20;border:1px solid #a5d6a7}"
    ".state-box.stopped{background:#ffebee;color:#c62828;border:1px solid #ef9a9a}"
    ".state-box .sub{font-size:12px;font-weight:500;margin-top:2px}"
    ".grp{width:100%;height:200px;display:block;border-radius:10px;background:#fbfdfb;margin:6px 0;border:1px solid #e0e7e2}"
    ".leg{font-size:11px;color:#64748b;margin:2px 0 6px}"
    ".leg span{display:inline-block;margin:0 5px}"
    ".leg .d1{display:inline-block;width:8px;height:3px;border-radius:1px;margin-right:3px}"
    "footer{font-size:11px;color:#2e7d32;margin:14px auto 4px;max-width:390px;opacity:.6}"
    "@media(max-width:420px){.sensor-card .val{font-size:19px}.topbar h1{font-size:16px}}"
  );
}

// ======================================================
// JS compartilhado (cacheado pelo navegador)
// ======================================================

void WebServerManager::handleScript() {
  server.send(200, "application/javascript",
    "var D={},gData={s1:[],s2:[],w1:[],w2:[],p1:[],p2:[]};"
    "function acao(cmd,area){fetch('/'+cmd+'?area='+area,{cache:'no-store'})}"
    "function fetchData(area,cb){"
    "fetch('/data?area='+area,{cache:'no-store'}).then(function(r){return r.json()}).then(function(d){"
    "D=d;if(cb)cb(d);"
    "var sp=d.solo,pct=sp>=0?sp:0,c='soil';"
    "if(sp<0)c='soil';else if(sp<=30)c='soil-low';else if(sp<=60)c='soil-mid';else c='soil';"
    "var e=document.getElementById('barSolo');"
    "e.style.width=pct+'%';e.innerText=sp>=0?sp+'%':'--';e.className='bar-fill '+c;"
    "var nv=parseFloat(d.niv);"
    "var e2=document.getElementById('barAgua');"
    "e2.style.width=nv+'%';e2.innerText=nv.toFixed(1)+'%';e2.className='bar-fill '+(nv<=20?'water-low':'water');"
    "document.getElementById('pctSolo').innerText=sp>=0?sp+'%':'--';"
    "document.getElementById('pctAgua').innerText=nv.toFixed(1)+'%';"
    "document.getElementById('statusText').innerText=sp<0?'Aguardando...':sp<=30?'Solo Seco':sp<=60?'Solo Umido':'Solo Ideal';"
    "document.getElementById('statusText').style.color=sp<0?'#64748b':sp<=30?'#c62828':sp<=60?'#ef6c00':'#2e7d32';"
    "document.getElementById('nivelText').innerText=nv<=20?'Reservatorio Baixo':'Reservatorio OK';"
    "document.getElementById('nivelText').style.color=nv<=20?'#c62828':'#1565c0';"
    "document.getElementById('badge').className='badge '+(d.on=='1'?'on':'off');"
    "document.getElementById('badge').innerText=d.on=='1'?'ONLINE':'OFFLINE';"
    "var dot=document.getElementById('dot');dot.className='dot '+(d.on=='1'?'on':'off');"
    "var st=d.on=='1'?'Sensor Online':'Sensor Offline';"
    "document.getElementById('sensorStatus').innerText=st;"
    "document.getElementById('temp').innerText=d.t!=null?d.t+'C':'--';"
    "document.getElementById('umid').innerText=d.u!=null?d.u+'%':'--';"
    "document.getElementById('tempo').innerText=d.tempo;"
    "var run=d.bomba=='1';"
    "document.getElementById('stateBox').className='state-box '+(run?'running':'stopped');"
    "document.getElementById('stateLabel').innerText=run?'IRRIGANDO':'PARADO';"
    "document.getElementById('stateSub').innerText=run?'Bomba ligada':'Bomba desligada';"
    "document.getElementById('autoCard').style.display=d.man=='1'?'none':'block';"
    "document.getElementById('manualCard').style.display=d.man=='1'?'block':'none';"
    "});}"
    "function fetchInfo(){"
    "fetch('/data',{cache:'no-store'}).then(function(r){return r.json()}).then(function(d){"
    "function st(id,on){var e=document.getElementById(id);e.className='badge '+(on?'on':'off');e.innerText=on?'ONLINE':'OFFLINE'}"
    "st('b1',d.on1=='1');st('b2',d.on2=='1');"
    "document.getElementById('t1').innerText=d.t1!=null?d.t1+'C':'--';"
    "document.getElementById('t2').innerText=d.t2!=null?d.t2+'C':'--';"
    "document.getElementById('u1').innerText=d.u1!=null?d.u1+'%':'--';"
    "document.getElementById('u2').innerText=d.u2!=null?d.u2+'%':'--';"
    "function dot2(id,on){var e=document.getElementById(id);e.className='dot '+(on?'on':'off')}"
    "dot2('dot1',d.on1=='1');dot2('dot2',d.on2=='1');"
    "});}"
    "function rc(c){var r=c.getBoundingClientRect(),d=window.devicePixelRatio||1;c.width=Math.floor(r.width*d);c.height=Math.floor(220*d);var x=c.getContext('2d');x.setTransform(d,0,0,d,0,0);return x}"
    "function drawCharts(id,series,max){"
    "var c=document.getElementById(id);if(!c)return;"
    "var x=rc(c),w=c.getBoundingClientRect().width,h=220,pl=40,pr=14,pt=18,pb=30;"
    "x.clearRect(0,0,w,h);"
    "var gl=max==1?[0,1]:[0,25,50,75,100];"
    "gl.forEach(function(v){var y=pt+(1-v/max)*(h-pt-pb);x.beginPath();x.moveTo(pl,y);x.lineTo(w-pr,y);x.strokeStyle='rgba(0,0,0,.08)';x.lineWidth=1;x.stroke();x.fillStyle='#5f6c60';x.font='11px Arial';x.textAlign='right';x.textBaseline='middle';x.fillText(v,pl-6,y)});"
    "var has=false;series.forEach(function(s){if(s.v&&s.v.length)has=true});"
    "if(!has){x.fillStyle='#5f6c60';x.font='14px Arial';x.textAlign='center';x.fillText('Aguardando...',w/2,h/2);return}"
    "var iw=w-pl-pr,ih=h-pt-pb;"
    "series.forEach(function(s){"
    "var v=s.v;if(!v||!v.length)return;var sx=v.length>1?iw/(v.length-1):iw;"
    "x.beginPath();v.forEach(function(y,i){var px=pl+i*sx,py=pt+(1-y/max)*ih;i?x.lineTo(px,py):x.moveTo(px,py)});"
    "x.strokeStyle=s.c;x.lineWidth=2.5;x.stroke();"
    "x.lineTo(pl+(v.length-1)*sx,pt+ih);x.lineTo(pl,pt+ih);x.closePath();x.fillStyle=s.f;x.fill();"
    "if(v.length<80)v.forEach(function(y,i){var px=pl+i*sx,py=pt+(1-y/max)*ih;x.beginPath();x.arc(px,py,2.5,0,Math.PI*2);x.fillStyle=s.c;x.fill()});"
    "});}"
    "function fetchGraf(){"
    "fetch('/grafdata',{cache:'no-store'}).then(function(r){return r.json()}).then(function(d){"
    "gData=d;"
    "drawCharts('cSolo',[{v:d.s1,c:'#2e7d32',f:'rgba(46,125,50,.08)'},{v:d.s2,c:'#ef6c00',f:'rgba(239,108,0,.08)'}],100);"
    "drawCharts('cAgua',[{v:d.w1,c:'#1565c0',f:'rgba(21,101,192,.08)'},{v:d.w2,c:'#e65100',f:'rgba(230,81,0,.08)'}],100);"
    "drawCharts('cBomba',[{v:d.p1,c:'#1976d2',f:'rgba(25,118,210,.08)'},{v:d.p2,c:'#c62828',f:'rgba(198,40,40,.08)'}],1);"
    "});}"
    "window.addEventListener('resize',function(){if(gData.s1.length)fetchGraf()});"
  );
}

// ======================================================
// HANDLERS DE AÇÃO
// ======================================================

void WebServerManager::handleSetManualMode() {
  int a = server.arg("area").toInt();
  if (a == 2) system2->setManualMode(true); else system1->setManualMode(true);
  server.send(200, "text/plain", "OK");
}

void WebServerManager::handleSetAutoMode() {
  int a = server.arg("area").toInt();
  if (a == 2) { system2->setManualMode(false); system2->pumpStop(); }
  else { system1->setManualMode(false); system1->pumpStop(); }
  server.send(200, "text/plain", "OK");
}

void WebServerManager::handlePumpStart() {
  int a = server.arg("area").toInt();
  if (a == 2) system2->pumpStart(); else system1->pumpStart();
  server.send(200, "text/plain", "OK");
}

void WebServerManager::handlePumpStop() {
  int a = server.arg("area").toInt();
  if (a == 2) system2->pumpStop(); else system1->pumpStop();
  server.send(200, "text/plain", "OK");
}

void WebServerManager::handleTestBuzzer() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(GPIO_BUZZER, HIGH);
    delay(50);
    digitalWrite(GPIO_BUZZER, LOW);
    delay(50);
  }
  server.send(200, "text/plain", "OK");
}

// ======================================================
// JSON
// ======================================================

void WebServerManager::handleData() {
  int a = server.arg("area").toInt();
  if (a == 1 || a == 2) {
    WaterLevelSensor* ws = (a == 1) ? waterSensor1 : waterSensor2;
    IrrigationSystem* sys = (a == 1) ? system1 : system2;
    SensorDataPacket sd = espnowManager->getData(a);
    float pct = espnowManager->getSoilPercent(a, SOIL_DRY_VALUE, SOIL_WET_VALUE);
    String r = "{\"solo\":" + String(pct<0?-1:(int)round(pct));
    r += ",\"on\":\"" + String(espnowManager->isSensorOnline(a)?"1":"0") + "\"";
    r += ",\"t\":" + String(isnan(sd.temperature)?"null":String(sd.temperature,1));
    r += ",\"u\":" + String(isnan(sd.humidity)?"null":String(sd.humidity,1));
    r += ",\"niv\":" + String(ws->getWaterLevel(),1);
    r += ",\"tempo\":\"" + sys->getLastIrrigationTime() + "\"";
    r += ",\"man\":\"" + String(sys->getManualMode()?"1":"0") + "\"";
    r += ",\"bomba\":\"" + String(sys->getPumpStatus()?"1":"0") + "\"}";
    server.send(200, "application/json", r);
    return;
  }
  SensorDataPacket s1 = espnowManager->getData(1);
  SensorDataPacket s2 = espnowManager->getData(2);
  String r = "{\"t1\":" + String(isnan(s1.temperature)?"null":String(s1.temperature,1));
  r += ",\"u1\":" + String(isnan(s1.humidity)?"null":String(s1.humidity,1));
  r += ",\"t2\":" + String(isnan(s2.temperature)?"null":String(s2.temperature,1));
  r += ",\"u2\":" + String(isnan(s2.humidity)?"null":String(s2.humidity,1));
  r += ",\"on1\":\"" + String(espnowManager->isSensorOnline(1)?"1":"0") + "\"";
  r += ",\"on2\":\"" + String(espnowManager->isSensorOnline(2)?"1":"0") + "\"";
  r += ",\"n1\":" + String(waterSensor1->getWaterLevel(),1);
  r += ",\"n2\":" + String(waterSensor2->getWaterLevel(),1);
  r += ",\"b1\":\"" + String(system1->getPumpStatus()?"1":"0") + "\"";
  r += ",\"b2\":\"" + String(system2->getPumpStatus()?"1":"0") + "\"";
  r += ",\"m1\":\"" + String(system1->getManualMode()?"1":"0") + "\"";
  r += ",\"m2\":\"" + String(system2->getManualMode()?"1":"0") + "\"";
  r += ",\"s1p\":" + String((int)round(espnowManager->getSoilPercent(1,SOIL_DRY_VALUE,SOIL_WET_VALUE)));
  r += ",\"s2p\":" + String((int)round(espnowManager->getSoilPercent(2,SOIL_DRY_VALUE,SOIL_WET_VALUE)));
  r += "}";
  server.send(200, "application/json", r);
}

void WebServerManager::handleChartData() {
  String r = "{";
  r += "\"s1\":" + dataLogger1->getSoilHistoryJson() + ",";
  r += "\"s2\":" + dataLogger2->getSoilHistoryJson() + ",";
  r += "\"w1\":" + dataLogger1->getWaterHistoryJson() + ",";
  r += "\"w2\":" + dataLogger2->getWaterHistoryJson() + ",";
  r += "\"p1\":" + dataLogger1->getPumpHistoryJson() + ",";
  r += "\"p2\":" + dataLogger2->getPumpHistoryJson() + ",";
  r += "\"c1\":" + String(dataLogger1->getTotalSamples()) + ",";
  r += "\"c2\":" + String(dataLogger2->getTotalSamples());
  r += "}";
  server.send(200, "application/json", r);
}

// ======================================================
// PÁGINAS HTML (leves - CSS e JS são cacheados)
// ======================================================

void WebServerManager::handleRoot() {
  server.send(200, "text/html",
    "<!DOCTYPE html><html lang='pt-BR'><head>"
    "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'>"
    "<title>Irrigação - RoboCore</title>"
    "<link rel='stylesheet' href='/style.css'>"
    "</head><body>"
    "<div class='topbar'><h1>Irrigação Automática</h1><div class='sub'>ROBOCORE · ECIT Auricélia</div></div>"
    "<div style='margin-top:10px'>"
    "<div class='card card-center'>"
    "<h2 style='text-align:center'>Selecione a Área</h2>"
    "<button class='green' onclick=\"window.location.href='/area1'\">Área 1</button>"
    "<button class='green' onclick=\"window.location.href='/area2'\">Área 2</button>"
    "</div>"
    "<div class='card card-center'>"
    "<button class='blue' onclick=\"window.location.href='/info'\">Informações</button>"
    "<button class='orange' onclick=\"window.location.href='/graficos'\">Gráficos</button>"
    "<button class='gray' onclick=\"window.location.href='/previsao'\">Previsão do Tempo</button>"
    "<button class='gray' onclick=\"window.location.href='/operacao'\">Operação</button>"
    "</div>"
    "</div>"
    "<footer>ROBOCORE · ECIT Profª Auricélia Maria da Costa</footer>"
    "</body></html>"
  );
}

void WebServerManager::handleArea(int num) {
  String n = String(num);
  String page = "<!DOCTYPE html><html lang='pt-BR'><head>";
  page += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'>";
  page += "<title>Área " + n + " - Irrigação</title>";
  page += "<link rel='stylesheet' href='/style.css'>";
  page += "</head><body>";
  page += "<div class='topbar'><h1>Área " + n + "</h1><div class='sub'>Monitoramento em tempo real</div></div>";
  page += "<div style='margin-top:10px'>";

  // Sensor status card
  page += "<div class='card'>";
  page += "<h2>Sensor Remoto</h2>";
  page += "<div class='status-row'><span class='dot off' id='dot'></span><span id='sensorStatus'>Aguardando...</span><span id='badge' class='badge off'>OFF</span></div>";
  page += "<div class='sensor-row'>";
  page += "<div class='sensor-card'><div class='val' id='temp'>--</div><div class='lbl'>Temperatura</div></div>";
  page += "<div class='sensor-card'><div class='val' id='umid'>--</div><div class='lbl'>Umidade do Ar</div></div>";
  page += "</div>";
  page += "</div>";

  // Soil card
  page += "<div class='card'>";
  page += "<h2>Umidade do Solo</h2>";
  page += "<div class='bar-wrap'><div class='lbl-row'><span>Seco</span><span class='pct' id='pctSolo'>0%</span><span>Molhado</span></div>";
  page += "<div class='bar'><div class='bar-fill soil' id='barSolo'>0%</div></div>";
  page += "<div style='font-size:13px;font-weight:600;margin-top:6px' id='statusText'>Aguardando...</div></div>";
  page += "</div>";

  // Water card
  page += "<div class='card'>";
  page += "<h2>Nível da Água</h2>";
  page += "<div class='bar-wrap'><div class='lbl-row'><span>Vazio</span><span class='pct' id='pctAgua'>0%</span><span>Cheio</span></div>";
  page += "<div class='bar'><div class='bar-fill water' id='barAgua'>0%</div></div>";
  page += "<div style='font-size:13px;font-weight:600;margin-top:6px' id='nivelText'>---</div></div>";
  page += "</div>";

  // State card
  page += "<div class='card'>";
  page += "<div class='state-box running' id='stateBox'><div id='stateLabel'>INICIANDO</div><div class='sub' id='stateSub'>---</div></div>";
  page += "<div class='status-row' style='font-size:13px;color:#64748b'>Ultima irrigacao: <span id='tempo' style='font-weight:700'>---</span></div>";
  page += "</div>";

  // Auto mode card
  page += "<div class='card card-center' id='autoCard'>";
  page += "<h2>Modo Automatico</h2>";
  page += "<p style='font-size:13px;color:#64748b;margin-bottom:8px'>Sistema controlando a irrigacao automaticamente</p>";
  page += "<button class='blue' onclick=\"acao('modoManual'," + n + ")\">Mudar para Manual</button>";
  page += "</div>";

  // Manual mode card
  page += "<div class='card card-center' id='manualCard' style='display:none'>";
  page += "<h2>Modo Manual</h2>";
  page += "<p style='font-size:13px;color:#64748b;margin-bottom:8px'>Controle a bomba manualmente</p>";
  page += "<button class='green' onclick=\"acao('ligar'," + n + ")\">Ligar Bomba</button>";
  page += "<button class='red' onclick=\"acao('desligar'," + n + ")\">Desligar Bomba</button>";
  page += "<button class='orange' onclick=\"acao('testarBuzzer',0)\">Testar Buzzer</button>";
  page += "<button class='gray' onclick=\"acao('modoAuto'," + n + ")\">Voltar Automatico</button>";
  page += "</div>";

  // Back button
  page += "<div class='card card-center'><button class='gray' onclick=\"window.location.href='/'\">Voltar ao Menu</button></div>";
  page += "</div>";
  page += "<footer>ROBOCORE · Área " + n + "</footer>";
  page += "<script src='/script.js'></script>";
  page += "<script>setInterval(function(){fetchData(" + n + ")},200);fetchData(" + n + ");</script>";
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void WebServerManager::handleInfo() {
  server.send(200, "text/html",
    "<!DOCTYPE html><html lang='pt-BR'><head>"
    "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'>"
    "<title>Informações</title><link rel='stylesheet' href='/style.css'>"
    "</head><body>"
    "<div class='topbar'><h1>Informações</h1><div class='sub'>Estado dos sensores remotos</div></div>"
    "<div style='margin-top:10px'>"
    "<div class='card'><h2>Sensores Remotos</h2>"
    "<div class='sensor-row'>"
    "<div class='sensor-card'><div class='val' style='font-size:15px'><span class='dot off' id='dot1'></span><span id='b1' class='badge off' style='margin-left:4px'>OFF</span></div><div class='lbl'>Sensor 1 (Area 1)</div><div class='val' id='t1'>--</div><div style='font-size:13px;color:#64748b' id='u1'>--</div></div>"
    "<div class='sensor-card'><div class='val' style='font-size:15px'><span class='dot off' id='dot2'></span><span id='b2' class='badge off' style='margin-left:4px'>OFF</span></div><div class='lbl'>Sensor 2 (Area 2)</div><div class='val' id='t2'>--</div><div style='font-size:13px;color:#64748b' id='u2'>--</div></div>"
    "</div></div>"
    "<div class='card card-center'>"
    "<div style='margin:8px 0;font-size:16px;font-weight:700'>IP: 192.168.4.1</div>"
    "<div style='margin:12px 0;font-size:15px;font-weight:600'>Equipe RoboCore</div>"
    "<div style='font-size:13px;color:#64748b;line-height:1.5'>Erick Marques · Abner Felipe · Bruno Silva</div>"
    "<div style='margin:14px 0 6px;font-size:12px;color:#64748b'>ECIT Profa Auricelia Maria da Costa · Caapora/PB</div>"
    "<button class='blue' onclick=\"window.location.href='/graficos'\">Gráficos</button>"
    "<button class='orange' onclick=\"window.location.href='/previsao'\">Previsao do Tempo</button>"
    "<button class='gray' onclick=\"window.location.href='/'\">Voltar</button>"
    "</div>"
    "</div>"
    "<footer>ROBOCORE · Informações do Sistema</footer>"
    "<script src='/script.js'></script>"
    "<script>setInterval(fetchInfo,1000);fetchInfo();</script>"
    "</body></html>"
  );
}

void WebServerManager::handleCharts() {
  server.send(200, "text/html",
    "<!DOCTYPE html><html lang='pt-BR'><head>"
    "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'>"
    "<title>Gráficos</title><link rel='stylesheet' href='/style.css'>"
    "</head><body>"
    "<div class='topbar'><h1>Gráficos</h1><div class='sub'>Historico em tempo real</div></div>"
    "<div style='margin-top:10px'>"
    "<div class='card'><h2>Umidade do Solo</h2><div class='leg'><span><span class='d1' style='background:#2e7d32'></span>Area 1</span><span><span class='d1' style='background:#ef6c00'></span>Area 2</span></div><canvas id='cSolo' class='grp'></canvas></div>"
    "<div class='card'><h2>Nivel da Agua</h2><div class='leg'><span><span class='d1' style='background:#1565c0'></span>Area 1</span><span><span class='d1' style='background:#e65100'></span>Area 2</span></div><canvas id='cAgua' class='grp'></canvas></div>"
    "<div class='card'><h2>Bomba</h2><div class='leg'><span><span class='d1' style='background:#1976d2'></span>Area 1</span><span><span class='d1' style='background:#c62828'></span>Area 2</span></div><canvas id='cBomba' class='grp'></canvas></div>"
    "<div class='card card-center'><button class='gray' onclick=\"window.location.href='/'\">Voltar ao Menu</button></div>"
    "</div>"
    "<footer>ROBOCORE · Gráficos do Sistema</footer>"
    "<script src='/script.js'></script>"
    "<script>setInterval(fetchGraf,500);fetchGraf();</script>"
    "</body></html>"
  );
}

void WebServerManager::handleForecast() {
  server.send(200, "text/html",
    "<!DOCTYPE html><html lang='pt-BR'><head>"
    "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'>"
    "<title>Previsão</title><link rel='stylesheet' href='/style.css'>"
    "<style>.hero{display:flex;align-items:center;justify-content:space-between;background:linear-gradient(135deg,#dbeafe,#eff6ff);border-radius:18px;padding:18px;margin-bottom:12px}.temp{font-size:40px;font-weight:800}.g2{display:grid;grid-template-columns:1fr 1fr;gap:10px}.g2-item{background:#f8fbff;border-radius:14px;padding:12px;border:1px solid #e2e8f0;text-align:center}</style>"
    "</head><body>"
    "<div class='topbar'><h1>Previsão do Tempo</h1><div class='sub'>João Pessoa - PB</div></div>"
    "<div style='margin-top:10px'>"
    "<div class='card card-center' style='max-width:460px'>"
    "<div class='hero'><div><div class='temp' id='tmp'>--°C</div><div id='clima' style='font-weight:600'>Carregando...</div></div></div>"
    "<div class='g2'>"
    "<div class='g2-item'><div class='lbl'>Umidade</div><div style='font-size:22px;font-weight:800' id='umd'>--</div></div>"
    "<div class='g2-item'><div class='lbl'>Vento</div><div style='font-size:22px;font-weight:800' id='vnt'>--</div></div>"
    "<div class='g2-item'><div class='lbl'>Chuva</div><div style='font-size:22px;font-weight:800' id='chv'>--</div></div>"
    "<div class='g2-item'><div class='lbl'>Atualizado</div><div style='font-size:22px;font-weight:800' id='atl'>--</div></div>"
    "</div>"
    "<button class='green' onclick=\"carregar()\">Atualizar</button>"
    "<button class='gray' onclick=\"window.location.href='/'\">Voltar</button>"
    "<p style='color:#64748b;margin-top:10px;font-size:13px' id='status'>Buscando...</p>"
    "</div></div>"
    "<script>"
    "function mw(c){switch(c){case 0:return{texto:'Ceu Limpo'};case 1:case 2:case 3:return{texto:'Parcialmente Nublado'};case 45:case 48:return{texto:'Neblina'};case 51:case 53:case 55:case 61:case 63:case 65:case 80:case 81:case 82:return{texto:'Chuva'};case 71:case 73:case 75:case 77:case 85:case 86:return{texto:'Neve'};default:return{texto:'Variavel'}}}"
    "var u='https://api.open-meteo.com/v1/forecast?latitude=-7.1195&longitude=-34.8450&current=temperature_2m,relative_humidity_2m,precipitation,weather_code,wind_speed_10m&timezone=America%2FRecife';"
    "async function carregar(){"
    "document.getElementById('status').innerText='Buscando...';"
    "try{"
    "var r=await fetch(u,{cache:'no-store'});if(!r.ok)throw Error();"
    "var d=await r.json(),a=d.current,c=mw(a.weather_code);"
    "document.getElementById('tmp').innerText=Math.round(a.temperature_2m)+'C';"
    "document.getElementById('clima').innerText=c.texto;"
    "document.getElementById('umd').innerText=Math.round(a.relative_humidity_2m)+'%';"
    "document.getElementById('vnt').innerText=Math.round(a.wind_speed_10m)+' km/h';"
    "document.getElementById('chv').innerText=Number(a.precipitation||0).toFixed(1)+' mm';"
    "document.getElementById('atl').innerText=new Date().toLocaleTimeString('pt-BR');"
    "document.getElementById('status').innerText='OK';"
    "}catch(e){document.getElementById('status').innerText='Erro'}"
    "}"
    "carregar();"
    "</script></body></html>"
  );
}

void WebServerManager::handleOperation() {
  server.send(200, "text/html",
    "<!DOCTYPE html><html lang='pt-BR'><head>"
    "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'>"
    "<title>Operação</title><link rel='stylesheet' href='/style.css'>"
    "</head><body>"
    "<div class='topbar'><h1>Operação do Sistema</h1><div class='sub'>Manual do usuario</div></div>"
    "<div style='margin-top:10px'>"
    "<div class='card' style='max-width:600px;text-align:left'>"
    "<h2>Como funciona</h2>"
    "<p style='line-height:1.7;font-size:15px;margin:8px 0;color:#374151'>O sistema monitora a umidade do solo e o nivel da agua em <b>duas areas independentes</b> usando ESP32 e sensores remotos via ESP-NOW.</p>"
    "<h2>Modo Automatico</h2>"
    "<p style='line-height:1.7;font-size:15px;margin:8px 0;color:#374151'>Cada area funciona de forma independente. Quando o solo fica abaixo de <b>40%</b> de umidade, a bomba liga. Acima de <b>70%</b>, desliga (histerese). Se o nivel da agua estiver baixo, a bomba e bloqueada e o buzzer dispara.</p>"
    "<h2>Modo Manual</h2>"
    "<p style='line-height:1.7;font-size:15px;margin:8px 0;color:#374151'>No modo manual, voce controla a bomba de cada area diretamente pela interface web.</p>"
    "<h2>Rede</h2>"
    "<p style='line-height:1.7;font-size:15px;margin:8px 0;color:#374151'>1. Conecte-se no WiFi <b>ROBOCORE FARM</b> (senha: 12345678).<br>2. Abra <b>192.168.4.1</b> no navegador.<br>3. Selecione a area que deseja monitorar.</p>"
    "</div>"
    "<div class='card card-center'><button class='gray' onclick=\"window.location.href='/'\">Voltar ao Menu</button></div>"
    "</div>"
    "<footer>ROBOCORE · Manual de Operação</footer>"
    "</body></html>"
  );
}
