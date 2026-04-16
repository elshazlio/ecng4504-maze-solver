#define UNIT_TEST
#include "../one_shot_v1.ino"

static SensorSnapshot makeSnapshot(bool sensor0,
                                   bool sensor1,
                                   bool sensor2,
                                   bool sensor3,
                                   bool sensor4) {
  SensorSnapshot snapshot = {};
  snapshot.line[0] = sensor0;
  snapshot.line[1] = sensor1;
  snapshot.line[2] = sensor2;
  snapshot.line[3] = sensor3;
  snapshot.line[4] = sensor4;
  snapshot.activeCount = static_cast<uint8_t>(sensor0 + sensor1 + sensor2 + sensor3 + sensor4);
  snapshot.allBlack = snapshot.activeCount == 5;
  snapshot.centerSeen = sensor1 || sensor2 || sensor3;
  snapshot.leftEdgeSeen = sensor0;
  snapshot.rightEdgeSeen = sensor4;
  return snapshot;
}

static void applySensorPattern(bool sensor0,
                               bool sensor1,
                               bool sensor2,
                               bool sensor3,
                               bool sensor4) {
  resetTestPinStates();
  g_testPinLow[SENS_PINS[0]] = sensor0;
  g_testPinLow[SENS_PINS[1]] = sensor1;
  g_testPinLow[SENS_PINS[2]] = sensor2;
  g_testPinLow[SENS_PINS[3]] = sensor3;
  g_testPinLow[SENS_PINS[4]] = sensor4;
}

