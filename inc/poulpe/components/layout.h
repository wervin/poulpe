#ifndef POULPE_COMPONENTS_LAYOUT_H
#define POULPE_COMPONENTS_LAYOUT_H

#include "poulpe/component.h"

enum poulpe_layout_type
{
    POULPE_LAYOUT_TYPE_VBOX,
    POULPE_LAYOUT_TYPE_HBOX
};

struct poulpe_layout
{
    struct poulpe_component base;
    enum poulpe_layout_type type;
    struct poulpe_component **children;
};

enum poulpe_error poulpe_layout_init(struct poulpe_layout **layout);
enum poulpe_error poulpe_layout_add_child(struct poulpe_layout *parent, struct poulpe_component *child);
enum poulpe_error poulpe_layout_update(struct poulpe_layout *layout, vec2 upper_left, vec2 lower_right);
enum poulpe_error poulpe_layout_draw(struct poulpe_layout *layout);
void poulpe_layout_destroy(struct poulpe_layout *layout);

#endif /* POULPE_COMPONENTS_LAYOUT_H */