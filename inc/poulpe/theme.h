#ifndef POULPE_THEME_H
#define POULPE_THEME_H

#include <cglm/cglm.h>

struct poulpe_theme
{
    vec4 airline_backgound;
    vec4 backgound;
    vec4 comment;
    vec4 cursor_insert;
    vec4 cursor_line_background;
    vec4 cursor_normal;
    vec4 dark;
    vec4 error;
    vec4 flash_color;
    vec4 hidden_text;
    vec4 identifier;
    vec4 info;
    vec4 keyword;
    vec4 light;
    vec4 line_number;
    vec4 line_number_active;
    vec4 line_number_background;
    vec4 mode;
    vec4 normal;
    vec4 number;
    vec4 parenthesis;
    vec4 string;
    vec4 tab_active;
    vec4 tab_border;
    vec4 tab_inactive;
    vec4 text;
    vec4 text_dim;
    vec4 visual_select_background;
    vec4 warning;
    vec4 whitespace;
    vec4 widger_border;
    vec4 widget_active;
    vec4 widget_background;
    vec4 widget_inactive;
};

extern const struct poulpe_theme poulpe_theme_light;
extern const struct poulpe_theme poulpe_theme_dark;

#endif /* POULPE_THEME_H */