# libCLI 0.1.0

libCLI is a small CLI library which:
- provides a basic CLI interface (command registration & execution),
- supports basic argument parsing (whitespace-delimited),
- provides a built-in help command,
- can integrate into any serial (printf based, MMIO based, etc.), and
- has fixed memory usage (does not dynamically allocate).

## Building

To build the project, add it as a submodule:
```
git submodule add <repository> libCLI
git submodule update --init --recursive
```

You can either reference the header and source directories directly or - if you're using CMake - add them as a subdirectory:
```cmake
add_subdirectory(../path/to/libCLI build_dir)

target_link_libraries(MyProject PRIVATE CLI)
```

## Usage

### Initialization

To set up a CLI, specify:
- a command storage array, `commands`;
- a CLI header, `cli`; and
- a writeback function, `writeback_printf`.

```c
// Declare the CLI header and its associated command list:
#define COMMAND_CAPACITY 32
CliCommand commands[COMMAND_CAPACITY];
CliNewInfo info = {
    .commands = commands,
    .commands_size = COMMAND_CAPACITY,
    .writeback = writeback_printf,
    .writeback_data = NULL,
};
CliHeader cli = libcli_new(&info);

// Register commands
libcli_add(cli, "enable-thing", "Enable a thing", enable_thing_command);
libcli_add(cli, "disable-thing", "Disable a thing", disable_thing_command);
libcli_add(cli, "add-stuff", "Add stuff to a thing", add_stuff_command);
// etc.
```

### Execution

To execute an input, call `libcli_run`.

```c
// NOTE: Must be `char*`, _not_ `const char*`. The parser operates in-place on the input string.
char* input = /* get user input from somewhere */;

// Call `run` on the input. The input is parsed and a command is dispatched.
libcli_run(cli, input, &userdata);
```

### Writeback

When the CLI needs to write something back to the user, it uses the `writeback` function passed in
during initialization. This could look like:

```c
void writeback_printf(const char* string, void* userdata) {
    (void)userdata;
    printf("%s", string);
}
```

Or, writing to a UART peripheral, for example:

```c
void write_to_uart(const char* string, void* userdata) {
    UART_HandleTypeDef uart = *(UART_HandleTypeDef*)userdata;
    size_t length = strlen(string);
    HAL_UART_Transmit(uart, string, length);
}
```

### Commands

When a command is called, it is passed an argument count, the array of arguments and userdata.

```c
void enable_thing_command(size_t argc, const char* const* argv, void* userdata) {
    if (argc != 1) {
        printf("error: expected 1 argument\n");
    } else {
        ThingData* things = (ThingData*)userdata;
        enable_thing_by_name(things, argv[0]);
    }
}
```
