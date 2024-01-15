#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include <cimgui.h>

#include <tree_sitter/api.h>

#include <sake/macro.h>
#include <sake/utils.h>
#include <sake/vector.h>

#include "poulpe/components/textedit.h"
#include "poulpe/components/selection.h"
#include "poulpe/components/cursor.h"
#include "poulpe/components/textview.h"
#include "poulpe/components/indentinfo.h"
#include "poulpe/components/statusbar.h"
#include "poulpe/components/finder.h"
#include "poulpe/editor.h"

#include "poulpe/log.h"
#include "poulpe/style.h"
#include "poulpe/textbuffer.h"
#include "poulpe/io.h"
#include "poulpe/history.h"

static void _ensure_cursor_visiblity(struct poulpe_textedit *textedit);
static void _ensure_find_results_visibility(struct poulpe_textedit * textedit);
static void _get_coordinates(struct poulpe_textedit *textedit, ImVec2 mouse_position, ImVec2 *position);
static enum poulpe_error _update_view(struct poulpe_textedit *textedit);

static void _draw_highlighted_tree(struct poulpe_textedit *textedit);
static void _draw_raw_tree(struct poulpe_textedit *textedit);
static void _draw_lines(struct poulpe_textedit *textedit, TSPoint from, TSPoint to, ImU32 color);
static void _draw_background(struct poulpe_textedit *textedit);
static void _draw_find_results(struct poulpe_textedit *textedit);
static void _draw_invisibles(struct poulpe_textedit *textedit);
static float _get_maximum_width(struct poulpe_textedit *textedit);
static void _add_widget(struct poulpe_textedit *textedit);

static enum poulpe_error _handle_mouse(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event);
static enum poulpe_error _handle_mouse_left_clicked(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event);
static enum poulpe_error _handle_mouse_left_double_clicked(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event);
static enum poulpe_error _handle_mouse_left_triple_clicked(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event);
static enum poulpe_error _handle_mouse_drag(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event);

