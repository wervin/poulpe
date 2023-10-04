#ifndef DEMO_ERROR_H
#define DEMO_ERROR_H

#define DEMO_ERROR                                                           \
    X(DEMO_ERROR_NONE,           0,  "No error")                             \
    X(DEMO_ERROR_VALUE,         -1,  "Value error")                          \
    X(DEMO_ERROR_FILE,          -2,  "File error")                           \
    X(DEMO_ERROR_MEMORY,        -3,  "Memory error")                         \
    X(DEMO_ERROR_VK,            -4,  "Vulkan error")                         \
    X(DEMO_ERROR_GLFW,          -5,  "GLFW error")                           \
    X(DEMO_ERROR_POULPE,        -6,  "Poulpe error")                         \

enum demo_error 
{
#define X(def, id, str) def=id,

    DEMO_ERROR

#undef X
};

const char * demo_error_str(enum demo_error err);

#endif /* DEMO_ERROR_H */