#include <stdlib.h>

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

    error = poulpe_editor_open_file("../../README.md");
    if (error != POULPE_ERROR_NONE)
        return error;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_editor_draw(void)
{   
    enum poulpe_error error = POULPE_ERROR_NONE;

    /* Will be removed in the future... */
    if (!igBegin("Editor", NULL, 0))
        goto end;

    if (!igBeginChild_Str("Poulpe", (ImVec2) {0}, false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove))
        goto end_child;

    error = poulpe_io_handle_keyboard((struct poulpe_component *) _editor.textview);
    if (error != POULPE_ERROR_NONE)
        goto end;
    
    error = poulpe_io_handle_mouse((struct poulpe_component *) _editor.textview);
    if (error != POULPE_ERROR_NONE)
        goto end;
    
    error = poulpe_component_draw((struct poulpe_component *) _editor.textview);
    if (error != POULPE_ERROR_NONE)
        goto end;

end_child:
    igEndChild();

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