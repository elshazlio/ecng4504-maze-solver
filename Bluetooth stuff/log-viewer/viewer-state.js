let nextGroupId = 1;

export function createInitialState() {
  return {
    connection: {
      state: "offline",
      label: "BLE offline - HM-10 unavailable",
    },
    entries: [],
  };
}

export function applyEvent(state, event) {
  if (!event || typeof event !== "object") {
    return state;
  }

  if (event.type === "status") {
    state.connection = {
      state: event.state ?? "searching",
      label: event.message ?? labelForState(event.state ?? "searching"),
    };
    return state;
  }

  if (event.type === "clear-ui") {
    state.entries = [];
    return state;
  }

  if (event.type !== "log" || typeof event.line !== "string") {
    return state;
  }

  const line = event.line.trim();
  if (!line) {
    return state;
  }

  const lineType = classifyLine(line);
  if (isNodeLine(line)) {
    state.entries.push({
      kind: "group",
      id: nextGroupId++,
      header: line,
      lineType,
      expanded: false,
      sensors: [],
    });
    return state;
  }

  if (lineType === "sensor") {
    const lastEntry = state.entries[state.entries.length - 1];
    if (lastEntry?.kind === "group") {
      lastEntry.sensors.push(line);
      return state;
    }
  }

  state.entries.push({
    kind: "line",
    line,
    lineType,
  });
  return state;
}

export function toggleGroup(state, groupId) {
  const group = state.entries.find(
    (entry) => entry.kind === "group" && entry.id === groupId,
  );
  if (group) {
    group.expanded = !group.expanded;
  }
  return state;
}

export function expandAllGroups(state) {
  for (const entry of state.entries) {
    if (entry.kind === "group") {
      entry.expanded = true;
    }
  }
  return state;
}

export function collapseAllGroups(state) {
  for (const entry of state.entries) {
    if (entry.kind === "group") {
      entry.expanded = false;
    }
  }
  return state;
}

export function copyTextFromState(state) {
  const lines = [];
  for (const entry of state.entries) {
    if (entry.kind === "group") {
      lines.push(entry.header);
      lines.push(...entry.sensors);
      continue;
    }
    lines.push(entry.line);
  }
  return lines.join("\n");
}

export function copyTurnsFromState(state) {
  const lines = [];
  for (const entry of state.entries) {
    if (entry.kind === "group") {
      lines.push(entry.header);
      continue;
    }
    if (entry.kind === "line" && isTurnLine(entry.line)) {
      lines.push(entry.line);
    }
  }
  return lines.join("\n");
}

function isTurnLine(line) {
  return line.includes("Node detected") || line.includes("Dead end");
}

export function labelForState(connectionState) {
  switch (connectionState) {
    case "connected":
      return "Connected";
    case "offline":
      return "BLE offline - HM-10 unavailable";
    case "retrying":
      return "Disconnected - retrying...";
    case "error":
      return "Connection error";
    case "searching":
    default:
      return "Searching for HMSoft...";
  }
}

export function classifyLine(line) {
  if (line.startsWith("S:")) {
    return "sensor";
  }
  if (isNodeLine(line)) {
    return "node";
  }
  if (line.includes("Error") || line.includes("error") || line.includes("exited")) {
    return "error";
  }
  return "status";
}

export function isNodeLine(line) {
  return line.includes("Node detected");
}
