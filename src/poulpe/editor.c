#include <string.h>

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
#include "poulpe/log.h"

#include "poulpe/component.h"
#include "poulpe/components/layout.h"
#include "poulpe/components/rectangle.h"

#include "sake/vector.h"
#include "sake/string.h"

struct _editor
{
    uint32_t flags;
    struct poulpe_theme theme;

    struct poulpe_layout *main_layout;
    struct poulpe_layout *tab_layout;
    struct poulpe_layout *content_layout;
};

struct _editor _editor = {0};

static enum poulpe_error _init_main_layout(void);
static enum poulpe_error _init_tab_layout(void);
static enum poulpe_error _init_content_layout(void);

enum poulpe_error poulpe_editor_init(void)
{    
    enum poulpe_error status = POULPE_ERROR_NONE;

    _editor.theme = poulpe_theme_light;

    status = _init_main_layout();
    if (status != POULPE_ERROR_NONE)
        return status;


    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_editor_draw(void)
{        
    ImVec2 min, max;

    igBegin("Poulpe", NULL, ImGuiWindowFlags_NoScrollbar);

    igGetCursorScreenPos(&min);
    igGetContentRegionAvail(&max);
    max.x = min.x + fmax(max.x, 0.0f);
    max.y = min.y + fmax(max.y, 0.0f);

    poulpe_component_update((struct poulpe_component *) _editor.main_layout, (vec2) {min.x, min.y}, (vec2) {max.x, max.y});
    poulpe_component_draw((struct poulpe_component *) _editor.main_layout);

    igEnd();

    return POULPE_ERROR_NONE;
}

void poulpe_editor_destroy(void)
{
    poulpe_layout_destroy(_editor.main_layout);
}

static enum poulpe_error _init_main_layout(void)
{
    enum poulpe_error status = POULPE_ERROR_NONE;
    
    status = poulpe_layout_init(&_editor.main_layout);
    if (status != POULPE_ERROR_NONE)
        return status;
    _editor.main_layout->type = POULPE_LAYOUT_TYPE_VBOX;
    
    status = _init_tab_layout();
    if (status != POULPE_ERROR_NONE)
        return status;

    status = _init_content_layout();
    if (status != POULPE_ERROR_NONE)
        return status;

    poulpe_layout_add_child(_editor.main_layout, (struct poulpe_component *) _editor.tab_layout);
    poulpe_layout_add_child(_editor.main_layout, (struct poulpe_component *) _editor.content_layout);

    return POULPE_ERROR_NONE;
}

static enum poulpe_error _init_tab_layout(void)
{
    enum poulpe_error status = POULPE_ERROR_NONE;
    struct poulpe_rectangle * bg1;
    struct poulpe_rectangle * bg2;

    status = poulpe_layout_init(&_editor.tab_layout);
    if (status != POULPE_ERROR_NONE)
        return status;
    _editor.tab_layout->type = POULPE_LAYOUT_TYPE_HBOX;
    _editor.tab_layout->base.fill_width = true;
    _editor.tab_layout->base.preferred_height = 50;

    status = poulpe_rectangle_init(&bg1);
    if (status != POULPE_ERROR_NONE)
        return status;
    bg1->base.fill_width = true;
    bg1->base.fill_height = true;
    memcpy(bg1->color, _editor.theme.warning, sizeof(vec4));

    status = poulpe_rectangle_init(&bg2);
    if (status != POULPE_ERROR_NONE)
        return status;
    bg2->base.fill_width = true;
    bg2->base.fill_height = true;
    memcpy(bg2->color, _editor.theme.string, sizeof(vec4));

    poulpe_layout_add_child(_editor.tab_layout, (struct poulpe_component *) bg1);
    poulpe_layout_add_child(_editor.tab_layout, (struct poulpe_component *) bg2);

    return POULPE_ERROR_NONE;
}

static enum poulpe_error _init_content_layout(void)
{
    enum poulpe_error status = POULPE_ERROR_NONE;
    struct poulpe_rectangle * bg;

    status = poulpe_layout_init(&_editor.content_layout);
    if (status != POULPE_ERROR_NONE)
        return status;
    _editor.content_layout->type = POULPE_LAYOUT_TYPE_VBOX;
    _editor.content_layout->base.fill_width = true;
    _editor.content_layout->base.fill_height = true;

    status = poulpe_rectangle_init(&bg);
    if (status != POULPE_ERROR_NONE)
        return status;
    bg->base.fill_width = true;
    bg->base.fill_height = true;
    memcpy(bg->color, _editor.theme.flash_color, sizeof(vec4));

    poulpe_layout_add_child(_editor.content_layout, (struct poulpe_component *) bg);

    return POULPE_ERROR_NONE;
}