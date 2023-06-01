#ifndef CLI_INTERNAL_PARSE_H
#define CLI_INTERNAL_PARSE_H

//
// Internal libCLI Argument Parser
//
// State-machine parser for converting input strings into argument arrays in-place.
//
// Each state (any of the parse_* functions) represents a vertex in the state machine lines in the
// form `return parse_*(parser);` represent state transitions. The `parser_read` and `parser_write`
// functions provide input and output to the machine.
//
// The write head must be guaranteed to never advance beyond the read head. This allows in-place
// parsing. Currently, this is guaranteed as all state functions read at least once before writing.
//

#include <stddef.h>
#include <stdbool.h>

typedef enum ParseStatus {
    // The entire string was parsed successfully.
    parse_status_success,

    // Expected a character after '/', but the string ended.
    parse_status_eof_after_slash,

    // The string ended before a closing `"`.
    parse_status_unterminated_double_quote,

    // The string ended before a closing `'`.
    parse_status_unterminated_single_quote,
} ParseStatus;

typedef struct ParseResult {
    // Status of the parse (parse_status_success if it succeeded).
    ParseStatus status;

    // If successful, the number of arguments collected by the parser.
    size_t argument_count;
} ParseResult;

ParseResult libcli_parse(char* input, const char** arguments, size_t max_arguments);

#endif // CLI_INTERNAL_PARSE_H
