#ifndef POULPE_THEME_H
#define POULPE_THEME_H

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

struct poulpe_theme
{
    ImVec4 airline_backgound;
    ImVec4 backgound;
    ImVec4 comment;
    ImVec4 cursor_insert;
    ImVec4 cursor_line_background;
    ImVec4 cursor_normal;
    ImVec4 dark;
    ImVec4 error;
    ImVec4 flash_color;
    ImVec4 hidden_text;
    ImVec4 identifier;
    ImVec4 info;
    ImVec4 keyword;
    ImVec4 light;
    ImVec4 line_number;
    ImVec4 line_number_active;
    ImVec4 line_number_background;
    ImVec4 mode;
    ImVec4 normal;
    ImVec4 number;
    ImVec4 parenthesis;
    ImVec4 string;
    ImVec4 tab_active;
    ImVec4 tab_border;
    ImVec4 tab_inactive;
    ImVec4 text;
    ImVec4 text_dim;
    ImVec4 visual_select_background;
    ImVec4 warning;
    ImVec4 whitespace;
    ImVec4 widger_border;
    ImVec4 widget_active;
    ImVec4 widget_background;
    ImVec4 widget_inactive;
};

extern const struct poulpe_theme poulpe_theme_light;
extern const struct poulpe_theme poulpe_theme_dark;

#endif /* POULPE_THEME_H */