#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define TEST_COMMAND "p4 login -s"
// #define TEST_COMMAND "p4 print -q  //codev_soda/mbdt/ProjectEnvs/Soda/Ecu/main/dependencies.cfg"
// #define TEST_COMMAND "p4 files -e //application_software/ucs/Systems/.../dependencies.cfg"
// #define TEST_COMMAND "p4 files -e //application_software/ucs/.../dependencies.cfg"

#define TEST_COMMAND "p4 login -s"



// use a single reallocation strategy
char* p4cmd(const char* command, size_t *output_len) {
    char full_cmd[256];
    snprintf(full_cmd, sizeof(full_cmd), "%s", command);

    FILE *fp = popen(full_cmd, "r");
    if (!fp) return NULL;
    
    size_t capacity = 1024;  // Start with reasonable size
    char *result = malloc(capacity);
    if (!result) {
        pclose(fp);
        return NULL;
    }
    
    size_t total_len = 0;
    int c;
    
    // Read character by character to avoid buffer copying
    while ((c = fgetc(fp)) != EOF) {
        if (total_len >= capacity - 1) {
            capacity *= 2;  // Double capacity when needed
            char *new_result = realloc(result, capacity); // 'realloc' also frees old memory 'result' if successful
            if (!new_result) {
                free(result);
                pclose(fp);
                return NULL;
            }
            result = new_result;
        }
        result[total_len++] = c;
    }
    
    result[total_len] = '\0';
    pclose(fp);
    
    // Shrink to actual size to save memory
    char *final_result = realloc(result, total_len + 1);
    if (final_result) result = final_result;
    
    if (output_len) *output_len = total_len;
    return result;
}

int test(void) {
    const char *command = TEST_COMMAND;
    size_t len; // optional length output
    char *output = p4cmd(command, &len);
    if (output) {
        printf("%s\n", output);
        // printf("Output length: %lu bytes\n", (unsigned long)len);
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