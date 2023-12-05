#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include <sake/vector.h>
#include <sake/string.h>
#include <sake/macro.h>

#include "poulpe/components/textview.h"
#include "poulpe/components/cursor.h"
#include "poulpe/components/selection.h"
#include "poulpe/components/linenumber.h"
#include "poulpe/components/textedit.h"

#include "poulpe/textbuffer.h"
#include "poulpe/log.h"
#include "poulpe/theme.h"

struct poulpe_textview * poulpe_textview_new(void)
{
    struct poulpe_textview *textview;

    textview = calloc(1, sizeof(struct poulpe_textview));
    if (!textview)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate textview");
        return NULL;
    }

    textview->textedit = (struct poulpe_textedit *) poulpe_component_new(POULPE_COMPONENT_TYPE_TEXTEDIT);
    if (!textview->textedit)
        return NULL;

    poulpe_textedit_set_textview(textview->textedit, textview);

    textview->linenumber = (struct poulpe_linenumber *) poulpe_component_new(POULPE_COMPONENT_TYPE_LINENUMBER);
    if (!textview->linenumber)
        return NULL;

    poulpe_linenumber_set_textview(textview->linenumber, textview);
    
    return textview;
}

void poulpe_textview_free(struct poulpe_textview *textview)
{
    poulpe_component_free((struct poulpe_component *) textview->linenumber);
    poulpe_component_free((struct poulpe_component *) textview->textedit);
    free(textview);
}

enum poulpe_error poulpe_textview_draw(struct poulpe_textview *textview)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    ImVec2 content;
    igGetContentRegionAvail(&content);
    ImGuiWindow *window = igGetCurrentWindowRead();
    float statusbar_size = floor(fmax(igGetTextLineHeight() * 1.10f, window->WindowRounding + 1.0f + igGetTextLineHeight() * 0.2f));

    if (!igBeginChild_Str("Poulpe##textview", (ImVec2) {0, content.y - statusbar_size}, true, ImGuiWindowFlags_NoScrollbar))
        goto end_child;

    error = poulpe_component_draw((struct poulpe_component *)textview->linenumber);
    if (error != POULPE_ERROR_NONE)
        return error;

    error = poulpe_component_draw((struct poulpe_component *)textview->textedit);
    if (error != POULPE_ERROR_NONE)
        return error;

    end_child:
        igEndChild();

    return error;
}

enum poulpe_error poulpe_textview_notify(struct poulpe_textview *textview, struct poulpe_event *event)
{
    SAKE_MACRO_UNUSED(textview);
    SAKE_MACRO_UNUSED(event);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textview_open_file(struct poulpe_textview *textview, const char *path)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    textview->textbuffer = poulpe_textbuffer_new();
    if (!textview->textbuffer)
        return POULPE_ERROR_MEMORY;

    error = poulpe_textbuffer_open_file(textview->textbuffer, path);
    if (error != POULPE_ERROR_NONE)
        return error;

    return POULPE_ERROR_NONE;
}

void poulpe_textview_set_editor(struct poulpe_textview *textview, struct poulpe_editor *editor)
{
    textview->editor = editor;
}