#ifndef POULPE_COMPONENTS_INDENTINFO_H
#define POULPE_COMPONENTS_INDENTINFO_H

#include "poulpe/component.h"

#define POULPE_INDENTINFOS                                      \
    X(POULPE_INDENTINFO_SPACE_2, 0, "Spaces: 2", "  ", 2)       \
    X(POULPE_INDENTINFO_SPACE_4, 1, "Spaces: 4", "    ", 4)     \
    X(POULPE_INDENTINFO_SPACE_8, 2, "Spaces: 8", "        ", 8) \
    X(POULPE_INDENTINFO_TAB_2, 3, "Tabs: 1", "\t", 1)           \
    X(POULPE_INDENTINFO_TAB_4, 4, "Tabs: 2", "\t\t", 2)

enum poulpe_indentinfo_type
{
#define X(__def, __id, __label, __str, __n) \
    __def = __id,

    POULPE_INDENTINFOS

#undef X
};

struct poulpe_statusbar;

struct poulpe_indentinfo
{
    struct poulpe_component base;
    struct poulpe_statusbar *statusbar;
    enum poulpe_indentinfo_type current;
};

void poulpe_indentinfo_set_statusbar(struct poulpe_indentinfo *indentinfo, struct poulpe_statusbar *statusbar);
const char *poulpe_indentinfo_str(enum poulpe_indentinfo_type type);
uint32_t poulpe_indentinfo_length(enum poulpe_indentinfo_type type);

#endif /* POULPE_COMPONENTS_INDENTINFO_H */