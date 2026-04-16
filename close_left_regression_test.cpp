#define UNIT_TEST
#include <cstdlib>
#include <iostream>

#include "working_code.ino"

static void expectEqual(uint8_t actual, uint8_t expected, const char* message) {
  if (actual != expected) {
    std::cerr << message << " expected " << static_cast<int>(expected)
              << " but got " << static_cast<int>(actual) << '\n';
    std::exit(1);
  }
}

static void advanceCommittedLeftToSpinPhase();

static void test_stale_01100_without_post_left_assist_stays_straight() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "baseline left node should still commit left");

  setTestMillis(LEFT_OUTER_SENSOR_MEMORY_MS + 10);
  setTestSensorPattern(0, 1, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_S,
    "without a committed left assist window, stale 01100 should remain straight follow"
  );
}

static void test_close_successive_left_after_committed_left_still_commits() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "first left junction should commit normally");

  executeDecision(DEC_SL);
  advanceCommittedLeftToSpinPhase();
  setTestSensorPattern(0, 0, 1, 0, 0);
  serviceCommittedTurn();
  serviceCommittedTurn();

  setTestSensorPattern(0, 1, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_SL,
    "after a committed left completes, a very close 01100 next junction should still commit left"
  );
}

static void test_post_left_close_junction_assist_expires() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "first left junction should commit before assist expiry");

  executeDecision(DEC_SL);
  advanceCommittedLeftToSpinPhase();
  setTestSensorPattern(0, 0, 1, 0, 0);
  serviceCommittedTurn();
  serviceCommittedTurn();

  setTestMillis(millis() + POST_LEFT_CLOSE_JUNCTION_ASSIST_MS + 10);
  setTestSensorPattern(0, 1, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_S,
    "after the post-turn assist window expires, isolated 01100 should fall back to straight follow"
  );
}

// executeDecision(DEC_SL) must arm immediately (no delay / no blocking align loop).
static void test_execute_sl_does_not_synchronously_burn_align_timeout() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "junction should commit close left before execution test");

  // No center line during creep + align: synchronous spinToCenterSensor runs until JUNCTION_TURN_TIMEOUT_MS.
  setTestSensorPattern(1, 0, 0, 0, 0);

  unsigned long t0 = millis();
  executeDecision(DEC_SL);
  unsigned long dt = millis() - t0;

  if (dt != 0) {
    std::cerr
      << "executeDecision(DEC_SL) should arm immediately without advancing millis; dt="
      << dt << " ms\n";
    std::exit(1);
  }
  if (!isCloseLeftManeuverActive()) {
    std::cerr << "close-left maneuver should remain active (armed asynchronously) after executeDecision\n";
    std::exit(1);
  }
}

static void test_non_left_decisions_clear_close_left_active_flag() {
  resetDecisionStateForTest();

  executeDecision(DEC_SL);
  if (!isCloseLeftManeuverActive()) {
    std::cerr << "close-left maneuver should arm before clear-state checks\n";
    std::exit(1);
  }

  executeDecision(DEC_S);
  if (isCloseLeftManeuverActive()) {
    std::cerr << "DEC_S should clear a stale close-left maneuver flag\n";
    std::exit(1);
  }

  executeDecision(DEC_SL);
  executeDecision(DEC_I);
  if (isCloseLeftManeuverActive()) {
    std::cerr << "DEC_I should clear a stale close-left maneuver flag\n";
    std::exit(1);
  }

  executeDecision(DEC_SL);
  executeDecision(255);
  if (isCloseLeftManeuverActive()) {
    std::cerr << "default executeDecision path should clear a stale close-left maneuver flag\n";
    std::exit(1);
  }
}

