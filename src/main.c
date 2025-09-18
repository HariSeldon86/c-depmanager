#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p4.h"
#include <ctype.h>

int test1(void) {
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

int test2(void) {

    struct link {
        struct link *next;
        char *path;
        char *branch;
    };

    FILE *fp = fopen("workspace.cfg", "r");
    if (!fp) {
        printf("Failed to open file.\n");
        return 1;
    }

    char buffer[256]; // buffer for line reading
    struct link* links = malloc(sizeof(struct link) * 256); // allocate memory for up to 256 links
    links->next = NULL;

    if (!links) {
        printf("Failed to allocate memory for links.\n");
        fclose(fp);
        return 1;
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
       
        // Parse through the line to find LINK entries
        int i = 0;
        int k = 0;

        while ( buffer[i] != '\0' ) {
            if (buffer[i] == '#') {
                break; // comment, ignore rest of line
            }
            // Check if the current character is the start of a "LINK" entry
            // If so, extract the path and branch from the line
            if (strncmp(&buffer[i], "LINK ", 5) == 0) {
                // Found a "LINK" entry, so extract the path and branch

                // skip the "LINK " part and any spaces
                int j = 0;
                while (isspace(buffer[i+5+j])) {
                    j++;
                }
                char *path = &buffer[i+5+j];

                // find the space that separates path and branch
                char *branch = strchr(path, ' ');
                if (branch) {
                    *branch = '\0'; // terminate the path string at the space
                    branch++; // move branch pointer to point to the branch string
                }

                // skip any leading spaces in branch
                j = 0;
                while (isspace(branch[j])) {
                    j++;
                }
                branch = &branch[j];

                // skip any trailing whitespace/newline in branch
                j = 0;
                while (isspace(branch[strlen(branch)-1-j])) {
                    j++;
                }
                branch[strlen(branch)-j] = '\0'; // terminate branch string

                // Store the extracted path and branch in the struct
                // links[k] = (struct link){strdup(path), strdup(branch)};
                // link[k].path = strdup(path);
                // link[k].branch = strdup(branch);
                
                links->path = strdup(path);
                links->branch = strdup(branch);

                k++; // move to next link entry

                printf("Path: '%s'\n", links->path);
                // printf("Branch: '%s'\n", link[k].branch);

                break; // done with this line, so move on to the next one
            }
            i++;
        }

        // printf("Index i: %d\n", i);
        /*
        for example

        | H | e | l | l | o | \n | \0 |
        | 0 | 1 | 2 | 3 | 4 |  5 |  6 |
        
        strlen = 6, does not count the '\0'
        i = 6, stops at '\0'
        buffer[i-1] = buffer[5] = '\n'
        */

        // printf("strlen: %lu\n", (unsigned long)strlen(buffer));
        // printf("i: %d\n", i);
        // printf("%c\n", buffer[i-1]); // should be '\0'
        
    }

    // int i;
    // while( (i = fgetc(fp)) != EOF ) {
    //     putchar(i);
    // }

    
    // typedef struct link {
    //     char *path;
    //     char *branch;
    // } Link;

    // Link *links = NULL;
    // size_t count = 0;

    // char *line = NULL;
    // size_t len = 0;
    // long read;

    // while ((read = getline(&line, &len, fp)) != -1) {
    //     Link *link = malloc(sizeof(Link));
    //     if (!link) {
    //         printf("Failed to allocate memory for link.\n");
    //         fclose(fp);
    //         free(line);
    //         return 1;
    //     }

    //     link->path = strdup(line);
    //     link->branch = NULL;

    //     links = realloc(links, (count + 1) * sizeof(Link));
    //     if (!links) {
    //         printf("Failed to allocate memory for links.\n");
    //         fclose(fp);
    //         free(line);
    //         return 1;
    //     }

    //     links[count] = *link;
    //     count++;

    //     free(link);
    // }

    fclose(fp);
    // free(line);

    return 0;

}

int main(void) {
    return test2();
}