# Automation Portfolio · Industrial Automation | IIoT | Industry 4.0

Hands-on projects in industrial automation, IIoT and industrial data: from
PLC logic and embedded devices on the shop floor to MQTT, historians and
dashboards on the IT side.

## About

Control and Automation Engineer working on OT/IT integration, data
connectivity and OEE enablement.

- PLC / OT systems and shop-floor integration (IEC 61131-3, CODESYS)
- IIoT: MQTT, OPC UA, edge computing, Unified Namespace
- OEE, downtime and Andon systems, real-time dashboards
- Data: Python, SQL, time-series databases, Power BI

## Featured project

### [CAC-01 · Compressed Air Plant: PLC → OPC UA → MQTT → TimescaleDB](https://github.com/MatheusEduGarcia/compressed-air-plant-opcua)

Three-compressor air plant programmed in ST and Ladder (MasterTool X /
CODESYS), exposed over OPC UA, published to an MQTT Unified Namespace,
stored in TimescaleDB and shown in Grafana. Includes a Python digital twin
of the PLC so the whole stack runs with `docker compose up`.

## Projects in this repository

| # | Project | Stack | Status |
|---|---|---|---|
| 01 | [ESP32 + MQTT Practical](projects/01-esp32-mqtt-practical/) | ESP32, Mosquitto, Node-RED, MQTT Explorer | Phase 0 (local) done |
| 02 | [ESP32 MQTT Andon System](projects/02-esp32-mqtt-andon-system/) | ESP32, MQTT, Node-RED Dashboard | Lab prototype |
| 03 | [Node-RED Flows](projects/03-node-red-flows/) | Node-RED | Reusable flows |

Each project folder has its own README with objective, architecture, how to
run and screenshots.

## Firmware credentials

The ESP32 sketches read Wi-Fi and broker settings from `secrets.h`, which is
not committed. In each sketch folder, copy `secrets.example.h` to
`secrets.h` and fill in your values.

## Tech stack

MQTT · OPC UA · Node-RED · ESP32 · CODESYS · Python · SQL · TimescaleDB ·
Grafana · Power BI · Docker

## Contact

- LinkedIn: https://www.linkedin.com/in/matheuseduardorgarcia/
- Email: matheuseduardorgarcia@gmail.com
