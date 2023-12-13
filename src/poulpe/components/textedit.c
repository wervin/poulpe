#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <math.h>

#include <cimgui.h>

#include <sake/macro.h>
#include <sake/utils.h>

#include "poulpe/components/textedit.h"
#include "poulpe/components/selection.h"
#include "poulpe/components/cursor.h"
#include "poulpe/components/textview.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"
#include "poulpe/textbuffer.h"
#include "poulpe/io.h"

static void _ensure_cursor_visiblity(struct poulpe_textedit *textedit);
static void _get_coordinates(struct poulpe_textedit *textedit, ImVec2 mouse_position, ImVec2 *utf8_coordinates);
static enum poulpe_error _update_view(struct poulpe_textedit *textedit);

static enum poulpe_error _handle_mouse(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event);
static enum poulpe_error _handle_mouse_left_click(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event);
static enum poulpe_error _handle_mouse_drag(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event);

static enum poulpe_error _handle_keyboard(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_delete(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_backspace(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_left(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_right(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_up(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_down(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_enter(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_tab(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_default(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event);

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

    ImGuiStyle *style = igGetStyle();

    igSameLine(0.0f, 0.0f);
    if (!igBeginChild_Str("Poulpe##textedit", (ImVec2) {0}, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysHorizontalScrollbar))
        goto end_child;

    error = poulpe_io_handle_keyboard((struct poulpe_component *) textedit);
    if (error != POULPE_ERROR_NONE)
        goto end_child;
    
    error = poulpe_io_handle_mouse((struct poulpe_component *) textedit);
    if (error != POULPE_ERROR_NONE)
        goto end_child;
    
    _update_view(textedit);

    ImDrawList *draw_list = igGetWindowDrawList();
    
    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);
    origin_screen_position.x += style->FramePadding.x;

    {
        ImGuiWindow *window = igGetCurrentWindowRead();
        ImDrawList_AddRectFilled(draw_list, window->InnerRect.Min, window->InnerRect.Max, igColorConvertFloat4ToU32(poulpe_theme_dark.backgound), 0.0f, 0);
    }

    if (poulpe_selection_active(textedit->selection))
    {
        error = poulpe_component_draw((struct poulpe_component *)textedit->selection);
        if (error != POULPE_ERROR_NONE)
            goto end_child;
    }

    error = poulpe_component_draw((struct poulpe_component *)textedit->cursor);
    if (error != POULPE_ERROR_NONE)
        goto end_child;

    float maximum_width = 0;

    poulpe_text text = textedit->textview->textbuffer->text;

    for (uint32_t i = textedit->line_start; i < textedit->line_end; i++)
    {
        poulpe_line line = text[i];

        ImVec2 line_start_position = {origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};
        ImVec2 text_start_position = { line_start_position.x, line_start_position.y};

        ImVec2 line_size;
        ImFont_CalcTextSizeA(&line_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, NULL, NULL);
        if (line_size.x > maximum_width)
            maximum_width = line_size.x;

        // uint32_t k = 0;
        uint32_t line_raw_size = poulpe_line_raw_size(line);
        // ImU32 color = poulpe_textbuffer_line_at(textedit->textview->textbuffer, i, 0)->color;
        ImU32 color = igColorConvertFloat4ToU32(poulpe_theme_dark.text);
        // char buffer[2 * line_raw_size + 1];
        // buffer[k] = '\0';

        uint32_t j = 0;
        while (j < line_raw_size)
        {
            // struct poulpe_glyph *g = poulpe_textbuffer_line_at(textedit->textview->textbuffer, i, j);

            // if (g->color != color)
            // {
            //     ImVec2 text_size;
            //     ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, buffer, NULL, NULL);

            //     ImDrawList_AddText_Vec2(draw_list,
            //                             text_start_position,
            //                             color,
            //                             buffer,
            //                             NULL);

            //     text_start_position.x += text_size.x;
            //     memset(buffer, 0, k);
            //     k = 0;
            //     color = g->color;
            // }

            // memcpy(buffer + k, (char *) g->utf8, utf8_size);
            // buffer[k + g->size] = '\0';

            if (line[j] == ' ')
            {
                ImVec2 text_size;
                ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + j + 1, NULL);
                ImVec2 space_size;
                ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);

                float font_size = igGetFontSize();
                ImVec2 center = {text_start_position.x + text_size.x - 0.5 * space_size.x, text_start_position.y + 0.5 * font_size};
                float radius = 1.5f;

                ImDrawList_AddCircleFilled(draw_list,
                                            center,
                                            radius,
                                            igColorConvertFloat4ToU32(poulpe_theme_dark.whitespace),
                                            0);
            }

            if (line[j] == '\t')
            {
                ImVec2 text_size;
                ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line , line + j + 1, NULL);
                ImVec2 tab_size;
                ImFont_CalcTextSizeA(&tab_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, "\t", NULL, NULL);

                float font_size = igGetFontSize();
                float thickness = 0.5f;
                ImVec2 center = {text_start_position.x + text_size.x - 0.5 * tab_size.x, text_start_position.y + 0.5 * font_size};

                ImDrawList_AddLine(draw_list,
                                    (ImVec2) { center.x - 0.5 * tab_size.x, center.y - thickness},
                                    (ImVec2) { center.x - 0.25 * tab_size.x, center.y - thickness},
                                    igColorConvertFloat4ToU32(poulpe_theme_dark.whitespace),
                                    thickness);

                ImDrawList_AddTriangleFilled(draw_list,
                                                (ImVec2) { center.x - 0.25 * tab_size.x, center.y - 0.15 * font_size},
                                                (ImVec2) { center.x - 0.25 * tab_size.x + 0.15 * font_size, center.y},
                                                (ImVec2) { center.x - 0.25 * tab_size.x, center.y + 0.15 * font_size },
                                                igColorConvertFloat4ToU32(poulpe_theme_dark.whitespace));
            }

            j += sake_utils_utf8_length(line[j]);
        }

        if (strlen(line))
        {
            ImDrawList_AddText_Vec2(draw_list,
                                    text_start_position,
                                    color,
                                    line,
                                    NULL);
        }
    }

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

    ImVec2 size = {maximum_width + scroll_delta_x, (poulpe_text_size(text) + 2) * igGetTextLineHeight() + scroll_delta_y };
    ImVec2 upper_left = {origin_screen_position.x, origin_screen_position.y};
    ImVec2 lower_right = {origin_screen_position.x + size.x, origin_screen_position.y + size.y};
    ImRect rect = {upper_left, lower_right};
    igItemSize_Vec2(size, -1.0f);
    igItemAdd(rect, 0, NULL, 0);
    
end_child:
    igEndChild();
    igPopStyleVar(1);

    return error;
}

void poulpe_textedit_set_textview(struct poulpe_textedit *textedit, struct poulpe_textview *textview)
{
    textedit->textview = textview;
}

static void _ensure_cursor_visiblity(struct poulpe_textedit *textedit)
{
    poulpe_text text = textedit->textview->textbuffer->text;
    ImVec2 *cursor_position = &textedit->cursor->position;
    poulpe_line line = text[(uint32_t) cursor_position->x];

    ImGuiWindow *window = igGetCurrentWindowRead();
    ImVec2 content;
    igGetContentRegionAvail(&content);

    ImVec2 text_size;
    ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line, line + (uint32_t) cursor_position->y, NULL);
    ImVec2 space_size;
    ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);

    uint32_t start = window->Scroll.y / igGetTextLineHeight();
    uint32_t end = (window->Scroll.y + content.y) / igGetTextLineHeight() - 1;
    end = fmin(end, poulpe_text_size(text));
    end = fmax(end, 0);

    if (textedit->cursor->position.x < start)
    {
        float scroll_y = 0.0f;
        scroll_y = textedit->cursor->position.x * igGetTextLineHeight();
        window->DC.CursorPos.y += window->Scroll.y - scroll_y;
        window->Scroll.y = scroll_y;
    }

    if (textedit->cursor->position.x > end)
    {
        float scroll_y = 0.0f;
        scroll_y = (textedit->cursor->position.x  + 1) * igGetTextLineHeight() - content.y;
        window->DC.CursorPos.y += window->Scroll.y - scroll_y;
        window->Scroll.y = scroll_y;
    }

    if (text_size.x < window->Scroll.x)
        window->Scroll.x = text_size.x;

    if (text_size.x > (window->Scroll.x + content.x))
        window->Scroll.x = text_size.x + space_size.x - content.x;
}

static void _get_coordinates(struct poulpe_textedit *textedit, ImVec2 mouse_position, ImVec2 *utf8_coordinates)
{
    poulpe_text text = textedit->textview->textbuffer->text;

    ImGuiStyle *style = igGetStyle();

    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);
    origin_screen_position.x += style->FramePadding.x;

    uint32_t line_index = 0;
    uint32_t last_line = poulpe_text_size(text) - 1;
    while (line_index < last_line && mouse_position.y > origin_screen_position.y + (line_index + 1) * igGetTextLineHeight())
        line_index++;
    
    poulpe_line line = text[line_index];
    uint32_t line_raw_size = poulpe_line_raw_size(line);
    uint32_t raw_glyph_index = 0;
    uint32_t utf8_glyph_index = 0;
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
            utf8_glyph_index = diff_text < diff_text_advance ? utf8_glyph_index : utf8_glyph_index + 1;
            break;
        }

        raw_glyph_index += sake_utils_utf8_length(line[raw_glyph_index]);
        utf8_glyph_index++;
    }
    utf8_coordinates->x = line_index;
    utf8_coordinates->y = utf8_glyph_index;
}

