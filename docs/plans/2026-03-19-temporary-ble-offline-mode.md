# Temporary BLE Offline Mode Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Disable HM-10 behavior safely while the module is dead, keep USB serial telemetry active, and make the BLE tools/docs report the temporary outage clearly.

**Architecture:** Keep the existing BLE structure in place, but gate firmware BLE usage behind one compile-time flag and make the support tools surface an explicit offline status instead of attempting live BLE work. This preserves the restoration path for the replacement module while avoiding misleading behavior now.

**Tech Stack:** Arduino-style C++ in `working_code.ino`, host-side `g++` tests in `tests/test_working_code_logic.cpp`, Python BLE scripts, Bun-based local log viewer

---

### Task 1: Add a failing firmware regression for BLE offline mode

**Files:**
- Modify: `tests/test_working_code_logic.cpp`
- Modify: `working_code.ino`
- Test: `tests/test_working_code_logic.cpp`

**Step 1: Write the failing test**

Add a test that expects the firmware to expose BLE as disabled while the outage flag is active.

**Step 2: Run test to verify it fails**

Run: `g++ -std=c++17 tests/test_working_code_logic.cpp -o /tmp/test_working_code_logic && /tmp/test_working_code_logic`
Expected: FAIL because the sketch does not yet expose the outage flag behavior

**Step 3: Write minimal implementation**

Add a compile-time BLE availability flag, route dual-print helpers through USB serial only when BLE is unavailable, and skip `BT.begin()` in `setup()`.

**Step 4: Run test to verify it passes**

Run: `g++ -std=c++17 tests/test_working_code_logic.cpp -o /tmp/test_working_code_logic && /tmp/test_working_code_logic`
Expected: PASS

### Task 2: Mark BLE tools as temporarily unavailable

**Files:**
- Modify: `Bluetooth stuff/ble_scan.py`
- Modify: `Bluetooth stuff/ble_log_receiver.py`
- Modify: `Bluetooth stuff/log-viewer/serve.ts`
- Modify: `Bluetooth stuff/log-viewer/viewer-state.js`
- Modify: `Bluetooth stuff/log-viewer/index.html`

**Step 1: Write the failing expectation**

Define the desired temporary-outage behavior: scripts should print/report that HM-10 is unavailable, and the viewer should show an offline label instead of searching forever.

**Step 2: Implement minimal outage status**

Change the tools so they short-circuit into an explicit outage status without attempting BLE connection logic.

**Step 3: Verify behavior**

Run the relevant local commands and confirm the reported status text reflects the outage.

### Task 3: Update documentation

**Files:**
- Modify: `docs/devices.md`

**Step 1: Update current hardware status**

Mark the HM-10 entry and associated workflow as temporarily unavailable due to hardware failure.

**Step 2: Verify wording**

Confirm the docs distinguish between temporary outage and permanent removal.
