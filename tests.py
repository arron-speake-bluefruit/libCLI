#!/usr/bin/env python3

import subprocess

EXAMPLES = {
    "": "",
    "enable": "missing arg item for enable\n",
    "enable thing": "missing arg value for enable\n",
    "enable thing blah": "bad argument value for enable\n",
    "enable thing -10": "enabled thing with value -10\n",
    "disable": "missing arg item for disable\n",
    "disable thing": "disabled thing\n",
    "disable thing blah": "too many args for disable\n",
    "disable-all": "disabled all items\n",
    "enable-all": "enabled all items\n",
}

success = True

for example_input in EXAMPLES:
    example = subprocess.Popen(["build/example/example", example_input], stdout = subprocess.PIPE)
    assert example.wait() == 0

    output_bytes = example.stdout.read()
    output_string = output_bytes.decode("utf-8")
    expected = EXAMPLES[example_input]

    if output_string == expected:
        print(f"`{example_input}` PASS")
    else:
        print(f"`{example_input}` FAIL: expected `{expected}` got `{output_string}`")
        success = False

if not success:
    exit(1)
