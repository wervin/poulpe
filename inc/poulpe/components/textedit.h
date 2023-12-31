#ifndef POULPE_COMPONENTS_TEXTEDIT_H
#define POULPE_COMPONENTS_TEXTEDIT_H

#include "poulpe/component.h"

struct poulpe_cursor;
struct poulpe_selection;
struct poulpe_textview;

struct poulpe_textedit
{
    struct poulpe_component base;
    struct poulpe_cursor *cursor;
    struct poulpe_selection *selection;
    struct poulpe_textview *textview;
    uint32_t line_start;
    uint32_t line_end;
    float scroll_y;
};

void poulpe_textedit_set_textview(struct poulpe_textedit *textedit, struct poulpe_textview *textview);

#endif /* POULPE_COMPONENTS_TEXTEDIT_H */