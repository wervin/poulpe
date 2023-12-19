#include <stdlib.h>
#include <float.h>
#include <math.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/linenumber.h"
#include "poulpe/components/textedit.h"
#include "poulpe/components/cursor.h"
#include "poulpe/components/textview.h"

#include "poulpe/log.h"
#include "poulpe/style.h"
#include "poulpe/textbuffer.h"

static void _update_linenumber(struct poulpe_linenumber *poulpe_linenumber);
static float _maximum_line_number_width(struct poulpe_linenumber *linenumber);

struct poulpe_linenumber * poulpe_linenumber_new(void)
{
    struct poulpe_linenumber *linenumber;

    linenumber = calloc(1, sizeof(struct poulpe_linenumber));
    if (!linenumber)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate linenumber");
        return NULL;
    }

    return linenumber;
}

void poulpe_linenumber_free(struct poulpe_linenumber *linenumber)
{
    free(linenumber);
}

enum poulpe_error poulpe_linenumber_notify(struct poulpe_linenumber *linenumber, struct poulpe_event *event)
{
    SAKE_MACRO_UNUSED(linenumber);
    SAKE_MACRO_UNUSED(event);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_linenumber_draw(struct poulpe_linenumber *linenumber)
{
    float max_line_number_size = _maximum_line_number_width(linenumber);

    igPushStyleVar_Vec2(ImGuiStyleVar_FramePadding, (ImVec2) {10.f, 0.f});

    ImGuiStyle *style = igGetStyle();
    
    if (!igBeginChild_Str("Poulpe##linenumber", (ImVec2) {max_line_number_size + 2 * style->FramePadding.x, 0.0f}, false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
        goto end_child;

    _update_linenumber(linenumber);

    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);

    ImDrawList *draw_list = igGetWindowDrawList();

    {
        ImGuiWindow *window = igGetCurrentWindowRead();
        ImDrawList_AddRectFilled(draw_list, window->InnerRect.Min, window->InnerRect.Max, igColorConvertFloat4ToU32(poulpe_style.theme->secondary_background), 0.0f, 0);
    }

    for (uint32_t i = linenumber->textview->textedit->line_start; i < linenumber->textview->textedit->line_end; i++)
    {
        ImVec2 line_start_position = {origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};

        char line_number_buffer[16];
        snprintf(line_number_buffer, 16, "%u", i + 1);
        ImVec2 line_number_size;
        ImFont_CalcTextSizeA(&line_number_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line_number_buffer, NULL, NULL);

        ImU32 color = i == linenumber->textview->textedit->cursor->position.x ? igColorConvertFloat4ToU32(poulpe_style.theme->secondary_text) : igColorConvertFloat4ToU32(poulpe_style.theme->muted_text);

        ImVec2 line_number_start_position = {line_start_position.x + max_line_number_size - line_number_size.x + style->FramePadding.x, line_start_position.y};
        ImDrawList_AddText_Vec2(draw_list,
                                line_number_start_position,
                                color,
                                line_number_buffer,
                                NULL);
    }

end_child:
    igEndChild();
    igPopStyleVar(1);

    return POULPE_ERROR_NONE;
}

void poulpe_linenumber_set_textview(struct poulpe_linenumber *linenumber, struct poulpe_textview *textview)
{
    linenumber->textview = textview;
}

static void _update_linenumber(struct poulpe_linenumber *linenumber)
{
    ImGuiWindow *window = igGetCurrentWindowRead();
    window->DC.CursorPos.y += (window->Scroll.y - linenumber->textview->textedit->scroll_y);
    window->Scroll.y = linenumber->textview->textedit->scroll_y;
}

static float _maximum_line_number_width(struct poulpe_linenumber *linenumber)
{
    poulpe_text text = linenumber->textview->textbuffer->text;
    uint32_t line_count = poulpe_text_size(text);

    char max_line_number_buffer[16];
    snprintf(max_line_number_buffer, 16, "%u", line_count);
    ImVec2 max_line_number_size;
    ImFont_CalcTextSizeA(&max_line_number_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, max_line_number_buffer, NULL, NULL);

    char min_line_number_buffer[16];
    snprintf(min_line_number_buffer, 16, "%04u", 0);
    ImVec2 min_line_number_size;
    ImFont_CalcTextSizeA(&min_line_number_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, min_line_number_buffer, NULL, NULL);

    return fmax(max_line_number_size.x, min_line_number_size.x);
}