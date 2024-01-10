#include "poulpe/components/indentinfo.h"

#include <stdlib.h>
#include <float.h>
#include <math.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/indentinfo.h"

#include "poulpe/log.h"

static const char *names[] = {
#define X(__def, __id, __label, __str, __n) \
    __label,

    POULPE_INDENTINFOS

#undef X
};

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
    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);
        
    if (igButton(names[indentinfo->current], (ImVec2){0, content.y}))
        igOpenPopup_Str("poulpe_indentinfo", 0);

    ImGuiStyle *style = igGetStyle();
    origin_screen_position.y -= sizeof(names) / sizeof(char *) * igGetTextLineHeight() + 
        2 * style->WindowPadding.y + 
        (sizeof(names) / sizeof(char *) - 1) * style->ItemSpacing.y;

    igSetNextWindowPos(origin_screen_position, 0, (ImVec2) {0, 0});
    if (igBeginPopup("poulpe_indentinfo", 0))
    {
        for (uint32_t i = 0; i < sizeof(names) / sizeof(char *); i++)
        {
            if (igSelectable_Bool(names[i], false, 0, (ImVec2){0, 0}))
                indentinfo->current = i;
        }
        igEndPopup();
    }
    
    return POULPE_ERROR_NONE;
}

void poulpe_indentinfo_set_statusbar(struct poulpe_indentinfo *indentinfo, struct poulpe_statusbar *statusbar)
{
    indentinfo->statusbar = statusbar;
}

const char *poulpe_indentinfo_str(enum poulpe_indentinfo_type type)
{
    switch (type)
    {
#define X(__def, __id, __label, __str, __n) \
    case __id:                        \
        return __str;

        POULPE_INDENTINFOS

#undef X

    default:
        return "    ";
    }
}

uint32_t poulpe_indentinfo_length(enum poulpe_indentinfo_type type)
{
    switch (type)
    {
#define X(__def, __id, __label, __str, __n) \
    case __id:                        \
        return __n;

        POULPE_INDENTINFOS

#undef X

    default:
        return 4;
    }
}