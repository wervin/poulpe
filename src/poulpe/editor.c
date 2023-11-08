#include <cglm/cglm.h>

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
#include "poulpe/layout.h"
#include "poulpe/log.h"

#include "sake/vector.h"
#include "sake/string.h"

struct _editor
{
    uint32_t flags;
    struct poulpe_theme theme;
    sake_string *commands;

    struct poulpe_layout *main_layout;
    struct poulpe_layout *tab_layout;
    struct poulpe_layout *content_layout;
};

struct _editor _editor = {0};

static enum poulpe_error _init_layouts(void);
static void update_layouts(void);

enum poulpe_error poulpe_editor_init(void)
{    
    enum poulpe_error status = POULPE_ERROR_NONE;

    status = _init_layouts();
    if (status != POULPE_ERROR_NONE)
        return status;

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

    _editor.theme = poulpe_theme_light;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_editor_draw(void)
{        
    igBegin("Poulpe", NULL, ImGuiWindowFlags_NoScrollbar);

    update_layouts();
    
    poulpe_display_draw_rect_filled(_editor.main_layout->upper_left,
                                    _editor.main_layout->lower_right,
                                    _editor.main_layout->upper_left,
                                    _editor.main_layout->lower_right,
                                    _editor.theme.backgound);

    poulpe_display_draw_rect_filled(_editor.tab_layout->upper_left,
                                    _editor.tab_layout->lower_right,
                                    _editor.tab_layout->upper_left,
                                    _editor.tab_layout->lower_right,
                                    _editor.theme.flash_color);
    
    poulpe_display_draw_rect_filled(_editor.content_layout->upper_left,
                                    _editor.content_layout->lower_right,
                                    _editor.content_layout->upper_left,
                                    _editor.content_layout->lower_right,
                                    _editor.theme.warning);

    // ImVec2 position = {min.x + 2.0, min.y + 2.0};
    // for (uint32_t i = 0; i < sake_vector_size(_editor.commands); i++)
    // {
    //     poulpe_display_draw_chars((vec2){position.x, position.y},
    //                               _editor.theme.text,
    //                               _editor.commands[i],
    //                               NULL);
    //     position.y += igGetFontSize();
    // }

    // poulpe_display_draw_rect_filled((vec2){min.x, max.y - 2},
    //                                 (vec2){max.x, max.y},
    //                                 _editor.theme.tab_inactive);

    igEnd();

    return POULPE_ERROR_NONE;
}

void poulpe_editor_destroy(void)
{
    sake_vector_free(_editor.commands);
    poulpe_layout_destroy(_editor.main_layout);
}

static enum poulpe_error _init_layouts(void)
{
    enum poulpe_error status = POULPE_ERROR_NONE;
    
    status = poulpe_layout_init(&_editor.main_layout);
    if (status != POULPE_ERROR_NONE)
        return status;
    
    status = poulpe_layout_init(&_editor.tab_layout);
    if (status != POULPE_ERROR_NONE)
        return status;

    status = poulpe_layout_init(&_editor.content_layout);
    if (status != POULPE_ERROR_NONE)
        return status;

    poulpe_layout_add_child(_editor.main_layout, _editor.tab_layout);
    poulpe_layout_add_child(_editor.main_layout, _editor.content_layout);

    _editor.main_layout->layout_type = POULPE_LAYOUT_TYPE_VBOX;
    _editor.tab_layout->layout_type = POULPE_LAYOUT_TYPE_HBOX;
    _editor.tab_layout->fill_width = true;
    _editor.tab_layout->preferred_height = 50;
    _editor.content_layout->layout_type = POULPE_LAYOUT_TYPE_VBOX;
    _editor.content_layout->fill_width = true;
    _editor.content_layout->fill_height = true;

    return POULPE_ERROR_NONE;
}

static void update_layouts(void)
{
    ImVec2 min, max;

    igGetCursorScreenPos(&min);
    igGetContentRegionAvail(&max);
    max.x = min.x + fmax(max.x, 0.0f);
    max.y = min.y + fmax(max.y, 0.0f);

    poulpe_layout_update(_editor.main_layout, (vec2) {min.x, min.y}, (vec2) {max.x, max.y});
    poulpe_layout_adjust(_editor.main_layout);
}