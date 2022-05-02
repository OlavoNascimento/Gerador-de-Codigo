#pragma once

#include <stdio.h>

#ifdef GERADOR_DEBUG
    #define LOG_INFO(fmt, ...) \
        printf("\033[0;34mINFO\033[0m:%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_WARNING(fmt, ...) \
        printf("\033[0;33mWARNING\033[0m:%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_ERROR(fmt, ...) \
        fprintf(stderr, "\033[0;31mERROR\033[0m:%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define LOG_INFO(fmt, ...)
    #define LOG_WARNING(fmt, ...)
    #define LOG_ERROR(fmt, ...)
#endif

char *copy_string(char *source);
