# Proper error message support

Either manually issue error messages, or provide enough information for user to build error
messages themselves.

# Additional argument types

- Support for booleans
- Fixed-size integers?
- char, short, long, long, long + unsigned variants?
- double and long double?
- custom enum types?

# Custom argument validation

Before calling final function, check arguments using other functions. Code re-use of validation,
connects directly to error message utility

# Optional arguments

Support for `0..inf` optional arguments after all required arguments

# Topic help

- Help command for known commands (`help <name>`)
- Prints summary (or more detailed info?)
- Prints names and types of arguments?

# No per-command argument count cap

Switch to in-memory database model

# Missing tests

A few tests are missing for:
- Adding command with conflicting name (eg. add "hello" command, then adding "hello" command again)
- Adding too many arguments to a command (ie. argument_count > cli_max_argument_count)
- Incorrect argument count (ie. argc < command.argument_count, argc > command.argument_count)
- Nothing happens on empty input
- Errors from libcli_parse are passed back out from libcli_run
- Quotes starting inside arguments (eg. `h"ello"`, `h'i'`)
- EOF after backslash inside quotes (eg. `'\` or `"\`)
