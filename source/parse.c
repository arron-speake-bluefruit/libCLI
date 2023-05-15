#include "cli/internal/parse.h"
#include <ctype.h>

typedef enum QuoteKind {
    quote_kind_unquoted,
    quote_kind_single,
    quote_kind_double,
} QuoteKind;

typedef struct Parser {
    const char* read;
    char* write;
    const char** arguments;
    size_t argument_count;
    size_t max_arguments;
} Parser;

static void parser_mark_argument(Parser* parser) {
    if (parser->argument_count < parser->max_arguments) {
        parser->arguments[parser->argument_count] = parser->write;
        parser->argument_count += 1;
    }
}

static char parser_read(Parser* parser) {
    char c = *parser->read;
    if (c != '\0') {
        parser->read += 1;
    }
    return c;
}

static void parser_write(Parser* parser, char c) {
    *parser->write = c;
    parser->write += 1;
}

static bool parse_argument(Parser* parser);

// parsing an argument, last char was a '\'
static bool parse_slash(Parser* parser) {
    char c = parser_read(parser);
    if (c == '\0') {
        return false;
    } else {
        parser_write(parser, c);
        return parse_argument(parser);
    }
}

static bool parse_single_quote(Parser* parser);

// parsing an argument, last char was a '\' inside of single quotes
static bool parse_single_quote_slash(Parser* parser) {
    char c = parser_read(parser);
    if (c == '\0') {
        return false;
    } else {
        parser_write(parser, c);
        return parse_single_quote(parser);
    }
}

// parsing an argument, inside of single-quotes
static bool parse_single_quote(Parser* parser) {
    char c = parser_read(parser);
    if (c == '\0') {
        return false;
    } else if (c == '\'') {
        return parse_argument(parser);
    } else if (c == '\\') {
        return parse_single_quote_slash(parser);
    } else {
        parser_write(parser, c);
        return parse_single_quote(parser);
    }
}

static bool parse_double_quote(Parser* parser);

// parsing an argument, last char was a '\' inside of double quotes
static bool parse_double_quote_slash(Parser* parser) {
    char c = parser_read(parser);
    if (c == '\0') {
        return false;
    } else {
        parser_write(parser, c);
        return parse_double_quote(parser);
    }
}

// parsing an argument, inside of double-quotes
static bool parse_double_quote(Parser* parser) {
    char c = parser_read(parser);
    if (c == '\0') {
        return false;
    } else if (c == '\"') {
        return parse_argument(parser);
    } else if (c == '\\') {
        return parse_double_quote_slash(parser);
    } else {
        parser_write(parser, c);
        return parse_double_quote(parser);
    }
}

static bool parse_space(Parser* parser);

// parsing an argument, no special characters in effect
static bool parse_argument(Parser* parser) {
    char c = parser_read(parser);
    if (c == '\0') {
        parser_write(parser, '\0');
        return true;
    } else if (isspace(c)) {
        parser_write(parser, '\0');
        return parse_space(parser);
    } else if (c == '\'') {
        return parse_single_quote(parser);
    } else if (c == '\"') {
        return parse_double_quote(parser);
    } else if (c == '\\') {
        return parse_slash(parser);
    } else {
        parser_write(parser, c);
        return parse_argument(parser);
    }
}

// parsing whitespace inbetween arguments
static bool parse_space(Parser* parser) {
    char c = parser_read(parser);
    if (c == '\0') {
        return true;
    } else if (isspace(c)) {
        return parse_space(parser);
    } else if (c == '\\') {
        parser_mark_argument(parser);
        return parse_slash(parser);
    } else if (c == '\'') {
        parser_mark_argument(parser);
        return parse_single_quote(parser);
    } else if (c == '"') {
        parser_mark_argument(parser);
        return parse_double_quote(parser);
    } else {
        parser_mark_argument(parser);
        parser_write(parser, c);
        return parse_argument(parser);
    }
}

ParseResult libcli_parse(char* input, const char** arguments, size_t max_arguments) {
    Parser parser = {
        .read = input,
        .write = input,
        .arguments = arguments,
        .argument_count = 0,
        .max_arguments = max_arguments,
    };

    bool success = parse_space(&parser);

    return (ParseResult) {
        .success = success,
        .argument_count = parser.argument_count,
    };
}
