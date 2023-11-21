#ifndef POULPE_ERROR_H
#define POULPE_ERROR_H

#define POULPE_ERRORS                                          \
    X(POULPE_ERROR_NONE, 0, "No error")                        \
    X(POULPE_ERROR_VALUE, -1, "Value error")                   \
    X(POULPE_ERROR_FILE, -2, "File error")                     \
    X(POULPE_ERROR_MEMORY, -3, "Memory error")                 \
    X(POULPE_ERROR_NOT_IMPLEMENTED, -4, "Not implemented yet") \
    X(POULPE_ERROR_UNKNOWN, -99, "Unknown error")

enum poulpe_error
{
#define X(__def, __id, __str) \
    __def = __id,

    POULPE_ERRORS

#undef X
};

const char * poulpe_error_str(enum poulpe_error err);

#endif /* POULPE_ERROR_H */