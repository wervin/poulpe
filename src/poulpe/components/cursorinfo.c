#include <stdlib.h>
#include <float.h>
#include <math.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/cursorinfo.h"
#include "poulpe/components/statusbar.h"

#include "poulpe/editor.h"
#include "poulpe/log.h"
#include "poulpe/text.h"

struct poulpe_cursorinfo * poulpe_cursorinfo_new(void)
{
    struct poulpe_cursorinfo *cursorinfo;

    cursorinfo = calloc(1, sizeof(struct poulpe_cursorinfo));
    if (!cursorinfo)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate cursorinfo");
        return NULL;
    }

    return cursorinfo;
}

void poulpe_cursorinfo_free(struct poulpe_cursorinfo *cursorinfo)
{
    free(cursorinfo);
}

enum poulpe_error poulpe_cursorinfo_notify(struct poulpe_cursorinfo *cursorinfo, struct poulpe_event *event)
{
    SAKE_MACRO_UNUSED(cursorinfo);
    SAKE_MACRO_UNUSED(event);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_cursorinfo_draw(struct poulpe_cursorinfo *cursorinfo)
{
    ImVec2 content;
    igGetContentRegionAvail(&content);

    ImVec2 cursor_position = poulpe_editor_cursor_position(cursorinfo->statusbar->editor);
    char buffer[256];
    snprintf(buffer, 256, "Ln %u, Col %u", (uint32_t) cursor_position.x + 1, (uint32_t) cursor_position.y + 1);

    igButton(buffer, (ImVec2) {0, content.y});

    return POULPE_ERROR_NONE;
}

void poulpe_cursorinfo_set_statusbar(struct poulpe_cursorinfo *cursorinfo, struct poulpe_statusbar *statusbar)
{
    cursorinfo->statusbar = statusbar;
}