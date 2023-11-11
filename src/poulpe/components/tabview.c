#include <string.h>

#include "poulpe/components/tabview.h"
#include "poulpe/components/layout.h"
#include "poulpe/components/rectangle.h"
#include "poulpe/components/tabmenu.h"

#include "poulpe/theme.h"

#include "poulpe/log.h"

#include "sake/vector.h"

enum poulpe_error poulpe_tabview_init(struct poulpe_tabview **tabview)
{
    struct poulpe_tabview *tabview_;

    tabview_ = calloc(1, sizeof(struct poulpe_tabview));
    if (!tabview_)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate tabview");
        return POULPE_ERROR_MEMORY;
    }

    tabview_->layouts = sake_vector_new(sizeof(struct poulpe_layout*), (void (*) (void*)) poulpe_component_destroy);
    if (!tabview_->layouts)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate tabview layouts");
        return POULPE_ERROR_MEMORY;
    }

    tabview_->base.parent = NULL;
    tabview_->base.type = POULPE_COMPONENT_TYPE_TABVIEW;

    *tabview = tabview_;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabview_add_layout(struct poulpe_tabview *tabview)
{
    enum poulpe_error status = POULPE_ERROR_NONE;
    struct poulpe_layout *layout;
    struct poulpe_rectangle * bg;

    status = poulpe_layout_init(&layout);
    if (status != POULPE_ERROR_NONE)
        return status;
    layout->type = POULPE_LAYOUT_TYPE_VBOX;
    layout->base.fill_width = true;
    layout->base.fill_height = true;

    status = poulpe_rectangle_init(&bg);
    if (status != POULPE_ERROR_NONE)
        return status;
    bg->base.fill_width = true;
    bg->base.fill_height = true;
    bg->color[0] = (float) rand() / (float) RAND_MAX;
    bg->color[1] = (float) rand() / (float) RAND_MAX;
    bg->color[2] = (float) rand() / (float) RAND_MAX;
    bg->color[3] = 1.0f;

    poulpe_layout_add_child(layout, (struct poulpe_component *) bg);

    layout->base.parent = (struct poulpe_component *) tabview;
    tabview->layouts = sake_vector_push_back(tabview->layouts, &layout);
    if (!tabview->layouts)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot push back tabview layouts");
        return POULPE_ERROR_MEMORY;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabview_update(struct poulpe_tabview *tabview, vec2 upper_left, vec2 lower_right)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    for (uint32_t i = 0; i < sake_vector_size(tabview->layouts); i++)
    {
        error = poulpe_component_update((struct poulpe_component *) tabview->layouts[i], upper_left, lower_right);
        if (error != POULPE_ERROR_NONE)
            return error;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabview_draw(struct poulpe_tabview *tabview)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    for (uint32_t i = 0; i < sake_vector_size(tabview->layouts); i++)
    {
        if (tabview->tabmenu->current_index != i)
            continue;
        error = poulpe_component_draw((struct poulpe_component *) tabview->layouts[i]);
        if (error != POULPE_ERROR_NONE)
            return error;
    }

    return POULPE_ERROR_NONE;
}

void poulpe_tabview_destroy(struct poulpe_tabview *tabview)
{
    sake_vector_free(tabview->layouts);
    free(tabview);
}
