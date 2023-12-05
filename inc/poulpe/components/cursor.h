#ifndef POULPE_COMPONENTS_CURSOR_H
#define POULPE_COMPONENTS_CURSOR_H

#include <sys/time.h>

#include "poulpe/component.h"

struct poulpe_textedit;

struct poulpe_cursor
{
    struct poulpe_component base;
    struct poulpe_textedit *textedit;
    ImVec2 position;
    struct timeval timer;
};

void poulpe_cursor_set_textedit(struct poulpe_cursor *cursor, struct poulpe_textedit *textedit);
void poulpe_cursor_reset(struct poulpe_cursor *cursor);
void poulpe_cursor_update(struct poulpe_cursor *cursor);
void poulpe_cursor_move_up(struct poulpe_cursor *cursor);
void poulpe_cursor_move_down(struct poulpe_cursor *cursor);
void poulpe_cursor_move_left(struct poulpe_cursor *cursor);
void poulpe_cursor_move_right(struct poulpe_cursor *cursor);
void poulpe_cursor_update_position(struct poulpe_cursor *cursor, ImVec2 position);

#endif /* POULPE_COMPONENTS_CURSOR_H */