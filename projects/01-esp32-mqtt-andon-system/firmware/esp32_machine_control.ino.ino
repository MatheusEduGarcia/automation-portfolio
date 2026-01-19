#include <WiFi.h>
#include <PubSubClient.h>

// ============================================================================
// ESP32 MQTT Machine Control - VERSAO COM LOGICA INTEGRADA
// Controla Start/Stop/Output/Defect com contadores e estados
// ============================================================================

// =======================
// CONFIGURACOES WIFI/MQTT
// =======================
static const char* WIFI_SSID  = "GSEU_WIFI";
static const char* WIFI_PASS  = "SUA_SENHA";

static const char* BROKER_HOST = "1192.168.0.10";
static const uint16_t BROKER_PORT = 1883;

static const char* TOPIC_BASE = "lab/line1/m1";

// =======================
// MAPEAMENTO DE IO
// =======================
#define BT_START  32  // Botao Start
#define BT_STOP   33  // Botao Stop
#define BT_OUTPUT 4   // Botao Output/Ciclo
#define BT_DEFECT 15  // Botao Defect

#define LED_START  23  // LED Verde (Start)
#define LED_STOP   16  // LED Vermelho (Stop)
#define LED_OUTPUT 17  // LED Azul (Output)
#define LED_DEFECT 19  // LED Amarelo (Defect)

const int BOTOES[] = {BT_START, BT_STOP, BT_OUTPUT, BT_DEFECT};
const int LEDS[]   = {LED_START, LED_STOP, LED_OUTPUT, LED_DEFECT};
const char* NOMES[] = {"start", "stop", "output", "defect"};
const int NUM_IO = 4;

// =======================
// ESTADO DA MAQUINA
// =======================
struct MachineState {
  bool running;           // Maquina rodando?
  bool outputActive;      // Output ativo?
  uint32_t cycleCount;    // Contador de ciclos
  uint32_t defectCount;   // Contador de defeitos
  uint32_t startTime;     // Timestamp do start
  uint32_t totalRuntime;  // Runtime acumulado (ms)
} machine;

// =======================
// DEBOUNCE DOS BOTOES
// =======================
uint32_t lastChangeMs[NUM_IO];
bool lastReadState[NUM_IO];
bool lastStableState[NUM_IO];

// =======================
// TIMING
// =======================
static const uint32_t WIFI_RETRY_MS   = 15000;
static const uint32_t MQTT_RETRY_MS   = 2000;
static const uint32_t DEBOUNCE_MS     = 50;
static const uint32_t HEARTBEAT_MS    = 10000;
static const uint32_t DEFECT_LED_MS   = 3000;  // LED defeito fica 3s ligado

// =======================
// MQTT
// =======================
WiFiClient espClient;
PubSubClient mqtt(espClient);
static char clientId[48];

// =======================
// HELPERS: TOPICS
// =======================
String t_button(const char* name)    { return String(TOPIC_BASE) + "/button/" + name; }
String t_led_state(const char* name) { return String(TOPIC_BASE) + "/led/" + name; }
String t_led_set(const char* name)   { return String(TOPIC_BASE) + "/led/" + name + "/set"; }
String t_metrics()                   { return String(TOPIC_BASE) + "/metrics"; }
String t_status()                    { return String(TOPIC_BASE) + "/status"; }

// =======================
// FUNCOES AUXILIARES
// =======================
void buildClientId() {
  uint64_t mac = ESP.getEfuseMac();
  snprintf(clientId, sizeof(clientId), "esp32-m1-%08X", (uint32_t)mac);
}

bool ensureWiFi() {
  if (WiFi.status() == WL_CONNECTED) return true;

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);

  Serial.print("Conectando WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
    if (millis() - start > WIFI_RETRY_MS) {
      Serial.println("\nTimeout WiFi");
      return false;
    }
  }

  Serial.print("\nWiFi OK! IP: ");
  Serial.println(WiFi.localIP());
  return true;
}

int payloadTo01(const byte* payload, unsigned int len) {
  if (len == 0) return -1;
  if (payload[0] == '1') return 1;
  if (payload[0] == '0') return 0;
  return -1;
}

// =======================
// MQTT PUBLISH
// =======================
void publishButton(const char* button, bool pressed) {
  char payload[2] = {pressed ? '1' : '0', '\0'};
  mqtt.publish(t_button(button).c_str(), payload, false);
}

