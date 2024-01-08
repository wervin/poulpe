#ifndef POULPE_COMPONENTS_SELECTION_H
#define POULPE_COMPONENTS_SELECTION_H

#include <cimgui.h>

#include "poulpe/component.h"

#include "poulpe/error.h"

struct poulpe_textedit;

struct poulpe_selection_area
{
    // uint32_t start_line_index;
    // uint32_t start_glyph_index;
    ImVec2 start;
    ImVec2 end;
    // uint32_t end_line_index;
    // uint32_t end_glyph_index;
};

struct poulpe_selection
{
    struct poulpe_component base;
    struct poulpe_textedit *textedit;
    struct poulpe_selection_area current;
    struct poulpe_selection_area ajusted;
};

void poulpe_selection_set_textedit(struct poulpe_selection *selection, struct poulpe_textedit *textedit);
bool poulpe_selection_active(struct poulpe_selection *selection);
enum poulpe_error poulpe_selection_delete(struct poulpe_selection *selection);
void poulpe_selection_clear(struct poulpe_selection *selection);
void poulpe_selection_update_start(struct poulpe_selection *selection, ImVec2 position);
void poulpe_selection_update_end(struct poulpe_selection *selection, ImVec2 position);

#endif /* POULPE_COMPONENTS_SELECTION_H */