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
#include "poulpe/log.h"

#include "poulpe/component.h"
#include "poulpe/components/layout.h"
#include "poulpe/components/tabmenu.h"
#include "poulpe/components/tabview.h"
#include "poulpe/components/tabitem.h"

#include "sake/vector.h"
#include "sake/string.h"
#include "sake/macro.h"

struct _editor
{
    struct poulpe_layout *main_layout;
    struct poulpe_tabmenu *tabmenu;
    struct poulpe_tabview *tabview;
};

struct _editor _editor = {0};

static enum poulpe_error _init_main_layout(void);

enum poulpe_error poulpe_editor_init(void)
{    
    enum poulpe_error status = POULPE_ERROR_NONE;

    status = _init_main_layout();
    if (status != POULPE_ERROR_NONE)
        return status;

    status = poulpe_editor_open_file("Untitled");
    if (status != POULPE_ERROR_NONE)
        return status;

    status = poulpe_editor_open_file("test.c");
    if (status != POULPE_ERROR_NONE)
        return status;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_editor_draw(void)
{        
    ImVec2 min, max;

    if (!igBegin("Poulpe", NULL, 0))
    {
        igEnd();
        return POULPE_ERROR_NONE;
    }

    igGetCursorScreenPos(&min);
    igGetContentRegionAvail(&max);
    max.x = min.x + fmax(max.x, 0.0f);
    max.y = min.y + fmax(max.y, 0.0f);

    poulpe_component_update((struct poulpe_component *) _editor.main_layout, (vec2) {min.x, min.y}, (vec2) {max.x, max.y});
    poulpe_component_draw((struct poulpe_component *) _editor.main_layout);

    igEnd();

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_editor_open_file(const char *filename)
{
    enum poulpe_error status = POULPE_ERROR_NONE;
   
    status = poulpe_tabmenu_add_tabitem(_editor.tabmenu, filename);
    if (status != POULPE_ERROR_NONE)
        return status;

    status = poulpe_tabview_add_layout(_editor.tabview);
    if (status != POULPE_ERROR_NONE)
        return status;

    return POULPE_ERROR_NONE;
}

void poulpe_editor_destroy(void)
{
    poulpe_component_destroy((struct poulpe_component *) _editor.main_layout);
}

static enum poulpe_error _init_main_layout(void)
{
    enum poulpe_error status = POULPE_ERROR_NONE;
    
    status = poulpe_layout_init(&_editor.main_layout);
    if (status != POULPE_ERROR_NONE)
        return status;
    _editor.main_layout->type = POULPE_LAYOUT_TYPE_VBOX;
    
    status = poulpe_tabmenu_init(&_editor.tabmenu);
    if (status != POULPE_ERROR_NONE)
        return status;
    _editor.tabmenu->base.fill_width = true;
    _editor.tabmenu->base.preferred_height = 20;

    status = poulpe_tabview_init(&_editor.tabview);
    if (status != POULPE_ERROR_NONE)
        return status;
    _editor.tabview->base.fill_width = true;
    _editor.tabview->base.fill_height = true;
    _editor.tabview->tabmenu = _editor.tabmenu;

    poulpe_layout_add_child(_editor.main_layout, (struct poulpe_component *) _editor.tabmenu);
    poulpe_layout_add_child(_editor.main_layout, (struct poulpe_component *) _editor.tabview);

    return POULPE_ERROR_NONE;
}