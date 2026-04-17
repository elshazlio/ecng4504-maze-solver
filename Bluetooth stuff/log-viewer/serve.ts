import { join } from "path";

const BLUETOOTH_DIR = join(import.meta.dir, "..");
const PYTHON = join(BLUETOOTH_DIR, "venv", "bin", "python");
const BLE_SCRIPT = join(BLUETOOTH_DIR, "ble_log_receiver.py");
/** Set false to run ble_log_receiver.py (needs MAZE_BT_SERIAL + paired HC-05 / ZS-040). */
const BLE_OUTAGE_MODE = false;
const clients = new Set<ServerWebSocket<unknown>>();

let currentStatus = {
  type: "status",
  state: BLE_OUTAGE_MODE ? "offline" : "searching",
  message: BLE_OUTAGE_MODE
    ? "Log viewer idle — set MAZE_BT_SERIAL for ZS-040"
    : "Connecting (Bluetooth SPP serial)…",
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
      message: String(event.message ?? "Waiting for telemetry…"),
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
      message: "Log viewer offline",
      error: "Set BLE_OUTAGE_MODE false and export MAZE_BT_SERIAL=/dev/cu.…",
    };
    broadcast(currentStatus);
    return;
  }

  bleProcess = Bun.spawn([PYTHON, BLE_SCRIPT], {
    cwd: BLUETOOTH_DIR,
    stdout: "pipe",
    stderr: "pipe",
    env: { ...process.env },
  });

  void readJsonLines(bleProcess.stdout, "log");
  void readJsonLines(bleProcess.stderr, "status");

  bleProcess.exited.then((code) => {
    bleProcess = null;
    currentStatus = {
      type: "status",
      state: "retrying",
      message: "Disconnected - retrying...",
      ...(code ? { error: `Python log receiver exited with code ${code}` } : {}),
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

console.log(`\n[log-viewer dev] http://localhost:${server.port}`);
if (BLE_OUTAGE_MODE) {
  console.log(
    "  Outage mode is on — set BLE_OUTAGE_MODE false in serve.ts and export MAZE_BT_SERIAL=/dev/cu.…",
  );
} else {
  console.log("  Telemetry: runs Bluetooth stuff/ble_log_receiver.py via:");
  console.log(`    ${PYTHON}`);
  console.log("  Pair ZS-040/HC-05 in macOS Bluetooth, then in this shell:");
  console.log("    export MAZE_BT_SERIAL=/dev/cu.<device>   # ls /dev/cu.*");
  console.log("    export MAZE_BT_BAUD=9600");
  console.log("  If spawn fails, create venv: cd Bluetooth\\ stuff && python3 -m venv venv && ./venv/bin/pip install -r requirements.txt\n");
}
