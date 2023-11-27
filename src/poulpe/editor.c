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
#include "poulpe/components/textview.h"

#include "sake/vector.h"

struct _editor
{    
    struct poulpe_textbuffer *textbuffer;
    struct poulpe_textview *textview;
};

struct _editor _editor = {0};

enum poulpe_error poulpe_editor_init(void)
{    
    enum poulpe_error error = POULPE_ERROR_NONE;

    error = poulpe_editor_open_file("../../test.md");
    if (error != POULPE_ERROR_NONE)
        return error;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_editor_draw(void)
{   
    enum poulpe_error error = POULPE_ERROR_NONE;

    ImGuiContext *context = igGetCurrentContext();
    ImGuiWindow *window = igGetCurrentWindowRead();
    /* Will be removed in the future... */
    if (!igBegin("Editor", NULL, 0))
        goto end;

    const float scrollbar_size = floor(fmax(context->FontSize * 1.10f, window->WindowRounding + 1.0f + context->FontSize * 0.2f));
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowMinSize, (ImVec2) {0.f, 0.f});
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2) {0.f, 0.f});
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 1.f);
    igPushStyleVar_Float(ImGuiStyleVar_ScrollbarSize, scrollbar_size);

    if (!igBeginChild_Str("Poulpe", (ImVec2) {0}, true, ImGuiWindowFlags_NoMove))
        goto end_child;
    
    error = poulpe_component_draw((struct poulpe_component *) _editor.textview);
    if (error != POULPE_ERROR_NONE)
        goto end_child;

end_child:
    igEndChild();
    igPopStyleVar(4);

end:
    igEnd();

    return error;
}

enum poulpe_error poulpe_editor_open_file(const char *filename)
{
    _editor.textbuffer = poulpe_textbuffer_new();
    if (!_editor.textbuffer)
        return POULPE_ERROR_MEMORY;

    poulpe_textbuffer_open(_editor.textbuffer, filename);

    _editor.textview = (struct poulpe_textview *) poulpe_component_new(POULPE_COMPONENT_TYPE_TEXTVIEW);
    if (!_editor.textview)
        return POULPE_ERROR_MEMORY;
    
    poulpe_textview_set_textbuffer(_editor.textview, _editor.textbuffer);
    return POULPE_ERROR_NONE;
}

void poulpe_editor_destroy(void)
{
    poulpe_component_free((struct poulpe_component *) _editor.textview);
    poulpe_textbuffer_free(_editor.textbuffer);
}