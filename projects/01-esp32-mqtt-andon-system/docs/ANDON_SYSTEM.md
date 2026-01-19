# 🚨 Sistema Andon - Explicação Completa

Este documento explica em detalhes o conceito, funcionamento e implementação do **Sistema Andon** neste projeto.

---

## 📋 Índice

1. [O que é Andon?](#-o-que-é-andon)
2. [História e Origem](#-história-e-origem)
3. [Como Funciona](#-como-funciona)
4. [Implementação no Projeto](#-implementação-no-projeto)
5. [Lógica de Estados](#-lógica-de-estados)
6. [Interface Visual](#-interface-visual)
7. [Casos de Uso Práticos](#-casos-de-uso-práticos)
8. [Benefícios](#-benefícios)
9. [Expansões Futuras](#-expansões-futuras)

---

## 🏭 O que é Andon?

### Definição

**Andon** (行灯) é uma palavra japonesa que significa **"lanterna"** ou **"luz"**.

No contexto industrial, é um **sistema de sinalização visual** que fornece feedback em tempo real sobre o status da produção na linha de manufatura.

### Conceito Principal

```
Status da Produção → Sinal Visual → Ação Imediata
```

O Andon permite que:
- **Operadores** vejam o status de qualquer máquina instantaneamente
- **Supervisores** identifiquem problemas rapidamente
- **Gerentes** monitorem a linha de produção inteira

---

## 📚 História e Origem

### Toyota Production System (TPS)

O sistema Andon foi desenvolvido pela **Toyota** como parte do **Toyota Production System (TPS)** nos anos 1950-1960.

**Conceitos relacionados:**
- **Jidoka** (自働化) - Automação com toque humano
- **Kaizen** (改善) - Melhoria contínua
- **Just-In-Time (JIT)** - Produção sob demanda

### Princípio do Jidoka

> "Dê às máquinas a capacidade de detectar quando algo está errado e pare automaticamente. Não passe produtos defeituosos para a próxima estação."

O Andon materializa esse princípio através de **feedback visual imediato**.

---

## 🎯 Como Funciona

### Conceito Tradicional

**Torre Andon Física:**

```
        ┌───────────┐
        │   🔴      │  ← VERMELHO: Linha parada (emergência/manutenção)
        ├───────────┤
        │   🟡      │  ← AMARELO: Problema/Atenção necessária
        ├───────────┤
        │   🟢      │  ← VERDE: Operação normal
        └─────┬─────┘
              │
           ───┴───
         (Montado sobre
          a máquina)
```

### Significado das Cores

| Cor | Status | Significado | Ação Requerida |
|-----|--------|-------------|----------------|
| 🟢 **Verde** | Normal | Produção dentro do esperado | Nenhuma |
| 🟡 **Amarelo** | Atenção | Problema detectado, mas produzindo | Verificar logo |
| 🔴 **Vermelho** | Parado | Linha parada ou emergência | Ação imediata |

### Fluxo de Operação

```
┌─────────────────────────────────────────────────────┐
│ 1. OPERAÇÃO NORMAL                                  │
│    Máquina produzindo → Andon VERDE                 │
└─────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────┐
│ 2. PROBLEMA DETECTADO                               │
│    Peça defeituosa → Operador pressiona botão       │
│    → Andon muda para AMARELO                        │
└─────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────┐
│ 3. SUPERVISOR NOTIFICADO                            │
│    Vê torre amarela → Vai até a máquina             │
│    → Investiga o problema                           │
└─────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────┐
│ 4A. PROBLEMA RESOLVIDO                              │
│     Ajuste feito → Produção continua                │
│     → Andon volta para VERDE                        │
└─────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────┐
│ 4B. PARADA NECESSÁRIA                               │
│     Problema sério → Linha para                     │
│     → Andon muda para VERMELHO                      │
└─────────────────────────────────────────────────────┘
```

---

## 💻 Implementação no Projeto

### Arquitetura Digital

Neste projeto, o Andon foi **digitalizado** e integrado com **IoT**:

```
Hardware (ESP32)     →  Software (Node-RED)  →  Visualização (Dashboard)
Botões físicos          Lógica Andon           Torre 3D visual
LEDs locais             MQTT messaging          Web interface
```

### Componentes do Sistema

#### 1. **Camada Física (ESP32)**
- Botões: START, STOP, OUTPUT, DEFECT
- LEDs locais para feedback imediato
- Conectividade WiFi + MQTT

#### 2. **Camada de Comunicação (MQTT)**
- Publicação de eventos
- Estados sincronizados
- Mensagens retained para persistência

#### 3. **Camada de Lógica (Node-RED)**
- Processamento de estados
- Regras de prioridade
- Contadores e métricas

#### 4. **Camada de Visualização (Dashboard)**
- Torre Andon 3D animada
- Contadores em tempo real
- Gauge de qualidade

---

## 🔄 Lógica de Estados

### Máquina de Estados

```
                    ┌──────────┐
          ┌────────▶│  PARADO  │◀────────┐
          │         │    🔴    │         │
          │         └──────────┘         │
          │              │               │
          │         [PRESSIONA           │
    [PRESSIONA      START]               │
     STOP]              │                │
          │              ▼               │
          │         ┌──────────┐         │
          │         │ RODANDO  │         │
          │         │    🟢    │         │
          │         └──────────┘         │
          │              │               │
          │         [PRESSIONA      [PROBLEMA
          │          DEFECT]        RESOLVIDO]
          │              │               │
          │              ▼               │
          │         ┌──────────┐         │
          └─────────│ PROBLEMA │─────────┘
                    │    🟡    │
                    └──────────┘
```

### Regras de Prioridade

**Ordem decrescente de prioridade:**

1. **🟡 AMARELO (Defeito)** - Maior prioridade
   - Se DEFECT ativo → Sempre amarelo
   - Independente de START/STOP

2. **🟢 VERDE (Rodando)** - Prioridade média
   - Se START ativo E STOP inativo
   - Sem defeitos ativos

3. **🔴 VERMELHO (Parado)** - Prioridade baixa
   - Se STOP ativo OU nenhum botão ativo
   - Estado padrão inicial

### Implementação em Código

```javascript
// Lógica simplificada (Node-RED function node)

function determinarEstadoAndon(leds) {
    // Prioridade 1: Defeito
    if (leds.defect === true) {
        return {
            cor: 'amarelo',
            texto: 'Problem appeared'
        };
    }
    
    // Prioridade 2: Rodando
    if (leds.start === true && leds.stop === false) {
        return {
            cor: 'verde',
            texto: 'Production is normal'
        };
    }
    
    // Prioridade 3: Parado (default)
    return {
        cor: 'vermelho',
        texto: 'Production has stopped'
    };
}
```

---

## 🎨 Interface Visual

### Torre Andon 3D

A visualização foi projetada para ser:

1. **Intuitiva** - Cores universais (semáforo)
2. **Imediata** - Atualização em tempo real (<100ms)
3. **Atrativa** - Efeitos visuais modernos
4. **Informativa** - Texto explicativo dinâmico

### Elementos Visuais

#### Luzes

```css
/* Verde - Normal */
background: radial-gradient(circle, #00FF00, #00AA00);
box-shadow: 0 0 40px #00FF00, 0 0 80px #00FF00;
animation: pulse-green 0.8s infinite;

/* Amarelo - Problema */
background: radial-gradient(circle, #FFFF00, #CCCC00);
box-shadow: 0 0 40px #FFFF00, 0 0 80px #FFFF00;
animation: pulse-yellow 1s infinite;

/* Vermelho - Parado */
background: radial-gradient(circle, #FF0000, #AA0000);
box-shadow: 0 0 40px #FF0000, 0 0 80px #FF0000;
animation: pulse-red 1s infinite;
```

#### Animações

**Verde (Produzindo):**
- Pulsação rápida (0.8s)
- Opacidade: 1 → 0.8 → 1
- Transmite: Energia, movimento, atividade

**Amarelo (Problema):**
- Pulsação média (1s)
- Opacidade: 1 → 0.7 → 1
- Transmite: Alerta, atenção necessária

**Vermelho (Parado):**
- Pulsação lenta (1s)
- Opacidade: 1 → 0.6 → 1
- Transmite: Urgência, ação necessária

---

## 🏭 Casos de Uso Práticos

### Caso 1: Linha de Montagem Automotiva

**Cenário:**
- 20 estações de trabalho em linha
- Cada estação tem uma torre Andon
- Dashboard central mostra todas as torres

**Operação:**
```
Estação 5: Parafuso torto detectado
↓
Operador pressiona DEFECT
↓
Torre da Estação 5 fica AMARELA
↓
Dashboard mostra visão geral:
 [🟢][🟢][🟢][🟢][🟡][🟢][🟢]...
         Estação 5 ↑
↓
Supervisor vai até Estação 5
↓
Problema corrigido
↓
Torre volta para VERDE
```

**Benefício:** Supervisor sabe exatamente onde ir sem perder tempo.

---

### Caso 2: Célula de Usinagem CNC

**Cenário:**
- 1 torno CNC
- Operador monitora 3 máquinas simultaneamente
- Andon ajuda a priorizar atenção

**Operação:**
```
Torno 1: Ferramenta desgastada
↓
Sistema detecta tolerância fora
↓
Auto-aciona DEFECT (via sensor)
↓
Torre AMARELA + Notificação
↓
Operador: "Torno 1 precisa atenção"
↓
Verifica após ciclo atual
↓
Troca ferramenta
↓
Torre volta VERDE
```

**Benefício:** Operador sabe qual máquina necessita atenção primeiro.

---

### Caso 3: Fundição

**Cenário:**
- Linha de moldagem de peças
- Inspeção visual de defeitos
- Rastreamento de qualidade

**Operação:**
```
Ciclo de Produção:
1. START → Torre VERDE
2. Moldar 10 peças → OUTPUT × 10
3. Inspetor encontra porosidade → DEFECT
4. Quality cai de 100% para 90%
5. Investigação → Temperatura do forno
6. Ajuste feito
7. Produção continua
8. Fim do turno → STOP → Torre VERMELHO
```

**Benefício:** Rastreamento completo com métricas de qualidade.

---

## 📊 Benefícios do Sistema

### 1. Visibilidade Instantânea

**Antes (sem Andon):**
```
Supervisor: "Como está a linha?"
Operador 1: "Tudo ok aqui"
Operador 2: "Tudo normal"
Operador 3: "Tá parado tem 10 min"
Supervisor: "Por quê ninguém me avisou?!"
```

**Depois (com Andon):**
```
Supervisor olha dashboard:
[🟢][🟢][🔴] ← Máquina 3 parada!
↓
Vai direto resolver
```

---

### 2. Redução de Tempo de Resposta

**Métricas típicas:**
- Tempo para identificar problema: **-80%**
- Tempo para supervisor chegar: **-60%**
- Downtime total: **-40%**

---

### 3. Empoderamento do Operador

**Cultura de qualidade:**
- Operador **pode** e **deve** parar a linha se detectar problema
- Sem medo de represália
- Foco em qualidade, não apenas quantidade

---

### 4. Dados para Melhoria Contínua

**Métricas coletadas:**
- Quantos defeitos por turno?
- Qual máquina tem mais problemas?
- Qual operador detecta mais defeitos? (positivo!)
- Horários de pico de problemas?

**Uso dos dados:**
```
Análise: Máquina 3 tem 3x mais defeitos que outras
↓
Investigação: Calibração inadequada
↓
Ação: Recalibração + Manutenção preventiva
↓
Resultado: Defeitos caem 70%
```

---

### 5. Comunicação Não-Verbal

**Vantagens:**
- Funciona em ambientes barulhentos
- Não depende de idioma
- Visível de longe
- 24/7 sem intervenção humana

---

## 🚀 Expansões Futuras

### Nível 1: Alertas Automáticos

```javascript
// Quando AMARELO > 5 minutos
if (estadoAmarelo > 300) {
    enviarEmail(supervisor);
    enviarTelegram(gerente);
    tocarBuzzer();
}
```

---

### Nível 2: Múltiplas Torres

**Dashboard com Grid:**

```
┌────────────────────────────────────────┐
│  Visão Geral da Fábrica               │
├────────────────────────────────────────┤
│                                        │
│  Linha 1:  [🟢][🟢][🟡][🟢][🔴]      │
│  Linha 2:  [🟢][🟢][🟢][🟢][🟢]      │
│  Linha 3:  [🟡][🟢][🟢][🔴][🟢]      │
│                                        │
│  Total: 3 problemas, 2 paradas        │
└────────────────────────────────────────┘
```

---

### Nível 3: Análise Preditiva

```javascript
// Machine Learning para prever problemas
modelo.prever({
    ciclosAcumulados: 1000,
    defeitosRecentes: 5,
    temperaturaSensor: 85,
    vibracaoSensor: 'alta'
});

// Resultado: "Provável falha em 2 horas"
// Ação: Manutenção preventiva agora
```

---

### Nível 4: Integração ERP/MES

```
Andon System ←→ MES ←→ ERP
     ↓            ↓       ↓
  Status      Produção  Vendas
  Real-time   Planning  Orders
```

**Benefícios:**
- Ajuste automático de programação
- Visibilidade end-to-end
- Decisões baseadas em dados reais

---

## 📖 Leituras Recomendadas

### Livros

1. **"The Toyota Way"** - Jeffrey Liker
   - Capítulo sobre Jidoka e Andon

2. **"Lean Thinking"** - James Womack
   - Princípios do Lean Manufacturing

3. **"Gemba Kaizen"** - Masaaki Imai
   - Melhoria contínua no chão de fábrica

### Artigos

- **Toyota Production System** (Wikipedia)
- **What is Andon?** (Lean Production)
- **Visual Management** (Lean Enterprise Institute)

### Vídeos

- **Toyota Factory Tour** - Ver Andon em ação real
- **Andon System Explained** - YouTube
- **Jidoka Principle** - Lean concepts

---

## 🎯 Resumo Executivo

### O que é Andon?

Sistema de **sinalização visual** que mostra o **status da produção** em **tempo real**.

### Por que usar?

- ✅ **Visibilidade**: Todo mundo vê o status
- ✅ **Rapidez**: Problemas identificados instantaneamente
- ✅ **Qualidade**: Cultura de parar e resolver
- ✅ **Dados**: Base para melhoria contínua

### Como funciona neste projeto?

```
Botões → ESP32 → MQTT → Node-RED → Dashboard → Torre Visual
                                                     ↓
                                              🟢 🟡 🔴
```

### Próximo passo?

**Instale e teste!** Veja a diferença na prática.

---

## 📞 Conclusão

O sistema Andon representa uma **filosofia de manufatura** tanto quanto uma ferramenta técnica.

**Princípios-chave:**
- Transparência total
- Empoderamento dos trabalhadores
- Foco em qualidade
- Melhoria contínua

Este projeto traz esses princípios para a **era digital**, mantendo a **essência** do conceito original Toyota.

---

**Versão:** 1.0.0  
**Baseado em:** Toyota Production System  
**Implementação:** IoT + Industry 4.0  
**Última Atualização:** Janeiro 2025

---

<div align="center">

**"Build quality in"** - Toyota Production System

[⬆ Voltar ao topo](#-sistema-andon---explicação-completa)

</div>
