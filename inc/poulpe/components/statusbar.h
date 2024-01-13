#ifndef POULPE_COMPONENTS_STATUSBAR_H
#define POULPE_COMPONENTS_STATUSBAR_H

#include "poulpe/component.h"

struct poulpe_editor;
struct poulpe_cursorinfo;
struct poulpe_indentinfo;
struct poulpe_textinfo;
struct poulpe_eofinfo;
struct poulpe_finder;

struct poulpe_statusbar
{
    struct poulpe_component base;
    struct poulpe_editor *editor;
    struct poulpe_cursorinfo *cursorinfo;
    struct poulpe_indentinfo *indentinfo;
    struct poulpe_textinfo *textinfo;
    struct poulpe_eofinfo *eofinfo;
    struct poulpe_finder *finder;
};

void poulpe_statusbar_set_editor(struct poulpe_statusbar *textview, struct poulpe_editor *editor);

#endif /* POULPE_COMPONENTS_STATUSBAR_H */