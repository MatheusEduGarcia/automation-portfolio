# ESP32 + MQTT Practical (Local → Cloud)

## Objective
Build a practical MQTT pipeline starting locally (PC broker + Node-RED machine signal simulator), then connect an ESP32, and finally migrate to HiveMQ Cloud without changing the topic structure.

## Scope
**Phase 0 (Local):**
- Mosquitto running on PC
- Node-RED simulating machine signals and publishing MQTT telemetry
- Validation with MQTT Explorer
- Evidence captured (screenshots + flow export + diagram)

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
- Evidence folder: `screenshots/`

- **Phase 0 Evidence**
- `screenshots/mqtt-explorer-local-telemetry.png`
- `screenshots/node-red-flow-snapshot.png`

**Flow**
- `projects/02-node-red-flows/flows/flow-simulator-v1.json`

## Mosquitto — Local Broker Validation
**Service status**
- Command: `sc query mosquitto`
- Expected: `STATE: 4 RUNNING`

**Test (publish/subscribe)**
Terminal A (subscriber):
- `mosquitto_sub -h localhost -t "lab/test" -v`

Terminal B (publisher):
- `mosquitto_pub -h localhost -t "lab/test" -m "ok"`

**Evidence**
- `screenshots/mosquitto-local-test.png`

## Phase 0 — Acceptance Criteria
- Mosquitto service running locally
- Pub/Sub test completed successfully
- MQTT Explorer connects to `localhost:1883`
- Node-RED publishes telemetry to the topic convention above
- Flow exported to `projects/02-node-red-flows/flows/flow-simulator-v1.json`
- Diagram + screenshots stored in the project folders

## Next Phases
- Phase 1: ESP32 connects locally and publishes telemetry/health
- Phase 2: Dashboard (Node-RED Dashboard or Grafana)
- Phase 3: HiveMQ Cloud (TLS + credentials), keep the same topics
