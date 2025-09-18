#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_COMMAND "p4 login -s"

int p4cmd(const char *cmd) {
    char full_cmd[256];
    snprintf(full_cmd, sizeof(full_cmd), "%s", cmd);
    return system(full_cmd);
}

int main(void) {
    const char *command = DEFAULT_COMMAND;
    int ret = p4cmd(command);
    if (ret != 0) {
        fprintf(stderr, "Error: '%s' failed with code %d\n", command, ret);
        return 1;
    }
    return 0;
}