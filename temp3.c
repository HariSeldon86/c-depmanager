#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PATH 512

#define DEBUG 0

/**
 * A struct to hold a list of strings.
 *
 * This struct is used to hold a list of strings, where each string is a path to a dependency.
 * The struct contains a pointer to an array of strings, and a size_t to hold the number of items in the array.
 */
typedef struct {
    char **items;     /**< pointer to an array of strings, each string is a path to a dependency */
    size_t count;     /**< number of items in the array */
} DependencyList;


// Trim leading and trailing spaces (in place)
char* trim(char *str) {
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
    if (strcmp(filename, "workspace.cfg") == 0) {
        f = fopen(filename, "r");
        if (!f) {
            perror(filename);
            return;
        }

    } else {
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
    
    while (fgets(line, sizeof(line), f)) {

        // Trim newline and carriage return characters
        // The '\r' character is carriage return (CR) and the '\n' character is line feed (LF)
        // In Windows, a newline is represented by both CR and LF characters, i.e. "\r\n"
        // In Unix, a newline is represented by just the LF character, i.e. "\n"
        // The 'strcspn' function removes all occurrences of the specified characters from the end of the string
        line[strcspn(line, "\r\n")] = '\0';

        // Trim left and right whitespace
        char *trimmed = trim(line);

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
            strncpy(root_folder, trim(trimmed + 12), sizeof(root_folder) - 1);
            root_folder[sizeof(root_folder) - 1] = '\0'; // Ensure null-termination
            printf("Root folder set to: '%s'\n", root_folder);
            has_root_folder = 1;
            continue;
        } else {
            fprintf(stderr, "Warning: Ignoring invalid line in %s: '%s'\n", filename, trimmed);
            continue;
        }

        // Parse the line to extract path, depot and branch
        char *path = trim(trimmed + 5); // skip "LINK "
        char *depot = extract_depot(path);
        
        char *branch = strchr(path, ' ');
        if (branch) {
            *branch++ = '\0'; // split the string into path and branch
            branch = trim(branch);
        }

        if (!depot) {
            
            if (has_root_folder) {
                printf("Adjusting path with root folder: '%s' + '%s'\n", root_folder, path);
                char new_path[MAX_PATH];
                snprintf(new_path, sizeof(new_path), "%s/%s", root_folder, path);
                // free(path);
                path = strdup(new_path);
                printf("Adjusted path with root folder: '%s'\n", path);
                depot = extract_depot(path);
                if (!depot) {
                    fprintf(stderr, "Warning: Invalid depot path in %s: '%s'\n", filename, path);
                    // free(path);
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
            snprintf(full_path, sizeof(full_path), "%s/%s", path, branch);
            add_unique(list, full_path); // add combined path and branch

            char depfilename[MAX_PATH];
            snprintf(depfilename, sizeof(depfilename), "%s/%s/dependencies.cfg", path, branch);

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


int main(int argc, char *argv[]) {
    DependencyList *list = create_list();
    parse_config("workspace.cfg", list);

    printf("Unique Dependencies:\n");
    for (size_t i = 0; i < list->count; i++) {
        printf("%s\n", list->items[i]);
    }

    free_list(list);
    return 0;
}
