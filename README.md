# TinyDB

TinyDB is a small, SQLite-inspired database implemented from scratch in C, featuring a B-tree storage engine.

**Status:**
This project is under active development. Core features are functional, with ongoing improvements to the B-tree implementation.

## Purpose
- Educational project to learn and demonstrate how database internals work
- Implements core database concepts: B-tree indexing, page-based storage, and REPL interface
- Clean, readable codebase designed for study and experimentation

## Features
- **SQL-like Commands**: `insert` and `select` statements
- **Meta-commands**: `.exit`, `.btree`, `.constants`, `.schema`, `.tables`, `.help`
- **B-tree Storage**: Leaf nodes with binary search for efficient key lookups
- **Duplicate Key Detection**: Prevents inserting rows with duplicate IDs
- **Persistent Storage**: Data saved to disk with automatic page-based caching
- **Schema Validation**: String length limits (username: 32 chars, email: 255 chars) and positive integer IDs
- **Page-based Architecture**: 4KB pages with lazy loading for memory efficiency
- **Root Page Management**: Automatic initialization of B-tree root node

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
TinyDB > insert 1 alice alice@example.com
Executed.
TinyDB > insert 2 bob bob@example.com
Executed.
TinyDB > select
(1, alice, alice@example.com)
(2, bob, bob@example.com)
Executed.
TinyDB > .btree
Tree:
leaf (size 2)
  - 0 : 1
  - 1 : 2
TinyDB > .exit
```

Data is automatically saved to the file when you exit.

### Available Commands

**SQL Commands:**
- `insert <id> <username> <email>` - Insert a new row
- `select` - Display all rows

**Meta Commands:**
- `.exit` - Save and exit the program
- `.btree` - Display the B-tree structure
- `.constants` - Show B-tree node constants (sizes, capacities)
- `.schema` - Display table schema
- `.tables` - Show table information (row count, pages, file size)
- `.help` - Display available commands

## Architecture Overview

### Core Components

- **B-tree Module** (`btree.c/h`): B-tree operations including node management, search, and insertion
  - Leaf node format with header and cells
  - Binary search for key lookups
  - Node type management (leaf/internal)
  
- **Pager Module** (`pager.c/h`): Page-based storage engine
  - 4KB page caching with lazy loading
  - File I/O operations (open, read, write, flush)
  - Up to 100 pages cached in memory
  
- **Cursor Module** (`cursor.c/h`): Row navigation and iteration
  - Cursor positioning within B-tree nodes
  - Cursor advancement through cells
  - Value access at cursor position

- **Table Module** (`table.c/h`): High-level database operations
  - Database open/close operations
  - Root page management
  - Key search and lookup (delegates to B-tree)

- **Statement Parser** (`statement.c/h`): SQL command parsing and execution
  - INSERT and SELECT statement preparation
  - Row validation (ID, string length)
  - Duplicate key detection

- **Row Serialization** (`row.c/h`): Data serialization/deserialization
  - Fixed-size row format (293 bytes)
  - Schema: id (4 bytes), username (33 bytes), email (256 bytes)

- **CLI Interface** (`input.c/h`, `main.c`): REPL and user interaction
  - Command parsing and routing
  - Meta-command handling
  - Interactive prompt

### Storage Layout

**Leaf Node Structure:**
```
+------------------+
| Node Type (1)    |  Common Node Header (6 bytes)
| Is Root (1)      |
| Parent Ptr (4)   |
+------------------+
| Num Cells (4)    |  Leaf Node Header (4 bytes)
+------------------+
| Cell 0           |  Cells (297 bytes each)
|  - Key (4)       |  Maximum 13 cells per leaf node
|  - Value (293)   |
| Cell 1           |
| ...              |
+------------------+
```

**Page Layout:**
- Page Size: 4096 bytes (4KB)
- Leaf Node Header: 10 bytes
- Cell Size: 297 bytes (4-byte key + 293-byte row)
- Max Cells per Leaf: 13 rows

**Row Format:**
- ID: 4 bytes (uint32_t)
- Username: 33 bytes (32 chars + null terminator)
- Email: 256 bytes (255 chars + null terminator)
- Total: 293 bytes

## Tests

The project includes a comprehensive test suite using pytest. Build the binary first, then run:

```sh
pytest tests/test_cli.py -v
```

## Learning Resources

This project follows the tutorial structure from [Let's Build a Simple Database](https://cstack.github.io/db_tutorial/), with additional enhancements and modifications.