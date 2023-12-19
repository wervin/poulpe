#ifndef POULPE_LANGUAGES_JSON_H
#define POULPE_LANGUAGES_JSON_H

#include <string.h>

#include <cimgui.h>

#include <poulpe/style.h>

#define POULPE_JSON_PATTERN                                                   \
  X(POULPE_JSON_PATTERN_STRING_SPECIAL_KEY, 1, keyword, "string.special.key", \
    "(pair key: (_) @string.special.key) ")                                   \
  X(POULPE_JSON_PATTERN_STRING, 2, string, "string",                          \
    "(string) @string ")                                                      \
  X(POULPE_JSON_PATTERN_NUMBER, 3, number, "number",                          \
    "(number) @number ")                                                      \
  X(POULPE_JSON_PATTERN_CONSTANT_BUILTIN, 4, constant, "constant.builtin",    \
    "[(null) (true) (false)] @constant.builtin ")                             \
  X(POULPE_JSON_PATTERN_ESCAPE, 5, feature, "escape",                         \
    "(escape_sequence) @escape ")                                             \
  X(POULPE_JSON_PATTERN_COMMENT, 6, comment, "comment",                       \
    "(comment) @comment")

const char *poulpe_json_extensions[] = {
    ".json"
};

const char *poulpe_json_highlight_query = 
#define X(__def, __id, __color, __key, __query)  \
    __query

    POULPE_JSON_PATTERN;

#undef X

static inline ImU32 poulpe_json_pattern_color(const char *pattern)
{
#define X(__def, __id, __color, __key, __query) \
  if (strcmp(pattern, __key) == 0)              \
    return igColorConvertFloat4ToU32(poulpe_style.theme->__color);

  POULPE_JSON_PATTERN

#undef X
  return 0;
}

#endif /* POULPE_LANGUAGES_JSON_H */