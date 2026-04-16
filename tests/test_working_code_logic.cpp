#define UNIT_TEST
#include <cstdlib>
#include <iostream>

#include "../working_code.ino"

static void expectEqual(uint8_t actual, uint8_t expected, const char* message) {
  if (actual != expected) {
    std::cerr << message << " expected " << static_cast<int>(expected)
              << " but got " << static_cast<int>(actual) << '\n';
    std::exit(1);
  }
}

static void expectTrue(bool condition, const char* message) {
  if (!condition) {
    std::cerr << message << '\n';
    std::exit(1);
  }
}

static void test_existing_pattern_helpers() {
  expectEqual(classifyIntersectionDecisionPattern(0, 0, 1, 0, 0), DEC_S, "centered pattern should stay straight");
  expectEqual(classifyIntersectionDecisionPattern(0, 1, 1, 1, 0), DEC_SL, "wide center should still classify as left-biased node");
  expectEqual(classifyIntersectionDecisionPattern(1, 1, 1, 0, 0), DEC_SL, "left-heavy node should classify left");
  expectEqual(classifyIntersectionDecisionPattern(0, 0, 1, 1, 1), DEC_I, "straight-right node should classify as intersection");
  isAllBlack = true;
  allBlackStartTime = 0;
  setTestMillis(0);
  expectEqual(chooseAllBlackDecision(DEC_SL, false), DEC_SL, "all-black should preserve fresh left commit");
  expectEqual(chooseAllBlackDecision(DEC_S, true), DEC_S, "all-black should wait for finish confirmation before ending");
  setTestMillis(FINISH_CONFIRM_MS + 20);
  expectEqual(chooseAllBlackDecision(DEC_S, true), DEC_E, "all-black with a clean approach should end after confirmation");
  expectEqual(chooseAllBlackDecision(DEC_S, false), DEC_S, "all-black without a clean approach should continue straight");
  isAllBlack = false;
  allBlackStartTime = 0;
}

static void test_all_black_preserves_left_after_real_node() {
  resetDecisionStateForTest();
  setTestSensorPattern(0, 1, 1, 1, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "wide node should commit left first");

  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(decideDirectionFull(), DEC_SL, "all-black crossing should preserve a real left node");
}

static void test_all_black_without_node_ends() {
  resetDecisionStateForTest();
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "clear center should stay straight");

  setTestMillis(20);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "a second centered sample should qualify the finish approach");

  setTestMillis(40);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(decideDirectionFull(), DEC_S, "all-black without prior node should confirm before ending");

  setTestMillis(FINISH_CONFIRM_MS + 60);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(decideDirectionFull(), DEC_E, "sustained all-black on a clean approach should end");
}

static void test_all_black_after_right_recovery_does_not_end() {
  resetDecisionStateForTest();
  setTestSensorPattern(0, 0, 0, 1, 0);
  expectEqual(decideDirectionFull(), DEC_R, "right-of-center recovery should steer right first");

  setTestMillis(20);
  setTestSensorPattern(0, 0, 1, 1, 0);
  expectEqual(decideDirectionFull(), DEC_S, "center-right follow should settle toward straight before finish evaluation");

  setTestMillis(40);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(decideDirectionFull(), DEC_S, "all-black after a biased recovery should not count as maze finish");
}

static void test_left_recovery_patterns_both_steer_left() {
  resetDecisionStateForTest();
  setTestSensorPattern(0, 1, 0, 0, 0);
  executeDecision(decideDirectionFull());
  const int softLeftCommand = getTestLastLeftCommand();
  const int softRightCommand = getTestLastRightCommand();

  resetDecisionStateForTest();
  setTestSensorPattern(1, 0, 0, 0, 0);
  executeDecision(decideDirectionFull());
  const int hardLeftCommand = getTestLastLeftCommand();
  const int hardRightCommand = getTestLastRightCommand();

  expectTrue(softLeftCommand < softRightCommand, "soft left recovery should still steer left");
  expectTrue(hardLeftCommand < hardRightCommand, "hard left recovery should still steer left");
}

static void test_single_center_sample_keeps_left_pending_until_node_resolves() {
  resetDecisionStateForTest();
  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left+center should still detect a possible left node");

  setTestMillis(40);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "single centered sample should keep scanning without committing");

  setTestMillis(80);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(
    decideDirectionFull(),
    DEC_SL,
    "a single centered sample during inspection should not erase the pending left turn"
  );
}

