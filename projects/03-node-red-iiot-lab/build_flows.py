"""Merge the per-project Node-RED flows into node-red/flows.json for the lab.

Each project keeps its own importable flow file (projects/*/flows/*.flow.json).
Shared config nodes (the MQTT broker) have the same id in every file, so they
are de-duplicated here.

    python build_flows.py
"""
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
merged, seen = [], set()
for f in sorted(root.glob("*/flows/*.flow.json")):
    for node in json.loads(f.read_text(encoding="utf-8")):
        if node["id"] not in seen:
            seen.add(node["id"])
            merged.append(node)
    print("added", f.relative_to(root))

out = Path(__file__).parent / "node-red" / "flows.json"
out.write_text(json.dumps(merged, indent=2), encoding="utf-8")
print(f"{len(merged)} nodes -> {out.relative_to(root)}")
