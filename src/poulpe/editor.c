#include <stdlib.h>
#include <math.h>

#include <cimgui.h>

#include "poulpe/editor.h"
#include "poulpe/log.h"
#include "poulpe/text.h"
#include "poulpe/textbuffer.h"
#include "poulpe/theme.h"
#include "poulpe/io.h"
#include "poulpe/event.h"

#include "poulpe/component.h"
#include "poulpe/components/tabview.h"

#include "sake/vector.h"

struct _editor
{    
    struct poulpe_tabview *tabview;
};

struct _editor _editor = {0};

enum poulpe_error poulpe_editor_init(void)
{    
    enum poulpe_error error = POULPE_ERROR_NONE;

    _editor.tabview = (struct poulpe_tabview *) poulpe_component_new(POULPE_COMPONENT_TYPE_TABVIEW);
    if (!_editor.tabview)
        return POULPE_ERROR_MEMORY;

    error = poulpe_editor_open_file("../../test1.md");
    if (error != POULPE_ERROR_NONE)
        return error;

    error = poulpe_editor_open_file("../../test2.md");
    if (error != POULPE_ERROR_NONE)
        return error;

    error = poulpe_editor_open_file("../../README.md");
    if (error != POULPE_ERROR_NONE)
        return error;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_editor_draw(void)
{   
    enum poulpe_error error = POULPE_ERROR_NONE;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2) {20.f, 20.f});

    /* Will be removed in the future... */
    if (!igBegin("Editor", NULL, 0))
        goto end;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowMinSize, (ImVec2) {0.f, 0.f});
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2) {0.f, 0.f});
    igPushStyleVar_Vec2(ImGuiStyleVar_ItemSpacing, (ImVec2) {0.f, 0.f});
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.f);
    
    if (!igBeginChild_Str("Poulpe##editor", (ImVec2) {0}, false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
        goto end_child;
    
    error = poulpe_component_draw((struct poulpe_component *) _editor.tabview);
    if (error != POULPE_ERROR_NONE)
        goto end_child;

end_child:
    igEndChild();
    igPopStyleVar(4);

end:
    igEnd();
    igPopStyleVar(1);

    return error;
}

enum poulpe_error poulpe_editor_open_file(const char *path)
{
    return poulpe_tabview_open_file(_editor.tabview, path);
}

void poulpe_editor_destroy(void)
{
    poulpe_component_free((struct poulpe_component *) _editor.tabview);
}