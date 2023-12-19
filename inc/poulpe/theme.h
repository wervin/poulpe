#ifndef POULPE_THEME_H
#define POULPE_THEME_H

#include <cimgui.h>


#define POULPE_THEMES \
    X(POULPE_THEME_DARK, 0, dark) \
    X(POULPE_THEME_LIGHT, 1, light)

enum poulpe_theme_type
{
#define X(__def, __id, __type) \
    __def = __id,

    POULPE_THEMES

#undef X
};

struct poulpe_theme
{
    ImVec4 main_background;
    ImVec4 secondary_background;
    ImVec4 border;
    ImVec4 hovered_border; 
    ImVec4 active_border;
    ImVec4 widget;
    ImVec4 hovered_widget; 
    ImVec4 active_widget;
    ImVec4 faint_text;
    ImVec4 muted_text;
    ImVec4 primary_text;
    ImVec4 secondary_text;
    ImVec4 highlighted_text;
    ImVec4 dimmed_text;
    ImVec4 error_text;
    ImVec4 warning_text;
    ImVec4 success_text;
    ImVec4 link;
    ImVec4 comment;
    ImVec4 operator;
    ImVec4 import;
    ImVec4 function;
    ImVec4 constant;
    ImVec4 keyword;
    ImVec4 string;
    ImVec4 variable;
    ImVec4 number;
    ImVec4 feature;
};

#endif /* POULPE_THEME_H */