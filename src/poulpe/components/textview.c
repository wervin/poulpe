#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "poulpe/components/textview.h"
#include "poulpe/components/cursor.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"

#include "sake/vector.h"
#include "sake/string.h"
#include "sake/macro.h"

struct poulpe_textview * poulpe_textview_new(void)
{
    struct poulpe_textview *textview;

    textview = calloc(1, sizeof(struct poulpe_textview));
    if (!textview)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate textview");
        return NULL;
    }

    textview->cursor = (struct poulpe_cursor *) poulpe_component_new(POULPE_COMPONENT_TYPE_CURSOR);
    poulpe_cursor_set_textview(textview->cursor, textview);
    return textview;
}

void poulpe_textview_free(struct poulpe_textview *textview)
{
    poulpe_component_free((struct poulpe_component *) textview->cursor);
    free(textview);
}

enum poulpe_error poulpe_textview_notify(struct poulpe_textview *textview, struct poulpe_event *event)
{
    SAKE_MACRO_UNUSED(textview);
    switch (event->type)
    {
    case POULPE_EVENT_TYPE_MOUSE:
    {
        igSetMouseCursor(ImGuiMouseCursor_TextInput);
        poulpe_component_notify((struct poulpe_component *) textview->cursor, event);
        break;
    }
    case POULPE_EVENT_TYPE_KEYBOARD:
    {
        struct poulpe_event_keyboard *keyboard_event = (struct poulpe_event_keyboard *)event;
        struct poulpe_glyph glyph = {.c = keyboard_event->c};
        textview->text[textview->cursor->line_index] = poulpe_line_insert(textview->text[textview->cursor->line_index], textview->cursor->glyph_index, &glyph);
        if (!textview->text[textview->cursor->line_index])
        {
            POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot insert new characters");
            return POULPE_ERROR_MEMORY;
        }
        poulpe_cursor_move_right(textview->cursor);
        break;
    }
    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown event type");
        return POULPE_ERROR_UNKNOWN;
    }
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textview_draw(struct poulpe_textview *textview)
{
    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);
    float maximum_width = 0;

    poulpe_component_draw((struct poulpe_component *)textview->cursor);

    for (uint32_t i = 0; i < sake_vector_size(textview->text); i++)
    {
        ImVec2 line_start_position = {origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};
        poulpe_line line = textview->text[i];
        char buffer[sake_vector_size(line) + 1];

        ImVec2 line_size;
        sake_string line_str = poulple_line_to_str(line);
        ImFont_CalcTextSizeA(&line_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line_str, NULL, NULL);
        if (line_size.x > maximum_width)
            maximum_width = line_size.x;
        sake_string_free(line_str);

        for (uint32_t j = 0; j < sake_vector_size(line); j++)
        {
            buffer[j] = line[j].c;
            buffer[j+1] = '\0';
            ImVec2 text_size;
            ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, buffer, NULL, NULL);
            ImVec2 space_size;
            ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);
            if (line[j].c == ' ')
            {
                float font_size = igGetFontSize();
                ImVec2 center = {line_start_position.x + text_size.x - 0.5 * space_size.x, line_start_position.y + 0.5 * font_size};
                float radius = 1.5f;
                ImDrawList *draw_list = igGetWindowDrawList();
                ImDrawList_AddCircleFilled(draw_list,
                                           center,
                                           radius,
                                           igColorConvertFloat4ToU32(poulpe_theme_dark.whitespace),
                                           0);
            }
        }
        buffer[sake_vector_size(line)] = '\0';
        
        if (strlen(buffer))
        {
            ImDrawList *draw_list = igGetWindowDrawList();
            ImDrawList_AddText_Vec2(draw_list,
                                    line_start_position,
                                    igColorConvertFloat4ToU32(poulpe_theme_dark.text),
                                    buffer,
                                    NULL);
        }
    }
   
    ImVec2 size = {maximum_width, sake_vector_size(textview->text) * igGetTextLineHeight()};
    ImVec2 upper_left = {origin_screen_position.x, origin_screen_position.y};
    ImVec2 lower_right = {origin_screen_position.x + size.x, origin_screen_position.y + size.y};
    ImRect rect = {upper_left, lower_right};
    igItemSize_Vec2(size, -1.0f);
    igItemAdd(rect, 0, NULL, 0);
    
    return POULPE_ERROR_NONE;
}


void poulpe_textview_set_text(struct poulpe_textview *textview, poulpe_text text)
{
    textview->text = text;
}