void publishLedState(const char* led, bool state) {
  char payload[2] = {state ? '1' : '0', '\0'};
  mqtt.publish(t_led_state(led).c_str(), payload, true);  // retained
}

void publishMetrics() {
  // Calcular runtime atual se maquina rodando
  uint32_t currentRuntime = 0;
  if (machine.running && machine.startTime > 0) {
    currentRuntime = (millis() - machine.startTime) / 1000;  // segundos
  }
  
  // Criar JSON com metricas
  char json[256];
  snprintf(json, sizeof(json),
    "{\"running\":%s,\"cycles\":%lu,\"defects\":%lu,\"runtime\":%lu,\"quality\":%.1f}",
    machine.running ? "true" : "false",
    machine.cycleCount,
    machine.defectCount,
    (machine.totalRuntime / 1000) + currentRuntime,
    machine.cycleCount > 0 ? 
      ((machine.cycleCount - machine.defectCount) * 100.0 / machine.cycleCount) : 100.0
  );
  
  mqtt.publish(t_metrics().c_str(), json, true);
}

void publishStatus(const char* state) {
  mqtt.publish(t_status().c_str(), state, true);
}

// =======================
// CONTROLE DE LED
// =======================
void setLED(int ledIndex, bool state) {
  digitalWrite(LEDS[ledIndex], state ? HIGH : LOW);
  publishLedState(NOMES[ledIndex], state);
}

// =======================
// LOGICA DA MAQUINA
// =======================
void handleStart() {
  if (!machine.running) {
    machine.running = true;
    machine.startTime = millis();
    
    // LEDs
    setLED(0, true);   // Start verde ON
    setLED(1, false);  // Stop vermelho OFF
    
    Serial.println("MAQUINA INICIADA");
    publishMetrics();
  }
}

void handleStop() {
  if (machine.running) {
    // Calcular runtime desta sessao
    if (machine.startTime > 0) {
      machine.totalRuntime += (millis() - machine.startTime);
    }
    
    machine.running = false;
    machine.outputActive = false;
    machine.startTime = 0;
    
    // LEDs
    setLED(0, false);  // Start verde OFF
    setLED(1, true);   // Stop vermelho ON
    setLED(2, false);  // Output OFF
    
    Serial.printf("MAQUINA PARADA | Runtime: %lu s\n", 
                  machine.totalRuntime / 1000);
    publishMetrics();
  }
}

void handleOutput() {
  if (machine.running) {
    // Incrementar contador de ciclos
    machine.cycleCount++;
    
    // Toggle LED output (pisca)
    machine.outputActive = !machine.outputActive;
    setLED(2, machine.outputActive);
    
    Serial.printf("OUTPUT #%lu | Qualidade: %.1f%%\n", 
                  machine.cycleCount,
                  machine.cycleCount > 0 ? 
                    ((machine.cycleCount - machine.defectCount) * 100.0 / machine.cycleCount) : 100.0);
    
    publishMetrics();
  }
}

void handleDefect() {
  // Incrementar contador
  machine.defectCount++;
  
  // Ligar LED defeito
  setLED(3, true);
  
  Serial.printf("DEFEITO #%lu | Qualidade: %.1f%%\n", 
                machine.defectCount,
                machine.cycleCount > 0 ? 
                  ((machine.cycleCount - machine.defectCount) * 100.0 / machine.cycleCount) : 0.0);
  
  publishMetrics();
}

// =======================
// MQTT CALLBACK
// =======================
void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  String t(topic);
  String prefix = String(TOPIC_BASE) + "/led/";
  
  if (!t.startsWith(prefix)) return;
  
  String rest = t.substring(prefix.length());
  int slash = rest.indexOf('/');
  if (slash <= 0) return;
  
  String ledName = rest.substring(0, slash);
  int v = payloadTo01(payload, length);
  if (v < 0) return;
  
  // Encontrar indice do LED
  for (int i = 0; i < NUM_IO; i++) {
    if (ledName == NOMES[i]) {
      setLED(i, v == 1);
      break;
    }
  }
}

