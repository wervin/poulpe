#ifndef POULPE_COMPONENTS_TEXTINFO_H
#define POULPE_COMPONENTS_TEXTINFO_H

#include "poulpe/component.h"

struct poulpe_statusbar;

struct poulpe_textinfo
{
    struct poulpe_component base;
    struct poulpe_statusbar *statusbar;
};

void poulpe_textinfo_set_statusbar(struct poulpe_textinfo *textinfo, struct poulpe_statusbar *statusbar);

#endif /* POULPE_COMPONENTS_TEXTINFO_H */