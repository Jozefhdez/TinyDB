# TinyDB

A small, SQLite-inspired database implemented from scratch in C.

Status: This project is still in development, expect rough edges and
incomplete features.

**Purpose:**
- Learn and demonstrate how a simple SQL-like engine and storage can be
	implemented in C. The codebase is intended for study, experimentation,
	and incremental improvement.

**Quick Features**
- Small command-line interface that accepts simple statements such as
	`insert`, `select`, and the meta-command `.exit`.
- Basic input parsing, storage, and constraints (string length limits,
	positive integer IDs) as exercised by the bundled tests.

**Build (recommended)**
From the repository root run:

```sh
make build/main
```

If you prefer to compile directly with `gcc` (ensure the `build/` folder
exists):

```sh
mkdir -p build
gcc -std=c11 -Wall -Wextra -g -o build/main src/TinyDB/*.c
```

**Run**

```sh
./build/main
```

Then enter commands interactively, for example:

```sh
insert 1 user1 person1@example.com
select
.exit
```

**Tests**
The repository includes a small CLI test harness. Build the binary first,
then run the tests with `pytest`:

```sh
make build/main
pytest src/TinyDB/tests/test_cli.py
```