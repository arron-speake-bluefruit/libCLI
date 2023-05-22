// Help command implementation when no arguments are given.
// Prints a list of all commands and their variants.

#define COMMAND(name, summary, ...) print(userdata, "    " #name " - " summary "\n");

print(userdata, "list of all commands (do `help [command]` for more information):\n");
COMMANDS
print(userdata, "    help - show information about commands\n");

#undef COMMAND
