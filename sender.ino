// ESP32 DevKit V1 — WebServer + Морзе на RGB (UTF-8) [ESP32 core v3.x]
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ---------- Wi-Fi ----------
const char* WIFI_SSID = "Aboba_123";
const char* WIFI_PASS = "88888888";
const char* AP_SSID   = "ESP32-Morse";
const char* AP_PASS   = "12345678"; // минимум 8 символовss

// ---------- Пины RGB ----------
const int PIN_R = 32;   // R (PWM)
const int PIN_G = 27;   // G (PWM)
const int PIN_B = 33;   // B (PWM)

// Если светодиод с общим анодом — инвертируем ШИМ (0=вкл, 255=выкл)
bool COMMON_ANODE = false;

// ---------- Морзе ----------
const int DOT_TIME       = 1200;
const int DASH_TIME      = DOT_TIME;
const int SYMBOL_PAUSE   = DOT_TIME;
const int LETTER_PAUSE   = DOT_TIME ;
const int WORD_PAUSE     = DOT_TIME ;
const bool SHOW_BLUE_PAUSE = true;

// ---------- PWM (LEDC v3 API) ----------
const uint32_t PWM_FREQ_HZ = 1000;
const uint8_t  PWM_RES_BITS = 8;

inline uint8_t duty(uint8_t v) { return COMMON_ANODE ? (255 - v) : v; }
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  ledcWrite(PIN_R, duty(r));
  ledcWrite(PIN_G, duty(g));
  ledcWrite(PIN_B, duty(b));
}
void ledOff() { setColor(0,0,0); }

void sendSync() {
  for (int i = 0; i < 3; i++) {
    setColor(0,0,255);   // синий
    delay(500);
    ledOff();
    delay(500);
  }
}


static void pauseSymbol() {
  // только пауза между точками/тире, без синего
  // delay(SYMBOL_PAUSE);
}

void dot()  { setColor(255,0,0); delay(DOT_TIME);  ledOff(); }
void dash() { setColor(0,255,0); delay(DASH_TIME); ledOff(); }

// ---------- Таблица Морзе (русские, прописные) ----------
struct MorseRU { uint32_t cp; const char* code; };
static const MorseRU MORSE_RU[] = {
  {0x0410,".-"},{0x0411,"-..."},{0x0412,".--"},{0x0413,"--."},{0x0414,"-.."},
  {0x0415,"."},{0x0401,"."},{0x0416,"...-"},{0x0417,"--.."},{0x0418,".."},
  {0x0419,".---"},{0x041A,"-.-"},{0x041B,".-.."},{0x041C,"--"},{0x041D,"-."},
  {0x041E,"---"},{0x041F,".--."},{0x0420,".-."},{0x0421,"..."},{0x0422,"-"},
  {0x0423,"..-"},{0x0424,"..-."},{0x0425,"...."},{0x0426,"-.-."},{0x0427,"---."},
  {0x0428,"----"},{0x0429,"--.-"},{0x042A,"--.--"},{0x042B,"-.--"},{0x042C,"-..-"},
  {0x042D,"..-.."},{0x042E,"..--"},{0x042F,".-.-"}
};
static const struct { char ch; const char* code; } MORSE_NUM[] = {
  {'0',"-----"},{'1',".----"},{'2',"..---"},{'3',"...--"},{'4',"....-"},
  {'5',"....."},{'6',"-...."},{'7',"--..."},{'8',"---.."},{'9',"----."}
};

const char* morseByCp(uint32_t cp) {
  if (cp >= 0x0430 && cp <= 0x044F) cp -= 0x20; // а..я -> А..Я
  if (cp == 0x0451) cp = 0x0401;               // ё -> Ё
  for (size_t i=0;i<sizeof(MORSE_RU)/sizeof(MorseRU);i++)
    if (MORSE_RU[i].cp == cp) return MORSE_RU[i].code;
  if (cp >= '0' && cp <= '9')
    for (auto &n : MORSE_NUM) if ((uint32_t)n.ch == cp) return n.code;
  return nullptr;
}

// ---------- UTF-8 → кодпоинт ----------
uint32_t nextUtf8Cp(const char* s, size_t len, size_t &i) {
  if (i >= len) return 0;
  uint8_t c = (uint8_t)s[i++];
  if (c < 0x80) return c;
  if ((c & 0xE0) == 0xC0 && i < len) { uint8_t c2=(uint8_t)s[i++]; return ((c&0x1F)<<6)|(c2&0x3F); }
  if ((c & 0xF0) == 0xE0 && i+1 < len) { uint8_t c2=(uint8_t)s[i++],c3=(uint8_t)s[i++]; return ((c&0x0F)<<12)|((c2&0x3F)<<6)|(c3&0x3F); }
  if ((c & 0xF8) == 0xF0 && i+2 < len) { uint8_t c2=(uint8_t)s[i++],c3=(uint8_t)s[i++],c4=(uint8_t)s[i++]; return ((c&0x07)<<18)|((c2&0x3F)<<12)|((c3&0x3F)<<6)|(c4&0x3F); }
  return 0xFFFD;
}

