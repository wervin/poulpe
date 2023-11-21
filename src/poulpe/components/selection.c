#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/selection.h"
#include "poulpe/components/textview.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"

struct poulpe_selection * poulpe_selection_new(void)
{
    struct poulpe_selection *selection;

    selection = calloc(1, sizeof(struct poulpe_selection));
    if (!selection)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate selection");
        return NULL;
    }

    return selection;
}

void poulpe_selection_free(struct poulpe_selection *selection)
{
    free(selection);
}

enum poulpe_error poulpe_selection_notify(struct poulpe_selection *selection, struct poulpe_event *event)
{
    SAKE_MACRO_UNUSED(selection);
    SAKE_MACRO_UNUSED(event);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_selection_draw(struct poulpe_selection *selection)
{
    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);

    uint32_t start_line_index = selection->start_line_index < selection->end_line_index ? selection->start_line_index : selection->end_line_index;
    uint32_t end_line_index = selection->start_line_index < selection->end_line_index ? selection->end_line_index : selection->start_line_index;
    uint32_t start_glyph_index, end_glyph_index = 0;
    if (start_line_index == end_line_index)
    {
        start_glyph_index = selection->start_glyph_index < selection->end_glyph_index ? selection->start_glyph_index : selection->end_glyph_index;
        end_glyph_index = selection->start_glyph_index < selection->end_glyph_index ? selection->end_glyph_index : selection->start_glyph_index;
    }
    else
    {
        start_glyph_index = selection->start_line_index < selection->end_line_index ? selection->start_glyph_index : selection->end_glyph_index;
        end_glyph_index = selection->start_line_index < selection->end_line_index ? selection->end_glyph_index : selection->start_glyph_index;
    }

    for (uint32_t i = start_line_index; i <= end_line_index; i++)
    {
        ImDrawList *draw_list = igGetWindowDrawList();
        ImVec2 space_size;
        ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);

        ImVec2 upper_left = {0};
        ImVec2 lower_right = {0};

        if (start_line_index == end_line_index)
        {
            float start_size = poulpe_textbuffer_line_subset_textsize(selection->textview->textbuffer, i, 0, start_glyph_index);
            float end_size = poulpe_textbuffer_line_subset_textsize(selection->textview->textbuffer, i, 0, end_glyph_index);
            upper_left = (ImVec2) {origin_screen_position.x + start_size, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + end_size, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else if (i == start_line_index)
        {
            float full_size = poulpe_textbuffer_line_full_textsize(selection->textview->textbuffer, i);
            float subset_size = poulpe_textbuffer_line_subset_textsize(selection->textview->textbuffer, i, 0, start_glyph_index);
            upper_left = (ImVec2) {origin_screen_position.x + subset_size, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + full_size, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else if (i == end_line_index)
        {
            float text_size = poulpe_textbuffer_line_subset_textsize(selection->textview->textbuffer, i, 0, end_glyph_index);
            text_size = fmax(text_size, space_size.x);
            upper_left = (ImVec2) {origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + text_size, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else
        {
            float text_size = poulpe_textbuffer_line_full_textsize(selection->textview->textbuffer, i);
            text_size = fmax(text_size, space_size.x);
            upper_left = (ImVec2) {origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + text_size, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }

        ImDrawList_AddRectFilled(draw_list, upper_left, lower_right, igColorConvertFloat4ToU32(poulpe_theme_dark.visual_select_background), 0.0f, 0);
    }

    return POULPE_ERROR_NONE;
}

void poulpe_selection_set_textview(struct poulpe_selection *selection, struct poulpe_textview *textview)
{
    selection->textview = textview;
}

void poulpe_selection_update_start(struct poulpe_selection *selection, ImVec2 position)
{        
    selection->start_line_index = position.x;
    selection->start_glyph_index = position.y;
}

void poulpe_selection_update_end(struct poulpe_selection *selection, ImVec2 position)
{
    selection->end_line_index = position.x;
    selection->end_glyph_index = position.y;
}