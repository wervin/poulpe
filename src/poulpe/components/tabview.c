#include <stdlib.h>
#include <float.h>

#include <cimgui.h>

#include <sake/vector.h>
#include <sake/macro.h>

#include "poulpe/components/tabview.h"
#include "poulpe/components/textview.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"
#include "poulpe/textbuffer.h"

struct poulpe_tabview * poulpe_tabview_new(void)
{
    struct poulpe_tabview *tabview;

    tabview = calloc(1, sizeof(struct poulpe_tabview));
    if (!tabview)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate tabview");
        return NULL;
    }

    tabview->textviews = sake_vector_new(sizeof(struct poulpe_textview *), (void (*) (void*)) poulpe_component_free);
    if (!tabview->textviews)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate textviews");
        return NULL;
    }

    return tabview;
}

void poulpe_tabview_free(struct poulpe_tabview *tabview)
{
    sake_vector_free(tabview->textviews);
    free(tabview);
}

enum poulpe_error poulpe_tabview_notify(struct poulpe_tabview *tabview, struct poulpe_event *event)
{
    SAKE_MACRO_UNUSED(tabview);
    SAKE_MACRO_UNUSED(event);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabview_draw(struct poulpe_tabview *tabview)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    igPushStyleColor_U32(ImGuiCol_Tab, igColorConvertFloat4ToU32(poulpe_theme_dark.tab_inactive));
    igPushStyleColor_U32(ImGuiCol_TabActive, igColorConvertFloat4ToU32(poulpe_theme_dark.tab_active));
    igPushStyleColor_U32(ImGuiCol_TabHovered, igColorConvertFloat4ToU32(poulpe_theme_dark.tab_inactive));
    igPushStyleColor_U32(ImGuiCol_TabUnfocused, igColorConvertFloat4ToU32(poulpe_theme_dark.tab_inactive));
    igPushStyleColor_U32(ImGuiCol_TabUnfocusedActive, igColorConvertFloat4ToU32(poulpe_theme_dark.tab_active));

    if (!igBeginTabBar("Poulpe##tabview", 0))
        goto end;


    for (uint32_t i = 0; i < sake_vector_size(tabview->textviews); i++)
    {
        if (!igBeginTabItem(tabview->textviews[i]->textbuffer->filename, NULL, 0))
            continue;

        error = poulpe_component_draw((struct poulpe_component *) tabview->textviews[i]);
        if (error != POULPE_ERROR_NONE)
        {
            igEndTabItem();
            goto end;
        }
            
        igEndTabItem();
    }
    igEndTabBar();

end:
    igPopStyleColor(5);
    return error;
}

enum poulpe_error poulpe_tabview_open_file(struct poulpe_tabview *tabview, const char *path)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    struct poulpe_textview *textview = (struct poulpe_textview *) poulpe_component_new(POULPE_COMPONENT_TYPE_TEXTVIEW);
    if (!textview)
        return POULPE_ERROR_MEMORY;

    error = poulpe_textview_open_file(textview, path);
    if (error != POULPE_ERROR_NONE)
        return error;

    tabview->textviews = sake_vector_push_back(tabview->textviews, &textview);
    if (!tabview->textviews)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot push back textview in tabview");
        return POULPE_ERROR_MEMORY;
    }

    return POULPE_ERROR_NONE;
}