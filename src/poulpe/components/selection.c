#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/selection.h"
#include "poulpe/components/textedit.h"
#include "poulpe/components/textview.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"
#include "poulpe/textbuffer.h"

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
    if ( selection->ajusted.end_line_index < selection->textedit->line_start || 
          selection->textedit->line_end < selection->ajusted.start_line_index )
        return POULPE_ERROR_NONE;

    poulpe_text text = selection->textedit->textview->textbuffer->text;

    uint32_t start = fmax(selection->ajusted.start_line_index, selection->textedit->line_start);
    uint32_t end = fmin(selection->ajusted.end_line_index, selection->textedit->line_end);

    ImGuiStyle *style = igGetStyle();
    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);
    origin_screen_position.x += style->FramePadding.x;

    for (uint32_t i = start; i <= end; i++)
    {
        poulpe_line line = text[i];

        ImDrawList *draw_list = igGetWindowDrawList();
        ImVec2 space_size;
        ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);

        ImVec2 upper_left = {0};
        ImVec2 lower_right = {0};

        if (i == selection->ajusted.start_line_index && i == selection->ajusted.end_line_index)
        {
            ImVec2 start_size;
            ImFont_CalcTextSizeA(&start_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + selection->ajusted.start_glyph_index, NULL);
            ImVec2 end_size;
            ImFont_CalcTextSizeA(&end_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + selection->ajusted.end_glyph_index, NULL);
            upper_left = (ImVec2) {origin_screen_position.x + start_size.x, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + end_size.x, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else if (i == selection->ajusted.start_line_index)
        {
            ImVec2 full_size;
            ImFont_CalcTextSizeA(&full_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, NULL, NULL);
            full_size.x = fmax(full_size.x, space_size.x);
            ImVec2 subset_size;
            ImFont_CalcTextSizeA(&subset_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + selection->ajusted.start_glyph_index, NULL);
            upper_left = (ImVec2) {origin_screen_position.x + subset_size.x, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + full_size.x, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else if (i == selection->ajusted.end_line_index)
        {
            ImVec2 subset_size;
            ImFont_CalcTextSizeA(&subset_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + selection->ajusted.end_glyph_index, NULL);
            subset_size.x = fmax(subset_size.x, space_size.x);
            upper_left = (ImVec2) {origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + subset_size.x, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else
        {
            ImVec2 text_size;
            ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, NULL, NULL);
            text_size.x = fmax(text_size.x, space_size.x);
            upper_left = (ImVec2) {origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + text_size.x, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }

        ImVec4 color = igIsWindowFocused(ImGuiFocusedFlags_ChildWindows) ? poulpe_theme_dark.hidden_text : poulpe_theme_dark.visual_select_background;
        ImDrawList_AddRectFilled(draw_list, upper_left, lower_right, igColorConvertFloat4ToU32(color), 0.0f, 0);
    }

    return POULPE_ERROR_NONE;
}

void poulpe_selection_set_textedit(struct poulpe_selection *selection, struct poulpe_textedit *textedit)
{
    selection->textedit = textedit;
}

bool poulpe_selection_active(struct poulpe_selection *selection)
{
    return selection->current.start_glyph_index != selection->current.end_glyph_index || 
            selection->current.start_line_index != selection->current.end_line_index;
}

enum poulpe_error poulpe_selection_delete(struct poulpe_selection *selection)
{   
    struct poulpe_textbuffer *textbuffer = selection->textedit->textview->textbuffer;
    poulpe_text text = textbuffer->text;
        
    uint32_t i = selection->ajusted.start_line_index;
    uint32_t j = i;
    while (i <= selection->ajusted.end_line_index)
    {
        if (selection->ajusted.start_line_index == selection->ajusted.end_line_index)
        {
            poulpe_line_erase_range(text[j], selection->ajusted.start_glyph_index, selection->ajusted.end_glyph_index);
            j++;
        }
        else if (i == selection->ajusted.start_line_index)
        {
            uint32_t line_size = poulpe_textbuffer_eof_size(textbuffer, text[j]);
            poulpe_line_erase_range(text[j], selection->ajusted.start_glyph_index, line_size);
            j++;
        }
        else if (i == selection->ajusted.end_line_index)
        {
            poulpe_line_erase_range(text[j], 0, selection->ajusted.end_glyph_index);
            j++;
        }
        else
        {
            poulpe_text_erase(text, j);
        }
        i++;
    }

    if (selection->ajusted.start_line_index < poulpe_text_size(text) - 1)
    {
        uint32_t line_size = poulpe_textbuffer_eof_size(textbuffer, text[selection->ajusted.start_line_index]);
        uint32_t next_line_size = poulpe_textbuffer_eof_size(textbuffer, text[selection->ajusted.start_line_index + 1]);
        text[selection->ajusted.start_line_index] = poulpe_line_insert(text[selection->ajusted.start_line_index], line_size, text[selection->ajusted.start_line_index + 1], text[selection->ajusted.start_line_index + 1] + next_line_size);
        poulpe_text_erase(text, selection->ajusted.start_line_index + 1);
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