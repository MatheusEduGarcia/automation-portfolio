# ESP32 + MQTT Practical (Local → Cloud)

## Objective
Build a practical MQTT pipeline starting locally (PC broker + Node-RED machine signal simulator), then connect an ESP32, and finally migrate to HiveMQ Cloud without changing the topic structure.

## Architecture — Phase 0 (Local)
**Components**
- Mosquitto (MQTT Broker) on PC
- Node-RED (Machine Signal Simulator) on PC
- MQTT Explorer (Validation / Monitoring)

**Data flow**
Node-RED → Mosquitto (PC) → MQTT Explorer / Dashboard

## Topic Convention (initial)
Base: `lab/line1/m1/`

**Telemetry**
- `lab/line1/m1/telemetry/state` (RUN | STOP | FAULT)
- `lab/line1/m1/telemetry/cycle_count` (int)
- `lab/line1/m1/telemetry/speed` (float)
- `lab/line1/m1/telemetry/ts` (timestamp)

**Commands (next step)**
- `lab/line1/m1/cmd` (RUN | STOP | RESET)

## Deliverables
- Node-RED flow export: `projects/02-node-red-flows/flows/flow-simulator-v1.json`
- Diagram: `diagrams/phase0-local.png`
- Evidence: `screenshots/` (MQTT Explorer + Node-RED flow + sample data)

## Next Phases
- Phase 1: ESP32 connects locally and publishes telemetry/health
- Phase 2: Dashboard (Node-RED Dashboard or Grafana)
- Phase 3: HiveMQ Cloud (TLS + credentials), keep the same topics
