#include "poulpe/components/rectangle.h"

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

enum poulpe_error poulpe_rectangle_init(struct poulpe_rectangle **rectangle)
{
    struct poulpe_rectangle *rectangle_;

    rectangle_ = calloc(1, sizeof(struct poulpe_rectangle));
    if (!rectangle_)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate rectangle");
        return POULPE_ERROR_MEMORY;
    }

    rectangle_->base.parent = NULL;
    rectangle_->base.type = POULPE_COMPONENT_TYPE_RECTANGLE;

    *rectangle = rectangle_;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_rectangle_update(struct poulpe_rectangle *rectangle, vec2 upper_left, vec2 lower_right)
{
    SAKE_MACRO_UNUSED(rectangle);
    SAKE_MACRO_UNUSED(upper_left);
    SAKE_MACRO_UNUSED(lower_right);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_rectangle_draw(struct poulpe_rectangle *rectangle)
{
    ImDrawList *draw_list = igGetWindowDrawList();
    ImDrawList_PushClipRect(draw_list,
                            (ImVec2) {rectangle->base.upper_left[0], rectangle->base.upper_left[1]},
                            (ImVec2) {rectangle->base.lower_right[0], rectangle->base.lower_right[1]},
                            false);
    ImDrawList_AddRectFilled(draw_list,
                             (ImVec2){rectangle->base.upper_left[0], rectangle->base.upper_left[1]},
                             (ImVec2){rectangle->base.lower_right[0], rectangle->base.lower_right[1]},
                             igColorConvertFloat4ToU32((ImVec4){rectangle->color[0],
                                                                rectangle->color[1],
                                                                rectangle->color[2],
                                                                rectangle->color[3]}),
                             0.0f,
                             0);
    ImDrawList_PopClipRect(draw_list);

    return POULPE_ERROR_NONE;
}

void poulpe_rectangle_destroy(struct poulpe_rectangle *rectangle)
{
    free(rectangle);
}