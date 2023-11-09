#include <string.h>
#include <stdlib.h>

#include "poulpe/log.h"
#include "poulpe/display.h"

#include "poulpe/components/rectangle.h"

#include "sake/vector.h"

enum poulpe_error poulpe_rectangle_init(struct poulpe_rectangle **rectangle)
{
    struct poulpe_rectangle *rectangle_;

    rectangle_ = calloc(1, sizeof(struct poulpe_rectangle));
    if (!rectangle_)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate layout");
        return POULPE_ERROR_MEMORY;
    }

    rectangle_->base.parent = NULL;
    rectangle_->base.type = POULPE_COMPONENT_TYPE_RECTANGLE;

    *rectangle = rectangle_;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_rectangle_update(struct poulpe_rectangle *rectangle, vec2 upper_left, vec2 lower_right)
{
    rectangle->base.x = upper_left[0];
    rectangle->base.y = upper_left[1];
    rectangle->base.width = lower_right[0] - upper_left[0];
    rectangle->base.height = lower_right[1] - upper_left[1];
    memcpy(rectangle->base.upper_left, upper_left, sizeof(vec2));
    memcpy(rectangle->base.lower_right, lower_right, sizeof(vec2));

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_rectangle_draw(struct poulpe_rectangle *rectangle)
{
    poulpe_display_draw_rect_filled(rectangle->base.upper_left,
                                    rectangle->base.lower_right,
                                    rectangle->base.upper_left,
                                    rectangle->base.lower_right,
                                    rectangle->color);
    return POULPE_ERROR_NONE;
}

void poulpe_rectangle_destroy(struct poulpe_rectangle *rectangle)
{
    free(rectangle);
}