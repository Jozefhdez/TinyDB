import subprocess
import tempfile
import os

def run_script(commands):
    fd, db_file = tempfile.mkstemp(suffix=".db")
    os.close(fd)
    
    try:
        proc = subprocess.Popen(
            ["./build/main", db_file],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        # Send commands, each followed by newline, and .exit at the end
        out, _ = proc.communicate('\n'.join(commands) + '\n')
        # Split output into lines and strip trailing whitespace
        return [line.rstrip("\n") for line in out.split('\n')]
    finally:
        # Clean up the temporary file
        if os.path.exists(db_file):
            os.remove(db_file)

def test_insert_and_select_row():
    commands = ["insert 1 user1 person1@example.com", "select", ".exit"]
    result = run_script(commands)

    assert "Executed." in result[0]
    assert "(1, user1, person1@example.com)" in result[1]

def test_insert_fails_when_table_full():
    # attempt one more than TABLE_MAX_ROWS to trigger the error
    commands = [f"insert {i} user{i} person{i}@example.com" for i in range(1, 1402)]
    commands.append(".exit")
    result = run_script(commands)

    assert any("Error: Table full." in line for line in result)

def test_insert_allows_max_length_strings():
    long_username = "a" * 32
    long_email = "a" * 255
    commands = [f"insert 1 {long_username} {long_email}", "select", ".exit",]

    result = run_script(commands)
    expected = [
        "TinyDB > Executed.",
        f"TinyDB > (1, {long_username}, {long_email})",
        "Executed.",
        "TinyDB > "
    ]
    
    assert result == expected

def test_insert_rejects_overlong_strings():
    long_username = "a" * 33
    long_email = "a" * 256
    commands = [f"insert 1 {long_username} {long_email}", "select", ".exit",]

    result = run_script(commands)
    expected = [
        "TinyDB > String is too long.",
        "TinyDB > Executed.",
        "TinyDB > "
    ]
    
    assert result == expected


def test_id_negative_errors():
    commands = ["insert -1 cstack foo@bar.com", "select", ".exit"]
    result = run_script(commands)

    assert any("ID must be positive." in line for line in result)
    assert any("Executed." in line for line in result)

def test_prints_constants():
    commands = [".constants", ".exit"]
    result = run_script(commands)
    
    expected = [
        "TinyDB > Constants:",
        "ROW_SIZE: 293",
        "COMMON_NODE_HEADER_SIZE: 6",
        "LEAF_NODE_HEADER_SIZE: 10",
        "LEAF_NODE_CELL_SIZE: 297",
        "LEAF_NODE_SPACE_FOR_CELLS: 4086",
        "LEAF_NODE_MAX_CELLS: 13",
        "TinyDB > "
    ]
    
    assert result == expected    
    assert result == expected

def test_prints_one_node_btree():
    commands = [
        "insert 3 user3 person3@example.com",
        "insert 1 user1 person1@example.com",
        "insert 2 user2 person2@example.com",
        ".btree",
        ".exit"
    ]
    result = run_script(commands)
    
    expected = [
        "TinyDB > Executed.",
        "TinyDB > Executed.",
        "TinyDB > Executed.",
        "TinyDB > Tree:",
        "- leaf (size 3)",
        "  - 1",
        "  - 2",
        "  - 3",
        "TinyDB > "
    ]
    
    assert result == expected

def test_prints_error_message_for_duplicate_id():
    commands = [
        "insert 1 user1 person1@example.com",
        "insert 1 user1 person1@example.com",
        "select",
        ".exit"
    ]
    result = run_script(commands)
    
    expected = [
        "TinyDB > Executed.",
        "TinyDB > Error: Duplicate key.",
        "TinyDB > (1, user1, person1@example.com)",
        "Executed.",
        "TinyDB > "
    ]
    
    assert result == expected

def test_allows_printing_out_structure_of_3_leaf_node_btree():
    commands = [f"insert {i} user{i} person{i}@example.com" for i in range(1, 15)]
    commands.append(".btree")
    commands.append("insert 15 user15 person15@example.com")
    commands.append(".exit")
    
    result = run_script(commands)
    
    # The btree output starts after the inserts (14 "Executed." messages + 14 "TinyDB > " prompts)
    btree_output = result[14:]
    
    expected = [
        "TinyDB > Tree:",
        "- internal (size 1)",
        "  - leaf (size 7)",
        "    - 1",
        "    - 2",
        "    - 3",
        "    - 4",
        "    - 5",
        "    - 6",
        "    - 7",
        "  - key 7",
        "  - leaf (size 7)",
        "    - 8",
        "    - 9",
        "    - 10",
        "    - 11",
        "    - 12",
        "    - 13",
        "    - 14",
        "TinyDB > Need to implement searching an internal node",
    ]
    
    assert btree_output == expected
