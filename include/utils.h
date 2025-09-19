#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* get_file_content(FILE* fp);

char* strtrim(char *str);

#endif // UTILS_H