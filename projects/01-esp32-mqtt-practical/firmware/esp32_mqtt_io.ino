#include <WiFi.h>
#include <PubSubClient.h>

// =======================
// WIFI + MQTT SETTINGS
// =======================
const char* WIFI_SSID = "SEU_WIFI";
const char* WIFI_PASS = "SUA_SENHA";

// IP do seu PC (Mosquitto). Ex: "192.168.0.10"
const char* BROKER_HOST = "192.168.0.10";
const uint16_t BROKER_PORT = 1883;

// Base topic do projeto
const char* TOPIC_BASE = "lab/line1/m1";

// =======================
// PIN MAP (SEU FIRMWARE)
#define BT1 32
#define BT2 33
#define BT3 4
#define BT4 15
#define BT5 27
#define BT6 14
#define BT7 12
#define BT8 13

#define RL1 23
#define RL2 21
#define RL3 22
#define RL4 19
#define RL5 18
#define RL6 2
#define RL7 25
#define RL8 26

const int botoes[] = {BT1, BT2, BT3, BT4, BT5, BT6, BT7, BT8};
const int saidas[] = {RL1, RL2, RL3, RL4, RL5, RL6, RL7, RL8};
const int NUM_CANAIS = 8;

// =======================
// MODE
// true = toggle (pressiona alterna 0/1)
// false = momentâneo (pressionado=1, solto=0)
#define MODO_TOGGLE true

// =======================
// DEBOUNCE
unsigned long ultimoDebounce[8] = {0,0,0,0,0,0,0,0};
bool estadoAnteriorBotao[8] = {true,true,true,true,true,true,true,true};
const unsigned long TEMPO_DEBOUNCE = 50; // ms

// Estado lógico do canal (para toggle). O LED/relé final é aplicado via MQTT out/set.
bool estadoLogico[8] = {false,false,false,false,false,false,false,false};

// =======================
// MQTT
WiFiClient espClient;
PubSubClient mqtt(espClient);

// =======================
// TOPIC HELPERS
String topicIn(int i) {
  return String(TOPIC_BASE) + "/io/in/" + String(i);
}
String topicOutSet(int i) {
  return String(TOPIC_BASE) + "/io/out/" + String(i) + "/set";
}
String topicOutState(int i) {
  return String(TOPIC_BASE) + "/io/out/" + String(i);
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void mqttConnect() {
  while (!mqtt.connected()) {
    String clientId = "esp32-io-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    Serial.print("MQTT connecting as ");
    Serial.println(clientId);

    if (mqtt.connect(clientId.c_str())) {
      Serial.println("MQTT connected");

      // Subscribe comandos para acender/apagar saídas
      for (int i = 0; i < NUM_CANAIS; i++) {
        mqtt.subscribe(topicOutSet(i).c_str(), 0);
      }

      // Publica estado inicial das saídas (retained)
      for (int i = 0; i < NUM_CANAIS; i++) {
        publishOutState(i, digitalRead(saidas[i]) == HIGH ? 1 : 0, true);
      }

    } else {
      Serial.print("MQTT failed, rc=");
      Serial.println(mqtt.state());
      delay(1000);
    }
  }
}

int payloadTo01(const byte* payload, unsigned int len) {
  if (len == 0) return -1;
  if (payload[0] == '1') return 1;
  if (payload[0] == '0') return 0;

  String s;
  for (unsigned int i = 0; i < len; i++) s += (char)payload[i];
  s.toLowerCase();
  s.trim();
  if (s == "true" || s == "on") return 1;
  if (s == "false" || s == "off") return 0;
  return -1;
}

void publishIn(int i, int v) {
  // v: 1 pressionado, 0 solto (momentâneo)
  // ou v: estado lógico (toggle)
  String t = topicIn(i);
  char payload[2] = {(char)(v ? '1' : '0'), '\0'};
  mqtt.publish(t.c_str(), payload, false);
}

void publishOutState(int i, int v, bool retained) {
  String t = topicOutState(i);
  char payload[2] = {(char)(v ? '1' : '0'), '\0'};
  mqtt.publish(t.c_str(), payload, retained);
}

void applyOutput(int i, int v) {
  digitalWrite(saidas[i], v ? HIGH : LOW);
  publishOutState(i, v, true); // confirmação (retained)
  Serial.printf("OUT%d <= %d (via MQTT)\n", i + 1, v);
}

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  String t = String(topic);
  String prefix = String(TOPIC_BASE) + "/io/out/";
  if (!t.startsWith(prefix)) return;

  // t = lab/line1/m1/io/out/<i>/set
  String rest = t.substring(prefix.length()); // "<i>/set"
  int slash = rest.indexOf('/');
  if (slash <= 0) return;

  int idx = rest.substring(0, slash).toInt();
  if (idx < 0 || idx >= NUM_CANAIS) return;

  int v = payloadTo01(payload, length);
  if (v < 0) return;

  applyOutput(idx, v);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 IO via MQTT ===");

  // Botões com pull-up interno
  for (int i = 0; i < NUM_CANAIS; i++) {
    pinMode(botoes[i], INPUT_PULLUP);
    estadoAnteriorBotao[i] = digitalRead(botoes[i]);
    Serial.printf("BT%d GPIO %d\n", i + 1, botoes[i]);
  }

  // Saídas
  for (int i = 0; i < NUM_CANAIS; i++) {
    pinMode(saidas[i], OUTPUT);
    digitalWrite(saidas[i], LOW);
    Serial.printf("OUT%d GPIO %d\n", i + 1, saidas[i]);
  }

  wifiConnect();
  mqtt.setServer(BROKER_HOST, BROKER_PORT);
  mqtt.setCallback(onMqttMessage);
  mqttConnect();

  Serial.printf("Mode: %s\n", MODO_TOGGLE ? "TOGGLE" : "MOMENTARY");
  Serial.println("Ready.");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) wifiConnect();
  if (!mqtt.connected()) mqttConnect();
  mqtt.loop();

  for (int i = 0; i < NUM_CANAIS; i++) {
    bool leitura = digitalRead(botoes[i]);

    // mudança detectada + debounce
    if (leitura != estadoAnteriorBotao[i]) {
      if (millis() - ultimoDebounce[i] > TEMPO_DEBOUNCE) {
        ultimoDebounce[i] = millis();

        // Pull-up: pressionado = LOW
        bool pressed = (leitura == LOW);

        if (MODO_TOGGLE) {
          // no toggle, apenas na borda de descida (pressed)
          if (pressed) {
            estadoLogico[i] = !estadoLogico[i];
            publishIn(i, estadoLogico[i] ? 1 : 0);
            Serial.printf("BT%d pressed -> publish IN%d=%d\n", i + 1, i + 1, estadoLogico[i] ? 1 : 0);
          }
        } else {
          // momentâneo: publica pressionado e solto
          publishIn(i, pressed ? 1 : 0);
          Serial.printf("BT%d %s -> publish IN%d=%d\n", i + 1, pressed ? "pressed" : "released", i + 1, pressed ? 1 : 0);
        }

        estadoAnteriorBotao[i] = leitura;
      }
    }
  }

  delay(1);
}
