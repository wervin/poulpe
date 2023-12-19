#include "poulpe/style.h"

#define X(__def, __id, __type) \
    extern const struct poulpe_theme poulpe_theme_##__type;

    POULPE_THEMES

#undef X

struct poulpe_style poulpe_style = {0};

void poulpe_style_set(struct poulpe_style_info *info)
{
    poulpe_style = (struct poulpe_style){0};

    poulpe_style.icon_font = info->icon_font;
    poulpe_style.large_font = info->large_font;
    poulpe_style.small_font = info->small_font;
    
    switch (info->theme)
    {
#define X(__def, __id, __type)                       \
    case __id:                                       \
        poulpe_style.theme = &poulpe_theme_##__type; \
        break;

        POULPE_THEMES

#undef X

    default:
        poulpe_style.theme = &poulpe_theme_dark;
    }
}