static enum poulpe_error _handle_keyboard(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_save(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_undo(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_redo(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_cut(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_copy(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_paste(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_select_all(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_alt_up(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_alt_down(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_shift_up(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_shift_down(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_shift_left(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_shift_right(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_delete(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_backspace(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_left(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_right(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_up(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_down(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_enter(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_shift_tab(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_tab(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_default(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);

static enum poulpe_error _push_new_action(struct poulpe_textedit *textedit,
                                          struct poulpe_event_keyboard *event,
                                          enum poulpe_error (*cb) (struct poulpe_textedit *, struct poulpe_event_keyboard *));

struct poulpe_textedit * poulpe_textedit_new(void)
{
    struct poulpe_textedit *textedit;

    textedit = calloc(1, sizeof(struct poulpe_textedit));
    if (!textedit)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate textedit");
        return NULL;
    }

    textedit->cursor = (struct poulpe_cursor *) poulpe_component_new(POULPE_COMPONENT_TYPE_CURSOR);
    if (!textedit->cursor)
        return NULL;

    poulpe_cursor_set_textedit(textedit->cursor, textedit);

    textedit->selection = (struct poulpe_selection *) poulpe_component_new(POULPE_COMPONENT_TYPE_SELECTION);
    if (!textedit->selection)
        return NULL;

    poulpe_selection_set_textedit(textedit->selection, textedit);

    return textedit;
}

void poulpe_textedit_free(struct poulpe_textedit *textedit)
{
    poulpe_component_free((struct poulpe_component *) textedit->cursor);
    poulpe_component_free((struct poulpe_component *) textedit->selection);
    free(textedit);
}

enum poulpe_error poulpe_textedit_notify(struct poulpe_textedit *textedit, struct poulpe_event *event)
{
    if (!igIsWindowFocused(ImGuiFocusedFlags_ChildWindows))
        return POULPE_ERROR_NONE;
    
    switch (event->type)
    { 
    case POULPE_EVENT_TYPE_MOUSE:
        return _handle_mouse(textedit, (struct poulpe_event_mouse *) event);

    case POULPE_EVENT_TYPE_KEYBOARD:
        return _handle_keyboard(textedit, (struct poulpe_event_keyboard *)event);

    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown event type");
        return POULPE_ERROR_UNKNOWN;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textedit_draw(struct poulpe_textedit *textedit)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    igPushStyleVar_Vec2(ImGuiStyleVar_FramePadding, (ImVec2) {1.f, 0.f});

    igSameLine(0.0f, 0.0f);
    if (!igBeginChild_Str("Poulpe##textedit", (ImVec2) {0}, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysHorizontalScrollbar))
        goto end_child;

    error = poulpe_io_handle_keyboard((struct poulpe_component *) textedit);
    if (error != POULPE_ERROR_NONE)
        goto end_child;
    
    error = poulpe_io_handle_mouse((struct poulpe_component *) textedit);
    if (error != POULPE_ERROR_NONE)
        goto end_child;

    _ensure_find_results_visibility(textedit);
    
    _update_view(textedit);

    _draw_background(textedit);

    _draw_find_results(textedit);

    if (poulpe_selection_active(textedit->selection))
    {
        error = poulpe_component_draw((struct poulpe_component *)textedit->selection);
        if (error != POULPE_ERROR_NONE)
            goto end_child;
    }

    error = poulpe_component_draw((struct poulpe_component *)textedit->cursor);
    if (error != POULPE_ERROR_NONE)
        goto end_child;

    if (textedit->textview->textbuffer->parser)
        _draw_highlighted_tree(textedit);
    else
        _draw_raw_tree(textedit);

    _draw_invisibles(textedit);
    
    _add_widget(textedit);
    
end_child:
    igEndChild();
    igPopStyleVar(1);

    return error;
}

void poulpe_textedit_set_textview(struct poulpe_textedit *textedit, struct poulpe_textview *textview)
{
    textedit->textview = textview;
}

void poulpe_textedit_ensure_visiblity(struct poulpe_textedit *textedit, ImVec2 position)
{
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;
    const char *line = poulpe_textbuffer_text_at(textbuffer, position.x);

    ImGuiWindow *window = igGetCurrentWindowRead();
    ImVec2 content;
    igGetContentRegionAvail(&content);

    ImVec2 text_size;
    ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + (uint32_t) position.y, NULL);
    ImVec2 space_size;
    ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);

    uint32_t start = window->Scroll.y / igGetTextLineHeight();
    uint32_t end = (window->Scroll.y + content.y) / igGetTextLineHeight() - 1;
    end = fmin(end, poulpe_textbuffer_text_size(textbuffer));
    end = fmax(end, 0);

    if (position.x < start)
    {
        float scroll_y = 0.0f;
        scroll_y = position.x * igGetTextLineHeight();
        window->DC.CursorPos.y += window->Scroll.y - scroll_y;
        window->Scroll.y = scroll_y;
    }

    if (position.x > end)
    {
        float scroll_y = 0.0f;
        scroll_y = (position.x  + 1) * igGetTextLineHeight() - content.y;
        window->DC.CursorPos.y += window->Scroll.y - scroll_y;
        window->Scroll.y = scroll_y;
    }

    if (text_size.x < window->Scroll.x)
        window->Scroll.x = text_size.x;

    if (text_size.x > (window->Scroll.x + content.x))
        window->Scroll.x = text_size.x + space_size.x - content.x;
}

static void _ensure_cursor_visiblity(struct poulpe_textedit *textedit)
{
    poulpe_textedit_ensure_visiblity(textedit, textedit->cursor->position);
}

static void _ensure_find_results_visibility(struct poulpe_textedit * textedit)
{
    struct poulpe_finder *finder = textedit->textview->editor->statusbar->finder;
    if (finder->focus && sake_vector_size(finder->results))
    {
        poulpe_textedit_ensure_visiblity(textedit, finder->results[finder->result_index]);
        finder->focus = false;
    }
}

static void _get_coordinates(struct poulpe_textedit *textedit, ImVec2 mouse_position, ImVec2 *position)
{
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    ImGuiStyle *style = igGetStyle();

    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);
    origin_screen_position.x += style->FramePadding.x;

    uint32_t line_index = 0;
    uint32_t last_line = poulpe_textbuffer_text_size(textbuffer) - 1;
    while (line_index < last_line && mouse_position.y > origin_screen_position.y + (line_index + 1) * igGetTextLineHeight())
        line_index++;
    
    const char *line = poulpe_textbuffer_text_at(textbuffer, line_index);
    uint32_t line_raw_size = poulpe_textbuffer_line_eof_size(textbuffer, line_index);
    uint32_t raw_glyph_index = 0;
    while (raw_glyph_index < line_raw_size)
    {
        ImVec2 text_size;
        ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + raw_glyph_index, NULL);
        ImVec2 text_size_advance;
        ImFont_CalcTextSizeA(&text_size_advance, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + raw_glyph_index + 1, NULL);

        /* Find the closest glyph */
        if (mouse_position.x < origin_screen_position.x + text_size_advance.x)
        {
            float diff_text = mouse_position.x - origin_screen_position.x - text_size.x;
            float diff_text_advance = origin_screen_position.x + text_size_advance.x - mouse_position.x;
            raw_glyph_index = diff_text < diff_text_advance ? raw_glyph_index : raw_glyph_index + sake_utils_utf8_length(line[raw_glyph_index]);
            break;
        }

        raw_glyph_index += sake_utils_utf8_length(line[raw_glyph_index]);
    }
    position->x = line_index;
    position->y = raw_glyph_index;
}

static enum poulpe_error _update_view(struct poulpe_textedit *textedit)
{
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    ImGuiWindow *window = igGetCurrentWindowRead();
    ImVec2 content;
    igGetContentRegionAvail(&content);
    
    textedit->line_start = window->Scroll.y / igGetTextLineHeight();
    textedit->line_end = ((window->Scroll.y + content.y) / igGetTextLineHeight()) + 2;
    textedit->line_end = fmin(textedit->line_end, poulpe_textbuffer_text_size(textbuffer));
    textedit->line_end = fmax(textedit->line_end, 0);

    textedit->scroll_y = window->Scroll.y;
    
    return POULPE_ERROR_NONE;
}

static void _draw_highlighted_tree(struct poulpe_textedit *textedit)
{
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    ts_query_cursor_exec(textbuffer->cursor, textbuffer->query, ts_tree_root_node(textbuffer->tree));
    
    uint32_t cursor_start_bytes = 0;
    uint32_t cursor_end_bytes = 0;
    TSPoint cursor_start_point = {0};
    TSPoint cursor_end_point = {0};
    TSQueryMatch match = {0};
    while (ts_query_cursor_next_match(textbuffer->cursor, &match))
    {   
        cursor_start_point = ts_node_start_point(match.captures[0].node);
        cursor_start_bytes = ts_node_start_byte(match.captures[0].node);
        if (cursor_start_bytes < cursor_end_bytes)
            continue;

        _draw_lines(textedit, cursor_end_point, cursor_start_point, igColorConvertFloat4ToU32(poulpe_style.theme->primary_text));

        cursor_end_bytes = ts_node_end_byte(match.captures[0].node);
        cursor_end_point = ts_node_end_point(match.captures[0].node);

        uint32_t t;
        const char *pattern = ts_query_capture_name_for_id(textbuffer->query, match.captures[0].index, &t);
        ImU32 color = poulpe_language_pattern_color(textbuffer->language_type, pattern);
        _draw_lines(textedit, cursor_start_point, cursor_end_point, color);
    }

    uint32_t last_row = poulpe_textbuffer_text_size(textbuffer) - 1;
    uint32_t last_col = poulpe_textbuffer_line_raw_size(textbuffer, last_row);
    cursor_start_point = cursor_end_point;
    cursor_end_point = (TSPoint) { last_row, last_col };
    _draw_lines(textedit, cursor_start_point, cursor_end_point, igColorConvertFloat4ToU32(poulpe_style.theme->primary_text));
}

static void _draw_raw_tree(struct poulpe_textedit *textedit)
{
    TSPoint cursor_start_point = {textedit->line_start, 0};
    uint32_t last_row = textedit->line_end - 1;
    uint32_t last_col = poulpe_textbuffer_line_raw_size(textedit->textview->textbuffer, last_row);
    TSPoint cursor_end_point = {last_row, last_col};
    _draw_lines(textedit, cursor_start_point, cursor_end_point, igColorConvertFloat4ToU32(poulpe_style.theme->primary_text));
}

static void _draw_lines(struct poulpe_textedit *textedit, TSPoint from, TSPoint to, ImU32 color)
{   
    ImGuiStyle *style = igGetStyle();

    ImDrawList *draw_list = igGetWindowDrawList();
    
    if (!(to.row >= textedit->line_start && from.row <= textedit->line_end))
        return;
    
    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);
    origin_screen_position.x += style->FramePadding.x;

    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;
    poulpe_text text = textbuffer->text;

    for (uint32_t i = from.row; i <= to.row; i++)
    {
        poulpe_line line = text[i];

        ImVec2 space_size;
        ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);

        ImVec2 text_start_position = {0};

        if (i == from.row && i == to.row)
        {
            ImVec2 start_size;
            ImFont_CalcTextSizeA(&start_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + from.column, NULL);
            text_start_position = (ImVec2){origin_screen_position.x + start_size.x, origin_screen_position.y + i * igGetTextLineHeight()};
            ImDrawList_AddText_Vec2(draw_list, text_start_position, color, line + from.column, line + to.column);
        }
        else if (i == from.row)
        {
            ImVec2 subset_size;
            ImFont_CalcTextSizeA(&subset_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + from.column, NULL);
            text_start_position = (ImVec2){origin_screen_position.x + subset_size.x, origin_screen_position.y + i * igGetTextLineHeight()};
            ImDrawList_AddText_Vec2(draw_list, text_start_position, color, line + from.column, NULL);
        }
        else if (i == to.row)
        {
            ImVec2 subset_size;
            ImFont_CalcTextSizeA(&subset_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + to.column, NULL);
            text_start_position = (ImVec2){origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};
            ImDrawList_AddText_Vec2(draw_list, text_start_position, color, line, line + to.column);
        }
        else
        {
            ImVec2 text_size;
            ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, NULL, NULL);
            text_start_position = (ImVec2){origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};
            ImDrawList_AddText_Vec2(draw_list, text_start_position, color, line, NULL);
        }
    }
}

static void _draw_background(struct poulpe_textedit *textedit)
{
    SAKE_MACRO_UNUSED(textedit);
    
    ImGuiWindow *window = igGetCurrentWindowRead();
    ImDrawList *draw_list = igGetWindowDrawList();
    ImDrawList_AddRectFilled(draw_list, window->InnerRect.Min, window->InnerRect.Max, igColorConvertFloat4ToU32(poulpe_style.theme->main_background), 0.0f, 0);
}

static void _draw_find_results(struct poulpe_textedit *textedit)
{
    ImGuiStyle *style = igGetStyle();
    ImDrawList *draw_list = igGetWindowDrawList();

    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);
    origin_screen_position.x += style->FramePadding.x;

    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    struct poulpe_finder *finder = textedit->textview->editor->statusbar->finder;
    if (!strlen(finder->find))
        return;

    ImVec2 current = finder->results[finder->result_index];
    ImVec2 pos = {textedit->line_start, 0};
    while (pos.x < textedit->line_end && poulpe_textbuffer_find(textedit->textview->textbuffer, finder->find, finder->case_sensitive, &pos))
    {
        const char* line = poulpe_textbuffer_text_at(textbuffer, pos.x);

        ImVec2 start_size;
        ImFont_CalcTextSizeA(&start_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + (uint32_t) pos.y, NULL);

        ImVec2 end_size;
        ImFont_CalcTextSizeA(&end_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + (uint32_t) pos.y + strlen(finder->find), NULL);

        ImVec2 upper_left = (ImVec2) {origin_screen_position.x + start_size.x, origin_screen_position.y + pos.x * igGetTextLineHeight()};
        ImVec2 lower_right = (ImVec2) {origin_screen_position.x + end_size.x, origin_screen_position.y + (pos.x  + 1) * igGetTextLineHeight() };

        ImVec4 color = pos.x == current.x && pos.y == current.y ? poulpe_style.theme->matched_text : poulpe_style.theme->searched_text;
        ImDrawList_AddRectFilled(draw_list, upper_left, lower_right, igColorConvertFloat4ToU32(color), 0.0f, 0);

        pos.y += strlen(finder->find);
    }
}

static void _draw_invisibles(struct poulpe_textedit *textedit)
{
    ImGuiStyle *style = igGetStyle();
    ImDrawList *draw_list = igGetWindowDrawList();

    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);
    origin_screen_position.x += style->FramePadding.x;

    for (uint32_t i = textedit->line_start; i < textedit->line_end; i++)
    {
        ImVec2 text_start_position = {origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};

        const char *line = poulpe_textbuffer_text_at(textbuffer, i);

        uint32_t line_raw_size = poulpe_textbuffer_line_raw_size(textbuffer, i);
        uint32_t j = 0;
        while (j < line_raw_size)
        {
            if (line[j] == ' ')
            {
                ImVec2 text_size;
                ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + j + 1, NULL);
                ImVec2 space_size;
                ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);

                float font_size = igGetFontSize();
                ImVec2 center = {text_start_position.x + text_size.x - 0.5 * space_size.x, text_start_position.y + 0.5 * font_size};
                float radius = 1.25f;

                ImDrawList_AddCircleFilled(draw_list,
                                           center,
                                           radius,
                                           igColorConvertFloat4ToU32(poulpe_style.theme->faint_text),
                                           0);
            }

            if (line[j] == '\t')
            {
                ImVec2 text_size;
                ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + j + 1, NULL);
                ImVec2 tab_size;
                ImFont_CalcTextSizeA(&tab_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, "\t", NULL, NULL);

                float font_size = igGetFontSize();
                float thickness = 0.5f;
                ImVec2 center = {text_start_position.x + text_size.x - 0.5 * tab_size.x, text_start_position.y + 0.5 * font_size};

                ImDrawList_AddLine(draw_list,
                                   (ImVec2){center.x - 0.5 * tab_size.x, center.y - thickness},
                                   (ImVec2){center.x - 0.25 * tab_size.x, center.y - thickness},
                                   igColorConvertFloat4ToU32(poulpe_style.theme->faint_text),
                                   thickness);

                ImDrawList_AddTriangleFilled(draw_list,
                                             (ImVec2){center.x - 0.25 * tab_size.x, center.y - 0.15 * font_size},
                                             (ImVec2){center.x - 0.25 * tab_size.x + 0.15 * font_size, center.y},
                                             (ImVec2){center.x - 0.25 * tab_size.x, center.y + 0.15 * font_size},
                                             igColorConvertFloat4ToU32(poulpe_style.theme->faint_text));
            }

            j += sake_utils_utf8_length(line[j]);
        }
    }
}

static float _get_maximum_width(struct poulpe_textedit *textedit)
{
    float maximum_width = 0;
    for (uint32_t i = textedit->line_start; i < textedit->line_end; i++)
    {
        const char *line = poulpe_textbuffer_text_at(textedit->textview->textbuffer, i);

        ImVec2 line_size;
        ImFont_CalcTextSizeA(&line_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, NULL, NULL);
        if (line_size.x > maximum_width)
            maximum_width = line_size.x;
    }
    return maximum_width;
}

static void _add_widget(struct poulpe_textedit *textedit)
{
    ImGuiStyle *style = igGetStyle();

    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);
    origin_screen_position.x += style->FramePadding.x;

    float maximum_width = _get_maximum_width(textedit);

    float scroll_delta_y = 0.0f;
    float scroll_delta_x = 0.0f;
    ImGuiWindow *window = igGetCurrentWindowRead();
    if (window->ScrollbarY)
    {
        ImVec2 content;
        igGetContentRegionAvail(&content);
        scroll_delta_y = content.y;
    }

    if (window->ScrollbarX)
    {
        ImVec2 space_size;
        ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);
        scroll_delta_x = space_size.x * 3;
    }

    ImVec2 size = {maximum_width + scroll_delta_x, (poulpe_textbuffer_text_size(textbuffer) + 2) * igGetTextLineHeight() + scroll_delta_y };
    ImVec2 upper_left = {origin_screen_position.x, origin_screen_position.y};
    ImVec2 lower_right = {origin_screen_position.x + size.x, origin_screen_position.y + size.y};
    ImRect rect = {upper_left, lower_right};
    igItemSize_Vec2(size, -1.0f);
    igItemAdd(rect, 0, NULL, 0);
}

static enum poulpe_error _handle_mouse(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    ImGuiWindow *window = igGetCurrentWindowRead();

    if (event->dragged && !ImRect_Contains_Vec2(&window->InnerRect, event->left_clicked_position))
        return POULPE_ERROR_NONE;
    
    if (ImRect_Contains_Vec2(&window->InnerRect, event->position))
        igSetMouseCursor(ImGuiMouseCursor_TextInput);

    if (!event->base.dirty)
        return POULPE_ERROR_NONE;

    if (event->left_clicked && ImRect_Contains_Vec2(&window->InnerRect, event->position))
    {
        error = _handle_mouse_left_clicked(textedit, event);
        if (error != POULPE_ERROR_NONE)
            return error;
    }

    if (event->left_double_clicked && ImRect_Contains_Vec2(&window->InnerRect, event->position))
    {
        error = _handle_mouse_left_double_clicked(textedit, event);
        if (error != POULPE_ERROR_NONE)
            return error;
    }

    if (event->left_triple_clicked && ImRect_Contains_Vec2(&window->InnerRect, event->position))
    {
        error = _handle_mouse_left_triple_clicked(textedit, event);
        if (error != POULPE_ERROR_NONE)
            return error;
    }

    if (event->dragged)
    {
        error = _handle_mouse_drag(textedit, event);
        if (error != POULPE_ERROR_NONE)
            return error;
    }

    return error;
}

static enum poulpe_error _handle_mouse_left_clicked(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event)
{
    ImVec2 position = {0};
    _get_coordinates(textedit, event->position, &position);
    poulpe_cursor_update_position(textedit->cursor, position);
    poulpe_selection_update_start(textedit->selection, position);
    poulpe_selection_update_end(textedit->selection, position);
    _ensure_cursor_visiblity(textedit);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_mouse_left_double_clicked(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event)
{
    ImVec2 position = {0};
    _get_coordinates(textedit, event->position, &position);

    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;
    uint32_t length = poulpe_textbuffer_line_eof_size(textbuffer, position.x);
    const char *line = poulpe_textbuffer_text_at(textbuffer, position.x);
    
    uint32_t start_word = position.y;
    uint32_t end_word = position.y;
    
    if (isspace(line[(uint32_t) position.y]))
    {        
        while (start_word > 0 && isspace(line[start_word]))
            start_word--;

        while (end_word < length && isspace(line[end_word]))
            end_word++;
    }
    else
    {
        while (start_word > 0 && !sake_utils_is_word_separator(line[start_word]))
            start_word--;

        while (end_word < length && !sake_utils_is_word_separator(line[end_word]))
            end_word++;
    }
    
    if (start_word)
        start_word++;

    ImVec2 start = {position.x, start_word};
    ImVec2 end = {position.x, end_word};
    poulpe_selection_update_start(textedit->selection, start);
    poulpe_selection_update_end(textedit->selection, end);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_mouse_left_triple_clicked(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event)
{
    ImVec2 position = {0};
    _get_coordinates(textedit, event->position, &position);

    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;
    uint32_t length = poulpe_textbuffer_line_eof_size(textbuffer, position.x);

    ImVec2 start = {position.x, 0.f};
    ImVec2 end = {position.x, length};
    poulpe_selection_update_start(textedit->selection, start);
    poulpe_selection_update_end(textedit->selection, end);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_mouse_drag(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event)
{
    ImVec2 position = {0};
    _get_coordinates(textedit, event->position, &position);
    poulpe_cursor_update_position(textedit->cursor, position);
    poulpe_selection_update_end(textedit->selection, position);
    _ensure_cursor_visiblity(textedit);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    if (event->ctrl && event->s)
        return _handle_keyboard_save(textedit, event);

    if (event->ctrl && event->z && !event->shift)
        return _handle_keyboard_undo(textedit, event);

    if ( (event->ctrl && event->y) || (event->ctrl && event->shift && event->z) )
        return _handle_keyboard_redo(textedit, event);

    if (event->ctrl && event->x)
        return _push_new_action(textedit, event, _handle_keyboard_cut);

    if (event->ctrl && event->c)
        return _handle_keyboard_copy(textedit, event);

    if (event->ctrl && event->a)
        return _handle_keyboard_select_all(textedit, event);

    if (event->alt && event->up)
        return _push_new_action(textedit, event, _handle_keyboard_alt_up);

    if (event->alt && event->down)
        return _push_new_action(textedit, event, _handle_keyboard_alt_down);

    if (event->shift && event->up)
        return _handle_keyboard_shift_up(textedit, event);

    if (event->shift && event->down)
        return _handle_keyboard_shift_down(textedit, event);

    if (event->shift && event->left)
        return _handle_keyboard_shift_left(textedit, event);

    if (event->shift && event->right)
        return _handle_keyboard_shift_right(textedit, event);

    if (event->ctrl && event->v)
        return _push_new_action(textedit, event, _handle_keyboard_paste);

    if (event->delete)
        return _push_new_action(textedit, event, _handle_keyboard_delete);

    if (event->backspace)
        return _push_new_action(textedit, event, _handle_keyboard_backspace);

    if (event->left)
        return _handle_keyboard_left(textedit, event);

    if (event->right)
        return _handle_keyboard_right(textedit, event);

    if (event->up)
        return _handle_keyboard_up(textedit, event);

    if (event->down)
        return _handle_keyboard_down(textedit, event);

    if (event->enter)
        return _push_new_action(textedit, event, _handle_keyboard_enter);

    if (event->tab && event->shift)
        return _push_new_action(textedit, event, _handle_keyboard_shift_tab);

    if (event->tab && !event->shift)
        return _push_new_action(textedit, event, _handle_keyboard_tab);

    if (event->count)
        return _push_new_action(textedit, event, _handle_keyboard_default);

    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_save(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);
    return poulpe_textbuffer_save_file(textedit->textview->textbuffer);
}

static enum poulpe_error _handle_keyboard_undo(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    struct poulpe_action *action = poulpe_history_current(textbuffer->history);
    if (action)
    {
        poulpe_cursor_update_position(textedit->cursor, action->before.cursor_position);
        poulpe_selection_update_start(textedit->selection, action->before.selection_start_position);
        poulpe_selection_update_end(textedit->selection, action->before.selection_end_position);

        error = poulpe_textbuffer_undo(textbuffer);
        if (error != POULPE_ERROR_NONE)
            return error;
        
        poulpe_textbuffer_tree_edit(textbuffer);
    }

    return error;
}

static enum poulpe_error _handle_keyboard_redo(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);
    
    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    error = poulpe_textbuffer_redo(textbuffer);
    if (error != POULPE_ERROR_NONE)
        return error;

    struct poulpe_action *action = poulpe_history_current(textbuffer->history);
    if (action)
    {
        poulpe_cursor_update_position(textedit->cursor, action->after.cursor_position);
        poulpe_selection_update_start(textedit->selection, action->after.selection_start_position);
        poulpe_selection_update_end(textedit->selection, action->after.selection_end_position);
        
        poulpe_textbuffer_tree_edit(textbuffer);
    }

    return error;
}

static enum poulpe_error _handle_keyboard_cut(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{   
    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    _handle_keyboard_copy(textedit, event);

    if (poulpe_selection_active(textedit->selection))
    {
        textedit->cursor->position = textedit->selection->ajusted.start;
        error = poulpe_selection_delete(textedit->selection);
        if (error != POULPE_ERROR_NONE)
            return error;
        poulpe_selection_clear(textedit->selection);
        poulpe_cursor_reset(textedit->cursor);
    }

    poulpe_textbuffer_tree_edit(textbuffer);
    poulpe_selection_update_start(textedit->selection, textedit->cursor->position);
    poulpe_selection_update_end(textedit->selection, textedit->cursor->position);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);

    return error;
}

static enum poulpe_error _handle_keyboard_copy(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);
    
    enum poulpe_error error = POULPE_ERROR_NONE;

    sake_string selection = poulpe_selection_to_str(textedit->selection);
    if (!selection)
        return POULPE_ERROR_MEMORY;

    igSetClipboardText(selection);

    sake_string_free(selection);
    
    return error;
}

static enum poulpe_error _handle_keyboard_paste(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);
    
    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    if (poulpe_selection_active(textedit->selection))
    {
        textedit->cursor->position = textedit->selection->ajusted.start;
        error = poulpe_selection_delete(textedit->selection);
        if (error != POULPE_ERROR_NONE)
            return error;
        poulpe_selection_clear(textedit->selection);
        poulpe_cursor_reset(textedit->cursor);
    }

    uint32_t line_index = textedit->cursor->position.x;
    uint32_t glyph_index = textedit->cursor->position.y;

    const char *clipboard = igGetClipboardText();
    uint32_t clipboard_size = strlen(clipboard);

    uint32_t i = 0;
    uint32_t j = 0;
    while (i < clipboard_size)
    {
        if (clipboard[i] == '\n')
        {
            error = poulpe_textbuffer_line_insert(textbuffer, line_index, glyph_index, clipboard + j, clipboard + i);
            if (error != POULPE_ERROR_NONE)
                return error;

            error = poulpe_textbuffer_text_insert(textbuffer, line_index);
            if (error != POULPE_ERROR_NONE)
                return error;

            error = poulpe_textbuffer_line_push_back(textbuffer, line_index,
                                                     poulpe_textbuffer_text_at(textbuffer, line_index + 1),
                                                     poulpe_textbuffer_text_at(textbuffer, line_index + 1) + glyph_index + i - j);
            if (error != POULPE_ERROR_NONE)
                return error;

            const char *eof = textbuffer->eof == POULPE_TEXTBUFFER_EOF_LF ? "\n" : "\r\n";
            error = poulpe_textbuffer_line_push_back(textbuffer, line_index, eof, NULL);
            if (error != POULPE_ERROR_NONE)
                return error;

            error = poulpe_textbuffer_line_erase_range(textbuffer, line_index + 1, 0, glyph_index + i - j);
            if (error != POULPE_ERROR_NONE)
                return error;

            line_index++;
            glyph_index = 0;
            j = i + 1;
        }

        i++;
    }   

    error = poulpe_textbuffer_line_insert(textbuffer, line_index, glyph_index, clipboard + j, clipboard + i);
    if (error != POULPE_ERROR_NONE)
        return error;

    textedit->cursor->position.x = line_index;
    textedit->cursor->position.y = glyph_index + i - j;

    poulpe_textbuffer_tree_edit(textbuffer);
    poulpe_selection_update_start(textedit->selection, textedit->cursor->position);
    poulpe_selection_update_end(textedit->selection, textedit->cursor->position);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);

    return error;
}

static enum poulpe_error _handle_keyboard_select_all(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    poulpe_selection_select_all(textedit->selection);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update_position(textedit->cursor, textedit->selection->ajusted.end);
    poulpe_cursor_update(textedit->cursor);
    
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_alt_up(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);
    
    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;
    struct poulpe_selection *selection = textedit->selection;
    uint32_t line_index = textedit->cursor->position.x;

    if (poulpe_selection_active(selection))
    {
        if (!selection->ajusted.start.x)
            return POULPE_ERROR_NONE;

        error = poulpe_textbuffer_text_insert(textbuffer, selection->ajusted.end.x + 1);
        if (error != POULPE_ERROR_NONE)
            return error;

        error = poulpe_textbuffer_line_push_back(textbuffer, selection->ajusted.end.x + 1, poulpe_textbuffer_text_at(textbuffer, selection->ajusted.start.x - 1), NULL);
        if (error != POULPE_ERROR_NONE)
            return error;

        error = poulpe_textbuffer_text_erase(textbuffer, selection->ajusted.start.x - 1);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_selection_move_start_up(selection);
        poulpe_selection_move_end_up(selection);
        poulpe_cursor_move_up(textedit->cursor);
    }
    else
    {
        if (!line_index)
            return POULPE_ERROR_NONE;

        error = poulpe_textbuffer_text_insert(textbuffer, line_index - 1);
        if (error != POULPE_ERROR_NONE)
            return error;

        error = poulpe_textbuffer_line_push_back(textbuffer, line_index - 1, poulpe_textbuffer_text_at(textbuffer, line_index + 1), NULL);
        if (error != POULPE_ERROR_NONE)
            return error;

        error = poulpe_textbuffer_text_erase(textbuffer, line_index + 1);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_cursor_move_up(textedit->cursor);
    }

    poulpe_textbuffer_tree_edit(textbuffer);
    poulpe_cursor_update(textedit->cursor);
    poulpe_selection_update(textedit->selection);
    _ensure_cursor_visiblity(textedit);
    
    return error;
}

static enum poulpe_error _handle_keyboard_alt_down(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;
    struct poulpe_selection *selection = textedit->selection;
    uint32_t line_index = textedit->cursor->position.x;
    uint32_t text_size = poulpe_textbuffer_text_size(textbuffer);

    if (poulpe_selection_active(selection))
    {
        if (text_size < 2)
            return error;
        
        if (selection->ajusted.end.x > (text_size - 2))
            return POULPE_ERROR_NONE;

        error = poulpe_textbuffer_text_insert(textbuffer, selection->ajusted.start.x);
        if (error != POULPE_ERROR_NONE)
            return error;

        error = poulpe_textbuffer_line_push_back(textbuffer, selection->ajusted.start.x, poulpe_textbuffer_text_at(textbuffer, selection->ajusted.end.x + 2), NULL);
        if (error != POULPE_ERROR_NONE)
            return error;

        error = poulpe_textbuffer_text_erase(textbuffer, selection->ajusted.end.x + 2);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_selection_move_start_down(selection);
        poulpe_selection_move_end_down(selection);
        poulpe_cursor_move_down(textedit->cursor);
    }
    else
    {
        if (text_size < 2)
            return error;

        if (line_index > (text_size - 2))
            return error;

        error = poulpe_textbuffer_text_insert(textbuffer, line_index + 2);
        if (error != POULPE_ERROR_NONE)
            return error;

        error = poulpe_textbuffer_line_push_back(textbuffer, line_index + 2, poulpe_textbuffer_text_at(textbuffer, line_index), NULL);
        if (error != POULPE_ERROR_NONE)
            return error;

        error = poulpe_textbuffer_text_erase(textbuffer, line_index);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_cursor_move_down(textedit->cursor);
    }

    poulpe_textbuffer_tree_edit(textbuffer);
    poulpe_cursor_update(textedit->cursor);
    poulpe_selection_update(textedit->selection);
    _ensure_cursor_visiblity(textedit);

    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_shift_up(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);
    poulpe_selection_move_end_up(textedit->selection);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update_position(textedit->cursor, textedit->selection->current.end);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_shift_down(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);
    poulpe_selection_move_end_down(textedit->selection);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update_position(textedit->cursor, textedit->selection->current.end);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_shift_left(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);
    poulpe_selection_move_end_left(textedit->selection);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update_position(textedit->cursor, textedit->selection->current.end);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_shift_right(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);
    poulpe_selection_move_end_right(textedit->selection);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update_position(textedit->cursor, textedit->selection->current.end);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_delete(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    if (poulpe_selection_active(textedit->selection))
    {
        textedit->cursor->position = textedit->selection->ajusted.start;
        error = poulpe_selection_delete(textedit->selection);
        if (error != POULPE_ERROR_NONE)
            return error;
        poulpe_selection_clear(textedit->selection);
        poulpe_cursor_reset(textedit->cursor);
    }
    else
    {
        uint32_t line_index = textedit->cursor->position.x;
        uint32_t glyph_index = textedit->cursor->position.y;
        uint32_t line_raw_size = poulpe_textbuffer_line_eof_size(textbuffer, line_index);

        if (glyph_index < line_raw_size)
        {
            error = poulpe_textbuffer_line_erase(textbuffer, line_index, glyph_index);
            if (error != POULPE_ERROR_NONE)
                return error;
        }
        else if (line_index < (poulpe_textbuffer_text_size(textbuffer) - 1))
        {
            uint32_t next_line_raw_size = poulpe_textbuffer_line_eof_size(textbuffer, line_index + 1);
            error = poulpe_textbuffer_line_insert(textbuffer,
                                                  line_index,
                                                  line_raw_size,
                                                  poulpe_textbuffer_text_at(textbuffer, line_index + 1),
                                                  poulpe_textbuffer_text_at(textbuffer, line_index + 1) + next_line_raw_size);
            if (error != POULPE_ERROR_NONE)
                return error;
            
            error = poulpe_textbuffer_text_erase(textbuffer, line_index + 1);
            if (error != POULPE_ERROR_NONE)
                return error;
        }
    }

    poulpe_textbuffer_tree_edit(textbuffer);
    poulpe_selection_update_start(textedit->selection, textedit->cursor->position);
    poulpe_selection_update_end(textedit->selection, textedit->cursor->position);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);
        
    return error;
}

static enum poulpe_error _handle_keyboard_backspace(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    if (poulpe_selection_active(textedit->selection))
    {
        textedit->cursor->position = textedit->selection->ajusted.start;
        error = poulpe_selection_delete(textedit->selection);
        if (error != POULPE_ERROR_NONE)
            return error;
        poulpe_selection_clear(textedit->selection);
        poulpe_cursor_reset(textedit->cursor);
    }
    else
    {
        uint32_t line_index = textedit->cursor->position.x;
        uint32_t glyph_index = textedit->cursor->position.y;

        if (glyph_index != 0)
        {
            uint32_t utf8_index = poulpe_textbuffer_line_utf8_index(textbuffer, line_index, glyph_index);
            uint32_t raw_index = poulpe_textbuffer_line_raw_index(textbuffer, line_index, utf8_index - 1);
            error = poulpe_textbuffer_line_erase(textbuffer, line_index, raw_index);
            if (error != POULPE_ERROR_NONE)
                return error;
            poulpe_cursor_move_left(textedit->cursor);
        }
        else if (line_index != 0)
        {
            uint32_t previous_size = poulpe_textbuffer_line_eof_size(textbuffer, line_index - 1);
            uint32_t current_size = poulpe_textbuffer_line_eof_size(textbuffer, line_index);
            error = poulpe_textbuffer_line_insert(textbuffer, line_index - 1,
                                                  previous_size,
                                                  poulpe_textbuffer_text_at(textbuffer, line_index),
                                                  poulpe_textbuffer_text_at(textbuffer, line_index) + current_size);
            if (error != POULPE_ERROR_NONE)
                return error;

            error = poulpe_textbuffer_text_erase(textbuffer, line_index);
            if (error != POULPE_ERROR_NONE)
                return error;
            
            poulpe_cursor_move_up(textedit->cursor);
            textedit->cursor->position.y = previous_size;
        }
    }

    poulpe_textbuffer_tree_edit(textbuffer);
    poulpe_selection_update_start(textedit->selection, textedit->cursor->position);
    poulpe_selection_update_end(textedit->selection, textedit->cursor->position);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);
    
    return error;
}

static enum poulpe_error _handle_keyboard_left(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;

    if (poulpe_selection_active(textedit->selection))
        textedit->cursor->position = textedit->selection->ajusted.start;
    else
        poulpe_cursor_move_left(textedit->cursor);

    poulpe_selection_update_start(textedit->selection, textedit->cursor->position);
    poulpe_selection_update_end(textedit->selection, textedit->cursor->position);
    poulpe_selection_update(textedit->selection);    
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);

    return error;
}

static enum poulpe_error _handle_keyboard_right(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;

    if (poulpe_selection_active(textedit->selection))
        textedit->cursor->position = textedit->selection->ajusted.end;
    else
        poulpe_cursor_move_right(textedit->cursor);

    poulpe_selection_update_start(textedit->selection, textedit->cursor->position);
    poulpe_selection_update_end(textedit->selection, textedit->cursor->position);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);

    return error;
}

static enum poulpe_error _handle_keyboard_up(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;

    if (poulpe_selection_active(textedit->selection))
        textedit->cursor->position = textedit->selection->ajusted.start;

    poulpe_cursor_move_up(textedit->cursor);
    poulpe_selection_update_start(textedit->selection, textedit->cursor->position);
    poulpe_selection_update_end(textedit->selection, textedit->cursor->position);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);

    
    return error;
}

static enum poulpe_error _handle_keyboard_down(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;

    if (poulpe_selection_active(textedit->selection))
        textedit->cursor->position = textedit->selection->ajusted.end;

    poulpe_cursor_move_down(textedit->cursor);
    poulpe_selection_update_start(textedit->selection, textedit->cursor->position);
    poulpe_selection_update_end(textedit->selection, textedit->cursor->position);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);
    
    return error;
}

static enum poulpe_error _handle_keyboard_enter(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    if (poulpe_selection_active(textedit->selection))
    {
        textedit->cursor->position = textedit->selection->ajusted.start;
        error = poulpe_selection_delete(textedit->selection);
        if (error != POULPE_ERROR_NONE)
            return error;
        poulpe_selection_clear(textedit->selection);
        poulpe_cursor_reset(textedit->cursor);
    }

    uint32_t line_index = textedit->cursor->position.x;
    uint32_t glyph_index = textedit->cursor->position.y;

    error = poulpe_textbuffer_text_insert(textbuffer, line_index);
    if (error != POULPE_ERROR_NONE)
        return error;

    error = poulpe_textbuffer_line_push_back(textbuffer, line_index,
                                            poulpe_textbuffer_text_at(textbuffer, line_index + 1),
                                            poulpe_textbuffer_text_at(textbuffer, line_index + 1) + glyph_index);
    if (error != POULPE_ERROR_NONE)
        return error;

    const char *eof = textbuffer->eof == POULPE_TEXTBUFFER_EOF_LF ? "\n" : "\r\n";
    error = poulpe_textbuffer_line_push_back(textbuffer, line_index, eof, NULL);
    if (error != POULPE_ERROR_NONE)
        return error;

    error = poulpe_textbuffer_line_erase_range(textbuffer, line_index + 1, 0, glyph_index);
    if (error != POULPE_ERROR_NONE)
        return error;

    textedit->cursor->position.y = 0;
    poulpe_cursor_move_down(textedit->cursor);

    poulpe_textbuffer_tree_edit(textbuffer);
    poulpe_selection_update_start(textedit->selection, textedit->cursor->position);
    poulpe_selection_update_end(textedit->selection, textedit->cursor->position);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);

    return error;
}

static enum poulpe_error _handle_keyboard_shift_tab(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;
    enum poulpe_indentinfo_type indentation = textedit->textview->editor->statusbar->indentinfo->current;    
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;
    struct poulpe_selection *selection = textedit->selection;

    if (poulpe_selection_active(selection))
    {
        uint32_t i = selection->ajusted.start.x;
        while (i <= selection->ajusted.end.x)
        {
            const char *line = poulpe_textbuffer_text_at(textbuffer, i);
            uint32_t line_length = poulpe_textbuffer_line_eof_size(textbuffer, i);
            uint32_t indent_length = poulpe_indentinfo_length(indentation);

            uint32_t j = 0;
            while (j < line_length && j < indent_length && (line[0] == ' ' || line[0] == '\t'))
            {
                error = poulpe_textbuffer_line_erase(textbuffer, i, 0);
                if (error != POULPE_ERROR_NONE)
                    return error;

                if (textedit->cursor->position.x == i)
                    poulpe_cursor_move_left(textedit->cursor);

                if (selection->ajusted.start.x == i)
                    poulpe_selection_move_start_left(selection);

                if (selection->ajusted.end.x == i)
                    poulpe_selection_move_end_left(selection);
                
                j++;
            }

            i++;
        }
        poulpe_selection_update(textedit->selection);
    }
    else
    {
        uint32_t line_index = textedit->cursor->position.x;
        const char *line = poulpe_textbuffer_text_at(textbuffer, line_index);
        uint32_t line_length = poulpe_textbuffer_line_eof_size(textbuffer, line_index);
        uint32_t indent_length = poulpe_indentinfo_length(indentation);

        uint32_t i = 0;
        while (i < line_length && i < indent_length && (line[0] == ' ' || line[0] == '\t'))
        {
            error = poulpe_textbuffer_line_erase(textbuffer, line_index, 0);
            if (error != POULPE_ERROR_NONE)
                return error;
            poulpe_cursor_move_left(textedit->cursor);
            i++;
        }
    }

    poulpe_textbuffer_tree_edit(textbuffer);

    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);

    return error;
}

static enum poulpe_error _handle_keyboard_tab(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    enum poulpe_error error = POULPE_ERROR_NONE;
    enum poulpe_indentinfo_type indentation = textedit->textview->editor->statusbar->indentinfo->current;    
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;
    struct poulpe_selection *selection = textedit->selection;

    if (poulpe_selection_active(selection))
    {
        uint32_t i = selection->ajusted.start.x;
        while (i <= selection->ajusted.end.x)
        {
            error = poulpe_textbuffer_line_insert(textbuffer, i, 0, poulpe_indentinfo_str(indentation), NULL);
            if (error != POULPE_ERROR_NONE)
                return error;
            i++;
        }

        for (uint32_t i = 0; i < poulpe_indentinfo_length(indentation); i++)
        {
            poulpe_selection_move_start_right(selection);
            poulpe_selection_move_end_right(selection);
            poulpe_cursor_move_right(textedit->cursor);
        }
        poulpe_selection_update(textedit->selection);
    }
    else
    {
        uint32_t line_index = textedit->cursor->position.x;
        uint32_t glyph_index = textedit->cursor->position.y;

        error = poulpe_textbuffer_line_insert(textbuffer, line_index, glyph_index, poulpe_indentinfo_str(indentation), NULL);
        if (error != POULPE_ERROR_NONE)
            return error;

        for (uint32_t i = 0; i < poulpe_indentinfo_length(indentation); i++)
            poulpe_cursor_move_right(textedit->cursor);
    }

    poulpe_textbuffer_tree_edit(textbuffer);

    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);

    return error;
}