static void test_finish_marker_still_works_after_center_release_from_node_scan() {
  resetDecisionStateForTest();
  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left glitch should start a node scan first");

  setTestMillis(20);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "first centered sample should keep scanning");

  setTestMillis(40);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "second centered sample should release the stale node scan");

  setTestMillis(60);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "clean centered follow after release should rebuild finish qualification");

  setTestMillis(80);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "a second clean centered follow should fully qualify the finish approach");

  setTestMillis(100);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(decideDirectionFull(), DEC_S, "finish should still require confirmation after centered node-scan release");

  setTestMillis(100 + FINISH_CONFIRM_MS + 20);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(decideDirectionFull(), DEC_E, "a real finish marker should still end after confirmation");
}

static void test_isolated_left_edge_is_not_a_committed_left_turn() {
  resetDecisionStateForTest();
  setTestSensorPattern(1, 0, 0, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_L,
    "isolated left edge should be treated as recovery steering, not a committed left node"
  );
}

static void test_isolated_right_edge_is_not_a_committed_right_turn() {
  resetDecisionStateForTest();
  setTestSensorPattern(0, 0, 0, 0, 1);
  expectEqual(
    decideDirectionFull(),
    DEC_R,
    "isolated right edge should be treated as recovery steering, not a committed right node"
  );
}

static void test_cleared_left_glitch_does_not_convert_later_all_black_into_left_turn() {
  resetDecisionStateForTest();
  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left+center should still detect a possible left node");

  setTestMillis(40);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "centered follow should continue straight after node entry");

  setTestMillis(80);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "repeated centered follow should keep the robot on the segment");

  setTestMillis(120);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "centered follow after the cleared glitch should rebuild finish qualification");

  setTestMillis(140);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "a second centered follow should fully qualify the finish approach");

  setTestMillis(160);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(decideDirectionFull(), DEC_S, "a cleared left glitch should not replay as a committed left turn before finish confirmation");

  setTestMillis(160 + FINISH_CONFIRM_MS + 20);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(
    decideDirectionFull(),
    DEC_E,
    "a cleared left glitch should not replay as a committed left turn once the finish bubble is confirmed"
  );
}

static void test_stable_turn_reacquire_requires_center_lock() {
  expectTrue(isStableTurnReacquirePattern(0, 0, 1, 0, 0), "center-only lock should count as stable reacquisition");
  expectTrue(!isStableTurnReacquirePattern(0, 1, 1, 0, 0), "left-leaning center hit should not count as stable reacquisition");
  expectTrue(!isStableTurnReacquirePattern(0, 0, 1, 1, 0), "right-leaning center hit should not count as stable reacquisition");
}

static void test_uturn_prefers_clockwise_spin() {
  resetDecisionStateForTest();
  setTestSensorPattern(0, 0, 1, 0, 0);
  executeDecision(DEC_U);
  expectTrue(testSawSpinRight(), "U-turn should use clockwise/right spin");
  expectTrue(!testSawSpinLeft(), "clockwise U-turn should not use the left spin path");
}

static void test_ble_telemetry_is_temporarily_disabled() {
  expectTrue(!isBluetoothTelemetryEnabled(), "BLE telemetry should be disabled while the HM-10 module is unavailable");
}

int main() {
  test_existing_pattern_helpers();
  test_all_black_preserves_left_after_real_node();
  test_all_black_without_node_ends();
  test_all_black_after_right_recovery_does_not_end();
  test_left_recovery_patterns_both_steer_left();
  test_single_center_sample_keeps_left_pending_until_node_resolves();
  test_finish_marker_still_works_after_center_release_from_node_scan();
  test_isolated_left_edge_is_not_a_committed_left_turn();
  test_isolated_right_edge_is_not_a_committed_right_turn();
  test_cleared_left_glitch_does_not_convert_later_all_black_into_left_turn();
  test_stable_turn_reacquire_requires_center_lock();
  test_uturn_prefers_clockwise_spin();
  test_ble_telemetry_is_temporarily_disabled();
  std::cout << "test_working_code_logic passed\n";
  return 0;
}
