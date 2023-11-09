#ifndef POULPE_TABVIEW_LAYOUT_H
#define POULPE_TABVIEW_LAYOUT_H

#include "poulpe/component.h"

struct poulpe_layout;

struct poulpe_tabview
{
    struct poulpe_component base;
    struct poulpe_layout *layout;
};

enum poulpe_error poulpe_tabview_init(struct poulpe_tabview **tabview);

#endif /* POULPE_TABVIEW_LAYOUT_H */