#ifndef POULPE_COMPONENTS_TABVIEW_H
#define POULPE_COMPONENTS_TABVIEW_H

#include "poulpe/component.h"

struct poulpe_layout;

struct poulpe_tabview
{
    struct poulpe_component base;
    struct poulpe_tabmenu *tabmenu;
    struct poulpe_layout **layouts;
};

enum poulpe_error poulpe_tabview_init(struct poulpe_tabview **tabview);
enum poulpe_error poulpe_tabview_add_layout(struct poulpe_tabview *tabview);

#endif /* POULPE_COMPONENTS_TABVIEW_H */