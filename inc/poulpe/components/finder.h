#ifndef POULPE_FINDER_H
#define POULPE_FINDER_H

#include <cimgui.h>

#include <sake/string.h>

#include "poulpe/component.h"

struct poulpe_statusbar;

struct poulpe_finder
{
    struct poulpe_component base;
    struct poulpe_statusbar *statusbar;
    char find[64];
    char replace[64];
    bool case_sensitive;
    ImVec2 *results;
    uint32_t result_index;
    bool focus;
};

void poulpe_finder_set_statusbar(struct poulpe_finder *finder, struct poulpe_statusbar *statusbar);

#endif /* POULPE_FINDER_H */