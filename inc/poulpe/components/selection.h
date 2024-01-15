#ifndef POULPE_COMPONENTS_SELECTION_H
#define POULPE_COMPONENTS_SELECTION_H

#include <cimgui.h>

#include <sake/string.h>

#include "poulpe/component.h"

#include "poulpe/error.h"

struct poulpe_textedit;

struct poulpe_selection_area
{
    ImVec2 start;
    ImVec2 end;
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
sake_string poulpe_selection_to_str(struct poulpe_selection *selection);
void poulpe_selection_move_start_right(struct poulpe_selection *selection);
void poulpe_selection_move_start_left(struct poulpe_selection *selection);
void poulpe_selection_move_start_up(struct poulpe_selection *selection);
void poulpe_selection_move_start_down(struct poulpe_selection *selection);
void poulpe_selection_move_end_right(struct poulpe_selection *selection);
void poulpe_selection_move_end_left(struct poulpe_selection *selection);
void poulpe_selection_move_end_up(struct poulpe_selection *selection);
void poulpe_selection_move_end_down(struct poulpe_selection *selection);
void poulpe_selection_select_all(struct poulpe_selection *selection);
void poulpe_selection_update(struct poulpe_selection *selection);

#endif /* POULPE_COMPONENTS_SELECTION_H */