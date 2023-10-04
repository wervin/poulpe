#include "demo/error.h"

const char* demo_error_str(enum demo_error err)
{
    switch (err)
    {
#define X(def,id,str)   \
    case id:            \
        return str;     \
    
    DEMO_ERROR

#undef X

    default:
        return "Unknown error";
    }
}