#ifndef POULPE_LANGUAGES_GLSL_H
#define POULPE_LANGUAGES_GLSL_H

#include <string.h>
#include <stdio.h>

#include <cimgui.h>

#include <poulpe/style.h>

#include <poulpe/languages/c.h>

#define POULPE_GLSL_PATTERN                                                      \
  POULPE_C_PATTERN                                                               \
  X(POULPE_GLSL_PATTERN_TYPE_QUALIFER, 16, keyword, "type.qualifier",            \
    "[ \"in\" \"out\" \"inout\" \"uniform\" "                                    \
    "\"shared\" \"layout\" \"attribute\" \"varying\" "                           \
    "\"buffer\" \"coherent\" \"readonly\" \"writeonly\" "                        \
    "\"precision\" \"highp\" \"mediump\" \"lowp\" "                              \
    "\"centroid\" \"sample\" \"patch\" \"smooth\" "                              \
    "\"flat\" \"noperspective\" \"invariant\" \"precise\" ] @type.qualifier ")   \
  X(POULPE_GLSL_PATTERN_TYPE_KEYWORD_FUNCTION, 17, keyword, "keyword.function",  \
    "\"subroutine\" @keyword.function ")                                         \
  X(POULPE_GLSL_PATTERN_TYPE_STORAGE_CLASS, 18, variable, "storageclass",        \
    "(extension_storage_class) @storageclass ")                                  \
  X(POULPE_GLSL_PATTERN_TYPE_VARIABLE_BUILTIN, 19, variable, "variable.builtin", \
    "((identifier) @variable.builtin (#lua-match? @variable.builtin \"^gl_\")) ")

const char *poulpe_glsl_extensions[] = {
    ".vert", ".tesc", ".tese", ".geom", ".frag", ".comp"};

const char *poulpe_glsl_highlight_query =
#define X(__def, __id, __color, __key, __query) \
  __query

    POULPE_GLSL_PATTERN;

#undef X

static inline ImU32 poulpe_glsl_pattern_color(const char *pattern)
{
#define X(__def, __id, __color, __key, __query)                  \
  if (strcmp(pattern, __key) == 0)                               \
    return igColorConvertFloat4ToU32(poulpe_style.theme->__color);

  POULPE_GLSL_PATTERN

#undef X
  return 0;
}

#endif /* POULPE_LANGUAGES_GLSL_H */
