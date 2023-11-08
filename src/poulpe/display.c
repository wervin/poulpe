#include <math.h>

#include <string.h>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_VULKAN
#include <cimgui_impl.h>

#include "poulpe/display.h"

enum poulpe_error poulpe_display_init(void)
{
    return POULPE_ERROR_NONE;
}

void poulpe_display_draw_chars(vec2 position, vec4 color, const char *text_begin, const char *text_end)
{
//     ImVec2 min, max;

//     igGetCursorScreenPos(&min);
//     igGetContentRegionAvail(&max);
//     max.x = fmax(max.x, 1.0f);
//     max.y = fmax(max.y, 1.0f);

//     max.x = min.x + max.x;
//     max.y = min.y + max.y;

    /* TODO improve clipping */
    ImDrawList *draw_list = igGetWindowDrawList();
    // ImDrawList_PushClipRect(draw_list, min, max, false);
    ImDrawList_AddText_Vec2(draw_list,
                            (ImVec2){position[0], position[1]},
                            igColorConvertFloat4ToU32((ImVec4){color[0], color[1], color[2], color[3]}),
                            text_begin,
                            text_end);
    // ImDrawList_PopClipRect(draw_list);
}

void poulpe_display_draw_rect_filled(vec2 upper_left,
                                     vec2 lower_right,
                                     vec2 upper_left_clipped,
                                     vec2 lower_right_clipped,
                                     vec4 color)
{
    ImDrawList *draw_list = igGetWindowDrawList();
    ImDrawList_PushClipRect(draw_list,
                            (ImVec2) {upper_left_clipped[0], upper_left_clipped[1]},
                            (ImVec2) {lower_right_clipped[0], lower_right_clipped[1]},
                            false);
    ImDrawList_AddRectFilled(draw_list,
                             (ImVec2){upper_left[0], upper_left[1]},
                             (ImVec2){lower_right[0], lower_right[1]},
                             igColorConvertFloat4ToU32((ImVec4){color[0], color[1], color[2], color[3]}),
                             0.0f,
                             0);
    ImDrawList_PopClipRect(draw_list);
}

void poulpe_display_destroy(void)
{

}