static enum poulpe_error _update_view(struct poulpe_textedit *textedit)
{
    poulpe_text text = textedit->textview->textbuffer->text;

    ImGuiWindow *window = igGetCurrentWindowRead();
    ImVec2 content;
    igGetContentRegionAvail(&content);
    
    textedit->line_start = window->Scroll.y / igGetTextLineHeight();
    textedit->line_end = ((window->Scroll.y + content.y) / igGetTextLineHeight()) + 2;
    textedit->line_end = fmin(textedit->line_end, poulpe_text_size(text));
    textedit->line_end = fmax(textedit->line_end, 0);

    textedit->scroll_y = window->Scroll.y;
    
    return POULPE_ERROR_NONE;
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
        error = _handle_mouse_left_click(textedit, event);
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

static enum poulpe_error _handle_mouse_left_click(struct poulpe_textedit *textedit, struct poulpe_event_mouse *event)
{
    ImVec2 position = {0};
    _get_coordinates(textedit, event->position, &position);
    poulpe_cursor_update_position(textedit->cursor, position);
    poulpe_selection_update_start(textedit->selection, position);
    poulpe_selection_update_end(textedit->selection, position);
    _ensure_cursor_visiblity(textedit);
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
    enum poulpe_error error = POULPE_ERROR_NONE;

