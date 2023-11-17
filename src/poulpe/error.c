#include "poulpe/error.h"

const char *poulpe_error_str(enum poulpe_error err)
{
    switch (err)
    {
#define X(__def, __id, __str) \
    case __id:                \
        return __str;

        POULPE_ERRORS

#undef X

    default:
        return "Unknown error";
    }
}