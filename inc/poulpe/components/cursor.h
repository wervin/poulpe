#ifndef POULPE_COMPONENTS_CURSOR_H
#define POULPE_COMPONENTS_CURSOR_H

#include <sys/time.h>

#include "poulpe/component.h"

struct poulpe_textview;

struct poulpe_cursor
{
    struct poulpe_component base;
    struct poulpe_textview *textview;
    uint32_t line_index;
    uint32_t glyph_index;
    struct timeval timer;
};

void poulpe_cursor_set_textview(struct poulpe_cursor *cursor, struct poulpe_textview *textview);
void poulpe_cursor_reset(struct poulpe_cursor *cursor);
void poulpe_cursor_move_up(struct poulpe_cursor *cursor);
void poulpe_cursor_move_down(struct poulpe_cursor *cursor);
void poulpe_cursor_move_left(struct poulpe_cursor *cursor);
void poulpe_cursor_move_right(struct poulpe_cursor *cursor);
void poulpe_cursor_update_position(struct poulpe_cursor *cursor, ImVec2 position);

#endif /* POULPE_COMPONENTS_CURSOR_H */