#ifndef POULPE_STYLE_H
#define POULPE_STYLE_H

#include "poulpe/theme.h"

typedef struct ImFont ImFont;

struct poulpe_style_info
{
    ImFont *large_font;
    ImFont *small_font;
    enum poulpe_theme_type theme;
};

struct poulpe_style
{
    ImFont *large_font;
    ImFont *small_font;
    const struct poulpe_theme *theme;
};

void poulpe_style_set(struct poulpe_style_info *info);

extern struct poulpe_style poulpe_style;

#endif /* POULPE_STYLE_H */