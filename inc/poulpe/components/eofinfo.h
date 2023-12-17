#ifndef POULPE_COMPONENTS_EOFINFO_H
#define POULPE_COMPONENTS_EOFINFO_H

#include "poulpe/component.h"

struct poulpe_statusbar;

struct poulpe_eofinfo
{
    struct poulpe_component base;
    struct poulpe_statusbar *statusbar;
};

void poulpe_eofinfo_set_statusbar(struct poulpe_eofinfo *eofinfo, struct poulpe_statusbar *statusbar);

#endif /* POULPE_COMPONENTS_EOFINFO_H */