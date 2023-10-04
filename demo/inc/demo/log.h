#ifndef DEMO_LOG_H
#define DEMO_LOG_H

#include <stdio.h>
#include <stdint.h>

#include "demo/error.h"

#define DEMO_LOG_INFO(...) demo_log(stdout, DEMO_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define DEMO_LOG_ERROR(error, msg) demo_log(stderr, DEMO_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, "[%s] %s\n", demo_error_str(error), msg)

#ifdef NDEBUG
#define DEMO_LOG_DEBUG(...)
#else
#define DEMO_LOG_DEBUG(...) demo_log(stderr, DEMO_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#endif

#define DEMO_LOG_LEVEL                  \
    X(DEMO_LOG_LEVEL_INFO, 0, "INFO")   \
    X(DEMO_LOG_LEVEL_ERROR, 1, "ERROR") \
    X(DEMO_LOG_LEVEL_DEBUG, 2, "DEBUG")

enum demo_log_level
{
#define X(def, id, str) def=id,

    DEMO_LOG_LEVEL

#undef X
};

void demo_log(FILE *stream, enum demo_log_level level, const char *file, uint32_t line, const char *func, const char *format, ...);
const char * demo_log_level_str(enum demo_log_level level);

#endif /* DEMO_LOG_H */