// =======================
// MQTT CONNECT
// =======================
bool ensureMQTT() {
  if (mqtt.connected()) return true;
  if (WiFi.status() != WL_CONNECTED) return false;

  mqtt.setServer(BROKER_HOST, BROKER_PORT);
  mqtt.setCallback(onMqttMessage);

  Serial.print("Conectando MQTT...");
  
  String willTopic = t_status();
  bool ok = mqtt.connect(clientId, willTopic.c_str(), 1, true, "offline");
  
  if (!ok) {
    Serial.printf(" FALHOU (rc=%d)\n", mqtt.state());
    return false;
  }

  Serial.println(" OK!");

  // Subscribe aos comandos de LED
  for (int i = 0; i < NUM_IO; i++) {
    mqtt.subscribe(t_led_set(NOMES[i]).c_str(), 0);
  }

  // Publicar status online e estados iniciais
  publishStatus("online");
  for (int i = 0; i < NUM_IO; i++) {
    publishLedState(NOMES[i], digitalRead(LEDS[i]));
  }
  publishMetrics();

  return true;
}

// =======================
// INICIALIZACAO
// =======================
void initIO() {
  Serial.println("\n=== Configurando I/O ===");
  
  // Botoes com pull-up
  for (int i = 0; i < NUM_IO; i++) {
    pinMode(BOTOES[i], INPUT_PULLUP);
    lastChangeMs[i] = 0;
    lastReadState[i] = HIGH;
    lastStableState[i] = HIGH;
  }
  
  // LEDs
  for (int i = 0; i < NUM_IO; i++) {
    pinMode(LEDS[i], OUTPUT);
    digitalWrite(LEDS[i], LOW);
  }
  
  // Estado inicial: Maquina parada (LED Stop ligado)
  machine.running = false;
  machine.outputActive = false;
  machine.cycleCount = 0;
  machine.defectCount = 0;
  machine.startTime = 0;
  machine.totalRuntime = 0;
  
  digitalWrite(LED_STOP, HIGH);  // Vermelho ON
  
  Serial.println("I/O configurado!\n");
}

// =======================
// SCAN BOTOES
// =======================
void scanButtons() {
  uint32_t now = millis();

  for (int i = 0; i < NUM_IO; i++) {
    bool currentRead = digitalRead(BOTOES[i]);

    if (currentRead != lastReadState[i]) {
      lastReadState[i] = currentRead;
      lastChangeMs[i] = now;
    }

    if ((now - lastChangeMs[i]) > DEBOUNCE_MS) {
      if (currentRead != lastStableState[i]) {
        lastStableState[i] = currentRead;
        bool pressed = (currentRead == LOW);  // Pull-up

        if (mqtt.connected()) {
          publishButton(NOMES[i], pressed);
        }

        // Executar logica apenas no PRESS
        if (pressed) {
          switch(i) {
            case 0: handleStart();  break;
            case 1: handleStop();   break;
            case 2: handleOutput(); break;
            case 3: handleDefect(); break;
          }
        }
      }
    }
  }
}

// =======================
// GERENCIAR LED DEFEITO
// =======================
uint32_t defectLedOnTime = 0;

void manageDefectLED() {
  // Se LED defeito esta ligado, verificar se passou 3s
  if (digitalRead(LED_DEFECT) == HIGH) {
    if (defectLedOnTime == 0) {
      defectLedOnTime = millis();
    } else if (millis() - defectLedOnTime > DEFECT_LED_MS) {
      setLED(3, false);  // Desligar LED defeito
      defectLedOnTime = 0;
    }
  }
}

// =======================
// SETUP E LOOP
// =======================
uint32_t lastMqttTry = 0;
uint32_t lastHeartbeat = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n========================================");
  Serial.println("  ESP32 Machine Control - v2.0");
  Serial.println("  Start/Stop/Output/Defect com Logica");
  Serial.println("========================================\n");

  buildClientId();
  Serial.printf("Client ID: %s\n\n", clientId);

  initIO();
  ensureWiFi();
  ensureMQTT();

  Serial.println("=== Sistema Pronto! ===\n");
}

void loop() {
  // WiFi
  if (WiFi.status() != WL_CONNECTED) {
    ensureWiFi();
  }

  // MQTT
  if (!mqtt.connected()) {
    uint32_t now = millis();
    if (now - lastMqttTry > MQTT_RETRY_MS) {
      lastMqttTry = now;
      ensureMQTT();
    }
  } else {
    mqtt.loop();
  }

  // Scan botoes
  scanButtons();
  
  // Gerenciar LED defeito (auto-off apos 3s)
  manageDefectLED();

  // Heartbeat
  if (mqtt.connected()) {
    uint32_t now = millis();
    if (now - lastHeartbeat > HEARTBEAT_MS) {
      lastHeartbeat = now;
      publishStatus("online");
      publishMetrics();
    }
  }

  delay(1);
}
