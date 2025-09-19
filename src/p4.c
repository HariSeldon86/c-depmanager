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



int p4_file_exists(const char *filename) {
    char tmpname[] = "p4tmpXXXXXX";
    int fd = mkstemp(tmpname);    // create unique temp file
    if (fd == -1) return 0;
    CLOSE(fd);                    // close the file descriptor, we don't need it anymore, but mkstemp()// will reopen with fopen()

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "p4 fstat \"%s\" > %s 2>&1", filename, tmpname);

    int ret = system(cmd);
    if (ret != 0) {
        remove(tmpname);
        return 0;
    }

    FILE *fp = fopen(tmpname, "r");
    if (!fp) {
        remove(tmpname);
        return 0;
    }

    char first[1024];
    int found = 1;
    fgets(first, sizeof(first), fp);
    if (strstr(first, "no such file(s)") != NULL) found = 0;
    fclose(fp);
    remove(tmpname);
    return found;
}

char* extract_depot(const char *path) {
    if (!path || strncmp(path, "//", 2) != 0) {
        return NULL; // not a depot path
    }

    // Find the slash after "//"
    const char *slash = strchr(path + 2, '/');
    if (!slash) {
        return strdup(path); // only "//depot" present
    }

    // Length from start of path to this slash
    size_t len = slash - path;
    char *depot = malloc(len + 1);
    if (!depot) return NULL;

    strncpy(depot, path, len);
    depot[len] = '\0';
    return depot;
}

// Create an empty list
/**
 * Creates an empty list.
 *
 * This function allocates memory for a DependencyList struct and initializes its members.
 * The items pointer is set to NULL, and the count is set to 0.
 *
 * @return A pointer to the newly created DependencyList struct.
 */
DependencyList* create_list() {
    DependencyList *list = malloc(sizeof(DependencyList));
    if (!list) return NULL; // out of memory

    list->items = NULL; // initially no items
    list->count = 0; // initially no items

    return list;
}

// Free the list
/**
 * Frees a DependencyList struct and all its associated memory.
 *
 * This function frees a DependencyList struct and all its associated memory.
 * It first frees the strings in the items array, then frees the items array itself, and finally frees the DependencyList struct.
 *
 * @param list The DependencyList struct to free.
 */
void free_list(DependencyList *list) {
    if (!list) return;
    for (size_t i = 0; i < list->count; i++) {
        free(list->items[i]); // free each string in the items array
    }
    free(list->items); // free the items array itself
    free(list); // free the DependencyList struct
}

// Check if item already exists
/**
 * Checks if a given path already exists in a DependencyList.
 *
 * This function checks if a given path already exists in a DependencyList.
 * It iterates over the items array in the DependencyList struct and compares each string with the given path.
 * If a match is found, it returns 1. If no match is found after checking all items, it returns 0.
 *
 * @param list The DependencyList struct to check.
 * @param path The path to check for.
 * @return 1 if the path already exists in the list, 0 otherwise.
 */
int contains(DependencyList *list, const char *path) {
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i], path) == 0) return 1; // if the path matches an item in the list, return 1
    }
    return 0; // if the path does not match any item in the list, return 0
}

// Add unique item
/**
 * Adds a unique item to a DependencyList.
 *
 * This function adds a unique item to a DependencyList.
 * It first checks if the given path already exists in the list.
 * If it does, the function does nothing and returns.
 * If it does not, the function increments the count of the list and reallocates the items array to make room for the new item.
 * It then copies the given path into the newly allocated space in the items array and increments the count of the list.
 *
 * @param list The DependencyList struct to add to.
 * @param path The path to add to the list.
 */
void add_unique(DependencyList *list, const char *path) {
    // Check if the path already exists in the list
    if (contains(list, path)) return;

    // Increment the count of the list and reallocate the items array to make room for the new item
    list->items = realloc(list->items, (list->count + 1) * sizeof(char*));
    if (!list->items) { perror("realloc"); exit(1); }

    // Copy the given path into the newly allocated space in the items array
    list->items[list->count++] = strdup(path);
}



