#include "poulpe/log.h"

#include "poulpe/components/layout.h"

#include "sake/vector.h"
#include "sake/macro.h"

static enum poulpe_error _adjust(struct poulpe_layout *layout);

enum poulpe_error poulpe_layout_init(struct poulpe_layout **layout)
{
    struct poulpe_layout *layout_;

    layout_ = calloc(1, sizeof(struct poulpe_layout));
    if (!layout_)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate layout");
        return POULPE_ERROR_MEMORY;
    }

    layout_->children = sake_vector_new(sizeof(struct poulpe_component*), (void (*) (void*)) poulpe_component_destroy);
    if (!layout_->children)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate layout children");
        return POULPE_ERROR_MEMORY;
    }

    layout_->base.parent = NULL;
    layout_->base.type = POULPE_COMPONENT_TYPE_LAYOUT;

    *layout = layout_;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_layout_add_child(struct poulpe_layout *parent, struct poulpe_component *child)
{
    child->parent = (struct poulpe_component *) parent;
    parent->children = sake_vector_push_back(parent->children, &child);
    if (!parent->children)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot push back layout children");
        return POULPE_ERROR_MEMORY;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_layout_update(struct poulpe_layout *layout, vec2 upper_left, vec2 lower_right)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    SAKE_MACRO_UNUSED(upper_left);
    SAKE_MACRO_UNUSED(lower_right);

    error = _adjust(layout);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_layout_draw(struct poulpe_layout *layout)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    for (uint32_t i = 0; i < sake_vector_size(layout->children); i++)
    {
        error = poulpe_component_draw((struct poulpe_component *) layout->children[i]);
        if (error != POULPE_ERROR_NONE)
            return error;
    }

    return POULPE_ERROR_NONE;
}

void poulpe_layout_destroy(struct poulpe_layout *layout)
{
    sake_vector_free(layout->children);
    free(layout);
}

static enum poulpe_error _adjust(struct poulpe_layout *layout)
{
    enum poulpe_error error = POULPE_ERROR_NONE;
    uint32_t filled_width_children_count = 0;
    uint32_t filled_height_children_count = 0;
    float width_remaining = layout->base.width;
    float height_remaining = layout->base.height;

    if (!sake_vector_size(layout->children))
        return POULPE_ERROR_NONE;

    for (uint32_t i = 0; i < sake_vector_size(layout->children); i++)
    {
        if (layout->children[i]->preferred_width && layout->type == POULPE_LAYOUT_TYPE_HBOX)
            width_remaining -= layout->children[i]->preferred_width > layout->base.width ? layout->base.width : layout->children[i]->preferred_width;
        else
            filled_width_children_count++;


        if (layout->children[i]->preferred_height && layout->type == POULPE_LAYOUT_TYPE_VBOX)
            height_remaining -= layout->children[i]->preferred_height > layout->base.height ? layout->base.height : layout->children[i]->preferred_height;
        else
            filled_height_children_count++;
    }

    uint32_t filled_width = width_remaining;
    uint32_t filled_width_remaining = 0;
    uint32_t filled_height = height_remaining;
    uint32_t filled_height_remaining = 0;
    if (filled_width_children_count != 0 && layout->type == POULPE_LAYOUT_TYPE_HBOX)
    {
        filled_width_remaining = (uint32_t) filled_width % filled_width_children_count;
        filled_width = (uint32_t) filled_width / filled_width_children_count;
    }

    if (filled_height_children_count != 0 && layout->type == POULPE_LAYOUT_TYPE_VBOX)
    {
        filled_height_remaining = (uint32_t) filled_height % filled_height_children_count;
        filled_height = (uint32_t) filled_height / filled_height_children_count;
    }

    vec2 upper_left = {layout->base.x, layout->base.y};
    for (uint32_t i = 0; i < sake_vector_size(layout->children); i++)
    {
        vec2 lower_right = {upper_left[0], upper_left[1]};
        if (layout->children[i]->preferred_width && layout->type == POULPE_LAYOUT_TYPE_HBOX)
            lower_right[0] += layout->children[i]->preferred_width > layout->base.width ? layout->base.width : layout->children[i]->preferred_width;
        else
        {
            lower_right[0] += i == 0 ? filled_width_remaining : 0;
            lower_right[0] += (float) filled_width;
        }

        if (layout->children[i]->preferred_height && layout->type == POULPE_LAYOUT_TYPE_VBOX)
            lower_right[1] += layout->children[i]->preferred_height > layout->base.height ? layout->base.height : layout->children[i]->preferred_height;
        else
        {
            lower_right[1] += i == 0 ? filled_height_remaining : 0;
            lower_right[1] += (float)filled_height;
        }

        error = poulpe_component_update((struct poulpe_component *) layout->children[i], upper_left, lower_right);
        if (error != POULPE_ERROR_NONE)
            return error;

        if (layout->type == POULPE_LAYOUT_TYPE_HBOX)
            upper_left[0] = lower_right[0];
        else
            upper_left[1] = lower_right[1];
    }

    return POULPE_ERROR_NONE;
}