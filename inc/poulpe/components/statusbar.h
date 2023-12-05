#ifndef POULPE_COMPONENTS_STATUSBAR_H
#define POULPE_COMPONENTS_STATUSBAR_H

#include "poulpe/component.h"

struct poulpe_editor;
struct poulpe_cursorinfo;
struct poulpe_indentinfo;
struct poulpe_textinfo;

struct poulpe_statusbar
{
    struct poulpe_component base;
    struct poulpe_editor *editor;
    struct poulpe_cursorinfo *cursorinfo;
    struct poulpe_indentinfo *indentinfo;
    struct poulpe_textinfo *textinfo;
};

void poulpe_statusbar_set_editor(struct poulpe_statusbar *textview, struct poulpe_editor *editor);

#endif /* POULPE_COMPONENTS_STATUSBAR_H */