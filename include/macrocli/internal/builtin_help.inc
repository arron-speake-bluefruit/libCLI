// Builtin `help` command which displays either a list of all commands or specific usage info.

if (strcmp("help", args[0]) == 0) {
    if (arg_count == 1) {
#include "builtin_help_general.inc"
        return true;
    }

    if (arg_count == 2) {
#include "builtin_help_topic.inc"
    }

    print(userdata, "error: bad arguments for `help`. accepted forms are:\n");
    print(userdata, "    help\n");
    print(userdata, "    help <string>\n");
    return false;
}
