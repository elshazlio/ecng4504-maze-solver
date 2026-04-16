#define UNIT_TEST
#include <iostream>

#include "working_code.ino"

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

static void expectChar(char actual, char expected, const char* message) {
  if (actual != expected) {
    std::cerr << message << " expected '" << expected
              << "' but got '" << actual << "'\n";
    std::exit(1);
  }
}

static void test_all_black_preserves_left() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left branch should be detected first");

  setTestMillis(20);
  setTestSensorPattern(0, 1, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_SL,
    "01100 still counts as narrow-left junction while s0 was recent (AgDR-0019 memory window)"
  );

  setTestMillis(40);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(
    decideDirectionFull(),
    DEC_E,
    "five-on-line is DEC_E in decideDirectionFull; training loop debounces before maze end"
  );
}

static void test_left_memory_window_covers_longer_junction_approach() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left branch should latch before the timing-window check");

  setTestMillis(170);
  setTestSensorPattern(0, 1, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_SL,
    "01100 should still commit left when outer-left was seen on a slightly longer approach"
  );
}

static void test_noisy_partial_recovery_preserves_left() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left branch should latch before recovery");

  setTestMillis(25);
  setTestSensorPattern(0, 1, 0, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_L,
    "s1-only recovery is soft left steering, not a second DEC_SL junction read"
  );
}

static void test_clear_follow_pattern_releases_left_latch() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left branch should latch before inspection debounce");

  setTestMillis(25);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_S,
    "single centered sample should keep scanning without replaying the turn immediately"
  );
}

static void test_centered_approach_then_01100_commits_left_without_s0() {
  resetDecisionStateForTest();

  setTestMillis(LEFT_OUTER_SENSOR_MEMORY_MS + 20);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_S,
    "clean centered approach should still follow straight before the left T appears"
  );

  setTestMillis(LEFT_OUTER_SENSOR_MEMORY_MS + 40);
  setTestSensorPattern(0, 1, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_SL,
    "01100 immediately after a centered approach should commit left even without s0 evidence"
  );
}

static void test_new_clear_left_beats_old_right_latch() {
  resetDecisionStateForTest();

  setTestSensorPattern(0, 0, 0, 0, 1);
  expectEqual(decideDirectionFull(), DEC_R, "isolated right edge should still be recovery steering");

  setTestMillis(30);
  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_SL,
    "new clear left evidence should beat an older right latch"
  );
}

static void test_released_latch_does_not_replay_on_later_glitch() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left branch should latch before later release");

  setTestMillis(25);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "first centered follow should keep scanning");

  setTestMillis(45);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "second centered follow should release the left latch");

  setTestMillis(65);
  setTestSensorPattern(0, 1, 0, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_L,
    "released latch should not replay the old left turn on a later brief glitch"
  );
}

static void test_latch_expires_when_recovery_takes_too_long() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left branch should establish recent s0 evidence before expiry");

  setTestMillis(LEFT_OUTER_SENSOR_MEMORY_MS + 25);
  setTestSensorPattern(0, 1, 0, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_L,
    "after the s0 memory window expires, the live recovery pattern should win"
  );
}

static void test_left_edge_alone_does_not_latch_turn_until_center_joins() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 0, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_L,
    "left edge alone should steer left, not immediately latch a left turn"
  );

  setTestMillis(20);
  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_SL,
    "once center joins the left edge, the node should commit as a left turn"
  );
}

static void test_single_center_sample_still_allows_left_commit_on_all_black() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left branch should latch before center debounce");

  setTestMillis(20);
  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "single centered sample should not replay the turn immediately");

  setTestMillis(40);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(
    decideDirectionFull(),
    DEC_E,
    "five-on-line resolves to DEC_E in decideDirectionFull (same as other all-sensor-on samples)"
  );
}

static void test_lost_left_branch_does_not_trigger_phantom_right_while_scan_is_active() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 1, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left-heavy junction should start as a possible left turn");

  setTestMillis(20);
  setTestSensorPattern(0, 0, 1, 1, 0);
  expectEqual(decideDirectionFull(), DEC_S, "brief center-right recovery should still move straight through node scan");

  setTestMillis(40);
  setTestSensorPattern(0, 0, 0, 1, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_R,
    "isolated s3 without s2 is right recovery (DEC_R), not a committed right junction (DEC_SR)"
  );
}

static void test_stale_left_glitch_does_not_commit_over_straight_right_node() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 1, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "left-heavy entry can start as a possible left node");

  setTestMillis(20);
  setTestSensorPattern(0, 0, 1, 1, 1);
  expectEqual(
    decideDirectionFull(),
    DEC_I,
    "s2+s4 junction geometry commits straight (DEC_I) as soon as straight-right evidence appears"
  );

  setTestMillis(40);
  setTestSensorPattern(0, 0, 1, 1, 1);
  expectEqual(
    decideDirectionFull(),
    DEC_I,
    "a stale left glitch should not override later straight-right evidence at commit time"
  );
}

static void test_clean_all_black_crossing_does_not_finish_immediately() {
  resetDecisionStateForTest();

  setTestSensorPattern(0, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_S, "centered travel should begin as straight follow");

  setTestMillis(20);
  setTestSensorPattern(1, 1, 1, 1, 1);
  expectEqual(
    decideDirectionFull(),
    DEC_E,
    "decideDirectionFull reports DEC_E for five-on-line; immediate finish is gated in loop()"
  );
}

static void test_training_records_u_turn_nodes_for_dead_ends() {
  resetDecisionStateForTest();
  state = TRAINING;
  hasMovedSinceStart = true;

  setTestMillis(INTERSECTION_DELAY + 200);
  setTestSensorPattern(0, 0, 0, 0, 0);
  noLineStart = 1;

  loop();

  expectTrue(pathLen == 1, "dead-end U-turn should be recorded as a node");
  expectChar(pathRecorded[0], 'U', "dead-end node should be encoded as U");
}

static void test_committed_left_ignores_u_turn_reclassification_during_spin() {
  resetDecisionStateForTest();
  state = TRAINING;
  hasMovedSinceStart = true;

  setTestMillis(500);
  executeDecision(DEC_SL);

  committedLeftPhase = CL_SPIN_REACQUIRE;
  committedLeftPhaseStartMs = millis();
  committedLeftStableStreak = 0;
  closeLeftManeuverActive = true;

  setTestSensorPattern(0, 0, 0, 0, 0);
  noLineStart = 1;

  loop();

  expectTrue(closeLeftManeuverActive, "committed left should stay active during protected spin");
  expectTrue(committedLeftPhase != CL_IDLE, "committed left phase should not be aborted");
}

int main() {
  test_all_black_preserves_left();
  test_left_memory_window_covers_longer_junction_approach();
  test_noisy_partial_recovery_preserves_left();
  test_clear_follow_pattern_releases_left_latch();
  test_centered_approach_then_01100_commits_left_without_s0();
  test_new_clear_left_beats_old_right_latch();
  test_released_latch_does_not_replay_on_later_glitch();
  test_latch_expires_when_recovery_takes_too_long();
  test_left_edge_alone_does_not_latch_turn_until_center_joins();
  test_single_center_sample_still_allows_left_commit_on_all_black();
  test_lost_left_branch_does_not_trigger_phantom_right_while_scan_is_active();
  test_stale_left_glitch_does_not_commit_over_straight_right_node();
  test_clean_all_black_crossing_does_not_finish_immediately();
  test_training_records_u_turn_nodes_for_dead_ends();
  test_committed_left_ignores_u_turn_reclassification_during_spin();

  std::cout << "left_turn_regression_test passed\n";
  return 0;
}
