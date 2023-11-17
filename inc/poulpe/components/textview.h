#ifndef POULPE_COMPONENTS_TEXTVIEW_H
#define POULPE_COMPONENTS_TEXTVIEW_H

#include "poulpe/component.h"

#include "poulpe/text.h"

struct poulpe_cursor;

struct poulpe_textview
{
    struct poulpe_component base;
    struct poulpe_cursor *cursor;
    poulpe_text text;
};

void poulpe_textview_set_text(struct poulpe_textview *textview, poulpe_text text);

#endif /* POULPE_COMPONENTS_TEXTVIEW_H */