// Phase progression: bounded creep (forward) before any left spin is commanded.
static void test_committed_left_starts_with_bounded_creep_not_spin() {
  resetDecisionStateForTest();
  setTestSensorPattern(1, 0, 1, 0, 0);
  executeDecision(DEC_SL);

  setTestMillis(STOP_SETTLE_MS + 1);
  serviceCommittedTurn();
  if (getTestLastLeftCommand() != BASE_SPEED || getTestLastRightCommand() != BASE_SPEED) {
    std::cerr << "after pre-creep settle, serviceCommittedTurn should command forward creep "
                 "(both sides BASE_SPEED)\n";
    std::exit(1);
  }
  if (testSawSpinLeft()) {
    std::cerr << "spin left must not start during the creep phase\n";
    std::exit(1);
  }
}

static void advanceCommittedLeftToSpinPhase() {
  setTestMillis(STOP_SETTLE_MS + 1);
  serviceCommittedTurn();

  setTestMillis(millis() + JUNCTION_CREEP_MS + 1);
  serviceCommittedTurn();

  setTestMillis(millis() + STOP_SETTLE_MS + 1);
  serviceCommittedTurn();
}

static void completeCommittedLeftTurnAfterLongRuntime() {
  advanceCommittedLeftToSpinPhase();

  setTestMillis(millis() + POST_LEFT_CLOSE_JUNCTION_ASSIST_MS + 10);
  setTestSensorPattern(1, 0, 0, 0, 0);
  serviceCommittedTurn();

  setTestSensorPattern(0, 0, 1, 0, 0);
  serviceCommittedTurn();
  serviceCommittedTurn();
}

// A large time jump must still service the creep phase before spin starts.
static void test_large_time_jump_still_services_creep_before_spin() {
  resetDecisionStateForTest();
  setTestSensorPattern(1, 0, 1, 0, 0);
  executeDecision(DEC_SL);

  // Even if millis jumps far ahead, the first service call must not skip the actual creep output.
  unsigned long largeJumpMs =
      STOP_SETTLE_MS + JUNCTION_CREEP_MS + STOP_SETTLE_MS + STOP_SETTLE_MS + 1;
  setTestMillis(largeJumpMs);
  setTestSensorPattern(1, 0, 0, 0, 0);

  unsigned long before = millis();
  serviceCommittedTurn();
  unsigned long after = millis();
  if (after != before) {
    std::cerr << "serviceCommittedTurn must not block with delay; millis advanced by "
              << (after - before) << " ms\n";
    std::exit(1);
  }
  if (getTestLastLeftCommand() != BASE_SPEED || getTestLastRightCommand() != BASE_SPEED) {
    std::cerr << "large time jump should still produce a real creep command before spin\n";
    std::exit(1);
  }
  if (testSawSpinLeft()) {
    std::cerr << "large time jump must not skip straight to spin/reacquire\n";
    std::exit(1);
  }
}

// Pure center-only outgoing line still clears committed state after two consistent snapshots.
static void test_committed_left_clears_after_stable_line_reacquire() {
  resetDecisionStateForTest();
  setTestSensorPattern(1, 0, 1, 0, 0);
  executeDecision(DEC_SL);

  setTestSensorPattern(1, 0, 0, 0, 0);
  advanceCommittedLeftToSpinPhase();
  if (!isCloseLeftManeuverActive()) {
    std::cerr << "maneuver should stay active until line is reacquired\n";
    std::exit(1);
  }

  setTestSensorPattern(0, 0, 1, 0, 0);
  serviceCommittedTurn();
  if (!isCloseLeftManeuverActive()) {
    std::cerr << "require two consecutive stable center-only samples before clearing turn\n";
    std::exit(1);
  }

  serviceCommittedTurn();
  if (isCloseLeftManeuverActive()) {
    std::cerr << "committed left should clear after stable outgoing line (2 samples)\n";
    std::exit(1);
  }
}

