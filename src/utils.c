#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

/**
 * Reads the content of a file and returns it as a dynamically allocated string.
 * The caller is responsible for freeing the returned string and closing the file pointer.
 * 
 * @param fp The file pointer to read from.
 * @return A dynamically allocated string containing the file content if successful, NULL otherwise.
 */
char* get_file_content(FILE* fp) {
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
                // pclose(fp);
                return NULL;
            }
            result = new_result;
        }
        result[total_len++] = c;
    }
    
    result[total_len] = '\0';
    // pclose(fp);
    
    // Shrink to actual size to save memory
    char *final_result = realloc(result, total_len + 1);
    if (final_result) result = final_result;
    
    return result;
}


// Trim leading and trailing spaces (in place)
char* strtrim(char *str) {
    char *end;

    // Trim leading space, by moving the pointer forward until it points to the first non-space character
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // all spaces?
        return str;

    // Trim trailing space, by moving the end pointer backward until it points to the last non-space character
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}