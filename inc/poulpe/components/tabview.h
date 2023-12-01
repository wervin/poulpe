#ifndef POULPE_COMPONENTS_TABVIEW_H
#define POULPE_COMPONENTS_TABVIEW_H

#include "poulpe/component.h"

#include "poulpe/error.h"

struct poulpe_textview;

struct poulpe_tabview
{
    struct poulpe_component base;
    struct poulpe_textview **textviews;
};

enum poulpe_error poulpe_tabview_open_file(struct poulpe_tabview *tabview, const char *path);

#endif /* POULPE_COMPONENTS_TABVIEW_H */