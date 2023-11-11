#ifndef POULPE_COMPONENTS_TABMENU_H
#define POULPE_COMPONENTS_TABMENU_H

#include "poulpe/component.h"

struct poulpe_tabitem;

struct poulpe_tabmenu
{
    struct poulpe_component base;
    uint32_t current_index;
    struct poulpe_tabitem **items;
};

enum poulpe_error poulpe_tabmenu_init(struct poulpe_tabmenu **tabmenu);
enum poulpe_error poulpe_tabmenu_add_tabitem(struct poulpe_tabmenu *tabmenu, const char *filename);


#endif /* POULPE_TABMENU_LAYOUT_H */