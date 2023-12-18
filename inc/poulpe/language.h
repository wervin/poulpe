#ifndef POULPE_LANGUAGE_H
#define POULPE_LANGUAGE_H

#include <cimgui.h>

typedef struct TSLanguage TSLanguage;

#define POULPE_LANGUAGES                  \
    X(POULPE_LANGUAGE_TYPE_JSON, 1, json) \
    X(POULPE_LANGUAGE_TYPE_C, 2, c)       \
    X(POULPE_LANGUAGE_TYPE_GLSL, 3, glsl)

enum poulpe_language_type
{
    POULPE_LANGUAGE_TYPE_RAW,
#define X(__def, __id, __type) \
    __def = __id,

    POULPE_LANGUAGES

#undef X
};

enum poulpe_language_type poulpe_language_auto_detect(const char *filename);
const TSLanguage *poulpe_language_parser(enum poulpe_language_type type);
const char *poulpe_language_query(enum poulpe_language_type type);
ImU32 poulpe_language_pattern_color(enum poulpe_language_type type, const char *pattern);

#endif /* POULPE_LANGUAGE_H */