#ifndef POULPE_COMPONENTS_TEXTVIEW_H
#define POULPE_COMPONENTS_TEXTVIEW_H

#include <cimgui.h>

#include "poulpe/component.h"
#include "poulpe/textbuffer.h"

struct poulpe_cursor;
struct poulpe_selection;

struct poulpe_textview
{
    struct poulpe_component base;
    struct poulpe_cursor *cursor;
    struct poulpe_textbuffer *textbuffer;
    struct poulpe_selection *selection;
};

void poulpe_textview_set_textbuffer(struct poulpe_textview *textview, struct poulpe_textbuffer *textbuffer);
void poulpe_textview_get_coordinates(struct poulpe_textview *textview, ImVec2 mouse_position, ImVec2 *coordinates);
float poulpe_textview_get_line_number_width(struct poulpe_textview *textview);

#endif /* POULPE_COMPONENTS_TEXTVIEW_H */