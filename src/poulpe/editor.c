#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_VULKAN
#include <cimgui_impl.h>

#include "poulpe/editor.h"
#include "poulpe/display.h"
#include "poulpe/theme.h"

#include "sake/vector.h"
#include "sake/string.h"

struct _editor
{
    uint32_t flags;
    struct poulpe_theme theme;
    sake_string *commands;
};

struct _editor _editor = {0};

enum poulpe_error poulpe_editor_init(void)
{
    _editor.theme = poulpe_theme_dark;

    _editor.commands = sake_vector_new(sizeof(sake_string), (void (*) (void*)) sake_string_free);


    {
        sake_string t = sake_string_new("sake_string 1");
        _editor.commands = sake_vector_push_back(_editor.commands, &t);
    }

    {
        sake_string t = sake_string_new("sake_string 2");
        _editor.commands = sake_vector_push_back(_editor.commands, &t);
    }

    {
        sake_string t = sake_string_new("sake_string 3");
        _editor.commands = sake_vector_push_back(_editor.commands, &t);
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_editor_draw(void)
{
    ImVec2 min, max;
    
    igBegin("Poulpe", NULL, 0);

    igGetCursorScreenPos(&min);
    igGetContentRegionAvail(&max);
    max.x = min.x + max.x;
    max.y = min.y + max.y;

    poulpe_display_draw_rect_filled(min, max, _editor.theme.backgound);
    
    poulpe_display_draw_chars((ImVec2) {min.x + 10, min.y + 10}, _editor.theme.text, _editor.commands[0], NULL);
    poulpe_display_draw_chars((ImVec2) {min.x + 20, min.y + 20}, _editor.theme.text, _editor.commands[1], NULL);
    poulpe_display_draw_chars((ImVec2) {min.x + 30, min.y + 30}, _editor.theme.text, _editor.commands[2], NULL);

    igEnd();

    return POULPE_ERROR_NONE;
}

void poulpe_editor_destroy(void)
{
    sake_vector_free(_editor.commands);
}