int main() {
  if (isNodeCandidatePattern(false, false, true, false, false)) {
    return 1;
  }

  if (!isNodeCandidatePattern(false, true, true, false, false)) {
    return 1;
  }

  if (!isNodeCandidatePattern(false, false, true, true, false)) {
    return 1;
  }

  if (isNodeCandidatePattern(false, true, true, true, false)) {
    return 1;
  }

  if (!isNodeCandidatePattern(false, false, false, false, true)) {
    return 1;
  }

  char path1[8] = {'L', 'B'};
  uint8_t path1Length = 2;
  if (!appendAndReduceMove(path1, path1Length, 8, 'L') || path1Length != 1 || path1[0] != 'S') {
    return 1;
  }

  char path2[] = {'L', 'B', 'S'};
  uint8_t path2Length = 3;
  if (!reduceLatestPath(path2, path2Length) || path2Length != 1 || path2[0] != 'R') {
    return 1;
  }

  char path3[] = {'L', 'B', 'R'};
  uint8_t path3Length = 3;
  if (!reduceLatestPath(path3, path3Length) || path3Length != 1 || path3[0] != 'B') {
    return 1;
  }

  char path4[] = {'S', 'B', 'L'};
  uint8_t path4Length = 3;
  if (!reduceLatestPath(path4, path4Length) || path4Length != 1 || path4[0] != 'R') {
    return 1;
  }

  char path5[] = {'S', 'B', 'S'};
  uint8_t path5Length = 3;
  if (!reduceLatestPath(path5, path5Length) || path5Length != 1 || path5[0] != 'B') {
    return 1;
  }

  char path6[] = {'S', 'B', 'R'};
  uint8_t path6Length = 3;
  if (!reduceLatestPath(path6, path6Length) || path6Length != 1 || path6[0] != 'L') {
    return 1;
  }

  char path7[] = {'R', 'B', 'L'};
  uint8_t path7Length = 3;
  if (!reduceLatestPath(path7, path7Length) || path7Length != 1 || path7[0] != 'B') {
    return 1;
  }

  char path8[] = {'R', 'B', 'S'};
  uint8_t path8Length = 3;
  if (!reduceLatestPath(path8, path8Length) || path8Length != 1 || path8[0] != 'L') {
    return 1;
  }

  char path9[] = {'R', 'B', 'R'};
  uint8_t path9Length = 3;
  if (!reduceLatestPath(path9, path9Length) || path9Length != 1 || path9[0] != 'S') {
    return 1;
  }

  if (chooseLeftHandDecision(true, true, true, true, false) != MOVE_LEFT) {
    return 1;
  }

  if (chooseLeftHandDecision(false, true, true, false, false) != MOVE_STRAIGHT) {
    return 1;
  }

  if (chooseLeftHandDecision(false, false, true, false, false) != MOVE_RIGHT) {
    return 1;
  }

  if (chooseLeftHandDecision(false, false, false, false, false) != MOVE_BACK) {
    return 1;
  }

  if (chooseLeftHandDecision(false, false, false, true, true) != MOVE_LEFT) {
    return 1;
  }

  const char storedPath[] = {'L', 'S', 'R', 'B'};
  const uint8_t checksum = computePathChecksum(storedPath, 4);
  if (!isStoredPathRecordValid(kPathRecordSignature, 1, storedPath, 4, checksum)) {
    return 1;
  }

  if (isStoredPathRecordValid(0x00, 1, storedPath, 4, checksum)) {
    return 1;
  }

  if (isStoredPathRecordValid(kPathRecordSignature, 1, storedPath, 4, checksum + 1)) {
    return 1;
  }

  if (advanceReplayIndex(0, 4) != 1) {
    return 1;
  }

  if (advanceReplayIndex(4, 4) != 4) {
    return 1;
  }

  if (classifyBubbleEvent(false, false, false) != BUBBLE_START_TRAINING) {
    return 1;
  }

  if (classifyBubbleEvent(true, false, false) != BUBBLE_FINISH_TRAINING) {
    return 1;
  }

  if (classifyBubbleEvent(false, true, true) != BUBBLE_FINISH_SOLVE) {
    return 1;
  }

  if (classifyBubbleEvent(false, false, true) != BUBBLE_IGNORE) {
    return 1;
  }

  if (chooseBangBangSteering(makeSnapshot(false, false, true, false, false), 1) != STEER_STRAIGHT) {
    return 1;
  }

  if (chooseBangBangSteering(makeSnapshot(false, true, true, false, false), 1) != STEER_SOFT_LEFT) {
    return 1;
  }

  if (chooseBangBangSteering(makeSnapshot(true, true, false, false, false), 1) != STEER_HARD_LEFT) {
    return 1;
  }

  if (chooseBangBangSteering(makeSnapshot(false, false, true, true, false), -1) != STEER_SOFT_RIGHT) {
    return 1;
  }

  if (chooseBangBangSteering(makeSnapshot(false, false, false, true, true), -1) != STEER_HARD_RIGHT) {
    return 1;
  }

  if (chooseBangBangSteering(makeSnapshot(false, true, false, true, false), -1) != STEER_SOFT_LEFT) {
    return 1;
  }

  if (chooseBangBangSteering(makeSnapshot(false, true, false, true, false), 1) != STEER_SOFT_RIGHT) {
    return 1;
  }

  resetLineFollowerState();
  if (chooseBangBangSteering(makeSnapshot(false, true, false, true, false), g_lastLineSign) != STEER_STRAIGHT) {
    return 1;
  }

  resetLineFollowerState();
  resetTestMotorCommands();
  resetTestPinStates();
  followLineStep(CRUISE_PWM);
  if (g_testLastLeftCommand != RECOVERY_PWM || g_testLastRightCommand != RECOVERY_PWM) {
    return 1;
  }

  resetLineFollowerState();
  resetTestMotorCommands();
  applySensorPattern(false, true, true, false, false);
  followLineStep(CRUISE_PWM);
  if (g_lastLineSign != -1) {
    return 1;
  }
  if (g_testLastLeftCommand != static_cast<int>(CRUISE_PWM) - FOLLOW_SOFT_BRAKE_PWM ||
      g_testLastRightCommand != static_cast<int>(CRUISE_PWM) + FOLLOW_SOFT_BOOST_PWM) {
    return 1;
  }

  resetTestMotorCommands();
  resetTestPinStates();
  followLineStep(CRUISE_PWM);
  if (g_testLastLeftCommand != -RECOVERY_PWM || g_testLastRightCommand != RECOVERY_PWM) {
    return 1;
  }

  if (chooseBangBangSteering(makeSnapshot(false, false, false, false, false), 1) != STEER_LOST) {
    return 1;
  }

  return 0;
}
