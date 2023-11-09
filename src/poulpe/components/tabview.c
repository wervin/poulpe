#include <string.h>

#include "poulpe/components/tabview.h"
#include "poulpe/components/layout.h"
#include "poulpe/components/rectangle.h"

#include "poulpe/theme.h"

#include "poulpe/log.h"

enum poulpe_error poulpe_tabview_init(struct poulpe_tabview **tabview)
{
    enum poulpe_error status = POULPE_ERROR_NONE;
    struct poulpe_tabview *tabview_;
    struct poulpe_rectangle * bg;

    tabview_ = calloc(1, sizeof(struct poulpe_tabview));
    if (!tabview_)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate tabview");
        return POULPE_ERROR_MEMORY;
    }

    status = poulpe_layout_init(&tabview_->layout);
    if (status != POULPE_ERROR_NONE)
        return status;
    tabview_->layout->type = POULPE_LAYOUT_TYPE_VBOX;
    tabview_->layout->base.fill_width = true;
    tabview_->layout->base.fill_height = true;

    status = poulpe_rectangle_init(&bg);
    if (status != POULPE_ERROR_NONE)
        return status;
    bg->base.fill_width = true;
    bg->base.fill_height = true;
    memcpy(bg->color, poulpe_theme_light.flash_color, sizeof(vec4));

    poulpe_layout_add_child(tabview_->layout, (struct poulpe_component *) bg);

    tabview_->base.parent = NULL;
    tabview_->base.type = POULPE_COMPONENT_TYPE_TABVIEW;

    *tabview = tabview_;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabview_update(struct poulpe_tabview *tabview, vec2 upper_left, vec2 lower_right)
{
    return poulpe_component_update((struct poulpe_component *) tabview->layout, upper_left, lower_right);
}

enum poulpe_error poulpe_tabview_draw(struct poulpe_tabview *tabview)
{
    return poulpe_component_draw((struct poulpe_component *) tabview->layout);
}

void poulpe_tabview_destroy(struct poulpe_tabview *tabview)
{
    poulpe_component_destroy((struct poulpe_component *) tabview->layout);
    free(tabview);
}
