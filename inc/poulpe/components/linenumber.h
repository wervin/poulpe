#ifndef POULPE_COMPONENTS_LINENUMBER_H
#define POULPE_COMPONENTS_LINENUMBER_H

#include "poulpe/component.h"

struct poulpe_textview;

struct poulpe_linenumber
{
    struct poulpe_component base;
    struct poulpe_textview *textview;
};

void poulpe_linenumber_set_textview(struct poulpe_linenumber *linenumber, struct poulpe_textview *textview);

#endif /* POULPE_COMPONENTS_LINENUMBER_H */