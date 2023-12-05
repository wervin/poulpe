#ifndef POULPE_COMPONENT_H
#define POULPE_COMPONENT_H

#include <stdbool.h>

#include <cimgui.h>

#include "poulpe/error.h"
#include "poulpe/event.h"

#define POULPE_COMPONENTS                              \
    X(POULPE_COMPONENT_TYPE_TEXTVIEW, 1, textview)     \
    X(POULPE_COMPONENT_TYPE_CURSOR, 2, cursor)         \
    X(POULPE_COMPONENT_TYPE_SELECTION, 3, selection)   \
    X(POULPE_COMPONENT_TYPE_LINENUMBER, 4, linenumber) \
    X(POULPE_COMPONENT_TYPE_TEXTEDIT, 5, textedit)     \
    X(POULPE_COMPONENT_TYPE_STATUSBAR, 6, statusbar)   \
    X(POULPE_COMPONENT_TYPE_CURSORINFO, 7, cursorinfo) \
    X(POULPE_COMPONENT_TYPE_INDENTINFO, 8, indentinfo) \
    X(POULPE_COMPONENT_TYPE_TEXTINFO, 9, textinfo)

enum poulpe_component_type
{
#define X(__def, __id, __type) \
    __def = __id,

    POULPE_COMPONENTS

#undef X
};

struct poulpe_component
{
    enum poulpe_component_type type;
    struct poulpe_component *parent;
};

struct poulpe_component * poulpe_component_new(enum poulpe_component_type type);
enum poulpe_error poulpe_component_draw(struct poulpe_component *component);
enum poulpe_error poulpe_component_notify(struct poulpe_component *component, struct poulpe_event *event);
void poulpe_component_free(struct poulpe_component *component);

#endif /* POULPE_COMPONENT_H */