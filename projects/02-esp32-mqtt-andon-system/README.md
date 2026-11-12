# 🚨 ESP32 MQTT Machine Control com Sistema Andon

> Sistema de controle industrial com torre Andon visual, monitoramento em tempo real e integração MQTT para Industry 4.0

![ESP32](https://img.shields.io/badge/ESP32-Supported-green?logo=espressif)
![Node-RED](https://img.shields.io/badge/Node--RED-v3.0+-red?logo=nodered)
![MQTT](https://img.shields.io/badge/MQTT-v3.1.1-blue)
![License](https://img.shields.io/badge/License-MIT-yellow)
![Status](https://img.shields.io/badge/Status-Lab%20prototype-blue)

---

## 📸 Sistema em Funcionamento

**Torre Andon Visual:**

```
       ┌─────────┐
       │   🔴    │  ← Production Stopped
       ├─────────┤
       │   🟡    │  ← Problem Appeared  
       ├─────────┤
       │   🟢    │  ← Production Normal (Active)
       └────┬────┘
            │
         ───┴───
```

**Dashboard Node-RED:**
- Torre Andon 3D com animações
- Contadores de Output e Defects em tempo real
- Gauge de Quality (0-100%)
- Monitoramento de conexão ESP32

---

## ✨ Features Principais

### 🎯 Sistema Andon Completo
- **🟢 Verde**: Produção normal (máquina rodando)
- **🟡 Amarelo**: Problema detectado (defeito identificado)
- **🔴 Vermelho**: Produção parada (máquina idle/stopped)
- Lógica de prioridade: Defeito > Rodando > Parado

### 📊 Monitoramento em Tempo Real
- Contador de ciclos produzidos (Output)
- Contador de defeitos detectados
- Cálculo automático de Quality (%)
- Runtime acumulado da máquina
- Status de conexão ESP32

### ⚡ Comunicação MQTT
- Publicação de eventos de botões
- Estados dos LEDs (retained)
- Métricas em formato JSON
- Last Will Testament (LWT)
- Auto-reconexão WiFi/MQTT

### 🎨 Interface Visual Profissional
- Torre Andon com efeitos 3D
- Animações de pulsação por estado
- Gradientes radiais e glow effects
- Design responsivo (desktop/tablet/mobile)
- Dashboard customizável

---

## 🎯 Casos de Uso

Este sistema é ideal para:

- **Linhas de Montagem**: Monitorar status de produção em tempo real
- **Células de Usinagem**: Detectar problemas de qualidade e paradas
- **Controle de Qualidade**: Rastrear defeitos e calcular quality metrics
- **Fundições**: Acompanhar ciclos e identificar não-conformidades
- **Industry 4.0**: Base para sistemas OEE e MES

---

## 🛠️ Hardware Necessário

### Componentes Principais
- **1x ESP32 Dev Module** (30 pinos)
- **4x Botões tácteis** (Push-button)
- **4x LEDs** (Verde, Vermelho, Azul, Amarelo)
- **4x Resistores 220Ω** (para LEDs)
- **Jumpers macho-macho**
- **Protoboard** (opcional, para testes)

### Mapeamento de Pinos

| Componente | GPIO | Função |
|------------|------|--------|
| Botão START | 32 | Iniciar produção |
| Botão STOP | 33 | Parar produção |
| Botão OUTPUT | 4 | Registrar ciclo |
| Botão DEFECT | 15 | Detectar defeito |
| LED START (Verde) | 23 | Indicador Start |
| LED STOP (Vermelho) | 16 | Indicador Stop |
| LED OUTPUT (Azul) | 17 | Indicador Output |
| LED DEFECT (Amarelo) | 19 | Indicador Defect |

**Conexão dos Botões:** Pull-up interno (LOW = pressionado)  
**Conexão dos LEDs:** Lógica positiva (HIGH = aceso)

---

## 📋 Pré-requisitos

### Software
- **Arduino IDE 2.x** ou superior
- **Node-RED 3.0+** instalado
- **Broker MQTT** (Mosquitto recomendado)
- **Node.js** (para Node-RED)

### Bibliotecas Arduino
- **WiFi.h** (nativa ESP32)
- **PubSubClient** (Nick O'Leary)

### Pacotes Node-RED
- **node-red-dashboard** (UI components)

---

## 🚀 Quick Start (5 Minutos)

### 1️⃣ Upload Firmware ESP32
```bash
1. Abra: firmware/esp32_machine_control/esp32_machine_control.ino
   (copie `secrets.example.h` para `secrets.h` na mesma pasta e preencha Wi-Fi e IP do broker)
2. Edite WiFi SSID e senha (linhas 12-14)
3. Edite IP do broker MQTT (linha 16)
4. Tools → Board → ESP32 Dev Module
5. Upload (Ctrl+U)
```

### 2️⃣ Configurar Node-RED
```bash
1. Abra Node-RED: http://localhost:1880
2. Menu → Import → Clipboard
3. Cole o conteúdo de: flows/andon_tower_flow.json
4. Deploy
```

### 3️⃣ Acessar Dashboard
```
URL: http://localhost:1880/ui
Tab: Andon System
```

### 4️⃣ Testar
```
- Pressione START → Torre fica 🟢 Verde
- Pressione OUTPUT → Contador incrementa
- Pressione DEFECT → Torre fica 🟡 Amarelo (3s)
- Pressione STOP → Torre fica 🔴 Vermelho
```

✅ **Pronto! Sistema funcionando!**

---

## 📚 Documentação Completa

### 📖 Guias Detalhados
- **[Installation Guide](docs/INSTALLATION.md)** - Passo a passo completo de instalação
- **[Andon System Explained](docs/ANDON_SYSTEM.md)** - Conceitos e funcionamento do Andon

### 🔧 Recursos Adicionais
- **Troubleshooting** - Ver seção abaixo
- **MQTT Topics** - Ver seção abaixo
- **API Reference** - Métricas JSON

---

## 📊 Arquitetura do Sistema

```
┌─────────────────────────────────────────────────────────┐
│                    ESP32 (Embedded)                      │
│  ┌──────────┐    ┌──────────────┐    ┌──────────┐      │
│  │ Botões   │───▶│  State       │───▶│   LEDs   │      │
│  │ Físicos  │    │  Machine     │    │  Físicos │      │
│  └──────────┘    │  + Metrics   │    └──────────┘      │
│                   └──────┬───────┘                       │
└──────────────────────────┼─────────────────────────────┘
                           │ MQTT (WiFi)
                           ▼
              ┌────────────────────────┐
              │    Mosquitto Broker    │
              └────────┬───────────────┘
                       │ Subscribe/Publish
                       ▼
              ┌────────────────────────┐
              │      Node-RED          │
              │  ┌──────────────────┐  │
              │  │  Andon Logic     │  │
              │  │  Torre Visual    │  │
              │  │  Metrics Display │  │
              │  └──────────────────┘  │
              └────────────────────────┘
                       │
                       ▼
              ┌────────────────────────┐
              │  Dashboard UI (Browser)│
              │  http://localhost:1880 │
              └────────────────────────┘
```

---

## 📡 Tópicos MQTT

### Publicados pelo ESP32

| Tópico | Payload | Retained | Descrição |
|--------|---------|----------|-----------|
| `lab/line1/m1/button/start` | `0` / `1` | ❌ | Evento botão Start |
| `lab/line1/m1/button/stop` | `0` / `1` | ❌ | Evento botão Stop |
| `lab/line1/m1/button/output` | `0` / `1` | ❌ | Evento botão Output |
| `lab/line1/m1/button/defect` | `0` / `1` | ❌ | Evento botão Defect |
| `lab/line1/m1/led/start` | `0` / `1` | ✅ | Estado LED Start |
| `lab/line1/m1/led/stop` | `0` / `1` | ✅ | Estado LED Stop |
| `lab/line1/m1/led/output` | `0` / `1` | ✅ | Estado LED Output |
| `lab/line1/m1/led/defect` | `0` / `1` | ✅ | Estado LED Defect |
| `lab/line1/m1/metrics` | `JSON` | ✅ | Métricas completas |
| `lab/line1/m1/status` | `online` / `offline` | ✅ | Status ESP32 (LWT) |

### Subscritos pelo ESP32

| Tópico | Payload | Descrição |
|--------|---------|-----------|
| `lab/line1/m1/led/start/set` | `0` / `1` | Comando LED Start |
| `lab/line1/m1/led/stop/set` | `0` / `1` | Comando LED Stop |
| `lab/line1/m1/led/output/set` | `0` / `1` | Comando LED Output |
| `lab/line1/m1/led/defect/set` | `0` / `1` | Comando LED Defect |

### Estrutura JSON de Métricas

```json
{
  "running": true,
  "cycles": 42,
  "defects": 2,
  "runtime": 1254,
  "quality": 95.2
}
```

---

## 🎨 Customização

### Mudar Base do Tópico MQTT
```cpp
// No firmware ESP32
static const char* TOPIC_BASE = "lab/line1/m1";
// Mude para: "fabrica/linha2/torno1"
```

### Ajustar Debounce dos Botões
```cpp
// No firmware ESP32
static const uint32_t DEBOUNCE_MS = 50;
// Aumente se houver bouncing
```

### Personalizar Cores da Torre Andon
```javascript
// No Node-RED template node
background: radial-gradient(circle, #FF0000, #AA0000);  // Vermelho
background: radial-gradient(circle, #FFFF00, #CCCC00);  // Amarelo
background: radial-gradient(circle, #00FF00, #00AA00);  // Verde
```

---

## 🐛 Troubleshooting

### ESP32 não conecta no WiFi
```
✅ Verificar SSID e senha corretos
✅ WiFi 2.4GHz (ESP32 não suporta 5GHz)
✅ Testar alcance do sinal
✅ Ver Serial Monitor (115200 baud)
```

### MQTT não conecta
```
✅ Broker está rodando? (systemctl status mosquitto)
✅ Firewall bloqueando porta 1883?
✅ IP do broker correto no firmware?
✅ Testar: mosquitto_sub -h localhost -t "#" -v
```

### Dashboard não atualiza
```
✅ Node-RED está rodando?
✅ Deploy foi executado?
✅ node-red-dashboard instalado?
✅ Verificar debug nodes no Node-RED
```

### LEDs não acendem
```
✅ Resistores conectados corretamente?
✅ Polaridade do LED correta?
✅ GPIO correto no código?
✅ Testar com digitalWrite direto
```

### Erro de compilação
```
✅ ESP32 board package instalado?
✅ Biblioteca PubSubClient instalada?
✅ Porta COM selecionada?
✅ Segurar botão BOOT durante upload
```

---

## 📈 Roadmap (Próximas Versões)

### v2.0 - Database Integration
- [ ] MySQL/InfluxDB storage
- [ ] Historical data logging
- [ ] Trend charts (Grafana)

### v3.0 - Advanced OEE
- [ ] Availability calculation
- [ ] Performance metrics
- [ ] Complete OEE dashboard

### v4.0 - Multi-Machine
- [ ] Support for multiple ESP32s
- [ ] Grid view of Andon towers
- [ ] Aggregated metrics

### v5.0 - Integrations
- [ ] ERP/MES integration
- [ ] Telegram/Email alerts
- [ ] Mobile app (React Native)

---

## 🤝 Contribuindo

Contribuições são bem-vindas! Para contribuir:

1. Fork o projeto
2. Crie uma branch (`git checkout -b feature/NovaFeature`)
3. Commit suas mudanças (`git commit -m 'Add: Nova feature'`)
4. Push para a branch (`git push origin feature/NovaFeature`)
5. Abra um Pull Request

**Áreas para contribuir:**
- Melhorias no dashboard
- Novos recursos de métricas
- Integrações com outros sistemas
- Documentação
- Tradução para outros idiomas

---

## 📄 Licença

Este projeto está sob a licença MIT. Veja o arquivo [LICENSE](../../LICENSE) para mais detalhes.

```
MIT License - Você pode usar, modificar e distribuir livremente
```

---

## 👤 Autor

**Matheus Garcia**
- 🏢 Senior Automation Engineer.
- 🎯 Digital Transformation & Innovation
- 🌍 São Paulo, Brazil

**Expertise:**
- Industrial IoT & Industry 4.0
- MQTT & Node-RED
- ESP32 Development
- OEE Monitoring Systems
- Digital Transformation

---

## 🙏 Agradecimentos

- **Toyota Production System** - Conceito Andon original
- **Espressif** - ESP32 platform
- **Node-RED Community** - Dashboard components
- **Eclipse Mosquitto** - MQTT broker

---

## 📞 Suporte

**Encontrou um bug?** Abra uma [Issue](https://github.com/SEU_USERNAME/esp32-mqtt-andon-system/issues)

**Tem uma dúvida?** Use as [Discussions](https://github.com/SEU_USERNAME/esp32-mqtt-andon-system/discussions)

**Quer compartilhar seu projeto?** Marque com `#ESP32Andon` nas redes sociais

---

## ⭐ Star History

Se este projeto foi útil para você, considere dar uma ⭐!

---

**Versão:** 1.0.0  
**Status:** Production Ready ✅  
**Última Atualização:** Janeiro 2025

---

<div align="center">

Made with ❤️ for Industry 4.0

[⬆ Voltar ao topo](#-esp32-mqtt-machine-control-com-sistema-andon)

</div>
