#ifndef POULPE_LAYOUT_H
#define POULPE_LAYOUT_H

#include <cglm/cglm.h>

#include "poulpe/error.h"

enum poulpe_layout_type
{
    POULPE_LAYOUT_TYPE_VBOX,
    POULPE_LAYOUT_TYPE_HBOX
};

struct poulpe_layout
{
    enum poulpe_layout_type layout_type;
    
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

    struct poulpe_layout *parent;
    struct poulpe_layout **children;
};

enum poulpe_error poulpe_layout_init(struct poulpe_layout **layout);
void poulpe_layout_update(struct poulpe_layout *layout, vec2 upper_left, vec2 lower_right);
void poulpe_layout_adjust(struct poulpe_layout *layout);
enum poulpe_error poulpe_layout_add_child(struct poulpe_layout *layout, struct poulpe_layout *child);
void poulpe_layout_destroy(struct poulpe_layout *layout);

#endif /* POULPE_LAYOUT_H */