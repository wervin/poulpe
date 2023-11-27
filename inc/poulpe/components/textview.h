#ifndef POULPE_COMPONENTS_TEXTVIEW_H
#define POULPE_COMPONENTS_TEXTVIEW_H

#include "poulpe/component.h"

struct poulpe_linenumber;
struct poulpe_textedit;
struct poulpe_textbuffer;

struct poulpe_textview
{
    struct poulpe_component base;
    struct poulpe_textedit *textedit;
    struct poulpe_linenumber *linenumber;
};

void poulpe_textview_set_textbuffer(struct poulpe_textview *textview, struct poulpe_textbuffer *textbuffer);

#endif /* POULPE_COMPONENTS_TEXTVIEW_H */