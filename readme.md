# macrocli

macroCLI is a macro-based code-generation utility for CLIs. See `example/main.c` for a usage example or `example/commands.c` for the corresponding CLI configuration.

## Structure

### Definitions

A CLI will have one or more **commands** defined. Each command has a unique **command name** and a short **command summary string**.

A command has one or more **command variants**. Each variant has an **associated function**, a **variant summary string** and zero or more **argument types**.

### Processing

When the CLI is passed an input string, it is parsed in-place (mutating the passed string). This produces an array of arguments. This step handles:
- Whitespace delimiters (`' '`, `'\t'` and `'\n'`) between arguments
- Quotation (`"` and `'`)
- Backslash escape (`\"` and `\'`)

For example, the following strings are converted like:
- (empty) `->` `[]`
- ` hello ` `->` `["hello"]`
- `"  hello " 'world \' '` `->` `["  hello ", "world ' "]`

The first argument is interpreted as the command name, which is used to look up a command. If a corresponding command is found, the rest of the arguments are checked against each of the command's variants.

If a variant's argument types match the input arguments, then the variant's associated function is called.

## Set up

To set up macroCLI, the `include/` directory must be added as an include directory and `source/parse.c` must be compiled and linked.

## Configuration

Configuration of macroCLI *must* take place *exactly once*, in a single file where all commands and their associated functions are defined.

The file should be a list of function declarations, followed by the definition of a `COMMANDS` macro and finally the inclusion of `macrocli/implementation.inc`. For example:
```c
#include <stdio.h>

void hello(void* userdata) {
    printf("Hello, world.\n);
}

#define COMMANDS \
    COMMAND(hello, "Say hello", ARG_0(hello, "Displays 'Hello, world.'"))

#include "macrocli/implementation.inc"
```

### COMMANDS Macro syntax

The COMMANDS macro contains all information about commands that is passed to the code generation inside `macrocli/implementation.inc`.

It must be declared as a (space-separated) sequence of `COMMAND(name, summary, args)` declarations where:
- `name` is an identifier, used for the command name;
- `summary` is a string literal, used by the help command when listing *all* commands; and
- `args` is a **command variant declaration sequence**, described below.

A command variant declaration sequence is a (space-separated) list of declarations in the form `ARGS_n(function, summary, types...)` where:
- `n` is an integer between 0 and 3 (eg. `ARGS_0`, `ARGS_3`, etc.);
- `function` is the command variant's assocaited function;
- `summary` is a string literal, used by the help command when asked about the command specifically; and
- `types...` are additional arguments of the variant's argument types (of length `n`).

See `example/commands.c` for an example.

Currently, only 3 arguments are supported at maximum (ie. `ARGS_3(...)`). This is only a limitation of implementation, and can be increased arbitratily if needed.

## Usage

To use the CLI, call the `macrocli_run` function with three arguments:
- Callback to a print function
- Userdata (passed to the print callback, and associated functions of commands)
- Input string

For example:

```c
static int my_print(void* userdata, const char* restrict format, ...) {
    (void)userdata;

    va_list args;
    va_start(args, format);
    int value = vprintf(format, args);
    va_end(args);
    return value;
}

void function(void) {
    char input[input_capacity];
    get_input(input);

    macrocli_run(my_print, NULL, input);
}
```

No I/O functions are called by macroCLI except for the passed-in print callback. To pass string into a UART peripheral, for example:


```c
static int my_print(void* userdata, const char* restrict format, ...) {
    UsartPeripheral usart = *(UsartPeripheral*)userdata;

    char output[128];

    va_list args;
    va_start(args, format);
    int length = vsnprintf(output, sizeof(output), format, args);
    va_end(args);

    usart_peripheral_transmit(output, length);
    return length;
}

void function(void) {
    char input[input_capacity];
    get_input(input);

    UsartPeripheral usart = ...;

    macrocli_run(my_print, &usart, input);
}
```

Since `macrocli_run` parses in-place and destroys the original string, the input buffer should be considered uninitialized after a call to `macrocli_run`.

## Not implemented

### Variadic arguments

Commands which take any number of inputs are not curently supported. Eg. functions like:
```c
void command(int value, const char* const* extra, unsigned extra_count) {
    printf("value = %i\n", value);
    printf("extra_count = %u\n", extra_count);
    for (unsigned i = 0; i < extra_count; i++) {
        printf("extra[%u] = %s\n", i, extra[i]);
    }
}
```

With a command invocation:
```
> command 123 any number of string arguments may follow
value = 123
extra_count = 7
extra[0] = any
extra[1] = number
extra[2] = of
extra[3] = string
extra[4] = arguments
extra[5] = may
extra[6] = follow
```

### Test suite

Previous versions of this library has proper testing, its very important to re-add comprehensive tests to this library.

### Parsing for arguments with unsigned integral types

Currently, unsigned types are not parsed correctly.
