#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "poulpe/language.h"
#include "poulpe/languages/json.h"
#include "poulpe/languages/c.h"
#include "poulpe/languages/glsl.h"

static const char *_get_filename_extension(const char *filename);
static bool _is_in( const char *extension, const char *extensions[], uint32_t n);

#define X(__def, __id, __type) \
    extern const TSLanguage *tree_sitter_##__type(void);

POULPE_LANGUAGES

#undef X

enum poulpe_language_type poulpe_language_auto_detect(const char *filename)
{
    const char *extension = _get_filename_extension(filename);
    if (!strlen(extension))
        return POULPE_LANGUAGE_TYPE_RAW;

#define X(__def, __id, __type)                    \
    if (_is_in(extension, poulpe_##__type##_extensions, sizeof(poulpe_##__type##_extensions) / sizeof(char *))) \
        return __def;

    POULPE_LANGUAGES

#undef X

    return POULPE_LANGUAGE_TYPE_RAW;
}

const TSLanguage *poulpe_language_parser(enum poulpe_language_type type)
{
    switch (type)
    {
#define X(__def, __id, __type) \
    case __id:                 \
        return tree_sitter_##__type();


    POULPE_LANGUAGES

#undef X
    
    default:
        return NULL;
    }
}

const char *poulpe_language_query(enum poulpe_language_type type)
{
    switch (type)
    {
#define X(__def, __id, __type) \
    case __id:                 \
        return poulpe_##__type##_highlight_query;

    POULPE_LANGUAGES

#undef X
    
    default:
        return NULL;
    }
}

ImU32 poulpe_language_pattern_color(enum poulpe_language_type type, const char *pattern)
{
    switch (type)
    {
#define X(__def, __id, __type) \
    case __id:                 \
        return poulpe_##__type##_pattern_color(pattern);

    POULPE_LANGUAGES

#undef X
    
    default:
        return 0;
    }
}

static const char *_get_filename_extension(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot;
}

static bool _is_in(const char *extension, const char *extensions[], uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
    {
        if (strcmp(extension, extensions[i]) == 0)
            return true;
    }
    return false;
}