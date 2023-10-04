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

void poulpe_display_draw_chars(ImVec2 position, ImVec4 color, const char *text_begin, const char *text_end)
{
    ImDrawList *draw_list = igGetWindowDrawList();
    ImDrawList_AddText_Vec2(draw_list, position, igColorConvertFloat4ToU32(color), text_begin, text_end);
}

void poulpe_display_draw_rect_filled(ImVec2 upper_left, ImVec2 lower_right, ImVec4 color)
{
    ImDrawList *draw_list = igGetWindowDrawList();
    ImDrawList_AddRectFilled(draw_list, upper_left, lower_right, igColorConvertFloat4ToU32(color), 0.0f, 0);
}

void poulpe_display_destroy(void)
{

}