#ifndef POULPE_COMPONENTS_RECTANGLE_H
#define POULPE_COMPONENTS_RECTANGLE_H

#include "poulpe/component.h"

struct poulpe_rectangle
{
    struct poulpe_component base;
    vec4 color;
};

enum poulpe_error poulpe_rectangle_init(struct poulpe_rectangle **rectangle);
enum poulpe_error poulpe_rectangle_update(struct poulpe_rectangle *rectangle, vec2 upper_left, vec2 lower_right);
enum poulpe_error poulpe_rectangle_draw(struct poulpe_rectangle *rectangle);
void poulpe_rectangle_destroy(struct poulpe_rectangle *rectangle);

#endif /* POULPE_COMPONENTS_RECTANGLE_H */