    if (event->delete)
    {
        if (poulpe_selection_active(textedit->selection))
        {
            textedit->cursor->position.x = textedit->selection->ajusted.start_line_index;
            textedit->cursor->position.y = textedit->selection->ajusted.start_glyph_index;
            error = poulpe_selection_delete(textedit->selection);
            if (error != POULPE_ERROR_NONE)
                return error;
            poulpe_selection_clear(textedit->selection);
            poulpe_cursor_reset(textedit->cursor);
        }
        else
        {
            error = _handle_keyboard_delete(textedit, event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }
        poulpe_cursor_update(textedit->cursor);
        _ensure_cursor_visiblity(textedit);
    }

    if (event->backspace)
    {
        if (poulpe_selection_active(textedit->selection))
        {
            textedit->cursor->position.x = textedit->selection->ajusted.start_line_index;
            textedit->cursor->position.y = textedit->selection->ajusted.start_glyph_index;
            error = poulpe_selection_delete(textedit->selection);
            if (error != POULPE_ERROR_NONE)
                return error;
            poulpe_selection_clear(textedit->selection);
            poulpe_cursor_reset(textedit->cursor);
        }
        else
        {
            error = _handle_keyboard_backspace(textedit, event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }
        poulpe_cursor_update(textedit->cursor);
        _ensure_cursor_visiblity(textedit);
    }

    if (event->left)
    {
        if (poulpe_selection_active(textedit->selection))
        {
            textedit->cursor->position.x = textedit->selection->ajusted.start_line_index;
            textedit->cursor->position.y = textedit->selection->ajusted.start_glyph_index;
            poulpe_selection_clear(textedit->selection);
            poulpe_cursor_reset(textedit->cursor);
        }
        else
        {
            error = _handle_keyboard_left(textedit, event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }
        poulpe_cursor_update(textedit->cursor);
        _ensure_cursor_visiblity(textedit);
    }

    if (event->right)
    {
        if (poulpe_selection_active(textedit->selection))
        {
            textedit->cursor->position.x = textedit->selection->ajusted.end_line_index;
            textedit->cursor->position.y = textedit->selection->ajusted.end_glyph_index;
            poulpe_selection_clear(textedit->selection);
            poulpe_cursor_reset(textedit->cursor);
        }
        else
        {
            error = _handle_keyboard_right(textedit, event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }
        poulpe_cursor_update(textedit->cursor);
        _ensure_cursor_visiblity(textedit);
    }

    if (event->up)
    {
        if (poulpe_selection_active(textedit->selection))
        {
            textedit->cursor->position.x = textedit->selection->ajusted.start_line_index;
            textedit->cursor->position.y = textedit->selection->ajusted.start_glyph_index;
            poulpe_selection_clear(textedit->selection);
        }

        error = _handle_keyboard_up(textedit, event);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_cursor_update(textedit->cursor);
        _ensure_cursor_visiblity(textedit);
    }

    if (event->down)
    {
        if (poulpe_selection_active(textedit->selection))
        {
            textedit->cursor->position.x = textedit->selection->ajusted.end_line_index;
            textedit->cursor->position.y = textedit->selection->ajusted.end_glyph_index;
            poulpe_selection_clear(textedit->selection);
        }

        error = _handle_keyboard_down(textedit, event);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_cursor_update(textedit->cursor);
        _ensure_cursor_visiblity(textedit);
    }

    if (event->enter)
    {
        if (poulpe_selection_active(textedit->selection))
        {
            textedit->cursor->position.x = textedit->selection->ajusted.start_line_index;
            textedit->cursor->position.y = textedit->selection->ajusted.start_glyph_index;
            error = poulpe_selection_delete(textedit->selection);
            if (error != POULPE_ERROR_NONE)
                return error;
            poulpe_selection_clear(textedit->selection);
            poulpe_cursor_reset(textedit->cursor);
        }

        error = _handle_keyboard_enter(textedit, event);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_cursor_update(textedit->cursor);
        _ensure_cursor_visiblity(textedit);
    }

    if (event->tab)
    {
        if (poulpe_selection_active(textedit->selection))
        {
            textedit->cursor->position.x = textedit->selection->ajusted.start_line_index;
            textedit->cursor->position.y = textedit->selection->ajusted.start_glyph_index;
            error = poulpe_selection_delete(textedit->selection);
            if (error != POULPE_ERROR_NONE)
                return error;
            poulpe_selection_clear(textedit->selection);
            poulpe_cursor_reset(textedit->cursor);
        }

        error = _handle_keyboard_tab(textedit, event);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_cursor_update(textedit->cursor);
        _ensure_cursor_visiblity(textedit);
    }

    if (event->count)
    {
        if (poulpe_selection_active(textedit->selection))
        {
            textedit->cursor->position.x = textedit->selection->ajusted.start_line_index;
            textedit->cursor->position.y = textedit->selection->ajusted.start_glyph_index;
            error = poulpe_selection_delete(textedit->selection);
            if (error != POULPE_ERROR_NONE)
                return error;
            poulpe_selection_clear(textedit->selection);
            poulpe_cursor_reset(textedit->cursor);
        }

        error = _handle_keyboard_default(textedit, event);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_cursor_update(textedit->cursor);
        _ensure_cursor_visiblity(textedit);
    }

    return error;
}

static enum poulpe_error _handle_keyboard_delete(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    poulpe_text text = textedit->textview->textbuffer->text;
    uint32_t line_index = textedit->cursor->position.x;
    uint32_t glyph_index = textedit->cursor->position.y;

    if (glyph_index < poulpe_line_raw_size(text[line_index]))
        poulpe_line_erase(text[line_index], glyph_index);
    else if (line_index < poulpe_text_size(text) - 1)
    {
        text[line_index] = poulpe_line_push_back(text[line_index], text[line_index + 1]);
        poulpe_text_erase(text, line_index + 1);
    }
        
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_backspace(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    poulpe_text text = textedit->textview->textbuffer->text;
    uint32_t line_index = textedit->cursor->position.x;
    uint32_t glyph_index = textedit->cursor->position.y;

    if (glyph_index != 0)
    {
        uint32_t utf8_index = poulpe_line_utf8_index(text[line_index], glyph_index);
        uint32_t raw_index = poulpe_line_raw_index(text[line_index], utf8_index - 1);
        poulpe_line_erase(text[line_index], raw_index);
        poulpe_cursor_move_left(textedit->cursor);
    }
    else if (line_index != 0)
    {
        uint32_t previous_size = poulpe_line_raw_size(text[line_index - 1]);
        text[line_index - 1] = poulpe_line_push_back(text[line_index - 1], text[line_index]);
        poulpe_text_erase(text, line_index);
        poulpe_cursor_move_up(textedit->cursor);
        textedit->cursor->position.y = previous_size;
    }
    
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_left(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    poulpe_cursor_move_left(textedit->cursor);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_right(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    poulpe_cursor_move_right(textedit->cursor);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_up(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    poulpe_cursor_move_up(textedit->cursor);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_down(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    poulpe_cursor_move_down(textedit->cursor);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_enter(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    struct poulpe_textbuffer *textbuffer = textedit->textview->textbuffer;
    uint32_t line_index = textedit->cursor->position.x;
    uint32_t glyph_index = textedit->cursor->position.y;

    poulpe_line line = poulpe_line_new_range(textbuffer->text[line_index], textbuffer->text[line_index] + glyph_index);
    if (!line)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
        return POULPE_ERROR_MEMORY;
    }

    poulpe_line_erase_range(textbuffer->text[line_index], 0, glyph_index);

    textbuffer->text = poulpe_text_insert(textbuffer->text, line_index, &line);
    if (!textbuffer->text)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert new line");
        return POULPE_ERROR_MEMORY;
    }

    textedit->cursor->position.y = 0;
    poulpe_cursor_move_down(textedit->cursor);

    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_tab(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);
    
    poulpe_text text = textedit->textview->textbuffer->text;
    uint32_t line_index = textedit->cursor->position.x;
    uint32_t glyph_index = textedit->cursor->position.y;

    text[line_index] = poulpe_line_insert(text[line_index], glyph_index, "\t");
    if (!text[line_index])
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back glyph");
        return POULPE_ERROR_MEMORY;
    }

    poulpe_cursor_move_right(textedit->cursor);

    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_default(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    poulpe_text text = textedit->textview->textbuffer->text;
    uint32_t line_index = textedit->cursor->position.x;
    for (uint32_t i = 0; i < event->count; i++)
    {
        uint32_t glyph_index = textedit->cursor->position.y;
        uint32_t utf8 = sake_utils_utf8_from_code_point(event->data[i]);
        text[line_index] = poulpe_line_insert(text[line_index], glyph_index, (const char *) &utf8);
        if (!text[line_index])
        {
            POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back glyph");
            return POULPE_ERROR_MEMORY;
        }
        poulpe_cursor_move_right(textedit->cursor);
    }
    return POULPE_ERROR_NONE;
}