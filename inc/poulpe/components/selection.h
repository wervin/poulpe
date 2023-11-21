#ifndef POULPE_COMPONENTS_SELECTION_H
#define POULPE_COMPONENTS_SELECTION_H

#include "poulpe/component.h"

struct poulpe_textview;

struct poulpe_selection
{
    struct poulpe_component base;
    struct poulpe_textview *textview;
    uint32_t start_line_index;
    uint32_t start_glyph_index;
    uint32_t end_line_index;
    uint32_t end_glyph_index;
};

void poulpe_selection_set_textview(struct poulpe_selection *selection, struct poulpe_textview *textview);
void poulpe_selection_update_start(struct poulpe_selection *selection, ImVec2 position);
void poulpe_selection_update_end(struct poulpe_selection *selection, ImVec2 position);

#endif /* POULPE_COMPONENTS_SELECTION_H */