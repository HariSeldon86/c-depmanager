#ifndef P4_H
#define P4_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "constants.h"

#ifdef _WIN32
#include <io.h>     // for _close
#define CLOSE _close
#else
#include <unistd.h> // for close
#define CLOSE close
#endif

// #define TEST_COMMAND "p4 login -s"
// #define TEST_COMMAND "p4 print -q  //codev_soda/mbdt/ProjectEnvs/Soda/Ecu/main/dependencies.cfg"
// #define TEST_COMMAND "p4 files -e //application_software/ucs/Systems/.../dependencies.cfg"
// #define TEST_COMMAND "p4 files -e //application_software/ucs/.../dependencies.cfg"

#define TEST_COMMAND "p4 login -s"
// #define TEST_COMMAND "dir"

#define MAX_PATH 512

#define WORKSPACE_CFG "workspace.cfg"
#define DEPENDENCIES_CFG "dependencies.cfg"

char* p4cmd(const char* command);

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


int p4_file_exists(const char *filename);
DependencyList* create_list();
void free_list(DependencyList *list);
void add_unique(DependencyList *list, const char *path);
void parse_config(const char *filename, DependencyList *list);
char* extract_depot(const char *path);
int contains(DependencyList *list, const char *path);

#endif // P4_H