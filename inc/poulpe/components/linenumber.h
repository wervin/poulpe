#ifndef POULPE_COMPONENTS_LINENUMBER_H
#define POULPE_COMPONENTS_LINENUMBER_H

#include "poulpe/component.h"

struct poulpe_textedit;

struct poulpe_linenumber
{
    struct poulpe_component base;
    struct poulpe_textedit *textedit;
};

void poulpe_linenumber_set_textedit(struct poulpe_linenumber *linenumber, struct poulpe_textedit *textedit);

#endif /* POULPE_COMPONENTS_LINENUMBER_H */