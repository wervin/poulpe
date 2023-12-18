#include <stdlib.h>
#include <float.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/statusbar.h"
#include "poulpe/components/cursorinfo.h"
#include "poulpe/components/eofinfo.h"
#include "poulpe/components/indentinfo.h"
#include "poulpe/components/textinfo.h"
#include "poulpe/component.h"

#include "poulpe/editor.h"
#include "poulpe/log.h"
#include "poulpe/theme.h"

struct poulpe_statusbar * poulpe_statusbar_new(void)
{
    struct poulpe_statusbar *statusbar;

    statusbar = calloc(1, sizeof(struct poulpe_statusbar));
    if (!statusbar)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate statusbar");
        return NULL;
    }

    statusbar->cursorinfo = (struct poulpe_cursorinfo *) poulpe_component_new(POULPE_COMPONENT_TYPE_CURSORINFO);
    if (!statusbar->cursorinfo)
        return NULL;

    statusbar->indentinfo = (struct poulpe_indentinfo *) poulpe_component_new(POULPE_COMPONENT_TYPE_INDENTINFO);
    if (!statusbar->indentinfo)
        return NULL;

    statusbar->textinfo = (struct poulpe_textinfo *) poulpe_component_new(POULPE_COMPONENT_TYPE_TEXTINFO);
    if (!statusbar->textinfo)
        return NULL;

    statusbar->eofinfo = (struct poulpe_eofinfo *) poulpe_component_new(POULPE_COMPONENT_TYPE_EOFINFO);
    if (!statusbar->textinfo)
        return NULL;

    poulpe_cursorinfo_set_statusbar(statusbar->cursorinfo, statusbar);
    poulpe_indentinfo_set_statusbar(statusbar->indentinfo, statusbar);
    poulpe_textinfo_set_statusbar(statusbar->textinfo, statusbar);
    poulpe_eofinfo_set_statusbar(statusbar->eofinfo, statusbar);

    return statusbar;
}

void poulpe_statusbar_free(struct poulpe_statusbar *statusbar)
{
    poulpe_component_free((struct poulpe_component*) statusbar->cursorinfo);
    poulpe_component_free((struct poulpe_component*) statusbar->indentinfo);
    poulpe_component_free((struct poulpe_component*) statusbar->textinfo);
    poulpe_component_free((struct poulpe_component*) statusbar->eofinfo);
    free(statusbar);
}

enum poulpe_error poulpe_statusbar_notify(struct poulpe_statusbar *statusbar, struct poulpe_event *event)
{
    SAKE_MACRO_UNUSED(statusbar);
    SAKE_MACRO_UNUSED(event);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_statusbar_draw(struct poulpe_statusbar *statusbar)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    igPushFont(statusbar->editor->small_font);
    if (!igBeginChild_Str("Poulpe##statusbar", (ImVec2) {0}, false, ImGuiWindowFlags_NoScrollbar))
        goto end_child;

    ImVec2 content;
    igGetContentRegionAvail(&content);

    ImDrawList *draw_list = igGetWindowDrawList();
    ImGuiWindow *window = igGetCurrentWindowRead();
    ImDrawList_AddRectFilled(draw_list, window->InnerRect.Min, window->InnerRect.Max, igColorConvertFloat4ToU32(poulpe_theme_dark.main_background), 0.0f, 0);

    igSameLine(0.0f, content.y);
    error = poulpe_component_draw((struct poulpe_component *) statusbar->indentinfo);
    if (error != POULPE_ERROR_NONE)
        return error;

    igSameLine(0.0f, content.y);
    error = poulpe_component_draw((struct poulpe_component *) statusbar->textinfo);
    if (error != POULPE_ERROR_NONE)
        return error;

    igSameLine(0.0f, content.y);
    error = poulpe_component_draw((struct poulpe_component *) statusbar->eofinfo);
    if (error != POULPE_ERROR_NONE)
        return error;

    igSameLine(0.0f, content.y);
    error = poulpe_component_draw((struct poulpe_component *) statusbar->cursorinfo);
    if (error != POULPE_ERROR_NONE)
        return error;

end_child:
    igEndChild();
    igPopFont();

    return error;
}

void poulpe_statusbar_set_editor(struct poulpe_statusbar *textview, struct poulpe_editor *editor)
{
    textview->editor = editor;
}