#include <stdlib.h>
#include <string.h>
#include <float.h>

#include <sake/vector.h>
#include <sake/string.h>
#include <sake/macro.h>

#include "poulpe/components/textview.h"
#include "poulpe/components/cursor.h"
#include "poulpe/components/selection.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"

static enum poulpe_error _handle_keyboard_delete(struct poulpe_textview *textview, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_backspace(struct poulpe_textview *textview, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_left(struct poulpe_textview *textview, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_right(struct poulpe_textview *textview, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_up(struct poulpe_textview *textview, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_down(struct poulpe_textview *textview, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_enter(struct poulpe_textview *textview, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_tab(struct poulpe_textview *textview, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_default(struct poulpe_textview *textview, struct poulpe_event_keyboard *event);

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
    if (!textview->cursor)
        return NULL;

    poulpe_cursor_set_textview(textview->cursor, textview);

    textview->selection = (struct poulpe_selection *) poulpe_component_new(POULPE_COMPONENT_TYPE_SELECTION);
    if (!textview->selection)
        return NULL;

    poulpe_selection_set_textview(textview->selection, textview);
    
    return textview;
}

void poulpe_textview_free(struct poulpe_textview *textview)
{
    poulpe_component_free((struct poulpe_component *) textview->selection);
    poulpe_component_free((struct poulpe_component *) textview->cursor);
    free(textview);
}

enum poulpe_error poulpe_textview_notify(struct poulpe_textview *textview, struct poulpe_event *event)
{
    switch (event->type)
    {
    case POULPE_EVENT_TYPE_MOUSE:
    {
        struct poulpe_event_mouse *mouse_event = (struct poulpe_event_mouse *) event;
        igSetMouseCursor(ImGuiMouseCursor_TextInput);

        if (!mouse_event->base.dirty)
            break;
        
        ImVec2 coordinates = {0};
        poulpe_textview_get_coordinates(textview, mouse_event->position, &coordinates);

        if (mouse_event->left_clicked)
        {
            poulpe_cursor_update_position(textview->cursor, coordinates);
            poulpe_selection_update_start(textview->selection, coordinates);
            poulpe_selection_update_end(textview->selection, coordinates);
        }

        if (mouse_event->dragged)
        {
            poulpe_cursor_update_position(textview->cursor, coordinates);
            poulpe_selection_update_end(textview->selection, coordinates);
        }
        
        break;
    }
    case POULPE_EVENT_TYPE_KEYBOARD:
    {
        enum poulpe_error error = POULPE_ERROR_NONE;
        struct poulpe_event_keyboard *keyboard_event = (struct poulpe_event_keyboard *)event;

        if (keyboard_event->delete)
        {
            error = _handle_keyboard_delete(textview, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }

        if (keyboard_event->backspace)
        {
            error = _handle_keyboard_backspace(textview, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }

        if (keyboard_event->left)
        {
            error = _handle_keyboard_left(textview, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }

        if (keyboard_event->right)
        {
            error = _handle_keyboard_right(textview, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }

        if (keyboard_event->up)
        {
            error = _handle_keyboard_up(textview, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }

        if (keyboard_event->down)
        {
            error = _handle_keyboard_down(textview, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }

        if (keyboard_event->enter)
        {
            error = _handle_keyboard_enter(textview, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }

        if (keyboard_event->tab)
        {
            error = _handle_keyboard_tab(textview, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }

        if (keyboard_event->count)
        {
            error = _handle_keyboard_default(textview, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;
        }
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

    poulpe_component_draw((struct poulpe_component *)textview->selection);
    poulpe_component_draw((struct poulpe_component *)textview->cursor);

    for (uint32_t i = 0; i < poulpe_textbuffer_text_size(textview->textbuffer); i++)
    {
        ImVec2 line_start_position = {origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};

        uint32_t line_length = poulpe_textbuffer_line_size(textview->textbuffer, i);
        /* worst case */
        char buffer[2 * line_length + 1];

        float line_textsize = poulpe_textbuffer_line_full_textsize(textview->textbuffer, i);
        if (line_textsize > maximum_width)
            maximum_width = line_textsize;

        
        uint32_t k = 0;
        for (uint32_t j = 0; j < line_length; j++)
        {
            struct poulpe_glyph *g = poulpe_textbuffer_line_at(textview->textbuffer, i, j);
            memcpy(buffer + k, (char *) g->utf8, g->size);
            buffer[k + g->size] = '\0';
            
            if (buffer[k] == ' ')
            {
                ImVec2 text_size;
                ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, buffer, NULL, NULL);
                ImVec2 space_size;
                ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);

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

            if (buffer[k] == '\t')
            {
                ImVec2 text_size;
                ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, buffer, NULL, NULL);
                ImVec2 tab_size;
                ImFont_CalcTextSizeA(&tab_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, "\t", NULL, NULL);

                float font_size = igGetFontSize();
                float thickness = 0.5f;
                ImVec2 center = {line_start_position.x + text_size.x - 0.5 * tab_size.x, line_start_position.y + 0.5 * font_size};
                ImDrawList *draw_list = igGetWindowDrawList();
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

            k += g->size;
        }

        buffer[k] = '\0';
        
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
   
    ImVec2 size = {maximum_width, poulpe_textbuffer_text_size(textview->textbuffer) * igGetTextLineHeight()};
    ImVec2 upper_left = {origin_screen_position.x, origin_screen_position.y};
    ImVec2 lower_right = {origin_screen_position.x + size.x, origin_screen_position.y + size.y};
    ImRect rect = {upper_left, lower_right};
    igItemSize_Vec2(size, -1.0f);
    igItemAdd(rect, 0, NULL, 0);
    
    return POULPE_ERROR_NONE;
}

void poulpe_textview_set_textbuffer(struct poulpe_textview *textview, struct poulpe_textbuffer *textbuffer)
{
    textview->textbuffer = textbuffer;
}

void poulpe_textview_get_coordinates(struct poulpe_textview *textview, ImVec2 mouse_position, ImVec2 *coordinates)
{
    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);

    uint32_t line_index = 0;
    uint32_t last_line = poulpe_textbuffer_text_size(textview->textbuffer) - 1;
    while (line_index < last_line && mouse_position.y > origin_screen_position.y + (line_index + 1) * igGetTextLineHeight())
        line_index++;
    
    uint32_t glyph_index = 0;
    while (glyph_index < poulpe_textbuffer_line_size(textview->textbuffer, line_index))
    {
        float text_size = poulpe_textbuffer_line_subset_textsize(textview->textbuffer, line_index, 0, glyph_index);
        float text_size_advance = poulpe_textbuffer_line_subset_textsize(textview->textbuffer, line_index, 0, glyph_index + 1);

        /* Find the closest glyph */
        if (mouse_position.x < origin_screen_position.x + text_size_advance)
        {
            float diff_text = mouse_position.x - origin_screen_position.x - text_size;
            float diff_text_advance = origin_screen_position.x + text_size_advance - mouse_position.x;
            glyph_index = diff_text < diff_text_advance ? glyph_index: glyph_index + 1;
            break;
        }
        glyph_index++;
    }

    coordinates->x = line_index;
    coordinates->y = glyph_index;
}

static enum poulpe_error _handle_keyboard_delete(struct poulpe_textview *textview, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index);
    textview->cursor->glyph_index = textview->cursor->glyph_index > line_length ? line_length : textview->cursor->glyph_index;

    if (textview->cursor->glyph_index < poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index))
        poulpe_textbuffer_line_erase(textview->textbuffer, textview->cursor->line_index, textview->cursor->glyph_index);
    else if (textview->cursor->line_index < poulpe_textbuffer_text_size(textview->textbuffer) - 1)
    {
        for (uint32_t i = 0; i < poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index + 1); i++)
        {
            struct poulpe_glyph *glyph = poulpe_textbuffer_line_at(textview->textbuffer, textview->cursor->line_index + 1, i);
            poulpe_textbuffer_line_push_back(textview->textbuffer, textview->cursor->line_index, glyph);
        }
        poulpe_textbuffer_text_erase(textview->textbuffer, textview->cursor->line_index + 1);
    }
        
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_backspace(struct poulpe_textview *textview, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index);
    textview->cursor->glyph_index = textview->cursor->glyph_index > line_length ? line_length : textview->cursor->glyph_index;

    if (textview->cursor->glyph_index)
    {
        poulpe_textbuffer_line_erase(textview->textbuffer, textview->cursor->line_index, textview->cursor->glyph_index - 1);
        poulpe_cursor_move_left(textview->cursor);
    }
    else if (textview->cursor->line_index)
    {
        uint32_t previous_size = poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index - 1);
        for (uint32_t i = 0; i < poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index); i++)
        {
            struct poulpe_glyph *glyph = poulpe_textbuffer_line_at(textview->textbuffer, textview->cursor->line_index, i);
            poulpe_textbuffer_line_push_back(textview->textbuffer, textview->cursor->line_index - 1, glyph);
        }

        poulpe_textbuffer_text_erase(textview->textbuffer, textview->cursor->line_index);
        poulpe_cursor_move_up(textview->cursor);
        textview->cursor->glyph_index = previous_size;
    }
    
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_left(struct poulpe_textview *textview, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index);
    textview->cursor->glyph_index = textview->cursor->glyph_index > line_length ? line_length : textview->cursor->glyph_index;

    poulpe_cursor_move_left(textview->cursor);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_right(struct poulpe_textview *textview, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index);
    textview->cursor->glyph_index = textview->cursor->glyph_index > line_length ? line_length : textview->cursor->glyph_index;

    poulpe_cursor_move_right(textview->cursor);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_up(struct poulpe_textview *textview, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    poulpe_cursor_move_up(textview->cursor);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_down(struct poulpe_textview *textview, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    poulpe_cursor_move_down(textview->cursor);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_enter(struct poulpe_textview *textview, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index);
    textview->cursor->glyph_index = textview->cursor->glyph_index > line_length ? line_length : textview->cursor->glyph_index;

    enum poulpe_error error =  poulpe_textbuffer_new_line(textview->textbuffer, textview->cursor->line_index, textview->cursor->glyph_index);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    textview->cursor->glyph_index = 0;
    poulpe_cursor_move_down(textview->cursor);
        
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_tab(struct poulpe_textview *textview, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index);
    textview->cursor->glyph_index = textview->cursor->glyph_index > line_length ? line_length : textview->cursor->glyph_index;
    
    struct poulpe_glyph glyph = {0};
    enum poulpe_error error = poulpe_glyph_from_char(&glyph, '\t');
    if (error != POULPE_ERROR_NONE)
        return error;

    error = poulpe_textbuffer_line_insert(textview->textbuffer, textview->cursor->line_index, textview->cursor->glyph_index, &glyph);
    if (error != POULPE_ERROR_NONE)
        return error;

    poulpe_cursor_move_right(textview->cursor);

    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_default(struct poulpe_textview *textview, struct poulpe_event_keyboard *event)
{
    uint32_t line_length = poulpe_textbuffer_line_size(textview->textbuffer, textview->cursor->line_index);
    textview->cursor->glyph_index = textview->cursor->glyph_index > line_length ? line_length : textview->cursor->glyph_index;

    for (uint32_t i = 0; i < event->count; i++)
    {
        struct poulpe_glyph glyph = {0};
        enum poulpe_error error = poulpe_glyph_from_char(&glyph, event->data[i]);
        if (error != POULPE_ERROR_NONE)
            return error;

        error = poulpe_textbuffer_line_insert(textview->textbuffer, textview->cursor->line_index, textview->cursor->glyph_index, &glyph);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_cursor_move_right(textview->cursor);
    }
    return POULPE_ERROR_NONE;
}