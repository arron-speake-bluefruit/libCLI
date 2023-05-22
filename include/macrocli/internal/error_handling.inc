// Error handling for each defined command.
// This code is jumped to from `command_lookup.inc` if command parsing fails.
// Each command has a `..._bad_args` block which prints an error message followed by all accepted
// forms of the command.

#define ARGS_0(function, summary) \
    print(userdata, "    %s \n", name_string); \

#define ARGS_1(function, summary, arg0) \
    print(userdata, "    %s <%s>\n", name_string, arg_type_name((arg0)0)); \

#define ARGS_2(function, summary, arg0, arg1) \
    print(userdata, "    %s <%s> <%s>\n", name_string, arg_type_name((arg0)0), arg_type_name((arg1)0)); \

#define ARGS_3(function, summary, arg0, arg1, arg2) \
    print(userdata, "    %s <%s> <%s> <%s>\n", name_string, arg_type_name((arg0)0), arg_type_name((arg1)0), arg_type_name((arg2)0)); \

#define COMMAND(name, summary, ...) \
    name##_bad_args: { \
        const char* name_string = #name; \
        print(userdata, "error: bad arguments for `" #name "`. accepted forms are:\n"); \
        __VA_ARGS__ \
        return false; \
    } \

COMMANDS

#undef COMMAND
#undef ARGS_0
#undef ARGS_1
#undef ARGS_2
#undef ARGS_3
