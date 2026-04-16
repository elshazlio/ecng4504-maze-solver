---
id: AgDR-0035
timestamp: 2026-04-02T00:20:13Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Malak line follower: core-sensor P controller

> In the context of `Malak's Stuff/car_code.ino`, facing a line follower that drifts into a slanted stance because steering is on/off and branch sensors can bias alignment, I decided to **use a proportional controller driven by the near-left, center, and near-right sensors, while keeping the far sensors for junction detection and recovery only**, to achieve **straighter line centering without weakening maze-node detection**, accepting **less aggressive recovery when only edge sensors briefly see the line**.

## Context
- `followLine()` currently applies fixed left/right steering steps, so correction strength does not scale with how far the line is from center.
- The sketch already uses the far-left and far-right sensors to detect branches and junctions.
- Letting far sensors influence normal centering can pull the robot sideways when a branch is visible.

## Options Considered

| Option | Pros | Cons |
|--------|------|------|
| **Core 3-sensor P controller (`leftNear`, `center`, `rightNear`)** | Keeps the robot straighter on the main line; avoids branch-induced slant; fits existing junction logic cleanly | Recovery is softer if only an outer sensor briefly sees the line |
| **All 5 sensors in the P error** | More aggressive when the robot drifts far from center | Can bias steering toward visible side branches and reintroduce slanted tracking near nodes |

## Decision
Chosen: **Core 3-sensor proportional steering with far-sensor fallback only for reacquisition**, because the user's goal is straighter tracking on the main line, and the far sensors already have a dedicated maze-decision role.

## Consequences
- Line-follow correction becomes strength-based instead of fixed left/right steps.
- Junction detection remains based on the far sensors, so maze behavior stays consistent with the current design.
- `lineFollowKp` and `maxLineCorrection` become the main tuning knobs for centering behavior.
