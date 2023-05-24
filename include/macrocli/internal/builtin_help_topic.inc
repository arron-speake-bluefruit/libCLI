// Help command implementation when no arguments are given.
// Prints a list of all commands and their variants.

#define ARGS_0(function, summary) \
    print(userdata, "    %s - %s \n", name_string, summary); \

#define ARGS_1(function, summary, arg0) \
    print(userdata, "    %s <%s> - %s\n", name_string, arg_type_name((arg0)0), summary); \

#define ARGS_2(function, summary, arg0, arg1) \
    print(userdata, "    %s <%s> <%s> - %s\n", name_string, arg_type_name((arg0)0), arg_type_name((arg1)0), summary); \

#define ARGS_3(function, summary, arg0, arg1, arg2) \
    print(userdata, "    %s <%s> <%s> <%s> - %s\n", name_string, arg_type_name((arg0)0), arg_type_name((arg1)0), arg_type_name((arg2)0), summary); \

#define COMMAND(name, summary, variants) \
    if (strcmp(#name, args[1]) == 0) { \
        const char* name_string = #name; \
        print(userdata, "list of forms of this command:\n"); \
        variants \
        return true; \
     } \

COMMANDS

if (strcmp("help", args[1]) == 0) {
    print(userdata, "list of forms of this command:\n");
    print(userdata, "    help - display a list of commands \n");
    print(userdata, "    help <string> - display usage of a command\n");
    return true;
}

print(userdata, "error: help: unknown command `%s`\n", args[1]);
return false;

#undef COMMAND
#undef ARGS_0
#undef ARGS_1
#undef ARGS_2
#undef ARGS_3
