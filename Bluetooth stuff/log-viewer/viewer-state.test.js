import { expect, test } from "bun:test";
import {
  applyEvent,
  copyTextFromState,
  createInitialState,
  toggleGroup,
} from "./viewer-state.js";

test("starts in searching state instead of connected", () => {
  const state = createInitialState();

  expect(state.connection.state).toBe("searching");
  expect(state.connection.label).toBe("Searching for HMSoft...");
});

test("only switches to connected after explicit backend event", () => {
  const state = createInitialState();

  applyEvent(state, { type: "status", state: "retrying" });
  expect(state.connection.label).toBe("Disconnected - retrying...");

  applyEvent(state, {
    type: "status",
    state: "connected",
    message: "Connected",
  });
  expect(state.connection.label).toBe("Connected");
});

test("groups sensor readings under each node line", () => {
  const state = createInitialState();

  applyEvent(state, { type: "log", line: "Start detected -> begin training" });
  applyEvent(state, { type: "log", line: "Node detected -> recorded as: L" });
  applyEvent(state, { type: "log", line: "S:00100 d=STR t=100" });
  applyEvent(state, { type: "log", line: "S:01100 d=SL t=200" });
  applyEvent(state, { type: "log", line: "Node detected -> recorded as: R" });
  applyEvent(state, { type: "log", line: "S:00110 d=SR t=300" });

  expect(state.entries).toHaveLength(3);
  expect(state.entries[0]).toMatchObject({
    kind: "line",
    line: "Start detected -> begin training",
  });
  expect(state.entries[1]).toMatchObject({
    kind: "group",
    header: "Node detected -> recorded as: L",
  });
  expect(state.entries[1].sensors).toEqual([
    "S:00100 d=STR t=100",
    "S:01100 d=SL t=200",
  ]);
  expect(state.entries[2]).toMatchObject({
    kind: "group",
    header: "Node detected -> recorded as: R",
  });
  expect(state.entries[2].sensors).toEqual(["S:00110 d=SR t=300"]);
});

test("clear removes rendered logs but preserves connection state", () => {
  const state = createInitialState();

  applyEvent(state, { type: "status", state: "connected", message: "Connected" });
  applyEvent(state, { type: "log", line: "Node detected -> recorded as: S" });
  applyEvent(state, { type: "log", line: "S:00100 d=STR t=100" });
  applyEvent(state, { type: "clear-ui" });

  expect(state.connection.state).toBe("connected");
  expect(state.entries).toEqual([]);

  applyEvent(state, { type: "status", state: "retrying" });
  expect(state.connection.label).toBe("Disconnected - retrying...");
});

test("copy text includes node headers and grouped sensors", () => {
  const state = createInitialState();

  applyEvent(state, { type: "log", line: "Node detected -> recorded as: L" });
  applyEvent(state, { type: "log", line: "S:11100 d=SL t=150" });
  applyEvent(state, { type: "log", line: "Dead end -> recorded U-turn" });

  expect(copyTextFromState(state)).toBe(
    [
      "Node detected -> recorded as: L",
      "S:11100 d=SL t=150",
      "Dead end -> recorded U-turn",
    ].join("\n"),
  );
});

test("toggleGroup expands and collapses a node section", () => {
  const state = createInitialState();

  applyEvent(state, { type: "log", line: "Node detected -> recorded as: L" });
  const groupId = state.entries[0].id;

  toggleGroup(state, groupId);
  expect(state.entries[0].expanded).toBe(true);

  toggleGroup(state, groupId);
  expect(state.entries[0].expanded).toBe(false);
});
