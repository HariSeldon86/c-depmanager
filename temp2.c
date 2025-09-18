#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH 512

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

    add_unique(list, filename); // add current file

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return;
    }

    char line[MAX_PATH];
    while (fgets(line, sizeof(line), f)) {
        // Trim newline
        line[strcspn(line, "\r\n")] = '\0';

        // Skip empty lines and comments
        if (line[0] == '\0' || line[0] == '#') continue;

        // Recursively parse child dependency
        parse_config(line, list);
    }

    fclose(f);
}


int main(int argc, char *argv[]) {
    DependencyList *list = create_list();
    parse_config("dependencies.cfg", list);

    printf("Unique Dependencies:\n");
    for (size_t i = 0; i < list->count; i++) {
        printf("%s\n", list->items[i]);
    }

    free_list(list);
    return 0;
}
