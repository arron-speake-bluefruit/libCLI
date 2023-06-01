#include "internal/parse.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void empty_input() {
    const char* arguments[1] = {0};
    char input[] = "";
    ParseResult result = libcli_parse(input, arguments, 1);

    assert(result.status == parse_status_success);
    assert(result.argument_count == 0);
}

static void whitespace_only() {
    const char* arguments[1] = {0};
    char input[] = " \n    \r\n   \t\t";
    ParseResult result = libcli_parse(input, arguments, 1);

    assert(result.status == parse_status_success);
    assert(result.argument_count == 0);
}

static void single_argument() {
    const char* arguments[1] = {0};
    char input[] = "hello";
    ParseResult result = libcli_parse(input, arguments, 1);

    assert(result.status == parse_status_success);
    assert(result.argument_count == 1);
    assert(strcmp("hello", arguments[0]) == 0);
}

static void many_arguments() {
    const char* arguments[5] = {0};
    char input[] = "hello these are arguments";
    ParseResult result = libcli_parse(input, arguments, 5);

    assert(result.status == parse_status_success);
    assert(result.argument_count == 4);
    assert(strcmp("hello", arguments[0]) == 0);
    assert(strcmp("these", arguments[1]) == 0);
    assert(strcmp("are", arguments[2]) == 0);
    assert(strcmp("arguments", arguments[3]) == 0);
}

static void single_quotes() {
    const char* arguments[2] = {0};
    char input[] = "'hello there'";
    ParseResult result = libcli_parse(input, arguments, 2);

    assert(result.status == parse_status_success);
    assert(result.argument_count == 1);
    assert(strcmp("hello there", arguments[0]) == 0);
}

static void double_quotes() {
    const char* arguments[2] = {0};
    char input[] = "\"hello there\"";
    ParseResult result = libcli_parse(input, arguments, 2);

    assert(result.status == parse_status_success);
    assert(result.argument_count == 1);
    assert(strcmp("hello there", arguments[0]) == 0);
}

static void mixed_quotes() {
    const char* arguments[3] = {0};
    char input[] = "\"\\\"hello' \\'there\\\"\" 'what\\'s \"up\\\"?'";
    ParseResult result = libcli_parse(input, arguments, 3);

    assert(result.status == parse_status_success);
    assert(result.argument_count == 2);
    assert(strcmp("\"hello' 'there\"", arguments[0]) == 0);
    assert(strcmp("what's \"up\"?", arguments[1]) == 0);
}

static void backslashes() {
    const char* arguments[2] = {0};
    char input[] = "\\\'\\\"";
    ParseResult result = libcli_parse(input, arguments, 2);

    assert(result.status == parse_status_success);
    assert(result.argument_count == 1);
    assert(strcmp("\'\"", arguments[0]) == 0);
}

static void eof_after_slash() {
    const char* arguments[1] = {0};
    char input[] = "\\";
    ParseResult result = libcli_parse(input, arguments, 1);

    assert(result.status == parse_status_eof_after_slash);
}

static void unterminated_double_quote() {
    const char* arguments[1] = {0};
    char input[] = "\"these quotes don't end!";
    ParseResult result = libcli_parse(input, arguments, 1);

    assert(result.status == parse_status_unterminated_double_quote);
}

static void unterminated_single_quote() {
    const char* arguments[1] = {0};
    char input[] = "\'uh oh";
    ParseResult result = libcli_parse(input, arguments, 1);

    assert(result.status == parse_status_unterminated_single_quote);
}

int main(void) {
    typedef void (*Test)(void);

    const Test tests[] = {
        empty_input,
        whitespace_only,
        single_argument,
        many_arguments,
        single_quotes,
        double_quotes,
        mixed_quotes,
        backslashes,
        eof_after_slash,
        unterminated_double_quote,
        unterminated_single_quote,
    };

    const size_t test_count = sizeof(tests) / sizeof(Test);
    for (size_t i = 0; i < test_count; i++) {
        Test test = tests[i];
        test();
    }

    printf("All tests (%zu) passed.\n", test_count);
}
