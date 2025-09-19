#include <stdio.h>
#include "p4.h"



int main(void) {
    DependencyList *list = create_list();
    parse_config(WORKSPACE_CFG, list);

    printf("Unique Dependencies:\n");
    for (size_t i = 0; i < list->count; i++) {
        printf("%s\n", list->items[i]);
    }

    free_list(list);
    
    return 0;
}