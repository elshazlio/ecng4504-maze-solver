#include "../working_code/working_code.ino/turn_logic.h"

int main() {
  TurnScanCounts rightOnlyIncomingLine{0, 1, 3};
  if (chooseIntersectionDecisionFromCounts(DEC_SR, rightOnlyIncomingLine) != DEC_SR) {
    return 1;
  }

  TurnScanCounts boundaryStraightAndRight{0, 2, 3};
  if (chooseIntersectionDecisionFromCounts(DEC_SR, boundaryStraightAndRight) != DEC_SR) {
    return 1;
  }

  TurnScanCounts confirmedStraightAndRight{0, 3, 3};
  if (chooseIntersectionDecisionFromCounts(DEC_SR, confirmedStraightAndRight) != DEC_I) {
    return 1;
  }

  TurnScanCounts confirmedLeftAndRight{3, 3, 3};
  if (chooseIntersectionDecisionFromCounts(DEC_I, confirmedLeftAndRight) != DEC_SL) {
    return 1;
  }

  TurnScanCounts straightBiasedRescanAfterLeftDetection{1, 4, 0};
  if (chooseIntersectionDecisionFromCounts(DEC_SL, straightBiasedRescanAfterLeftDetection) != DEC_SL) {
    return 1;
  }

  TurnScanCounts noConfirmedRescan{0, 0, 1};
  if (chooseIntersectionDecisionFromCounts(DEC_I, noConfirmedRescan) != DEC_I) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(true, 200, 120) != DEC_S) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(true, 300, 120) != DEC_S) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(true, 320, 120) != DEC_S) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(true, 419, 200) != DEC_S) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(true, 420, 119) != DEC_S) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(true, 539, 200) != DEC_S) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(true, 540, 200) != DEC_U) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(true, 600, 119) != DEC_S) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(true, 600, 120) != DEC_U) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(true, 380, 200) != DEC_S) {
    return 1;
  }

  if (chooseNoLineRecoveryDecision(false, 200, 120) != DEC_U) {
    return 1;
  }

  return 0;
}
