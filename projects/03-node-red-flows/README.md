# Node-RED flows

Reusable Node-RED flows used by the other projects in this portfolio.

| Flow | What it does | Used by |
|---|---|---|
| [`flow-simulator-v1.json`](flows/flow-simulator-v1.json) | Simulates a machine (RUN / STOP / FAULT, cycle count, speed) and publishes telemetry to `lab/line1/m1/telemetry/*` | [01-esp32-mqtt-practical](../01-esp32-mqtt-practical/) |

The Andon dashboard flow lives with its project:
[`02-esp32-mqtt-andon-system/flows/andon_tower_flow.json`](../02-esp32-mqtt-andon-system/flows/andon_tower_flow.json).

## Import

Node-RED menu **Import → select a file to import**, pick the JSON, then
**Deploy**. Point the MQTT broker node at your Mosquitto instance
(default `localhost:1883`).
