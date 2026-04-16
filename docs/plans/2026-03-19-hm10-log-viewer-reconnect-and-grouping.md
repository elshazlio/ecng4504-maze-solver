# HM-10 Log Viewer Reconnect And Grouping Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Make the local HM-10 log viewer report real BLE state, reconnect automatically after car power cycles, and collapse sensor logs between `Node detected` lines.

**Architecture:** Replace the per-tab BLE subprocess with one persistent backend supervisor that owns the HM-10 connection and broadcasts structured status/log events over WebSocket. Keep the page as a lightweight renderer that groups sensor lines beneath each `Node detected` header and updates status from backend events only.

**Tech Stack:** Bun server, Python `bleak` client, plain HTML/CSS/JavaScript, Bun test runner, Markdown docs.

---

### Task 1: Add failing tests for viewer grouping and status behavior

**Files:**
- Create: `Bluetooth stuff/log-viewer/viewer-state.test.js`
- Create: `Bluetooth stuff/log-viewer/viewer-state.js`

**Step 1: Write the failing test**

Create tests for these behaviors:
- `Connected` is only shown after an explicit backend connected event, not merely after a WebSocket opens.
- Sensor lines after `Node detected` are grouped under that node until the next `Node detected`.
- Clearing rendered logs does not reset reconnect logic state.

**Step 2: Run test to verify it fails**

Run: `cd "Bluetooth stuff/log-viewer" && bun test`

Expected: FAIL because the state helper does not exist yet.

**Step 3: Write minimal implementation**

Add a small pure helper module that converts backend events into view state and grouped log structures.

**Step 4: Run test to verify it passes**

Run: `cd "Bluetooth stuff/log-viewer" && bun test`

Expected: PASS

### Task 2: Make the Python BLE reader emit structured lifecycle events

**Files:**
- Modify: `Bluetooth stuff/ble_log_receiver.py`

**Step 1: Write the failing test**

Extend the viewer-state test fixture inputs to include `searching`, `connected`, `disconnected`, and `error` backend events so the frontend behavior is pinned down before backend changes.

**Step 2: Run test to verify it fails**

Run: `cd "Bluetooth stuff/log-viewer" && bun test`

Expected: FAIL because the current UI only understands raw text.

**Step 3: Write minimal implementation**

Change the Python reader so it:
- Emits newline-delimited JSON events.
- Retries forever when HMSoft is unavailable.
- Reports `searching`, `connected`, `disconnected`, and `error` transitions.
- Streams each incoming BLE log line as a structured `log` event.

**Step 4: Run test to verify it passes**

Run: `cd "Bluetooth stuff/log-viewer" && bun test`

Expected: PASS

### Task 3: Replace the Bun per-socket spawn with a persistent supervisor

**Files:**
- Modify: `Bluetooth stuff/log-viewer/serve.ts`

**Step 1: Write the failing test**

Add one test that feeds repeated backend status events into the viewer-state helper and confirms reconnect UI behavior survives log clearing and later connected events.

**Step 2: Run test to verify it fails**

Run: `cd "Bluetooth stuff/log-viewer" && bun test`

Expected: FAIL until the event model is fully supported.

**Step 3: Write minimal implementation**

Update the Bun server to:
- Start one long-lived Python subprocess when the server boots.
- Parse newline-delimited JSON from stdout.
- Broadcast each event to all connected WebSocket clients.
- Restart the subprocess if it exits unexpectedly.

**Step 4: Run test to verify it passes**

Run: `cd "Bluetooth stuff/log-viewer" && bun test`

Expected: PASS

### Task 4: Update the page UI for true status and collapsible node groups

**Files:**
- Modify: `Bluetooth stuff/log-viewer/index.html`

**Step 1: Write the failing test**

Add tests for group creation:
- First `Node detected` creates group 1.
- Sensor lines append into the active node group.
- Next `Node detected` closes the current group and starts a new one.
- Non-sensor status lines remain standalone.

**Step 2: Run test to verify it fails**

Run: `cd "Bluetooth stuff/log-viewer" && bun test`

Expected: FAIL until the reducer and DOM rendering logic are updated.

**Step 3: Write minimal implementation**

Update the page so it:
- Displays `Searching for HMSoft...`, `Connected`, or `Disconnected - retrying...` from backend events.
- Uses collapsible node sections with compact headers and sensor counts.
- Leaves `Copy` and `Clear` working on the current rendered data.

**Step 4: Run test to verify it passes**

Run: `cd "Bluetooth stuff/log-viewer" && bun test`

Expected: PASS

### Task 5: Verify locally

**Files:**
- Modify: `Bluetooth stuff/log-viewer/index.html`
- Modify: `Bluetooth stuff/log-viewer/serve.ts`
- Modify: `Bluetooth stuff/ble_log_receiver.py`

**Step 1: Run automated verification**

Run: `cd "Bluetooth stuff/log-viewer" && bun test`

Expected: PASS

**Step 2: Restart the local server**

Run: `cd "Bluetooth stuff/log-viewer" && bun run start`

Expected: Server starts on `http://localhost:3000`.

**Step 3: Manual hardware validation**

Open the page, power-cycle the car, and confirm:
- Status stays `Searching...` until BLE really connects.
- The page reconnects after the car restarts without a manual refresh.
- Sensor lines are grouped under each `Node detected` section and expand/collapse correctly.
