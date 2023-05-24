// Implementaion of the main command lookup for the CLI.
// This goes through each defined command, checks if the command matches and that arguments are correct.
// On success:
// - the corresponding command function is called.
// - `return true` is executed
// On failure:
// - A `goto *_bad_args` statement is executed, jumping to a later defined error handling block.

#define ARGS_0(function, summary) { \
        if (arg_count == 1) { \
            function(userdata); \
            return true; \
        } \
    } \

#define ARGS_1(function, summary, arg0_type) { \
        arg0_type arg0; \
        if ((arg_count == 2) && parse_arg(arg0)(args[1], &arg0)) { \
            function(arg0, userdata); \
            return true; \
        } \
    } \

#define ARGS_2(function, summary, arg0_type, arg1_type) { \
        arg0_type arg0; \
        arg1_type arg1; \
        if ((arg_count == 3) && parse_arg(arg0)(args[1], &arg0) && parse_arg(arg1)(args[2], &arg1)) { \
            function(arg0, arg1, userdata); \
            return true; \
        } \
    } \

#define ARGS_3(function, summary, arg0_type, arg1_type, arg2_type) { \
        arg0_type arg0; \
        arg1_type arg1; \
        arg2_type arg2; \
        if ((arg_count == 4) && parse_arg(arg0)(args[1], &arg0) && parse_arg(arg1)(args[2], &arg1) && parse_arg(arg2)(args[3], &arg2)) { \
            function(arg0, arg1, arg2, userdata); \
            return true; \
        } \
    } \

#define COMMAND(name, summary, variants) \
    if (strcmp(#name, args[0]) == 0) { \
        variants \
        goto name##_bad_args; \
    } \

COMMANDS

#undef COMMAND
#undef ARGS_0
#undef ARGS_1
#undef ARGS_2
#undef ARGS_3
