import subprocess
import tempfile
import os

def run_script(commands):
    fd, db_file = tempfile.mkstemp(suffix=".db")
    os.close(fd)
    
    try:
        proc = subprocess.Popen(
            ["../build/main", db_file],
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

    commands = [f"insert {i} user{i} person{i}@example.com" for i in range(1, 100)]
    commands.append(".exit")
    result = run_script(commands)

    assert any("Need to implement splitting internal node" in line for line in result)

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
        "LEAF_NODE_HEADER_SIZE: 14",
        "LEAF_NODE_CELL_SIZE: 297",
        "LEAF_NODE_SPACE_FOR_CELLS: 4082",
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
        "TinyDB > Executed.",
        "TinyDB > ",
    ]
    
    assert btree_output == expected

def test_allows_printing_out_the_structure_of_a_4_leaf_node():
    script = [
        "insert 18 user18 person18@example.com",
        "insert 7 user7 person7@example.com",
        "insert 10 user10 person10@example.com",
        "insert 29 user29 person29@example.com",
        "insert 23 user23 person23@example.com",
        "insert 4 user4 person4@example.com",
        "insert 14 user14 person14@example.com",
        "insert 30 user30 person30@example.com",
        "insert 15 user15 person15@example.com",
        "insert 26 user26 person26@example.com",
        "insert 22 user22 person22@example.com",
        "insert 19 user19 person19@example.com",
        "insert 2 user2 person2@example.com",
        "insert 1 user1 person1@example.com",
        "insert 21 user21 person21@example.com",
        "insert 11 user11 person11@example.com",
        "insert 6 user6 person6@example.com",
        "insert 20 user20 person20@example.com",
        "insert 5 user5 person5@example.com",
        "insert 8 user8 person8@example.com",
        "insert 9 user9 person9@example.com",
        "insert 3 user3 person3@example.com",
        "insert 12 user12 person12@example.com",
        "insert 27 user27 person27@example.com",
        "insert 17 user17 person17@example.com",
        "insert 16 user16 person16@example.com",
        "insert 13 user13 person13@example.com",
        "insert 24 user24 person24@example.com",
        "insert 25 user25 person25@example.com",
        "insert 28 user28 person28@example.com",
        ".btree",
        ".exit",
    ]
    result = run_script(script)

def test_prints_all_rows_in_multi_level_tree():
    script = []
    for i in range(1, 16):
        script.append(f"insert {i} user{i} person{i}@example.com")
    script.append("select")
    script.append(".exit")
    result = run_script(script)

    expected = [
        "TinyDB > (1, user1, person1@example.com)",
        "(2, user2, person2@example.com)",
        "(3, user3, person3@example.com)",
        "(4, user4, person4@example.com)",
        "(5, user5, person5@example.com)",
        "(6, user6, person6@example.com)",
        "(7, user7, person7@example.com)",
        "(8, user8, person8@example.com)",
        "(9, user9, person9@example.com)",
        "(10, user10, person10@example.com)",
        "(11, user11, person11@example.com)",
        "(12, user12, person12@example.com)",
        "(13, user13, person13@example.com)",
        "(14, user14, person14@example.com)",
        "(15, user15, person15@example.com)",
        "Executed.", "TinyDB > ",
    ]
    assert result[15:] == expected
