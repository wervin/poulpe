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
void poulpe_textedit_get_view(struct poulpe_textedit *textedit, uint32_t *start, uint32_t *end);
void poulpe_textedit_get_coordinates(struct poulpe_textedit *textedit, ImVec2 mouse_position, ImVec2 *coordinates);
void poulpe_textedit_ensure_cursor_visiblity(struct poulpe_textedit *textedit);

#endif /* POULPE_COMPONENTS_TEXTEDIT_H */