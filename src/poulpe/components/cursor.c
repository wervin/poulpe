#include <stdlib.h>
#include <float.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/cursor.h"
#include "poulpe/components/textedit.h"
#include "poulpe/components/textview.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"
#include "poulpe/textbuffer.h"

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
    SAKE_MACRO_UNUSED(cursor);
    SAKE_MACRO_UNUSED(event);
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_cursor_draw(struct poulpe_cursor *cursor)
{
    if (!igIsWindowFocused(ImGuiFocusedFlags_ChildWindows))
        return POULPE_ERROR_NONE;
    
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    long seconds = current_time.tv_sec - cursor->timer.tv_sec;
    long microseconds = current_time.tv_usec - cursor->timer.tv_usec;
    double elasped = seconds * 1e6 + microseconds;
    if (elasped < CURSOR_BLINK_DELAY)
    {
        ImGuiStyle *style = igGetStyle();
        ImVec2 origin_screen_position;
        igGetCursorScreenPos(&origin_screen_position);
        origin_screen_position.x += style->FramePadding.x;

        float text_size = poulpe_textbuffer_line_subset_textsize(cursor->textedit->textview->textbuffer, cursor->position.x, 0, cursor->position.y);
        float text_start = origin_screen_position.x + text_size;
        float cursor_width = 1.5f;

        ImVec2 start = {text_start, origin_screen_position.y + cursor->position.x * igGetTextLineHeight()};
        ImVec2 end = {text_start, origin_screen_position.y + (cursor->position.x + 1) * igGetTextLineHeight()};
        ImDrawList *draw_list = igGetWindowDrawList();
        ImDrawList_AddLine(draw_list, start, end, igColorConvertFloat4ToU32(poulpe_theme_dark.cursor_normal), cursor_width);
    }

    if (elasped > CURSOR_BLINK_DELAY * 2)
        cursor->timer = current_time;

    return POULPE_ERROR_NONE;
}

void poulpe_cursor_set_textedit(struct poulpe_cursor *cursor, struct poulpe_textedit *textedit)
{
    cursor->textedit = textedit;
}

void poulpe_cursor_update(struct poulpe_cursor *cursor)
{
    uint32_t line_length = poulpe_textbuffer_line_size(cursor->textedit->textview->textbuffer, cursor->position.x);
    cursor->position.y = cursor->position.y > line_length ? line_length : cursor->position.y;
}

void poulpe_cursor_reset(struct poulpe_cursor *cursor)
{
    gettimeofday(&cursor->timer, NULL);
}

void poulpe_cursor_move_up(struct poulpe_cursor *cursor)
{
    if (cursor->position.x > 0)
        cursor->position.x--;
    poulpe_cursor_reset(cursor);
}

void poulpe_cursor_move_down(struct poulpe_cursor *cursor)
{
    if (cursor->position.x < (poulpe_textbuffer_text_size(cursor->textedit->textview->textbuffer) - 1))
        cursor->position.x++;
    poulpe_cursor_reset(cursor);
}

void poulpe_cursor_move_left(struct poulpe_cursor *cursor)
{
    if (cursor->position.y > 0)
        cursor->position.y--;
    poulpe_cursor_reset(cursor);
}

void poulpe_cursor_move_right(struct poulpe_cursor *cursor)
{
    if (cursor->position.y < (poulpe_textbuffer_line_size(cursor->textedit->textview->textbuffer, cursor->position.x)))
        cursor->position.y++;
    poulpe_cursor_reset(cursor);
}

void poulpe_cursor_update_position(struct poulpe_cursor *cursor, ImVec2 position)
{        
    cursor->position.x = position.x;
    cursor->position.y = position.y;
    poulpe_cursor_reset(cursor);
}