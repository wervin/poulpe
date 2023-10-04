#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_VULKAN
#include <cimgui_impl.h>

#include "poulpe/editor.h"

#include "demo/ui.h"
#include "demo/renderer.h"
#include "demo/window.h"
#include "demo/log.h"

struct _ui
{
    ImGuiContext *imgui_context;
    ImGuiIO *imgui_io;
};

static struct _ui _ui = {0};

enum demo_error demo_ui_init(void)
{
    enum demo_error status = DEMO_ERROR_NONE;

    _ui.imgui_context = igCreateContext(NULL);
    _ui.imgui_io = igGetIO();
    _ui.imgui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    status = primsa_window_init_ui();
    if (status != DEMO_ERROR_NONE)
        return status;

    status = demo_renderer_init_ui();
    if (status != DEMO_ERROR_NONE)
        return status;

    if (poulpe_editor_init() != POULPE_ERROR_NONE)
    {
        DEMO_LOG_ERROR(DEMO_ERROR_POULPE, "Cannot initialize poulpe editor");
        return DEMO_ERROR_POULPE;
    }

    return DEMO_ERROR_NONE;
}

void demo_ui_draw(void)
{   
    demo_window_refresh_ui();
    demo_renderer_refresh_ui();

    igNewFrame();

    igShowMetricsWindow(NULL);

    poulpe_editor_draw();
    
    igRender();
}

void demo_ui_destroy(void)
{
    poulpe_editor_destroy();
    demo_renderer_destroy_ui();
    demo_window_destroy_ui();
    igDestroyContext(_ui.imgui_context);
}