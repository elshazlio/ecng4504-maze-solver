---
id: AgDR-0012
timestamp: 2026-03-19T00:00:00Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Use A Persistent HM-10 Viewer Backend

> In the context of the local HM-10 log viewer, facing misleading UI connection state, lost reconnect behavior after the car power cycles, and noisy unreadable sensor output, I decided to use a persistent server-managed BLE backend with UI-side grouped rendering to achieve accurate status and automatic recovery, accepting more backend state management than the current per-page spawn model.

## Context
- The current Bun page opens a WebSocket and immediately marks the UI as connected when the socket opens, even though BLE connection may still be in progress.
- The current server spawns a BLE reader per browser socket, so clearing logs or losing the BLE device often leaves the page needing a manual refresh to recover cleanly.
- The user wants sensor lines grouped between `Node detected` lines and expandable for easier reading.
- Browser-only BLE was already ruled out because the user wants automatic reconnect behavior.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| Persistent server-managed BLE session with broadcast events to the UI | Accurate BLE lifecycle state, automatic reconnect after power cycling, single source of truth for all tabs, keeps browser simple | Requires backend supervisor logic and event protocol instead of raw text piping |
| Keep per-page WebSocket and respawn BLE reader on each page load | Smaller change from current code, simpler server file | Connection state stays tied to page lifecycle, reconnects remain fragile, multiple tabs can compete for BLE |
| Return to browser-managed Web Bluetooth | Familiar direct browser API, no Python bridge | Cannot reliably auto-connect or auto-reconnect due to browser permission and gesture limits |

## Decision
Chosen: **Persistent server-managed BLE session with broadcast events to the UI**, because the core problems are lifecycle and truth-of-state problems that are best solved once in the backend instead of asking each browser tab to infer BLE status from a short-lived subprocess.

## Consequences
- The Bun server will maintain one long-lived BLE supervisor process and stream structured status/log events to the browser.
- The page will show `searching`, `connected`, and `reconnecting` based on backend events rather than WebSocket state alone.
- Sensor lines will be grouped client-side under each `Node detected` anchor for readability.
- The implementation is slightly more complex than raw stdout piping and needs tests around grouping/state behavior.
