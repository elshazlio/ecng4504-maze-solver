# Narrow Junction And Finish Retune Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Make the maze follower respect narrow 10 cm junctions, stop falsely ending at non-finish nodes, and reduce phantom left/U-turn cascades.

**Architecture:** Keep the existing self-contained controller in `working_code.ino`, but tighten three behaviors: finish qualification, node-scan release, and fallback steering while node scan is active. Drive the change with the host-side regression harness first, then make the minimum code change needed to satisfy the new cases.

**Tech Stack:** Arduino-style C++ in `working_code.ino`, host-side `g++` regression tests in `left_turn_regression_test.cpp`

---

### Task 1: Add failing regression coverage

**Files:**
- Modify: `left_turn_regression_test.cpp`
- Test: `left_turn_regression_test.cpp`

**Step 1: Write the failing test**

Add regression cases for:
- a narrow left junction that briefly returns to centered samples before resolving should still commit left
- an all-black crossing reached from ordinary centered travel should not immediately finish unless finish qualification is satisfied

**Step 2: Run test to verify it fails**

Run: `g++ -std=c++17 left_turn_regression_test.cpp -o /tmp/left_turn_regression_test && /tmp/left_turn_regression_test`
Expected: FAIL in one of the new cases or the existing stale-left-release case

**Step 3: Write minimal implementation**

Update `working_code.ino` so finish detection requires stronger evidence than a single all-black sample, and so active node scan falls back to live steering when the current pattern clearly contradicts the stale candidate.

**Step 4: Run test to verify it passes**

Run: `g++ -std=c++17 left_turn_regression_test.cpp -o /tmp/left_turn_regression_test && /tmp/left_turn_regression_test`
Expected: PASS

### Task 2: Reconcile the secondary logic harness

**Files:**
- Modify: `tests/test_working_code_logic.cpp`
- Modify: `working_code.ino`
- Test: `tests/test_working_code_logic.cpp`

**Step 1: Write or update the failing test**

Bring the helper-level expectations in `tests/test_working_code_logic.cpp` back in sync with the controller API and the new finish-qualification behavior.

**Step 2: Run test to verify it fails**

Run: `g++ -std=c++17 tests/test_working_code_logic.cpp -o /tmp/test_working_code_logic && /tmp/test_working_code_logic`
Expected: FAIL due to outdated helper signature or incorrect behavior expectations

**Step 3: Write minimal implementation**

Adjust helper signatures or tests as needed so the helper-level harness checks the same rules as the main regression file.

**Step 4: Run test to verify it passes**

Run: `g++ -std=c++17 tests/test_working_code_logic.cpp -o /tmp/test_working_code_logic && /tmp/test_working_code_logic`
Expected: PASS

### Task 3: Verify the sketch edits

**Files:**
- Modify: `working_code.ino`

**Step 1: Run regression verification**

Run both host-side test binaries after the code change.

**Step 2: Run lint check**

Use the editor lint diagnostics for `working_code.ino` and the modified test files.

**Step 3: Review behavior assumptions**

Confirm the controller now assumes:
- ordinary 10 cm junctions are not finish markers
- finish requires special qualification
- narrow left branches are not released too quickly
- stale left intent does not override a clear live right-recovery pattern
