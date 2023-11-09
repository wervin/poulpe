#include "poulpe/error.h"

const char *poulpe_error_str(enum poulpe_error err)
{
    switch (err)
    {
#define X(def, id, str) \
    case id:            \
        return str;

        POULPE_ERRORS

#undef X

    default:
        return "Unknown error";
    }
}