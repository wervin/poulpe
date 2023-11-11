#ifndef POULPE_COMPONENTS_TABITEM_H
#define POULPE_COMPONENTS_TABITEM_H

#include "poulpe/component.h"

#define POULPE_TABITEM_MAX_LENGTH   255

struct poulpe_tabitem
{
    struct poulpe_component base;
    uint32_t index;
    char text[POULPE_TABITEM_MAX_LENGTH + 1];
};

enum poulpe_error poulpe_tabitem_init(struct poulpe_tabitem **tabitem);

#endif /* POULPE_TABITEM_LAYOUT_H */