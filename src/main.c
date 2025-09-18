#include <stdio.h>
#include <stdlib.h>
#include "p4.h"

int test(void) {
    const char *command = TEST_COMMAND;
    // char *output = _p4cmd(command, NULL);
    char *output = p4cmd(command);
    if (output) {
        printf("%s\n", output);
        free(output);
        return 0;
    } else {
        printf("Failed to capture command output.\n");
        return 1;
    }
}

int main(void) {
    return test();
}