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

## Build
You need a C compiler (e.g., gcc or clang). From this folder, run:

```sh
mkdir -p build
gcc -std=c11 -Wall -Wextra -g -o build/main main.c input.c row.c table.c statement.c
```

## Run

```sh
./build/main
```

Then enter commands interactively, for example:

```
insert 1 user1 person1@example.com
select
.exit
```

## Tests
There is a small CLI test harness using pytest. Build the binary first, then run:

```sh
pytest tests/test_cli.py
```