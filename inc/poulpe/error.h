#ifndef POULPE_ERROR_H
#define POULPE_ERROR_H

#define POULPE_ERROR                                                           \
    X(POULPE_ERROR_NONE,           0,  "No error")                             \
    X(POULPE_ERROR_VALUE,         -1,  "Value error")                          \
    X(POULPE_ERROR_FILE,          -2,  "File error")                           \
    X(POULPE_ERROR_MEMORY,        -3,  "Memory error")                         \

enum poulpe_error 
{
#define X(def, id, str) def=id,

    POULPE_ERROR

#undef X
};

const char * poulpe_error_str(enum poulpe_error err);

#endif /* POULPE_ERROR_H */