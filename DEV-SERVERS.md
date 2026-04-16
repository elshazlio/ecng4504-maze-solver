# Local dev servers (Bluetooth log viewer + maze dashboard)

Copy-paste the block below into Terminal. It starts **both** servers in the background, waits a moment, then opens **two browser tabs** (ports **3000** and **8765**).

## One shot — start everything + open browser

```bash
cd "/Users/elshazlio/Documents/My Projects/Embedded/Bluetooth stuff/log-viewer" && bun run start &
cd "/Users/elshazlio/Documents/My Projects/Embedded/Bluetooth stuff/maze_dashboard" && source .venv/bin/activate && uvicorn app:app --host 127.0.0.1 --port 8765 &
sleep 2
open "http://localhost:3000" "http://127.0.0.1:8765"
```

- **HM-10 log viewer:** [http://localhost:3000](http://localhost:3000)  
- **Maze dashboard (FastAPI):** [http://127.0.0.1:8765](http://127.0.0.1:8765)

## Prerequisites

- [Bun](https://bun.sh) installed (`bun --version`).
- Maze dashboard venv + deps (first time only):

```bash
cd "/Users/elshazlio/Documents/My Projects/Embedded/Bluetooth stuff/maze_dashboard"
python3 -m venv .venv && source .venv/bin/activate && pip install -r requirements.txt
```

## Stop the servers

```bash
kill $(lsof -t -i:3000) $(lsof -t -i:8765) 2>/dev/null
```

Or close the terminal session that started them.

## Run only one app

**Log viewer:**

```bash
cd "/Users/elshazlio/Documents/My Projects/Embedded/Bluetooth stuff/log-viewer" && bun run start
```

**Maze dashboard:**

```bash
cd "/Users/elshazlio/Documents/My Projects/Embedded/Bluetooth stuff/maze_dashboard" && source .venv/bin/activate && uvicorn app:app --host 127.0.0.1 --port 8765
```

Then open the matching URL in your browser (no `open` command unless you add it).