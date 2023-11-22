#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/selection.h"
#include "poulpe/components/textview.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"

static void _update_selection(struct poulpe_selection *selection);

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

    float max_line_number_size = poulpe_textview_get_line_number_width(selection->textview);

    for (uint32_t i = selection->ajusted.start_line_index; i <= selection->ajusted.end_line_index; i++)
    {
        ImDrawList *draw_list = igGetWindowDrawList();
        ImVec2 space_size;
        ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);

        ImVec2 upper_left = {0};
        ImVec2 lower_right = {0};

        if (selection->ajusted.start_line_index == selection->ajusted.end_line_index)
        {
            float start_size = poulpe_textbuffer_line_subset_textsize(selection->textview->textbuffer, i, 0, selection->ajusted.start_glyph_index);
            float end_size = poulpe_textbuffer_line_subset_textsize(selection->textview->textbuffer, i, 0, selection->ajusted.end_glyph_index);
            upper_left = (ImVec2) {origin_screen_position.x + max_line_number_size + start_size, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + max_line_number_size + end_size, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else if (i == selection->ajusted.start_line_index)
        {
            float full_size = poulpe_textbuffer_line_full_textsize(selection->textview->textbuffer, i);
            float subset_size = poulpe_textbuffer_line_subset_textsize(selection->textview->textbuffer, i, 0, selection->ajusted.start_glyph_index);
            upper_left = (ImVec2) {origin_screen_position.x + max_line_number_size + subset_size, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + max_line_number_size + full_size, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else if (i == selection->ajusted.end_line_index)
        {
            float text_size = poulpe_textbuffer_line_subset_textsize(selection->textview->textbuffer, i, 0, selection->ajusted.end_glyph_index);
            text_size = fmax(text_size, space_size.x);
            upper_left = (ImVec2) {origin_screen_position.x + max_line_number_size, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + max_line_number_size + text_size, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else
        {
            float text_size = poulpe_textbuffer_line_full_textsize(selection->textview->textbuffer, i);
            text_size = fmax(text_size, space_size.x);
            upper_left = (ImVec2) {origin_screen_position.x + max_line_number_size, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + max_line_number_size + text_size, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }

        ImDrawList_AddRectFilled(draw_list, upper_left, lower_right, igColorConvertFloat4ToU32(poulpe_theme_dark.visual_select_background), 0.0f, 0);
    }

    return POULPE_ERROR_NONE;
}

void poulpe_selection_set_textview(struct poulpe_selection *selection, struct poulpe_textview *textview)
{
    selection->textview = textview;
}

bool poulpe_selection_active(struct poulpe_selection *selection)
{
    return !(selection->current.start_glyph_index == selection->current.end_glyph_index && 
                selection->current.start_line_index == selection->current.end_line_index);
}

enum poulpe_error poulpe_selection_delete(struct poulpe_selection *selection)
{   
    uint32_t i = selection->ajusted.start_line_index;
    uint32_t line = i;
    while (i <= selection->ajusted.end_line_index)
    {
        if (selection->ajusted.start_line_index == selection->ajusted.end_line_index)
        {
            for (uint32_t j = selection->ajusted.start_glyph_index; j < selection->ajusted.end_glyph_index; j++)
                poulpe_textbuffer_line_erase(selection->textview->textbuffer, line, selection->ajusted.start_glyph_index);
            line++;
        }
        else if (i == selection->ajusted.start_line_index)
        {
            uint32_t line_size = poulpe_textbuffer_line_size(selection->textview->textbuffer, line);
            for (uint32_t j = selection->ajusted.start_glyph_index; j < line_size; j++)
                poulpe_textbuffer_line_erase(selection->textview->textbuffer, line, selection->ajusted.start_glyph_index);
            line++;
        }
        else if (i == selection->ajusted.end_line_index)
        {
            for (uint32_t j = 0; j < selection->ajusted.end_glyph_index; j++)
                poulpe_textbuffer_line_erase(selection->textview->textbuffer, line, 0);
            line++;
        }
        else
        {
            poulpe_textbuffer_text_erase(selection->textview->textbuffer, line);
        }
        i++;
    }

    if (selection->ajusted.start_line_index < poulpe_textbuffer_text_size(selection->textview->textbuffer) - 1)
    {
        for (uint32_t i = 0; i < poulpe_textbuffer_line_size(selection->textview->textbuffer, selection->ajusted.start_line_index + 1); i++)
        {
            struct poulpe_glyph *glyph = poulpe_textbuffer_line_at(selection->textview->textbuffer, selection->ajusted.start_line_index + 1, i);
            poulpe_textbuffer_line_push_back(selection->textview->textbuffer, selection->ajusted.start_line_index, glyph);
        }
        poulpe_textbuffer_text_erase(selection->textview->textbuffer, selection->ajusted.start_line_index + 1);
    }

    return POULPE_ERROR_NONE;
}

void poulpe_selection_clear(struct poulpe_selection *selection)
{
    selection->current = (struct poulpe_selection_area) {0};
}

void poulpe_selection_update_start(struct poulpe_selection *selection, ImVec2 position)
{        
    selection->current.start_line_index = position.x;
    selection->current.start_glyph_index = position.y;
    _update_selection(selection);
}

void poulpe_selection_update_end(struct poulpe_selection *selection, ImVec2 position)
{
    selection->current.end_line_index = position.x;
    selection->current.end_glyph_index = position.y;
    _update_selection(selection);
}

static void _update_selection(struct poulpe_selection *selection)
{
    if (selection->current.start_line_index < selection->current.end_line_index)
    {
        selection->ajusted.start_line_index = selection->current.start_line_index;
        selection->ajusted.end_line_index = selection->current.end_line_index;
    }
    else
    {
        selection->ajusted.start_line_index = selection->current.end_line_index;
        selection->ajusted.end_line_index = selection->current.start_line_index;
    }
       
    if (selection->ajusted.start_line_index == selection->ajusted.end_line_index)
    {
        selection->ajusted.start_glyph_index = selection->current.start_glyph_index < selection->current.end_glyph_index ? 
                                                    selection->current.start_glyph_index : selection->current.end_glyph_index;
        selection->ajusted.end_glyph_index = selection->current.start_glyph_index < selection->current.end_glyph_index ? 
                                                    selection->current.end_glyph_index : selection->current.start_glyph_index;
    }
    else
    {
        selection->ajusted.start_glyph_index = selection->current.start_line_index < selection->current.end_line_index ? 
                                                    selection->current.start_glyph_index : selection->current.end_glyph_index;
        selection->ajusted.end_glyph_index = selection->current.start_line_index < selection->current.end_line_index ? 
                                                    selection->current.end_glyph_index : selection->current.start_glyph_index;
    }
}