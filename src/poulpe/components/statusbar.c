#include <stdlib.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/statusbar.h"

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

    return statusbar;
}

void poulpe_statusbar_free(struct poulpe_statusbar *statusbar)
{
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

    igPushFont(NULL);

    if (!igBeginChild_Str("Poulpe##statusbar", (ImVec2) {0}, true, 0))
        goto end_child;

    igButton("Spaces: 4", (ImVec2) {0});
    igSameLine(0.0f, -1.0f);
    igButton("Ln 46, Col 23", (ImVec2) {0});

end_child:
    igEndChild();
    igPopFont();

    return error;
}