static void test_left_biased_center_lock_can_finish_committed_left() {
  resetDecisionStateForTest();
  setTestSensorPattern(1, 0, 1, 0, 0);
  executeDecision(DEC_SL);
  advanceCommittedLeftToSpinPhase();

  if (!isCloseLeftManeuverActive()) {
    std::cerr << "maneuver should still be active before left-biased center reacquire\n";
    std::exit(1);
  }

  setTestSensorPattern(0, 1, 1, 0, 0);
  serviceCommittedTurn();
  serviceCommittedTurn();

  if (isCloseLeftManeuverActive()) {
    std::cerr << "left-biased center lock should be able to finish a committed left on tight exits\n";
    std::exit(1);
  }

  expectEqual(
    decideDirectionFull(),
    DEC_SL,
    "once the committed left finishes on 01100, the next close left should be visible immediately"
  );
}

static void test_post_turn_close_left_assist_survives_long_left_completion() {
  resetDecisionStateForTest();

  setTestSensorPattern(1, 0, 1, 0, 0);
  expectEqual(decideDirectionFull(), DEC_SL, "first left junction should commit before long completion test");

  executeDecision(DEC_SL);
  completeCommittedLeftTurnAfterLongRuntime();

  if (isCloseLeftManeuverActive()) {
    std::cerr << "close-left maneuver should be finished before post-turn assist check\n";
    std::exit(1);
  }

  setTestSensorPattern(0, 1, 1, 0, 0);
  expectEqual(
    decideDirectionFull(),
    DEC_SL,
    "post-turn close-left assist should be armed when the committed left finishes, not when it starts"
  );
}

static void test_training_does_not_record_duplicate_l_while_same_left_is_active() {
  resetDecisionStateForTest();
  state = TRAINING;
  hasMovedSinceStart = true;

  setTestMillis(INTERSECTION_DELAY + 1);
  setTestSensorPattern(1, 0, 1, 0, 0);
  loop();

  if (pathLen != 1 || pathRecorded[0] != 'L') {
    std::cerr << "first training pass should record one L for the committed left\n";
    std::exit(1);
  }
  if (!isCloseLeftManeuverActive()) {
    std::cerr << "committed left should stay active after the first recorded training pass\n";
    std::exit(1);
  }

  setTestMillis(millis() + INTERSECTION_DELAY + 1);
  setTestSensorPattern(1, 0, 1, 0, 0);
  loop();

  if (pathLen != 1) {
    std::cerr << "training should not append a second L while the same committed left is still active\n";
    std::exit(1);
  }
}

static void test_training_does_not_record_s_when_active_left_flickers_to_straight() {
  resetDecisionStateForTest();
  state = TRAINING;
  hasMovedSinceStart = true;

  setTestMillis(INTERSECTION_DELAY + 1);
  setTestSensorPattern(1, 0, 1, 0, 0);
  loop();

  if (pathLen != 1 || pathRecorded[0] != 'L') {
    std::cerr << "first training pass should record one L before straight-flicker check\n";
    std::exit(1);
  }
  if (!isCloseLeftManeuverActive()) {
    std::cerr << "committed left should still be active before straight-flicker check\n";
    std::exit(1);
  }

  setTestMillis(millis() + INTERSECTION_DELAY + 1);
  setTestSensorPattern(0, 0, 1, 1, 1);
  loop();

  if (pathLen != 1) {
    std::cerr << "training should not append S/R while the same committed left is still active\n";
    std::exit(1);
  }
}

int main() {
  test_stale_01100_without_post_left_assist_stays_straight();
  test_close_successive_left_after_committed_left_still_commits();
  test_post_left_close_junction_assist_expires();
  test_execute_sl_does_not_synchronously_burn_align_timeout();
  test_non_left_decisions_clear_close_left_active_flag();
  test_committed_left_starts_with_bounded_creep_not_spin();
  test_large_time_jump_still_services_creep_before_spin();
  test_committed_left_clears_after_stable_line_reacquire();
  test_left_biased_center_lock_can_finish_committed_left();
  test_post_turn_close_left_assist_survives_long_left_completion();
  test_training_does_not_record_duplicate_l_while_same_left_is_active();
  test_training_does_not_record_s_when_active_left_flickers_to_straight();
  std::cout << "close_left_regression_test passed\n";
  return 0;
}
