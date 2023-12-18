#ifndef POULPE_LANGUAGES_GLSL_H
#define POULPE_LANGUAGES_GLSL_H

#include <string.h>
#include <stdio.h>

#include <cimgui.h>

#include <poulpe/theme.h>

#define POULPE_GLSL_PATTERN                                                               \
  X(POULPE_GLSL_PATTERN_KEYWORD, 1, keyword, "keyword",                                   \
    "\"break\" @keyword "                                                                 \
    "\"case\" @keyword "                                                                  \
    "\"const\" @keyword "                                                                 \
    "\"continue\" @keyword "                                                              \
    "\"default\" @keyword "                                                               \
    "\"do\" @keyword "                                                                    \
    "\"else\" @keyword "                                                                  \
    "\"enum\" @keyword "                                                                  \
    "\"extern\" @keyword "                                                                \
    "\"for\" @keyword "                                                                   \
    "\"if\" @keyword "                                                                    \
    "\"inline\" @keyword "                                                                \
    "\"return\" @keyword "                                                                \
    "\"sizeof\" @keyword "                                                                \
    "\"static\" @keyword "                                                                \
    "\"struct\" @keyword "                                                                \
    "\"switch\" @keyword "                                                                \
    "\"typedef\" @keyword "                                                               \
    "\"union\" @keyword "                                                                 \
    "\"volatile\" @keyword "                                                              \
    "\"while\" @keyword "                                                                 \
    "\"#define\" @keyword "                                                               \
    "\"#elif\" @keyword "                                                                 \
    "\"#else\" @keyword "                                                                 \
    "\"#endif\" @keyword "                                                                \
    "\"#if\" @keyword "                                                                   \
    "\"#ifdef\" @keyword "                                                                \
    "\"#ifndef\" @keyword "                                                               \
    "\"#include\" @keyword "                                                              \
    "(preproc_directive) @keyword ")                                                      \
  X(POULPE_GLSL_PATTERN_OPERATOR, 2, operator, "operator",                                \
    "\"--\" @operator "                                                                   \
    "\"-\" @operator "                                                                    \
    "\"-=\" @operator "                                                                   \
    "\"->\" @operator "                                                                   \
    "\"=\" @operator "                                                                    \
    "\"!=\" @operator "                                                                   \
    "\"*\" @operator "                                                                    \
    "\"&\" @operator "                                                                    \
    "\"&&\" @operator "                                                                   \
    "\"+\" @operator "                                                                    \
    "\"++\" @operator "                                                                   \
    "\"+=\" @operator "                                                                   \
    "\"<\" @operator "                                                                    \
    "\"==\" @operator "                                                                   \
    "\">\" @operator "                                                                    \
    "\"||\" @operator ")                                                                  \
  X(POULPE_GLSL_PATTERN_DELIMITER, 3, primary_text, "delimiter",                          \
    "\".\" @delimiter "                                                                   \
    "\";\" @delimiter ")                                                                  \
  X(POULPE_GLSL_PATTERN_STRING, 4, string, "string",                                      \
    "(string_literal) @string "                                                           \
    "(system_lib_string) @string ")                                                       \
  X(POULPE_GLSL_PATTERN_NULL, 5, constant, "constant",                                    \
    "(null) @constant ")                                                                  \
  X(POULPE_GLSL_PATTERN_NUMBER, 6, number, "number",                                      \
    "(number_literal) @number "                                                           \
    "(char_literal) @number ")                                                            \
  X(POULPE_GLSL_PATTERN_FUNCTION, 7, function, "function",                                \
    "(call_expression function: (identifier) @function) "                                 \
    "(call_expression function: (field_expression field: (field_identifier) @function)) " \
    "(function_declarator declarator: (identifier) @function) ")                          \
  X(POULPE_GLSL_PATTERN_FUNCTION_SPECIAL, 8, function, "function.special",                \
    "(preproc_function_def name: (identifier) @function.special) ")                       \
  X(POULPE_GLSL_PATTERN_PROPERTY, 9, variable, "property",                                \
    "(field_identifier) @property ")                                                      \
  X(POULPE_GLSL_PATTERN_LABEL, 10, variable, "label",                                     \
    "(statement_identifier) @label ")                                                     \
  X(POULPE_GLSL_PATTERN_TYPE, 11, variable, "type",                                       \
    "(type_identifier) @type "                                                            \
    "(primitive_type) @type "                                                             \
    "(sized_type_specifier) @type ")                                                      \
  X(POULPE_GLSL_PATTERN_CONSTANT, 12, constant, "constant",                               \
    "((identifier) @constant (#match? @constant \"^[A-Z][A-Z\\d_]*$\")) ")                \
  X(POULPE_GLSL_PATTERN_VARIABLE, 13, variable, "variable",                               \
    "(identifier) @variable ")                                                            \
  X(POULPE_GLSL_PATTERN_COMMENT, 15, comment, "comment",                                  \
    "(comment) @comment ")                                                                \
  X(POULPE_GLSL_PATTERN_TYPE_QUALIFER, 16, keyword, "type.qualifier",                    \
    "[ \"in\" \"out\" \"inout\" \"uniform\" "                                             \
    "\"shared\" \"layout\" \"attribute\" \"varying\" "                                    \
    "\"buffer\" \"coherent\" \"readonly\" \"writeonly\" "                                 \
    "\"precision\" \"highp\" \"mediump\" \"lowp\" "                                       \
    "\"centroid\" \"sample\" \"patch\" \"smooth\" "                                       \
    "\"flat\" \"noperspective\" \"invariant\" \"precise\" ] @type.qualifier ")            \
  X(POULPE_GLSL_PATTERN_TYPE_KEYWORD_FUNCTION, 17, keyword, "keyword.function",           \
    "\"subroutine\" @keyword.function ")                                                  \
  X(POULPE_GLSL_PATTERN_TYPE_STORAGE_CLASS, 18, variable, "storageclass",                 \
    "(extension_storage_class) @storageclass ")                                           \
  X(POULPE_GLSL_PATTERN_TYPE_VARIABLE_BUILTIN, 19, variable, "variable.builtin",          \
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
    return igColorConvertFloat4ToU32(poulpe_theme_dark.__color);

  POULPE_GLSL_PATTERN

#undef X
  return 0;
}

#endif /* POULPE_LANGUAGES_GLSL_H */