void sendMorseUTF8(const String& s) {
  const char* buf = s.c_str();
  size_t len = s.length(), i = 0;
  bool hadWord = false; // был ли хотя бы один символ/слово
  sendSync();

  while (true) {
    uint32_t cp = nextUtf8Cp(buf, len, i);
    if (cp == 0) break;

    if (cp == ' ') {
      // между словами — синий
      if (SHOW_BLUE_PAUSE) {
        setColor(0,0,255);
        delay(WORD_PAUSE);
        ledOff();
      } else {
        delay(WORD_PAUSE);
      }
      hadWord = true;
      continue;
    }

    if (const char* code = morseByCp(cp)) {
      // каждая буква целиком — без паузы между точками/тире
      for (size_t j=0; code[j]; j++) {
        if (code[j]=='.') dot(); else dash();
      }

      // пауза между буквами (не словами)
      delay(LETTER_PAUSE);
      hadWord = true;
    }
  }

  // финальный синий после последнего слова
  if (hadWord && SHOW_BLUE_PAUSE) {
    setColor(0,0,255);
    delay(WORD_PAUSE);
    ledOff();
  }
}


// ---------- Веб-сервер ----------
WebServer server(80);
volatile bool morseBusy = false;
TaskHandle_t morseHandle = nullptr;


const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html><html lang="ru"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 Морзе RGB</title>
<style>
body{font-family:system-ui,-apple-system,Segoe UI,Roboto,Arial,sans-serif;margin:24px;line-height:1.4}
.card{max-width:720px;margin:auto;padding:20px;border:1px solid #ddd;border-radius:12px;box-shadow:0 4px 20px rgba(0,0,0,.05)}
h1{margin:0 0 12px}.muted{color:#666}
textarea{width:100%;font-size:16px;padding:10px;border:1px solid #bbb;border-radius:8px}
button{margin-top:12px;padding:10px 16px;font-size:16px;border:0;border-radius:10px;background:#0066ff;color:#fff;cursor:pointer}
</style></head><body>
<div class="card">
<h1>ESP32 Морзе RGB</h1>
<p class="muted">Введите текст (UTF-8, кириллица поддерживается) и отправьте.</p>
<form method="POST" action="/send">
  <textarea name="text" rows="4" placeholder="Пример: СИГНАЛ 123" required></textarea>
  <button type="submit">Отправить</button>
</form>
<p class="muted" id="st">Состояние: <span>—</span></p>
</div>
<script>
async function poll(){
  try{const r=await fetch('/status'); const j=await r.json();
      document.querySelector('#st span').textContent=j.busy?'передача идёт…':'свободен';}
  catch(e){document.querySelector('#st span').textContent='нет соединения';}
}
poll(); setInterval(poll,1000);
</script>
</body></html>
)HTML";

void handleRoot(){ server.send_P(200,"text/html; charset=utf-8",INDEX_HTML); }

void morseTask(void* param){
  String* sp=(String*)param;
  sendMorseUTF8(*sp);
  delete sp;
  morseBusy=false;
  vTaskDelete(nullptr);
}

void handleSend(){
  if(!server.hasArg("text")) { server.send(400,"text/plain; charset=utf-8","Ошибка: нет поля 'text'"); return; }
  if(morseBusy){ server.send(200,"text/plain; charset=utf-8","Занят: передача ещё идёт."); return; }
  String text=server.arg("text"); text.trim(); if(!text.length()){ server.send(400,"text/plain; charset=utf-8","Пустой текст"); return; }
  morseBusy=true; String* copy=new String(text);
  xTaskCreatePinnedToCore(morseTask,"morse",4096,copy,1,&morseHandle,1);
  server.send(200,"text/plain; charset=utf-8","OK: передача запущена");
}

void handleStatus(){ server.send(200,"application/json", String("{\"busy\":") + (morseBusy?"true":"false") + "}"); }
void handleNotFound(){ server.send(404,"text/plain; charset=utf-8","404 Not Found"); }

// ---------- setup / loop ----------
void setup(){
  Serial.begin(115200); delay(100);

  // Новый LEDC API (ESP32 core 3.x)
  ledcAttach(PIN_R, PWM_FREQ_HZ, PWM_RES_BITS);
  ledcAttach(PIN_G, PWM_FREQ_HZ, PWM_RES_BITS);
  ledcAttach(PIN_B, PWM_FREQ_HZ, PWM_RES_BITS);
  ledOff();

  // STA, затем AP при неудаче
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Подключаюсь к Wi-Fi \"%s\"...", WIFI_SSID);
  uint32_t t0=millis();
  while(WiFi.status()!=WL_CONNECTED && millis()-t0<15000){ Serial.print("."); delay(500); }
  Serial.println();
  if(WiFi.status()==WL_CONNECTED){
    Serial.print("STA IP: "); Serial.println(WiFi.localIP());
  } else {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/send", HTTP_POST, handleSend);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP сервер запущен на порту 80");
}

void loop(){
  // обязательно вызывать, чтобы веб-сервер отвечал на запросы
  server.handleClient(); // см. любые ESP32 WebServer примеры
}
