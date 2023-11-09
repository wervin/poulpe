#ifndef POULPE_COMPONENT_H
#define POULPE_COMPONENT_H

#include <stdbool.h>

#include <cglm/cglm.h>

#include "poulpe/error.h"

#define POULPE_COMPONENTS                            \
    X(POULPE_COMPONENT_TYPE_LAYOUT, 0, layout)       \
    X(POULPE_COMPONENT_TYPE_RECTANGLE, 1, rectangle) \
    X(POULPE_COMPONENT_TYPE_TABMENU, 2, tabmenu)     \
    X(POULPE_COMPONENT_TYPE_TABVIEW, 3, tabview)

enum poulpe_component_type 
{
#define X(def, id, type) def=id,

    POULPE_COMPONENTS

#undef X
};

struct poulpe_component
{
    enum poulpe_component_type type;

    float x;
    float y;
    float width;
    float height;
    float preferred_width;
    float preferred_height;
    bool fill_width;
    bool fill_height;
    
    vec2 upper_left;
    vec2 lower_right;

    vec4 padding;
    vec4 margin;

    struct poulpe_component *parent;
};

enum poulpe_error poulpe_component_update(struct poulpe_component *component, vec2 upper_left, vec2 lower_right);
enum poulpe_error poulpe_component_draw(struct poulpe_component *component);
void poulpe_component_destroy(struct poulpe_component *component);

#endif /* POULPE_COMPONENT_H */