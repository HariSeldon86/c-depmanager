#include <stdio.h>
#include <stdlib.h>
#include "p4.h"

/**
 * Runs a command and returns its output as a dynamically allocated string.
 * The caller is responsible for freeing the returned string.
 * 
 * @param command The command to run.
 * @return A dynamically allocated string containing the command output if successful, NULL otherwise.
 */

char* p4cmd(const char* command) {
    // char full_cmd[1024];
    // snprintf(full_cmd, sizeof(full_cmd), "%s", command);

    FILE *fp = popen(command, "r");
    if (!fp) return NULL;

    char* output = get_file_content(fp);
    if (!output) {
        pclose(fp);
        return NULL;
    }
    pclose(fp);
    return output;
}