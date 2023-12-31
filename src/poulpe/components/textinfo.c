#include "poulpe/components/textinfo.h"

#include <stdlib.h>
#include <float.h>
#include <math.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/textinfo.h"

#include "poulpe/log.h"

struct poulpe_textinfo * poulpe_textinfo_new(void)
{
    struct poulpe_textinfo *textinfo;

    textinfo = calloc(1, sizeof(struct poulpe_textinfo));
    if (!textinfo)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate textinfo");
        return NULL;
    }

    return textinfo;
}

void poulpe_textinfo_free(struct poulpe_textinfo *textinfo)
{
    free(textinfo);
}

enum poulpe_error poulpe_textinfo_notify(struct poulpe_textinfo *textinfo, struct poulpe_event *event)
{
    SAKE_MACRO_UNUSED(textinfo);
    SAKE_MACRO_UNUSED(event);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textinfo_draw(struct poulpe_textinfo *textinfo)
{
    ImVec2 content;
    igGetContentRegionAvail(&content);

    igButton("GLSL", (ImVec2) {0, content.y});

    return POULPE_ERROR_NONE;
}

void poulpe_textinfo_set_statusbar(struct poulpe_textinfo *textinfo, struct poulpe_statusbar *statusbar)
{
    textinfo->statusbar = statusbar;
}