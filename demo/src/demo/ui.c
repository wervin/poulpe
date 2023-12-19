#include <cimgui.h>

#include <sake/vector.h>

#include "poulpe.h"

#include "demo/ui.h"
#include "demo/renderer.h"
#include "demo/window.h"
#include "demo/log.h"
#include "demo/error.h"

struct _ui
{
    struct poulpe_editor **editors;
};

static struct _ui _ui = {0};

static enum demo_error _open_editor(const char *path);

enum demo_error demo_ui_init(void)
{
    enum demo_error error = DEMO_ERROR_NONE;

    igCreateContext(NULL);
    ImGuiIO *io = igGetIO();
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImFont *large_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "assets/fonts/Cousine-Regular.ttf", 18.0f, NULL, NULL);
    ImFont *small_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "assets/fonts/Cousine-Regular.ttf", 11.0f, NULL, NULL);
    ImFont *icon_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "assets/fonts/fontawesome-webfont.ttf", 18.0f, NULL, NULL);

    io->FontDefault = large_font;

    struct poulpe_style_info style_info = {
        .large_font = large_font,
        .small_font = small_font,
        .icon_font = icon_font,
        .theme = POULPE_THEME_DARK
    };

    poulpe_style_set(&style_info);

    error = primsa_window_init_ui();
    if (error != DEMO_ERROR_NONE)
        return error;

    error = demo_renderer_init_ui();
    if (error != DEMO_ERROR_NONE)
        return error;

    _ui.editors = sake_vector_new(sizeof(struct poulpe_editor *), (void (*) (void*)) poulpe_editor_free);
    if (!_ui.editors)
    {
        DEMO_LOG_ERROR(DEMO_ERROR_MEMORY, "Cannot allocate editors");
        return DEMO_ERROR_MEMORY;
    }

    error = _open_editor("tests/test.json");
    if (error != DEMO_ERROR_NONE)
        return error;

    error = _open_editor("tests/test.frag");
    if (error != DEMO_ERROR_NONE)
        return error;

    error = _open_editor("tests/test.c");
    if (error != DEMO_ERROR_NONE)
        return error;

    return DEMO_ERROR_NONE;
}

void demo_ui_draw(void)
{   
    demo_window_refresh_ui();
    demo_renderer_refresh_ui();

    igNewFrame();

    igShowMetricsWindow(NULL);
    
    for (uint32_t i = 0; i < sake_vector_size(_ui.editors); i++)
    {
        const char *filename = poulpe_editor_filename(_ui.editors[i]);
        igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){0.f, 0.f});
        if (igBegin(filename, NULL, 0))
            poulpe_editor_draw(_ui.editors[i]);
        igEnd();
        igPopStyleVar(1);
    }

    igRender();
}

void demo_ui_destroy(void)
{
    sake_vector_free(_ui.editors);
    demo_renderer_destroy_ui();
    demo_window_destroy_ui();
    igDestroyContext(igGetCurrentContext());
}

static enum demo_error _open_editor(const char *path)
{
    struct poulpe_editor *editor = poulpe_editor_new(path);
    if (!editor)
        return DEMO_ERROR_MEMORY;

    _ui.editors = sake_vector_push_back(_ui.editors, &editor);
    if (!_ui.editors)
    {
        DEMO_LOG_ERROR(DEMO_ERROR_MEMORY, "Cannot push back textview in tabview");
        return DEMO_ERROR_MEMORY;
    }

    return DEMO_ERROR_NONE;
}