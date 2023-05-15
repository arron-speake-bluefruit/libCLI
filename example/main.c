#include "cli/cli.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("expected 1 argument\n");
        return 1;
    }

    // set up input buffer
    char input[1024] = {0};
    memcpy(input, argv[1], strlen(argv[1]));

    // pass input to libCLI
    libcli_run(input, NULL);
}
