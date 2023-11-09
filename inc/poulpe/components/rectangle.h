#ifndef POULPE_COMPONENTS_RECTANGLE_H
#define POULPE_COMPONENTS_RECTANGLE_H

#include "poulpe/component.h"

struct poulpe_rectangle
{
    struct poulpe_component base;
    vec4 color;
};

enum poulpe_error poulpe_rectangle_init(struct poulpe_rectangle **rectangle);

#endif /* POULPE_COMPONENTS_RECTANGLE_H */