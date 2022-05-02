#include "utils.h"

#include <stdlib.h>
#include <string.h>

char *copy_string(char *source) {
    if (!source) {
        LOG_WARNING("Source string is NULL\n");
        return NULL;
    }
    char *string = calloc(sizeof *string, strlen(source) + 1);
    strcpy(string, source);
    return string;
}
