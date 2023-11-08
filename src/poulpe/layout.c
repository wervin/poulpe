#include <string.h>

#include "poulpe/layout.h"
#include "poulpe/log.h"

#include "sake/vector.h"

enum poulpe_error poulpe_layout_init(struct poulpe_layout **layout)
{
    struct poulpe_layout *layout_;

    layout_ = calloc(1, sizeof(struct poulpe_layout));
    if (!layout_)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate layout");
        return POULPE_ERROR_MEMORY;
    }

    layout_->children = sake_vector_new(sizeof(struct poulpe_layout*), (void (*) (void*)) poulpe_layout_destroy);
    if (!layout_->children)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate layout children");
        return POULPE_ERROR_MEMORY;
    }

    layout_->parent = NULL;

    *layout = layout_;

    return POULPE_ERROR_NONE;
}

void poulpe_layout_update(struct poulpe_layout *layout, vec2 upper_left, vec2 lower_right)
{
    layout->x = upper_left[0];
    layout->y = upper_left[1];
    layout->width = lower_right[0] - upper_left[0];
    layout->height = lower_right[1] - upper_left[1];
    memcpy(layout->upper_left, upper_left, sizeof(vec2));
    memcpy(layout->lower_right, lower_right, sizeof(vec2));
}

void poulpe_layout_adjust(struct poulpe_layout *layout)
{
    uint32_t filled_width_children_count = 0;
    uint32_t filled_height_children_count = 0;
    float width_remaining = layout->width;
    float height_remaining = layout->height;

    if (!sake_vector_size(layout->children))
        return;

    for (uint32_t i = 0; i < sake_vector_size(layout->children); i++)
    {
        if (layout->children[i]->preferred_width && layout->layout_type == POULPE_LAYOUT_TYPE_HBOX)
            width_remaining -= layout->children[i]->preferred_width > layout->width ? layout->width : layout->children[i]->preferred_width;
        else
            filled_width_children_count++;


        if (layout->children[i]->preferred_height && layout->layout_type == POULPE_LAYOUT_TYPE_VBOX)
            height_remaining -= layout->children[i]->preferred_height > layout->height ? layout->height : layout->children[i]->preferred_height;
        else
            filled_height_children_count++;
    }

    uint32_t filled_width = width_remaining;
    uint32_t filled_width_remaining = 0;
    uint32_t filled_height = height_remaining;
    uint32_t filled_height_remaining = 0;
    if (filled_width_children_count != 0 && layout->layout_type == POULPE_LAYOUT_TYPE_HBOX)
    {
        filled_width_remaining = (uint32_t) filled_width % filled_width_children_count;
        filled_width = (uint32_t) filled_width / filled_width_children_count;
    }

    if (filled_height_children_count != 0 && layout->layout_type == POULPE_LAYOUT_TYPE_VBOX)
    {
        filled_height_remaining = (uint32_t) filled_height % filled_height_children_count;
        filled_height = (uint32_t) filled_height / filled_height_children_count;
    }

    vec2 upper_left = {layout->x, layout->y};
    for (uint32_t i = 0; i < sake_vector_size(layout->children); i++)
    {
        vec2 lower_right = {upper_left[0], upper_left[1]};
        if (layout->children[i]->preferred_width && layout->layout_type == POULPE_LAYOUT_TYPE_HBOX)
            lower_right[0] += layout->children[i]->preferred_width > layout->width ? layout->width : layout->children[i]->preferred_width;
        else
        {
            lower_right[0] += i == 0 ? filled_width_remaining : 0;
            lower_right[0] += (float) filled_width;
        }

        if (layout->children[i]->preferred_height && layout->layout_type == POULPE_LAYOUT_TYPE_VBOX)
            lower_right[1] += layout->children[i]->preferred_height > layout->height ? layout->height : layout->children[i]->preferred_height;
        else
        {
            lower_right[1] += i == 0 ? filled_height_remaining : 0;
            lower_right[1] += (float)filled_height;
        }

        poulpe_layout_update(layout->children[i], upper_left, lower_right);
        poulpe_layout_adjust(layout->children[i]);

        if (layout->layout_type == POULPE_LAYOUT_TYPE_HBOX)
            upper_left[0] = lower_right[0];
        else
            upper_left[1] = lower_right[1];
    }
}

enum poulpe_error poulpe_layout_add_child(struct poulpe_layout *layout, struct poulpe_layout *child)
{
    child->parent = layout;
    layout->children = sake_vector_push_back(layout->children, &child);
    if (!layout->children)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate layout children");
        return POULPE_ERROR_MEMORY;
    }

    return POULPE_ERROR_NONE; 
}

void poulpe_layout_destroy(struct poulpe_layout *layout)
{
    sake_vector_free(layout->children);
    free(layout);
}