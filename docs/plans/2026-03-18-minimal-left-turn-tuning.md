# Minimal Left-Turn Tuning Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Improve reliability of missed left turns in `working_code.ino` with the smallest safe behavior change.

**Architecture:** Keep the current follower structure, use the existing `UNIT_TEST` compatibility path to pin down one missed-left regression, then minimally tune pending intersection memory and left-turn execution timing. Avoid FSM rewrites or unrelated motor-control changes.

**Tech Stack:** Arduino `.ino` sketch, `g++` host-side regression test via `#define UNIT_TEST`, Markdown docs.

---

### Task 1: Reproduce the missed-left behavior in a host-side test

**Files:**
- Modify: `working_code.ino`
- Create: `left_turn_regression_test.cpp`

**Step 1: Write the failing test**

Create a small host-side test that includes `working_code.ino` in `UNIT_TEST` mode and verifies that once a left intersection has been detected, a brief narrower pattern does not immediately erase that pending left intent before the junction is fully crossed.

**Step 2: Run test to verify it fails**

Run: `g++ -std=c++17 left_turn_regression_test.cpp -o left_turn_regression_test && ./left_turn_regression_test`

Expected: FAIL because the current logic resets pending intent too aggressively.

**Step 3: Write minimal implementation**

Adjust the pending-intersection retention rule so committed left/right intent survives short low-sensor-count windows while the robot is still crossing the node.

**Step 4: Run test to verify it passes**

Run: `g++ -std=c++17 left_turn_regression_test.cpp -o left_turn_regression_test && ./left_turn_regression_test`

Expected: PASS

### Task 2: Improve the physical left-turn commitment

**Files:**
- Modify: `working_code.ino`

**Step 1: Write the failing test**

Add one more host-side assertion for a wide-node left classification pattern if needed.

**Step 2: Run test to verify it fails**

Use the same `g++` command and confirm the new expectation fails for the right reason.

**Step 3: Write minimal implementation**

Slightly increase left/right turn commitment by tuning forward creep and spin settle timing without changing the overall control structure.

**Step 4: Run test to verify it passes**

Use the same `g++` command and confirm all assertions pass.

### Task 3: Verify and document

**Files:**
- Modify: `working_code.ino`
- Check: `docs/agdr/AgDR-0007-minimal-left-node-reliability-tuning.md`

**Step 1: Run the regression test**

Run: `g++ -std=c++17 left_turn_regression_test.cpp -o left_turn_regression_test && ./left_turn_regression_test`

Expected: PASS

**Step 2: Check edited files for issues**

Review the changed sketch for consistency and run lints/diagnostics if available.

**Step 3: Hardware validation**

Re-run the car on the same maze and specifically watch the two previously missed left nodes.
