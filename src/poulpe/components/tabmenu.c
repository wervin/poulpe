#include <string.h>

#include "poulpe/components/tabmenu.h"
#include "poulpe/components/tabitem.h"

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_VULKAN
#include <cimgui_impl.h>

#include "poulpe/log.h"

#include "sake/macro.h"
#include "sake/vector.h"

enum poulpe_error poulpe_tabmenu_init(struct poulpe_tabmenu **tabmenu)
{
    struct poulpe_tabmenu *tabmenu_;

    tabmenu_ = calloc(1, sizeof(struct poulpe_tabmenu));
    if (!tabmenu_)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate tabmenu");
        return POULPE_ERROR_MEMORY;
    }

    tabmenu_->items = sake_vector_new(sizeof(struct poulpe_tabitem*), (void (*) (void*)) poulpe_component_destroy);
    if (!tabmenu_->items)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate tabmenu items");
        return POULPE_ERROR_MEMORY;
    }

    tabmenu_->base.parent = NULL;
    tabmenu_->base.type = POULPE_COMPONENT_TYPE_TABMENU;
    tabmenu_->current_index = 0;

    *tabmenu = tabmenu_;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabmenu_add_tabitem(struct poulpe_tabmenu *tabmenu, const char *filename)
{
    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_tabitem *tabitem;

    error = poulpe_tabitem_init(&tabitem);
    if (error != POULPE_ERROR_NONE)
        return error;
    tabitem->index = sake_vector_size(tabmenu->items);
    strncpy(tabitem->text, filename, POULPE_TABITEM_MAX_LENGTH);

    tabitem->base.parent = (struct poulpe_component *) tabmenu;
    tabmenu->items = sake_vector_push_back(tabmenu->items, &tabitem);
    if (!tabmenu->items)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot push back tabmenu items");
        return POULPE_ERROR_MEMORY;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabmenu_update(struct poulpe_tabmenu *tabmenu, vec2 upper_left, vec2 lower_right)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    for (uint32_t i = 0; i < sake_vector_size(tabmenu->items); i++)
    {
        error = poulpe_component_update((struct poulpe_component *) tabmenu->items[i], upper_left, lower_right);
        if (error != POULPE_ERROR_NONE)
            return error;
    }
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabmenu_draw(struct poulpe_tabmenu *tabmenu)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    for (uint32_t i = 0; i < sake_vector_size(tabmenu->items); i++)
    {
        error = poulpe_component_draw((struct poulpe_component *) tabmenu->items[i]);
        if (error != POULPE_ERROR_NONE)
            return error;
    }

    return POULPE_ERROR_NONE;
}

void poulpe_tabmenu_destroy(struct poulpe_tabmenu *tabmenu)
{
    sake_vector_free(tabmenu->items);
    free(tabmenu);
}
