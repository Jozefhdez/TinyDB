import subprocess

def run_script(commands):
    proc = subprocess.Popen(
        ["./build/main"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    # Send commands, each followed by newline, and .exit at the end
    out, _ = proc.communicate('\n'.join(commands) + '\n')
    # Split output into lines and strip trailing whitespace
    return [line.rstrip("\n") for line in out.split('\n')]

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