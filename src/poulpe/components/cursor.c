#include <stdlib.h>
#include <float.h>

#include <cimgui.h>

#include "poulpe/components/cursor.h"
#include "poulpe/components/textview.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"

#include "sake/vector.h"

#define CURSOR_BLINK_DELAY      750000

struct poulpe_cursor * poulpe_cursor_new(void)
{
    struct poulpe_cursor *cursor;

    cursor = calloc(1, sizeof(struct poulpe_cursor));
    if (!cursor)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate cursor");
        return NULL;
    }

    gettimeofday(&cursor->timer, NULL);

    return cursor;
}

void poulpe_cursor_free(struct poulpe_cursor *cursor)
{
    free(cursor);
}

enum poulpe_error poulpe_cursor_notify(struct poulpe_cursor *cursor, struct poulpe_event *event)
{
    switch (event->type)
    {
    case POULPE_EVENT_TYPE_MOUSE:
    {
        struct poulpe_event_mouse *mouse_event = (struct poulpe_event_mouse *) event;
        if (mouse_event->left_clicked)
            poulpe_cursor_update_position(cursor, mouse_event->position);
        break;
    }
    case POULPE_EVENT_TYPE_KEYBOARD:
    {
        break;
    }
    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown event type");
        return POULPE_ERROR_UNKNOWN;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_cursor_draw(struct poulpe_cursor *cursor)
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    long seconds = current_time.tv_sec - cursor->timer.tv_sec;
    long microseconds = current_time.tv_usec - cursor->timer.tv_usec;
    double elasped = seconds * 1e6 + microseconds;

    if (elasped < CURSOR_BLINK_DELAY)
    {
        ImVec2 origin_screen_position;
        igGetCursorScreenPos(&origin_screen_position);

        float font_size = igGetFontSize();

        ImVec2 text_size;
        sake_string line_str = poulple_line_to_str(cursor->textview->text[cursor->line_index]);
        ImFont_CalcTextSizeA(&text_size, igGetFont(), font_size, FLT_MAX, -1.0f, line_str, line_str + cursor->glyph_index, NULL);
        sake_string_free(line_str);

        float text_start = origin_screen_position.x + text_size.x;
        float cursor_width = 2.0;

        ImVec2 start = {text_start - cursor_width * 0.5, origin_screen_position.y + cursor->line_index * igGetTextLineHeight()};
        ImVec2 end = {text_start + cursor_width * 0.5, origin_screen_position.y + (cursor->line_index + 1) * igGetTextLineHeight()};
        ImDrawList *draw_list = igGetWindowDrawList();
        ImDrawList_AddRectFilled(draw_list, start, end, igColorConvertFloat4ToU32(poulpe_theme_dark.cursor_normal), 0.0f, 0);
    }

    if (elasped > CURSOR_BLINK_DELAY * 2)
        cursor->timer = current_time;

    return POULPE_ERROR_NONE;
}

void poulpe_cursor_set_textview(struct poulpe_cursor *cursor, struct poulpe_textview *textview)
{
    cursor->textview = textview;
}

void poulpe_cursor_reset(struct poulpe_cursor *cursor)
{
    gettimeofday(&cursor->timer, NULL);
}

void poulpe_cursor_move_up(struct poulpe_cursor *cursor)
{

}

void poulpe_cursor_move_down(struct poulpe_cursor *cursor)
{

}

void poulpe_cursor_move_left(struct poulpe_cursor *cursor)
{
    if (cursor->glyph_index > 0)
        cursor->glyph_index--;
    poulpe_cursor_reset(cursor);
}

void poulpe_cursor_move_right(struct poulpe_cursor *cursor)
{
    if (cursor->glyph_index < sake_vector_size(cursor->textview->text[cursor->line_index]))
        cursor->glyph_index++;
    poulpe_cursor_reset(cursor);
}

void poulpe_cursor_update_position(struct poulpe_cursor *cursor, ImVec2 position)
{
    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);

    uint32_t line_index = 0;
    uint32_t last_line = sake_vector_size(cursor->textview->text) - 1;
    while (line_index < last_line && position.y > origin_screen_position.y + (line_index + 1) * igGetTextLineHeight())
        line_index++;
    
    poulpe_line line = cursor->textview->text[line_index];
    sake_string line_str = poulple_line_to_str(line);
    uint32_t glyph_index = 0;
    while (glyph_index < sake_vector_size(line))
    {
        ImVec2 text_size, text_size_advance;
        ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line_str, line_str + glyph_index, NULL);
        ImFont_CalcTextSizeA(&text_size_advance, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line_str, line_str + glyph_index + 1, NULL);
        if (position.x < origin_screen_position.x + text_size_advance.x)
        {
            float diff_text = position.x - origin_screen_position.x - text_size.x;
            float diff_text_advance = origin_screen_position.x + text_size_advance.x - position.x;
            glyph_index = diff_text < diff_text_advance ? glyph_index: glyph_index + 1;
            break;
        }
        glyph_index++;
    }

    sake_string_free(line_str);
        
    cursor->line_index = line_index;
    cursor->glyph_index = glyph_index;
    poulpe_cursor_reset(cursor);
}