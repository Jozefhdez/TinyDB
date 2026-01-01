# TinyDB

TinyDB is a small, SQLite-inspired database implemented from scratch in C.

**Status:**
This project is under development. Expect rough edges and incomplete features.

## Purpose
- To learn and demonstrate how a simple SQL-like engine and storage can be implemented in C.
- The codebase is intended for study, experimentation, and incremental improvement.

## Features
- Command-line interface that accepts simple statements such as `insert`, `select`, and the meta-command `.exit`.
- Basic input parsing, storage, and constraints (string length limits, positive integer IDs).
- Persistent storage: Data is saved to a file on disk and reloaded on startup.
- Page-based storage with lazy loading for efficient memory usage.

## Build
You need a C compiler (e.g., gcc or clang). From this folder, run:

```sh
make
```

This compiles all source files into object files in the `build/` directory and links them into the executable `build/main`.

## Run

```sh
./build/main <database_file>
```

Replace `<database_file>` with the path to your database file (e.g., `tinydb.db`). If the file doesn't exist, it will be created.

Then enter commands interactively, for example:

```
insert 1 user1 person1@example.com
select
.exit
```

Data is automatically saved to the file when you exit.

## Architecture Overview
- **Table**: Manages rows and delegates to the Pager for storage.
- **Pager**: Handles page caching, file I/O, and lazy loading of data from disk.
- **Row/Statement**: Define data structures and parse SQL-like commands.
- **Input**: Manages user input buffering.

Pages are 4KB chunks; rows are packed into pages for efficient storage.

## Tests
There is a small CLI test harness using pytest. Build the binary first, then run:

```sh
pytest tests/test_cli.py
```