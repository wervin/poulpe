#ifndef POULPE_COMPONENTS_INDENTINFO_H
#define POULPE_COMPONENTS_INDENTINFO_H

#include "poulpe/component.h"

struct poulpe_statusbar;

struct poulpe_indentinfo
{
    struct poulpe_component base;
    struct poulpe_statusbar *statusbar;
};

void poulpe_indentinfo_set_statusbar(struct poulpe_indentinfo *indentinfo, struct poulpe_statusbar *statusbar);

#endif /* POULPE_COMPONENTS_INDENTINFO_H */