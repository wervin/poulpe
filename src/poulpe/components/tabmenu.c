#include "poulpe/components/tabmenu.h"

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

enum poulpe_error poulpe_tabmenu_init(struct poulpe_tabmenu **tabmenu)
{
    struct poulpe_tabmenu *tabmenu_;

    tabmenu_ = calloc(1, sizeof(struct poulpe_tabmenu));
    if (!tabmenu_)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate tabmenu");
        return POULPE_ERROR_MEMORY;
    }

    tabmenu_->base.parent = NULL;
    tabmenu_->base.type = POULPE_COMPONENT_TYPE_TABMENU;

    *tabmenu = tabmenu_;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabmenu_update(struct poulpe_tabmenu *tabmenu, vec2 upper_left, vec2 lower_right)
{
    SAKE_MACRO_UNUSED(tabmenu);
    SAKE_MACRO_UNUSED(upper_left);
    SAKE_MACRO_UNUSED(lower_right);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabmenu_draw(struct poulpe_tabmenu *tabmenu)
{
    SAKE_MACRO_UNUSED(tabmenu);
    igSameLine(0.0f, 0.0f);
    igButton("test", (ImVec2) {50, 30});
    igSameLine(0.0f, 0.0f);
    igButton("test", (ImVec2) {50, 30});
    return POULPE_ERROR_NONE;
}

void poulpe_tabmenu_destroy(struct poulpe_tabmenu *tabmenu)
{
    free(tabmenu);
}
