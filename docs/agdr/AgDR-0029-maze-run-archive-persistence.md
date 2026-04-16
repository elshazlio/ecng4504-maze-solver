---
id: AgDR-0029
timestamp: 2026-03-24T12:00:00Z
agent: cursor
model: composer
trigger: user-prompt
status: executed
---

# Maze dashboard run archive storage format

> In the context of persisting saved run snapshots (telemetry + map) across browser sessions and backend restarts, facing the need for a simple, dependency-free store, I decided to use a single JSON file beside the app with a versioned envelope and a capped list of runs, accepting weaker query semantics and manual merge discipline over a database.

## Context
- The user wants a drawer UI listing past runs with full log + map snapshot data.
- The FastAPI process may restart; storage must survive without an external service.
- The dashboard already uses FastAPI with no ORM.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| **Single JSON file** (`maze_run_archive.json`) | No new dependencies; trivial backup/inspect; fine for tens of runs | Whole-file rewrite; not ideal for huge histories or concurrent writers |
| **SQLite** | Scalable; indexed queries; atomic updates | New conceptual surface area; file format less human-grep-friendly for quick debugging |

## Decision
Chosen: **single JSON file with `{ version, runs[] }` and a cap (e.g. 100 entries)**, because run snapshots are small, traffic is single-user, and the project already favors minimal moving parts (see AgDR-0012 style persistence).

## Consequences
- Archive read/write uses short critical sections with an asyncio lock; acceptable for this workload.
- Users can copy or delete `maze_run_archive.json` to backup or reset history.
- Very large logs in many saved runs could grow the file; the cap and in-memory log limits on the live session mitigate this.
