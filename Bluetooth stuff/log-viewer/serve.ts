import { join } from "path";

const BLUETOOTH_DIR = join(import.meta.dir, "..");
const PYTHON = join(BLUETOOTH_DIR, "venv", "bin", "python");
const BLE_SCRIPT = join(BLUETOOTH_DIR, "ble_log_receiver.py");
const BLE_OUTAGE_MODE = true;
const clients = new Set<ServerWebSocket<unknown>>();

let currentStatus = {
  type: "status",
  state: BLE_OUTAGE_MODE ? "offline" : "searching",
  message: BLE_OUTAGE_MODE ? "BLE offline - HM-10 unavailable" : "Searching for HMSoft...",
};
let bleProcess: ReturnType<typeof Bun.spawn> | null = null;
let restartTimer: ReturnType<typeof setTimeout> | null = null;

function broadcast(event: Record<string, unknown>) {
  const payload = JSON.stringify(event);
  for (const client of clients) {
    client.send(payload);
  }
}

function handleBackendEvent(event: Record<string, unknown>) {
  if (event.type === "status") {
    currentStatus = {
      type: "status",
      state: String(event.state ?? "searching"),
      message: String(event.message ?? "Searching for HMSoft..."),
      ...(event.error ? { error: String(event.error) } : {}),
    };
  }
  broadcast(event);
}

async function readJsonLines(stream: ReadableStream<Uint8Array>, fallbackType: "log" | "status") {
  const reader = stream.getReader();
  const decoder = new TextDecoder();
  let buffer = "";

  while (true) {
    const { done, value } = await reader.read();
    if (done) {
      break;
    }

    buffer += decoder.decode(value, { stream: true });
    const lines = buffer.split("\n");
    buffer = lines.pop() ?? "";

    for (const rawLine of lines) {
      const line = rawLine.trim();
      if (!line) {
        continue;
      }

      try {
        handleBackendEvent(JSON.parse(line));
      } catch {
        if (fallbackType === "status") {
          handleBackendEvent({
            type: "status",
            state: "retrying",
            message: "Disconnected - retrying...",
            error: line,
          });
        } else {
          handleBackendEvent({ type: "log", line });
        }
      }
    }
  }
}

function scheduleRestart() {
  if (restartTimer || bleProcess) {
    return;
  }
  restartTimer = setTimeout(() => {
    restartTimer = null;
    startBleSupervisor();
  }, 1500);
}

function startBleSupervisor() {
  if (bleProcess) {
    return;
  }

  if (BLE_OUTAGE_MODE) {
    currentStatus = {
      type: "status",
      state: "offline",
      message: "BLE offline - HM-10 unavailable",
      error: "Temporary outage: suspected 5V overvoltage damage. Use USB serial telemetry.",
    };
    broadcast(currentStatus);
    return;
  }

  bleProcess = Bun.spawn([PYTHON, BLE_SCRIPT], {
    cwd: BLUETOOTH_DIR,
    stdout: "pipe",
    stderr: "pipe",
  });

  void readJsonLines(bleProcess.stdout, "log");
  void readJsonLines(bleProcess.stderr, "status");

  bleProcess.exited.then((code) => {
    bleProcess = null;
    currentStatus = {
      type: "status",
      state: "retrying",
      message: "Disconnected - retrying...",
      ...(code ? { error: `BLE process exited with code ${code}` } : {}),
    };
    broadcast(currentStatus);
    scheduleRestart();
  });
}

const server = Bun.serve({
  port: 3000,
  fetch(req, server) {
    const url = new URL(req.url);
    if (url.pathname === "/" || url.pathname === "/index.html") {
      return new Response(Bun.file(import.meta.dir + "/index.html"), {
        headers: { "Content-Type": "text/html" },
      });
    }
    if (url.pathname === "/viewer-state.js") {
      return new Response(Bun.file(join(import.meta.dir, "viewer-state.js")), {
        headers: { "Content-Type": "text/javascript" },
      });
    }
    if (url.pathname === "/ws") {
      const upgraded = server.upgrade(req);
      if (upgraded) return undefined;
      return new Response("WebSocket upgrade failed", { status: 400 });
    }
    return new Response("Not found", { status: 404 });
  },
  websocket: {
    open(ws) {
      clients.add(ws);
      ws.send(JSON.stringify(currentStatus));
    },
    close(ws) {
      clients.delete(ws);
    },
    message() {},
  },
});

startBleSupervisor();

console.log(`Log viewer: http://localhost:${server.port}`);
if (BLE_OUTAGE_MODE) {
  console.log("BLE offline mode active - viewer will report HM-10 outage status only");
} else {
  console.log("Auto-connects to HMSoft via Python backend (no browser BLE needed)");
}
