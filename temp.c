#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH 512

// Node for a dependency
typedef struct Dependency {
    char *path;                     // path to this dependency.cfg
    struct Dependency **children;   // array of child dependencies
    size_t child_count;             // number of children
} Dependency;

Dependency* create_dependency(const char *path) {
    Dependency *dep = malloc(sizeof(Dependency));
    if (!dep) { perror("malloc"); exit(1); }
    dep->path = strdup(path);
    dep->children = NULL;
    dep->child_count = 0;
    return dep;
}

void add_child(Dependency *parent, Dependency *child) {
    parent->children = realloc(parent->children,
                               (parent->child_count + 1) * sizeof(Dependency*));
    if (!parent->children) { perror("realloc"); exit(1); }
    parent->children[parent->child_count++] = child;
}

void free_dependency(Dependency *dep) {
    if (!dep) return;
    for (size_t i = 0; i < dep->child_count; i++) {
        free_dependency(dep->children[i]);
    }
    free(dep->children);
    free(dep->path);
    free(dep);
}

void print_dependency(const Dependency *dep, int level) {
    if (!dep) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf("%s\n", dep->path);
    for (size_t i = 0; i < dep->child_count; i++) {
        print_dependency(dep->children[i], level + 1);
    }
}

Dependency* parse_config(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return NULL;
    }

    Dependency *tree = create_dependency(filename);
    char line[MAX_PATH];

    while (fgets(line, sizeof(line), f)) {
        // Trim newline
        line[strcspn(line, "\r\n")] = '\0';

        // Skip empty lines and comments
        if (line[0] == '\0' || line[0] == '#') continue;

        // Recursively parse child
        Dependency *child = parse_config(line);
        if (child) {
            add_child(tree, child);
        }
    }

    fclose(f);
    return tree;
}


int main(int argc, char *argv[]) {

    Dependency *tree = parse_config("dependencies.cfg");
    if (!tree) return 1;

    printf("Dependency Tree:\n");
    print_dependency(tree, 0);

    free_dependency(tree);
    return 0;
}
