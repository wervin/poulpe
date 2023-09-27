#ifndef POULPE_LOG_H
#define POULPE_LOG_H

#include <stdio.h>
#include <stdint.h>

#include "poulpe/error.h"

#define POULPE_LOG_INFO(...) poulpe_log(stdout, POULPE_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define POULPE_LOG_ERROR(error, msg) poulpe_log(stderr, POULPE_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, "[%s] %s\n", poulpe_error_str(error), msg)

#ifdef NDEBUG
#define POULPE_LOG_DEBUG(...)
#else
#define POULPE_LOG_DEBUG(...) poulpe_log(stderr, POULPE_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#endif

#define POULPE_LOG_LEVEL                  \
    X(POULPE_LOG_LEVEL_INFO, 0, "INFO")   \
    X(POULPE_LOG_LEVEL_ERROR, 1, "ERROR") \
    X(POULPE_LOG_LEVEL_DEBUG, 2, "DEBUG")

enum poulpe_log_level
{
#define X(def, id, str) def=id,

    POULPE_LOG_LEVEL

#undef X
};

void poulpe_log(FILE *stream, enum poulpe_log_level level, const char *file, uint32_t line, const char *func, const char *format, ...);
const char * poulpe_log_level_str(enum poulpe_log_level level);

#endif /* POULPE_LOG_H */