#include "poulpe/components/indentinfo.h"

#include <stdlib.h>
#include <float.h>
#include <math.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/indentinfo.h"

#include "poulpe/log.h"

struct poulpe_indentinfo * poulpe_indentinfo_new(void)
{
    struct poulpe_indentinfo *indentinfo;

    indentinfo = calloc(1, sizeof(struct poulpe_indentinfo));
    if (!indentinfo)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate indentinfo");
        return NULL;
    }

    return indentinfo;
}

void poulpe_indentinfo_free(struct poulpe_indentinfo *indentinfo)
{
    free(indentinfo);
}

enum poulpe_error poulpe_indentinfo_notify(struct poulpe_indentinfo *indentinfo, struct poulpe_event *event)
{
    SAKE_MACRO_UNUSED(indentinfo);
    SAKE_MACRO_UNUSED(event);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_indentinfo_draw(struct poulpe_indentinfo *indentinfo)
{
    ImVec2 content;
    igGetContentRegionAvail(&content);

    igButton("Spaces: 4", (ImVec2) {0, content.y});

    return POULPE_ERROR_NONE;
}

void poulpe_indentinfo_set_statusbar(struct poulpe_indentinfo *indentinfo, struct poulpe_statusbar *statusbar)
{
    indentinfo->statusbar = statusbar;
}