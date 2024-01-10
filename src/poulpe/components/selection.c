#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <cimgui.h>

#include <sake/macro.h>
#include <sake/utils.h>

#include "poulpe/components/selection.h"
#include "poulpe/components/textedit.h"
#include "poulpe/components/textview.h"

#include "poulpe/log.h"
#include "poulpe/style.h"
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
    if ( selection->ajusted.end.x < selection->textedit->line_start || 
          selection->textedit->line_end < selection->ajusted.start.x )
        return POULPE_ERROR_NONE;

    poulpe_text text = selection->textedit->textview->textbuffer->text;

    uint32_t start = fmax(selection->ajusted.start.x, selection->textedit->line_start);
    uint32_t end = fmin(selection->ajusted.end.x, selection->textedit->line_end);

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

        if (i == selection->ajusted.start.x && i == selection->ajusted.end.x)
        {
            ImVec2 start_size;
            ImFont_CalcTextSizeA(&start_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + (uint32_t) selection->ajusted.start.y, NULL);
            ImVec2 end_size;
            ImFont_CalcTextSizeA(&end_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + (uint32_t) selection->ajusted.end.y, NULL);
            upper_left = (ImVec2) {origin_screen_position.x + start_size.x, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + end_size.x, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else if (i == selection->ajusted.start.x)
        {
            ImVec2 full_size;
            ImFont_CalcTextSizeA(&full_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, NULL, NULL);
            full_size.x = fmax(full_size.x, space_size.x);
            ImVec2 subset_size;
            ImFont_CalcTextSizeA(&subset_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + (uint32_t) selection->ajusted.start.y, NULL);
            upper_left = (ImVec2) {origin_screen_position.x + subset_size.x, origin_screen_position.y + i * igGetTextLineHeight()};
            lower_right = (ImVec2) {origin_screen_position.x + full_size.x, origin_screen_position.y + (i + 1) * igGetTextLineHeight() };
        }
        else if (i == selection->ajusted.end.x)
        {
            ImVec2 subset_size;
            ImFont_CalcTextSizeA(&subset_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + (uint32_t) selection->ajusted.end.y, NULL);
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

        ImVec4 color = igIsWindowFocused(ImGuiFocusedFlags_ChildWindows) ? poulpe_style.theme->highlighted_text : poulpe_style.theme->dimmed_text;
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
    return selection->current.start.y != selection->current.end.y || 
            selection->current.start.x != selection->current.end.x;
}

enum poulpe_error poulpe_selection_delete(struct poulpe_selection *selection)
{   
    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = selection->textedit->textview->textbuffer;
        
    uint32_t i = selection->ajusted.start.x;
    uint32_t j = i;
    while (i <= selection->ajusted.end.x)
    {
        if (selection->ajusted.start.x == selection->ajusted.end.x)
        {
            error = poulpe_textbuffer_line_erase_range(textbuffer, j, selection->ajusted.start.y, selection->ajusted.end.y);
            if (error != POULPE_ERROR_NONE)
                return error;
            j++;
        }
        else if (i == selection->ajusted.start.x)
        {
            uint32_t line_size = poulpe_textbuffer_line_eof_size(textbuffer, j);
            error = poulpe_textbuffer_line_erase_range(textbuffer, j, selection->ajusted.start.y, line_size);
            if (error != POULPE_ERROR_NONE)
                return error;
            j++;
        }
        else if (i == selection->ajusted.end.x)
        {
            error = poulpe_textbuffer_line_erase_range(textbuffer, j, 0, selection->ajusted.end.y);
            if (error != POULPE_ERROR_NONE)
                return error;
            j++;
        }
        else
        {
            error = poulpe_textbuffer_text_erase(textbuffer, j);
            if (error != POULPE_ERROR_NONE)
                return error;
        }
        i++;
    }

    if (selection->ajusted.start.x < poulpe_textbuffer_text_size(textbuffer) - 1)
    {
        uint32_t line_size = poulpe_textbuffer_line_eof_size(textbuffer, selection->ajusted.start.x);
        uint32_t next_line_size = poulpe_textbuffer_line_eof_size(textbuffer, selection->ajusted.start.x + 1);
        error = poulpe_textbuffer_line_insert(textbuffer,
                                              selection->ajusted.start.x,
                                              line_size,
                                              poulpe_textbuffer_text_at(textbuffer, selection->ajusted.start.x + 1),
                                              poulpe_textbuffer_text_at(textbuffer, selection->ajusted.start.x + 1) + next_line_size);
        if (error != POULPE_ERROR_NONE)
            return error;
        
        error = poulpe_textbuffer_text_erase(textbuffer, selection->ajusted.start.x + 1);
        if (error != POULPE_ERROR_NONE)
            return error;
    }

    return POULPE_ERROR_NONE;
}

void poulpe_selection_clear(struct poulpe_selection *selection)
{
    selection->current = (struct poulpe_selection_area) {0};
}

void poulpe_selection_update_start(struct poulpe_selection *selection, ImVec2 position)
{        
    selection->current.start.x = position.x;
    selection->current.start.y = position.y;
    _update_selection(selection);
}

void poulpe_selection_update_end(struct poulpe_selection *selection, ImVec2 position)
{
    selection->current.end.x = position.x;
    selection->current.end.y = position.y;
    _update_selection(selection);
}

sake_string poulpe_selection_to_str(struct poulpe_selection *selection)
{
    struct poulpe_textbuffer *textbuffer = selection->textedit->textview->textbuffer;

    sake_string str = sake_string_new("", NULL);
    if (!str)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate string");
        return NULL;
    }

    uint32_t i = selection->ajusted.start.x;
    while (i <= selection->ajusted.end.x)
    {
        if (selection->ajusted.start.x == selection->ajusted.end.x)
        {
            str = sake_string_push_back(str,
                                        poulpe_textbuffer_text_at(textbuffer, i) + (uint32_t)selection->ajusted.start.y,
                                        poulpe_textbuffer_text_at(textbuffer, i) + (uint32_t)selection->ajusted.end.y);
            if (!str)
            {
                POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot push back selection string");
                return NULL;
            }
        }
        else if (i == selection->ajusted.start.x)
        {
            uint32_t line_size = poulpe_textbuffer_line_raw_size(textbuffer, i);
            str = sake_string_push_back(str,
                                        poulpe_textbuffer_text_at(textbuffer, i) + (uint32_t)selection->ajusted.start.y,
                                        poulpe_textbuffer_text_at(textbuffer, i) + line_size);
            if (!str)
            {
                POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot push back selection string");
                return NULL;
            }
        }
        else if (i == selection->ajusted.end.x)
        {
            str = sake_string_push_back(str,
                                        poulpe_textbuffer_text_at(textbuffer, i),
                                        poulpe_textbuffer_text_at(textbuffer, i) + (uint32_t)selection->ajusted.end.y);
            if (!str)
            {
                POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot push back selection string");
                return NULL;
            }
        }
        else
        {
            str = sake_string_push_back(str, poulpe_textbuffer_text_at(textbuffer, i), NULL);
            if (!str)
            {
                POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot push back selection string");
                return NULL;
            }
        }
        i++;
    }
    return str;
}

void poulpe_selection_move_right(struct poulpe_selection *selection)
{
    struct poulpe_textbuffer *textbuffer = selection->textedit->textview->textbuffer;
    poulpe_text text = textbuffer->text;

    {
        poulpe_line line = text[(uint32_t)selection->ajusted.start.x];
        uint32_t line_size = poulpe_textbuffer_line_eof_size(textbuffer, selection->ajusted.start.x);
        if (selection->ajusted.start.y < line_size)
            selection->ajusted.start.y += sake_utils_utf8_length(line[(uint32_t)selection->ajusted.start.y]);
    }

    {
        poulpe_line line = text[(uint32_t)selection->ajusted.end.x];
        uint32_t line_size = poulpe_textbuffer_line_eof_size(textbuffer, selection->ajusted.end.x);
        if (selection->ajusted.end.y < line_size)
            selection->ajusted.end.y += sake_utils_utf8_length(line[(uint32_t)selection->ajusted.end.y]);
    }
}

static void _update_selection(struct poulpe_selection *selection)
{   
    if (selection->current.start.x < selection->current.end.x)
    {
        selection->ajusted.start.x = selection->current.start.x;
        selection->ajusted.end.x = selection->current.end.x;
    }
    else
    {
        selection->ajusted.start.x = selection->current.end.x;
        selection->ajusted.end.x = selection->current.start.x;
    }
       
    if (selection->ajusted.start.x == selection->ajusted.end.x)
    {
        selection->ajusted.start.y = selection->current.start.y < selection->current.end.y ? 
                                                    selection->current.start.y : selection->current.end.y;
        selection->ajusted.end.y = selection->current.start.y < selection->current.end.y ? 
                                                    selection->current.end.y : selection->current.start.y;
    }
    else
    {
        selection->ajusted.start.y = selection->current.start.x < selection->current.end.x ? 
                                                    selection->current.start.y : selection->current.end.y;
        selection->ajusted.end.y = selection->current.start.x < selection->current.end.x ? 
                                                    selection->current.end.y : selection->current.start.y;
    }
}