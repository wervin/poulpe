#ifndef POULPE_TABMENU_LAYOUT_H
#define POULPE_TABMENU_LAYOUT_H

#include "poulpe/component.h"

struct poulpe_tabmenu
{
    struct poulpe_component base;
};

enum poulpe_error poulpe_tabmenu_init(struct poulpe_tabmenu **tabmenu);

#endif /* POULPE_TABMENU_LAYOUT_H */