static enum poulpe_error _handle_keyboard_default(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    if (poulpe_selection_active(textedit->selection))
    {
        textedit->cursor->position = textedit->selection->ajusted.start;
        error = poulpe_selection_delete(textedit->selection);
        if (error != POULPE_ERROR_NONE)
            return error;
        poulpe_selection_clear(textedit->selection);
        poulpe_cursor_reset(textedit->cursor);
    }

    uint32_t line_index = textedit->cursor->position.x;
    for (uint32_t i = 0; i < event->count; i++)
    {
        uint32_t glyph_index = textedit->cursor->position.y;
        uint32_t utf8 = sake_utils_utf8_from_code_point(event->data[i]);
        enum poulpe_error error = poulpe_textbuffer_line_insert(textbuffer, line_index, glyph_index, (const char *) &utf8, NULL);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_cursor_move_right(textedit->cursor);
    }

    poulpe_textbuffer_tree_edit(textbuffer);
    poulpe_selection_update_start(textedit->selection, textedit->cursor->position);
    poulpe_selection_update_end(textedit->selection, textedit->cursor->position);
    poulpe_selection_update(textedit->selection);
    poulpe_cursor_update(textedit->cursor);
    _ensure_cursor_visiblity(textedit);

    return error;
}

static enum poulpe_error _push_new_action(struct poulpe_textedit *textedit,
                                          struct poulpe_event_keyboard *event,
                                          enum poulpe_error (*cb) (struct poulpe_textedit *, struct poulpe_event_keyboard *))
{
    enum poulpe_error error = POULPE_ERROR_NONE;
    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;

    error = poulpe_textbuffer_new_action(textbuffer);
    if (error != POULPE_ERROR_NONE)
        return error;

    struct poulpe_action *action = poulpe_history_back(textbuffer->history);
    action->before.cursor_position = textedit->cursor->position;
    action->before.selection_start_position = textedit->selection->current.start;
    action->before.selection_end_position = textedit->selection->current.end;

    error = cb(textedit, event);
    if (error != POULPE_ERROR_NONE)
        return error;

    action->after.cursor_position = textedit->cursor->position;
    action->after.selection_start_position = textedit->selection->current.start;
    action->after.selection_end_position = textedit->selection->current.end;

    return error;
}