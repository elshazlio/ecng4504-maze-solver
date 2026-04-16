---
name: maze reliability strategy
overview: Research-backed strategy to improve maze-running reliability by separating straight-line control from intersection decision logic, reducing false left decisions, and standardizing dead-end recovery.
todos:
  - id: capture-failures
    content: Document the exact straight, missed-left, and dead-end failure patterns from current runs
    status: pending
  - id: verify-sensors
    content: Confirm whether the 5-sensor array provides analog readings or only digital outputs
    status: pending
  - id: choose-controller
    content: "Adopt hybrid control: weighted/PD segment following plus short-debounce intersection FSM"
    status: pending
  - id: standardize-uturn
    content: Empirically compare clockwise vs anticlockwise 180s and pick the more reliable execution direction
    status: pending
  - id: define-test-matrix
    content: Create a repeatable tuning checklist for straights, left nodes, T-junctions, and dead ends
    status: pending
isProject: false
---

# Maze Reliability Strategy

## What The Research Says

- Similar 5-sensor line-maze robots usually split the problem into two layers:
- `segment following`: continuous line tracking using weighted line position plus `P`/`PD` steering.
- `node handling`: short-lived state machine for intersections, dead ends, and committed turns.
- Reliable projects tend to use short temporal confirmation for discrete events, not long averaging windows. A 20-sample vote can help as a debounce layer, but it is usually too blunt to be the main decision mechanism for fast intersections.
- Dead ends are commonly recorded as abstract `B`/back at the maze layer, while clockwise vs anticlockwise 180s is chosen at the motion layer based on which one reacquires the line more reliably on the actual chassis.

## Current Code Risks

- `[working_code.ino](/Users/elshazlio/Documents/My Projects/Embedded/working_code.ino)` currently uses pattern classification for both normal tracking and intersection decisions. That makes it vulnerable to transient sensor blips and motor bias.
- The `pendingIntersectionDecision` latch and `all black` handling can preserve a stale left-turn intent long enough to create phantom lefts on what should be straights.
- Turn completion currently depends on reacquiring the center sensor quickly, which is a common source of missed left turns and unstable U-turn exits.

## Approaches Considered

- **Approach 1: Keep current pattern+latch design, add long-window averaging**
- Pros: smallest code change, easy to explain.
- Cons: likely adds delay, can miss brief left branches, and does not address motor bias or fragile turn reacquisition.
- **Approach 2: Hybrid controller with short debounce and explicit intersection FSM**
- Pros: matches how similar projects are usually built, addresses both phantom lefts and missed-left intersections, and keeps the left-hand rule intact at the decision layer.
- Cons: moderate refactor and tuning effort.
- **Approach 3: Hardware-assisted correction with encoders/closed-loop wheel speed**
- Pros: strongest cure for left/right bias.
- Cons: larger hardware/software scope and may be unnecessary if trim plus better logic is enough.

## Recommendation

- Choose **Approach 2** first.
- Keep a small amount of temporal buffering, but use it only to confirm events such as `intersection seen`, `dead end confirmed`, and `line reacquired`.
- If the robot still pulls left after this redesign, add a motor trim step next; if encoders exist, then closed-loop wheel balancing becomes the next upgrade.

## Practical Strategy

- Separate the control logic in `[working_code.ino](/Users/elshazlio/Documents/My Projects/Embedded/working_code.ino)` into four behaviors:
- `followSegment`: continuous steering for normal straights/curves.
- `inspectNode`: move slightly into the node and latch `found_left`, `found_straight`, `found_right`.
- `executeCommittedTurn`: once a turn is chosen, stop re-deciding until the outgoing line is stably reacquired.
- `recoverLostLineOrDeadEnd`: confirm loss briefly, then perform a standardized back turn.
- Treat the colleague’s `20 recent readings` idea as a **debounce aid**, not the main controller:
- Good use: require 3-5 recent confirmations before declaring `left exists` or `dead end`.
- Bad use: average 20 samples to decide steering or intersection type while the robot is actively crossing a node.
- Test clockwise and anticlockwise `DEC_U` physically and standardize on whichever reacquires the center line more reliably; keep the maze path symbol as `B`/U-turn regardless.

## Evidence To Gather Before Any Rewrite

- Record repeated runs for three cases: straight segment, left intersection, dead end.
- Compare whether phantom lefts correlate more with sensor patterns or with equal-PWM motor bias.
- Check whether the 5-eye module exposes analog values or only digital thresholds; analog makes weighted position and calibration much stronger.

## Success Criteria

- The robot can traverse straight segments without spontaneous left decisions.
- Left intersections are detected consistently during exploration.
- Dead-end recovery uses the mechanically more reliable 180-degree direction.
- Maze exploration still follows left-hand rule, but physical turning becomes more robust and deterministic.

## Key References

- Pololu maze-solving example: left-hand rule, phased intersection inspection, and path simplification.
- QTR sensor usage notes: calibration plus weighted line position with `PD`.
- UKMARS line-follower notes: last-seen-direction recovery and event-based logic for markers/crossings.