void parse_config(const char *filename, DependencyList *list) {

    FILE *f;
    if (strcmp(filename, WORKSPACE_CFG) == 0) {
        f = fopen(filename, "r");
        if (!f) {
            perror(filename);
            return;
        }
    } else {
        // check if file exists in Perforce
        if (!VERBOSE && !p4_file_exists(filename)) return;

        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "p4 print -q %s", filename);

        f = popen(cmd, "r");
        if (!f) {
            perror("popen");
            return;
        }
    }   

    char line[MAX_PATH];
    char root_folder[MAX_PATH];
    char has_root_folder = 0;

    // if (fgetc(f) == EOF) printf("No such file(s): %s\n", filename); // should be first character of the file
    
    while (fgets(line, sizeof(line), f)) {

        // Trim newline and carriage return characters
        // The '\r' character is carriage return (CR) and the '\n' character is line feed (LF)
        // In Windows, a newline is represented by both CR and LF characters, i.e. "\r\n"
        // In Unix, a newline is represented by just the LF character, i.e. "\n"
        // The 'strcspn' function removes all occurrences of the specified characters from the end of the string
        line[strcspn(line, "\r\n")] = '\0';

        // if (has_no_deps_file(line)) printf("No such file(s): %s\n", filename);

        // Trim left and right whitespace
        char *trimmed = strtrim(line);

        // Skip empty lines and comments
        if (trimmed[0] == '\0' || trimmed[0] == '#') continue;

        // Now line should start with "LINK <path> <branch>"
        if (strncmp(trimmed, "LINK ", 5) == 0) {
            // ok
        } else if (strncmp(trimmed, "ROOT_FOLDER ", 12) == 0) {
            if (has_root_folder) {
                fprintf(stderr, "Warning: Multiple ROOT_FOLDER entries found in %s. Using the first one: '%s'\n", filename, root_folder);
                continue;
            }
            strncpy(root_folder, strtrim(trimmed + 12), sizeof(root_folder) - 1);
            root_folder[sizeof(root_folder) - 1] = '\0'; // Ensure null-termination
            if(DEBUG) printf("Root folder set to: '%s'\n", root_folder);
            has_root_folder = 1;
            continue;
        } else {
            fprintf(stderr, "Warning: Ignoring invalid line in %s: '%s'\n", filename, trimmed);
            continue;
        }

        // Parse the line to extract path, depot and branch
        char *path = strtrim(trimmed + 5); // skip "LINK "
        char *depot = extract_depot(path);
        
        char *branch = strchr(path, ' ');
        if (branch) {
            *branch++ = '\0'; // split the string into path and branch
            branch = strtrim(branch);
        }

        if (!depot) {
            
            if (has_root_folder) {
                char new_path[MAX_PATH*2];
                snprintf(new_path, sizeof(new_path), "%s/%s", root_folder, path);
                path = strdup(new_path);
                if(DEBUG) printf("Adjusted path with root folder: '%s'\n", path);
                depot = extract_depot(path);
                if (!depot) {
                    fprintf(stderr, "Warning: Invalid depot path in %s: '%s'\n", filename, path);
                    continue;
                }
            } else {
                fprintf(stderr, "Warning: Invalid depot path in %s: '%s'\n", filename, path);
                continue;
            }

        }

        if (DEBUG) {
            printf("Line: '%s'\n", line);
            printf("  Path: '%s'\n", path);
            printf("  Depot: '%s'\n", depot);
            printf("  Branch: '%s'\n", branch ? branch : "(none)");
            printf("\n");
        }
        
        if (branch != NULL && strcmp(branch, "~") != 0) {
            // Combine path and branch for unique entry
            char full_path[MAX_PATH];
            snprintf(full_path, sizeof(full_path), "%s:%s", path, branch);
            add_unique(list, full_path); // add combined path and branch

            char depfilename[MAX_PATH];
            snprintf(depfilename, sizeof(depfilename), "%s/%s/%s", path, branch, DEPENDENCIES_CFG);

            // Recursively parse child dependency
            parse_config(depfilename, list);

        } else {
            add_unique(list, path); // add just the path if no branch
        }

        // Recursively parse child dependency
        // parse_config(line, list);
    }

    fclose(f);
}
