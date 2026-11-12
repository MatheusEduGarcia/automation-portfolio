# 📦 Guia de Instalação Completo

Este guia detalha todos os passos necessários para instalar e configurar o **ESP32 MQTT Machine Control com Sistema Andon** do zero.

---

## 📋 Índice

1. [Pré-requisitos](#-pré-requisitos)
2. [Instalação do Software](#-instalação-do-software)
3. [Configuração do Hardware](#-configuração-do-hardware)
4. [Upload do Firmware ESP32](#-upload-do-firmware-esp32)
5. [Instalação do Node-RED](#-instalação-do-node-red)
6. [Configuração do Dashboard](#-configuração-do-dashboard)
7. [Teste do Sistema](#-teste-do-sistema)
8. [Troubleshooting](#-troubleshooting)

---

## ✅ Pré-requisitos

### Hardware Necessário

- [ ] **ESP32 Dev Module** (30 pinos)
- [ ] **4x Botões** push-button (tácteis)
- [ ] **4x LEDs** (1 verde, 1 vermelho, 1 azul, 1 amarelo)
- [ ] **4x Resistores 220Ω** (para LEDs)
- [ ] **Jumpers** macho-macho
- [ ] **Protoboard** (para prototipagem)
- [ ] **Cabo USB** Micro-USB ou USB-C (conforme seu ESP32)

### Software Necessário

- [ ] **Windows 10/11**, **macOS** ou **Linux**
- [ ] **Arduino IDE 2.x** ou superior
- [ ] **Node.js 18.x** ou superior
- [ ] **Node-RED 3.x** ou superior
- [ ] **Broker MQTT** (Mosquitto)

### Conhecimentos Recomendados

- ✅ Conceitos básicos de eletrônica
- ✅ Noções de Arduino/ESP32
- ✅ Familiaridade com linha de comando
- ✅ Conceitos básicos de MQTT

---

## 💻 Instalação do Software

### 1️⃣ Arduino IDE

#### Windows
```
1. Acesse: https://www.arduino.cc/en/software
2. Download: "Windows Win 10 and newer, 64 bits"
3. Execute o instalador
4. Siga o wizard de instalação
5. Marque: "Install USB driver"
```

#### macOS
```
1. Acesse: https://www.arduino.cc/en/software
2. Download: "macOS Intel, 10.14: "Mojave" or newer, 64 bits"
3. Abra o .dmg
4. Arraste Arduino IDE para Applications
```

#### Linux (Ubuntu/Debian)
```bash
# Via AppImage
wget https://downloads.arduino.cc/arduino-ide/arduino-ide_latest_Linux_64bit.AppImage
chmod +x arduino-ide_latest_Linux_64bit.AppImage
./arduino-ide_latest_Linux_64bit.AppImage

# Ou via Snap
sudo snap install arduino-ide
```

---

### 2️⃣ ESP32 Board Support

**No Arduino IDE:**

1. Abra **File → Preferences**

2. Em **Additional Boards Manager URLs**, adicione:
```
https://dl.espressif.com/dl/package_esp32_index.json
```

3. Clique **OK**

4. Abra **Tools → Board → Boards Manager**

5. Busque por: `esp32`

6. Instale: **esp32 by Espressif Systems** (versão 2.0.11 ou superior)

7. Aguarde o download (pode demorar alguns minutos)

---

### 3️⃣ Biblioteca PubSubClient

**No Arduino IDE:**

1. Abra **Sketch → Include Library → Manage Libraries**

2. Busque por: `PubSubClient`

3. Instale: **PubSubClient by Nick O'Leary** (versão 2.8.0 ou superior)

4. Clique **Install**

---

### 4️⃣ Node.js

#### Windows
```
1. Acesse: https://nodejs.org/
2. Download: "LTS (Long Term Support)"
3. Execute o instalador
4. Siga o wizard (todas opções padrão)
5. Verifique instalação:
   - Abra CMD
   - Digite: node --version
   - Deve mostrar: v18.x.x ou superior
```

#### macOS
```bash
# Via Homebrew (recomendado)
brew install node

# Ou download direto de nodejs.org
```

#### Linux (Ubuntu/Debian)
```bash
# Adicionar repositório NodeSource
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -

# Instalar Node.js
sudo apt-get install -y nodejs

# Verificar versão
node --version
npm --version
```

---

### 5️⃣ Node-RED

#### Instalação Global (Recomendado)

**Windows/macOS/Linux:**
```bash
# Instalar Node-RED globalmente
npm install -g --unsafe-perm node-red

# Verificar instalação
node-red --version

# Iniciar Node-RED
node-red

# Acesse: http://localhost:1880
```

#### Instalação via Docker (Alternativa)

```bash
# Pull da imagem
docker pull nodered/node-red

# Executar container
docker run -it -p 1880:1880 --name mynodered nodered/node-red
```

---

### 6️⃣ Mosquitto MQTT Broker

#### Windows
```
1. Acesse: https://mosquitto.org/download/
2. Download: "Windows 64-bit installer"
3. Execute o instalador
4. Siga o wizard
5. Após instalação:
   - Services → Mosquitto Broker → Start
```

#### macOS
```bash
# Via Homebrew
brew install mosquitto

# Iniciar broker
brew services start mosquitto

# Verificar status
brew services list
```

#### Linux (Ubuntu/Debian)
```bash
# Instalar Mosquitto
sudo apt update
sudo apt install mosquitto mosquitto-clients

# Iniciar serviço
sudo systemctl start mosquitto
sudo systemctl enable mosquitto

# Verificar status
sudo systemctl status mosquitto

# Testar
mosquitto_sub -h localhost -t test &
mosquitto_pub -h localhost -t test -m "Hello MQTT"
```

---

## 🔌 Configuração do Hardware

### Esquema de Conexões

#### Botões (com Pull-up Interno)

```
Botão START:
  - Pino 1 → GPIO 32 (ESP32)
  - Pino 2 → GND

Botão STOP:
  - Pino 1 → GPIO 33 (ESP32)
  - Pino 2 → GND

Botão OUTPUT:
  - Pino 1 → GPIO 4 (ESP32)
  - Pino 2 → GND

Botão DEFECT:
  - Pino 1 → GPIO 15 (ESP32)
  - Pino 2 → GND
```

**Nota:** O ESP32 usa pull-up interno, então LOW = pressionado

#### LEDs (com Resistor Limitador)

```
LED START (Verde):
  - Anodo (+) → Resistor 220Ω → GPIO 23
  - Catodo (-) → GND

LED STOP (Vermelho):
  - Anodo (+) → Resistor 220Ω → GPIO 16
  - Catodo (-) → GND

LED OUTPUT (Azul):
  - Anodo (+) → Resistor 220Ω → GPIO 17
  - Catodo (-) → GND

LED DEFECT (Amarelo):
  - Anodo (+) → Resistor 220Ω → GPIO 19
  - Catodo (-) → GND
```

### Tabela Resumida

| Componente | GPIO | Tipo | Observação |
|------------|------|------|------------|
| BT_START | 32 | INPUT_PULLUP | LOW = pressed |
| BT_STOP | 33 | INPUT_PULLUP | LOW = pressed |
| BT_OUTPUT | 4 | INPUT_PULLUP | LOW = pressed |
| BT_DEFECT | 15 | INPUT_PULLUP | LOW = pressed |
| LED_START | 23 | OUTPUT | HIGH = ON |
| LED_STOP | 16 | OUTPUT | HIGH = ON |
| LED_OUTPUT | 17 | OUTPUT | HIGH = ON |
| LED_DEFECT | 19 | OUTPUT | HIGH = ON |

### Verificação da Montagem

- [ ] Todos os botões conectados ao GND
- [ ] Todos os LEDs com resistor 220Ω
- [ ] Polaridade dos LEDs correta (+ e -)
- [ ] GPIOs corretos conforme tabela
- [ ] Alimentação 5V/3.3V conectada

---

## 📤 Upload do Firmware ESP32

### Passo 1: Preparar o Firmware

1. **Baixe o arquivo**: `firmware/esp32_machine_control/esp32_machine_control.ino` (e copie `secrets.example.h` para `secrets.h`)

2. **Abra no Arduino IDE**

3. **Edite as configurações em `secrets.h`** (esse arquivo fica fora do git, veja `.gitignore`):

```cpp
// secrets.h: SUAS CREDENCIAIS
static const char* WIFI_SSID  = "SEU_WIFI_AQUI";
static const char* WIFI_PASS  = "SUA_SENHA_AQUI";

// IP DO SEU BROKER MQTT
static const char* BROKER_HOST = "192.168.1.XXX";
```

**Como encontrar o IP do broker:**
```bash
# Windows
ipconfig

# macOS/Linux
ifconfig
# ou
ip addr show
```

---

### Passo 2: Configurar Placa

1. **Tools → Board → esp32 → ESP32 Dev Module**

2. **Configurações recomendadas:**
```
Upload Speed: 921600
CPU Frequency: 240MHz
Flash Frequency: 80MHz
Flash Mode: QIO
Flash Size: 4MB (32Mb)
Partition Scheme: Default 4MB with spiffs
```

3. **Tools → Port → Selecione a porta COM do ESP32**
   - Windows: `COM3`, `COM4`, etc.
   - macOS: `/dev/cu.usbserial-XXXX`
   - Linux: `/dev/ttyUSB0` ou `/dev/ttyACM0`

**Não aparece porta COM?**
```
✅ Verifique cabo USB (alguns são só para carga)
✅ Instale driver CP2102 ou CH340
✅ Reinicie o computador
```

---

### Passo 3: Upload

1. **Clique no botão Upload** (seta →) ou **Ctrl+U**

2. **Durante o upload**, se necessário:
   - Segure o botão **BOOT** no ESP32
   - Aguarde aparecer "Connecting..."
   - Solte após aparecer "Writing at..."

3. **Aguarde finalizar** (30-60 segundos)

4. **Sucesso:**
```
Hard resetting via RTS pin...
Upload complete!
```

---

### Passo 4: Verificar Monitor Serial

1. **Abra Serial Monitor**: **Tools → Serial Monitor**

2. **Configure baud rate**: `115200`

3. **Pressione botão RESET** no ESP32

4. **Saída esperada:**
```
========================================
  ESP32 Machine Control - v2.0
  Start/Stop/Output/Defect com Logica
========================================

Client ID: esp32-m1-XXXXXXXX

=== Configurando I/O ===
I/O configurado!

Conectando WiFi: SEU_WIFI_AQUI
...
WiFi OK! IP: 192.168.1.XXX

Conectando MQTT... OK!

=== Sistema Pronto! ===
```

✅ **Se ver essa saída, firmware está OK!**

---

## 🎨 Instalação do Node-RED

### Passo 1: Instalar Dashboard

```bash
# Parar Node-RED (se estiver rodando)
# Ctrl+C no terminal

# Instalar node-red-dashboard
npm install -g node-red-dashboard

# Ou, se Node-RED já estiver rodando:
# Palette Manager → Install → node-red-dashboard
```

---

### Passo 2: Iniciar Node-RED

```bash
# Iniciar Node-RED
node-red

# Aguarde ver:
# [info] Server now running at http://127.0.0.1:1880/
```

**Manter rodando:**
- Windows: Mantenha o CMD aberto
- Linux/macOS: Use `nohup node-red &` ou instale como serviço

---

### Passo 3: Acessar Interface

1. Abra navegador: **http://localhost:1880**

2. Você verá a interface de desenvolvimento do Node-RED

---

## 📊 Configuração do Dashboard

### Passo 1: Importar Fluxo

1. No Node-RED, clique no **menu** (☰ canto superior direito)

2. Selecione: **Import → Clipboard**

3. **Abra o arquivo**: `flows/andon_tower_flow.json`

4. **Copie TODO o conteúdo** (Ctrl+A, Ctrl+C)

5. **Cole no campo** do Node-RED

6. Clique **Import**

7. Você verá o fluxo aparecer na área de trabalho

---

### Passo 2: Configurar Broker MQTT

1. **Dê duplo clique** em qualquer nó MQTT (mqtt in/out)

2. Clique no **ícone de lápis** ao lado de "Server"

3. **Configure:**
```
Server: localhost (ou IP do seu broker)
Port: 1883
Client ID: nodered-andon-system
```

4. **Clique Done** e depois **Done** novamente

---

### Passo 3: Deploy

1. Clique no botão **Deploy** (canto superior direito)

2. Aguarde mensagem: "Successfully deployed"

---

### Passo 4: Acessar Dashboard

1. Abra nova aba do navegador

2. Acesse: **http://localhost:1880/ui**

3. Você verá o **Sistema Andon** funcionando!

---

## 🧪 Teste do Sistema

### Teste 1: Conexão ESP32

**No Serial Monitor, deve ver:**
```
WiFi OK! IP: 192.168.1.XXX
Conectando MQTT... OK!
=== Sistema Pronto! ===
```

**No Dashboard:**
```
ESP32: 🟢 ONLINE
```

✅ **Conexão OK**

---

### Teste 2: Botão START

1. **Pressione** o botão START físico

2. **No Serial Monitor:**
```
MAQUINA INICIADA
[MQTT] LED start: LIGADO
```

3. **No Dashboard:**
```
Torre Andon: 🟢 Verde aceso
Texto: "Production is normal"
```

✅ **START OK**

---

### Teste 3: Botão OUTPUT

1. **Pressione** o botão OUTPUT físico

2. **No Serial Monitor:**
```
OUTPUT #1 | Qualidade: 100.0%
```

3. **No Dashboard:**
```
📦 OUTPUT: 1 (incrementou)
Quality: 100%
```

✅ **OUTPUT OK**

---

### Teste 4: Botão DEFECT

1. **Pressione** o botão DEFECT físico

2. **No Serial Monitor:**
```
DEFEITO #1 | Qualidade: 0.0%
```

3. **No Dashboard:**
```
Torre Andon: 🟡 Amarelo aceso
❌ DEFECTS: 1 (incrementou)
Quality: 0% (ou menor)
```

4. **Aguarde 3 segundos**

5. **Torre volta** para 🟢 Verde (se ainda rodando)

✅ **DEFECT OK**

---

### Teste 5: Botão STOP

1. **Pressione** o botão STOP físico

2. **No Serial Monitor:**
```
MAQUINA PARADA | Runtime: XX s
```

3. **No Dashboard:**
```
Torre Andon: 🔴 Vermelho aceso
Texto: "Production has stopped"
Runtime: XX s (parou de contar)
```

✅ **STOP OK**

---

## ✅ Checklist Final

Marque tudo que está funcionando:

**Hardware:**
- [ ] ESP32 conectado e alimentado
- [ ] 4 botões respondendo
- [ ] 4 LEDs acendem corretamente
- [ ] Conexões verificadas

**Software:**
- [ ] Arduino IDE instalado
- [ ] ESP32 board package instalado
- [ ] PubSubClient instalada
- [ ] Firmware compilou sem erros
- [ ] Upload bem-sucedido

**Conectividade:**
- [ ] WiFi conectado
- [ ] MQTT conectado
- [ ] Broker MQTT rodando
- [ ] Node-RED rodando
- [ ] Dashboard acessível

**Funcionalidades:**
- [ ] START funciona (verde acende)
- [ ] STOP funciona (vermelho acende)
- [ ] OUTPUT incrementa contador
- [ ] DEFECT incrementa contador
- [ ] Quality atualiza
- [ ] Torre Andon muda de cor

---

## 🐛 Troubleshooting

### Problema: ESP32 não conecta WiFi

**Sintomas:**
```
Conectando WiFi: SEU_WIFI
.....................
Timeout WiFi
```

**Soluções:**
1. Verifique SSID e senha corretos
2. WiFi deve ser 2.4GHz (ESP32 não suporta 5GHz)
3. Aproxime ESP32 do roteador
4. Reinicie o roteador
5. Teste outro WiFi

---

### Problema: MQTT não conecta

**Sintomas:**
```
Conectando MQTT... FALHOU (rc=-2)
```

**Códigos de erro:**
- `-2`: Conexão recusada
- `-4`: Timeout
- `5`: Não autorizado

**Soluções:**
1. Verifique se broker está rodando:
```bash
# Windows
services.msc → Mosquitto Broker

# Linux
systemctl status mosquitto
```

2. Teste manualmente:
```bash
mosquitto_sub -h localhost -t "#" -v
```

3. Verifique firewall (porta 1883)

4. Confirme IP correto no código

---

### Problema: Dashboard não abre

**Sintomas:**
```
http://localhost:1880/ui → Página não encontrada
```

**Soluções:**
1. Verifique se Node-RED está rodando
2. Instale node-red-dashboard:
```bash
npm install -g node-red-dashboard
```
3. Reinicie Node-RED
4. Faça Deploy novamente

---

### Problema: Torre Andon não muda de cor

**Sintomas:**
- Botões funcionam no serial
- LEDs físicos acendem
- Mas dashboard não atualiza

**Soluções:**
1. Verifique tópicos MQTT:
```bash
mosquitto_sub -h localhost -t "lab/line1/m1/#" -v
```

2. Confira broker configurado no Node-RED

3. Veja debug nodes no Node-RED

4. Limpe cache do navegador (Ctrl+Shift+R)

---

### Problema: Erro de compilação

**Sintoma:**
```
'WiFi' was not declared in this scope
```

**Solução:**
- Instale ESP32 board package

**Sintoma:**
```
PubSubClient.h: No such file
```

**Solução:**
- Instale biblioteca PubSubClient

---

### Problema: Porta COM não aparece

**Sintomas:**
- Tools → Port está vazio/cinza

**Soluções (Windows):**
1. Instale driver CP2102:
   - https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

2. Ou driver CH340:
   - http://www.wch-ic.com/downloads/CH341SER_ZIP.html

3. Reinicie o computador

4. Troque o cabo USB

---

## 📞 Próximos Passos

Agora que o sistema está instalado e funcionando:

1. ✅ **Explore o Dashboard**: Teste todos os botões e veja métricas
2. ✅ **Customize**: Mude cores, adicione features
3. ✅ **Integre**: Conecte com banco de dados
4. ✅ **Expanda**: Adicione mais máquinas
5. ✅ **Compartilhe**: Mostre seu projeto!

---

## 📚 Recursos Adicionais

- **[Andon System Explained](ANDON_SYSTEM.md)** - Entenda a lógica
- **[README Principal](../README.md)** - Visão geral do projeto
- **Documentação ESP32**: https://docs.espressif.com/
- **Node-RED Docs**: https://nodered.org/docs/
- **MQTT Essentials**: https://www.hivemq.com/mqtt-essentials/

---

**Instalação Completa! 🎉**

Se tudo funcionou, você tem um sistema Andon profissional rodando!

**Versão:** 1.0.0  
**Última Atualização:** Janeiro 2025
