#include <stdlib.h>
#include <float.h>
#include <math.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/eofinfo.h"
#include "poulpe/components/statusbar.h"

#include "poulpe/editor.h"
#include "poulpe/log.h"
#include "poulpe/text.h"

struct poulpe_eofinfo * poulpe_eofinfo_new(void)
{
    struct poulpe_eofinfo *eofinfo;

    eofinfo = calloc(1, sizeof(struct poulpe_eofinfo));
    if (!eofinfo)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate eofinfo");
        return NULL;
    }

    return eofinfo;
}

void poulpe_eofinfo_free(struct poulpe_eofinfo *eofinfo)
{
    free(eofinfo);
}

enum poulpe_error poulpe_eofinfo_notify(struct poulpe_eofinfo *eofinfo, struct poulpe_event *event)
{
    SAKE_MACRO_UNUSED(eofinfo);
    SAKE_MACRO_UNUSED(event);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_eofinfo_draw(struct poulpe_eofinfo *eofinfo)
{
    SAKE_MACRO_UNUSED(eofinfo);
    
    ImVec2 content;
    igGetContentRegionAvail(&content);

    igButton("LF", (ImVec2){0, content.y});

    return POULPE_ERROR_NONE;
}

void poulpe_eofinfo_set_statusbar(struct poulpe_eofinfo *eofinfo, struct poulpe_statusbar *statusbar)
{
    eofinfo->statusbar = statusbar;
}