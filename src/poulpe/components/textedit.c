#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <math.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "poulpe/components/textedit.h"
#include "poulpe/components/selection.h"
#include "poulpe/components/cursor.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"
#include "poulpe/textbuffer.h"
#include "poulpe/io.h"

static enum poulpe_error _update_view(struct poulpe_textedit *textedit);
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
    free(textedit);
}

enum poulpe_error poulpe_textedit_notify(struct poulpe_textedit *textedit, struct poulpe_event *event)
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
        poulpe_textedit_get_coordinates(textedit, mouse_event->position, &coordinates);

        if (mouse_event->left_clicked)
        {
            poulpe_cursor_update_position(textedit->cursor, coordinates);
            poulpe_selection_update_start(textedit->selection, coordinates);
            poulpe_selection_update_end(textedit->selection, coordinates);
            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }

        if (mouse_event->dragged)
        {
            poulpe_cursor_update_position(textedit->cursor, coordinates);
            poulpe_selection_update_end(textedit->selection, coordinates);
            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }
        
        break;
    }
    case POULPE_EVENT_TYPE_KEYBOARD:
    {
        enum poulpe_error error = POULPE_ERROR_NONE;
        struct poulpe_event_keyboard *keyboard_event = (struct poulpe_event_keyboard *)event;

        if (keyboard_event->delete)
        {
            if (poulpe_selection_active(textedit->selection))
            {
                textedit->cursor->line_index = textedit->selection->ajusted.start_line_index;
                textedit->cursor->glyph_index = textedit->selection->ajusted.start_glyph_index;
                error = poulpe_selection_delete(textedit->selection);
                if (error != POULPE_ERROR_NONE)
                    return error;
                poulpe_selection_clear(textedit->selection);
                poulpe_cursor_reset(textedit->cursor);
            }
            else
            {
                error = _handle_keyboard_delete(textedit, keyboard_event);
                if (error != POULPE_ERROR_NONE)
                    return error;
            }
            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }

        if (keyboard_event->backspace)
        {
            if (poulpe_selection_active(textedit->selection))
            {
                textedit->cursor->line_index = textedit->selection->ajusted.start_line_index;
                textedit->cursor->glyph_index = textedit->selection->ajusted.start_glyph_index;
                error = poulpe_selection_delete(textedit->selection);
                if (error != POULPE_ERROR_NONE)
                    return error;
                poulpe_selection_clear(textedit->selection);
                poulpe_cursor_reset(textedit->cursor);
            }
            else
            {
                error = _handle_keyboard_backspace(textedit, keyboard_event);
                if (error != POULPE_ERROR_NONE)
                    return error;
            }
            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }

        if (keyboard_event->left)
        {
            if (poulpe_selection_active(textedit->selection))
            {
                textedit->cursor->line_index = textedit->selection->ajusted.start_line_index;
                textedit->cursor->glyph_index = textedit->selection->ajusted.start_glyph_index;
                poulpe_selection_clear(textedit->selection);
                poulpe_cursor_reset(textedit->cursor);
            }
            else
            {
                error = _handle_keyboard_left(textedit, keyboard_event);
                if (error != POULPE_ERROR_NONE)
                    return error;
            }
            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }

        if (keyboard_event->right)
        {
            if (poulpe_selection_active(textedit->selection))
            {
                textedit->cursor->line_index = textedit->selection->ajusted.end_line_index;
                textedit->cursor->glyph_index = textedit->selection->ajusted.end_glyph_index;
                poulpe_selection_clear(textedit->selection);
                poulpe_cursor_reset(textedit->cursor);
            }
            else
            {
                error = _handle_keyboard_right(textedit, keyboard_event);
                if (error != POULPE_ERROR_NONE)
                    return error;
            }
            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }

        if (keyboard_event->up)
        {
            if (poulpe_selection_active(textedit->selection))
            {
                textedit->cursor->line_index = textedit->selection->ajusted.start_line_index;
                textedit->cursor->glyph_index = textedit->selection->ajusted.start_glyph_index;
                poulpe_selection_clear(textedit->selection);
            }

            error = _handle_keyboard_up(textedit, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;

            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }

        if (keyboard_event->down)
        {
            if (poulpe_selection_active(textedit->selection))
            {
                textedit->cursor->line_index = textedit->selection->ajusted.end_line_index;
                textedit->cursor->glyph_index = textedit->selection->ajusted.end_glyph_index;
                poulpe_selection_clear(textedit->selection);
            }
            
            error = _handle_keyboard_down(textedit, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;
            
            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }

        if (keyboard_event->enter)
        {
            if (poulpe_selection_active(textedit->selection))
            {
                textedit->cursor->line_index = textedit->selection->ajusted.start_line_index;
                textedit->cursor->glyph_index = textedit->selection->ajusted.start_glyph_index;
                error = poulpe_selection_delete(textedit->selection);
                if (error != POULPE_ERROR_NONE)
                    return error;
                poulpe_selection_clear(textedit->selection);
                poulpe_cursor_reset(textedit->cursor);
            }
            
            error = _handle_keyboard_enter(textedit, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;

            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }

        if (keyboard_event->tab)
        {
            if (poulpe_selection_active(textedit->selection))
            {
                textedit->cursor->line_index = textedit->selection->ajusted.start_line_index;
                textedit->cursor->glyph_index = textedit->selection->ajusted.start_glyph_index;
                error = poulpe_selection_delete(textedit->selection);
                if (error != POULPE_ERROR_NONE)
                    return error;
                poulpe_selection_clear(textedit->selection);
                poulpe_cursor_reset(textedit->cursor);
            }

            error = _handle_keyboard_tab(textedit, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;

            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }

        if (keyboard_event->count)
        {
            if (poulpe_selection_active(textedit->selection))
            {
                textedit->cursor->line_index = textedit->selection->ajusted.start_line_index;
                textedit->cursor->glyph_index = textedit->selection->ajusted.start_glyph_index;
                error = poulpe_selection_delete(textedit->selection);
                if (error != POULPE_ERROR_NONE)
                    return error;
                poulpe_selection_clear(textedit->selection);
                poulpe_cursor_reset(textedit->cursor);
            }
            
            error = _handle_keyboard_default(textedit, keyboard_event);
            if (error != POULPE_ERROR_NONE)
                return error;

            poulpe_textedit_ensure_cursor_visiblity(textedit);
        }
        break;
    }
    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown event type");
        return POULPE_ERROR_UNKNOWN;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textedit_draw(struct poulpe_textedit *textedit)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    igSameLine(0.0f, 0.0f);
    if (!igBeginChild_Str("Poulpe##textedit", (ImVec2) {0}, false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar))
        goto end_child;

    error = poulpe_io_handle_keyboard((struct poulpe_component *) textedit);
    if (error != POULPE_ERROR_NONE)
        goto end_child;
    
    error = poulpe_io_handle_mouse((struct poulpe_component *) textedit);
    if (error != POULPE_ERROR_NONE)
        goto end_child;
    
    _update_view(textedit);

    if (poulpe_selection_active(textedit->selection))
        poulpe_component_draw((struct poulpe_component *)textedit->selection);
    
    poulpe_component_draw((struct poulpe_component *)textedit->cursor);
    
    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);

    printf("textedit->line_start: %u\n", textedit->line_start);
    printf("textedit->line_end: %u\n", textedit->line_end);
    printf("textedit->cursor->line_index: %u\n", textedit->cursor->line_index);

    float maximum_width = 0;
    for (uint32_t i = textedit->line_start; i < textedit->line_end; i++)
    {
        ImVec2 line_start_position = {origin_screen_position.x, origin_screen_position.y + i * igGetTextLineHeight()};
        ImVec2 text_start_position = { line_start_position.x, line_start_position.y};

        uint32_t line_length = poulpe_textbuffer_line_size(textedit->textbuffer, i);
        /* worst case */
        char buffer[2 * line_length + 1];

        float line_textsize = poulpe_textbuffer_line_full_textsize(textedit->textbuffer, i);
        if (line_textsize > maximum_width)
            maximum_width = line_textsize;

        uint32_t k = 0;
        for (uint32_t j = 0; j < line_length; j++)
        {
            struct poulpe_glyph *g = poulpe_textbuffer_line_at(textedit->textbuffer, i, j);
            memcpy(buffer + k, (char *) g->utf8, g->size);
            buffer[k + g->size] = '\0';

            if (buffer[k] == ' ')
            {
                ImVec2 text_size;
                ImFont_CalcTextSizeA(&text_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, buffer, NULL, NULL);
                ImVec2 space_size;
                ImFont_CalcTextSizeA(&space_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, " ", NULL, NULL);

                float font_size = igGetFontSize();
                ImVec2 center = {text_start_position.x + text_size.x - 0.5 * space_size.x, text_start_position.y + 0.5 * font_size};
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
                ImVec2 center = {text_start_position.x + text_size.x - 0.5 * tab_size.x, text_start_position.y + 0.5 * font_size};

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
                                    text_start_position,
                                    igColorConvertFloat4ToU32(poulpe_theme_dark.text),
                                    buffer,
                                    NULL);
        }
    }

    {
        ImVec2 size = {maximum_width, poulpe_textbuffer_text_size(textedit->textbuffer) * igGetTextLineHeight()};
        ImVec2 upper_left = {origin_screen_position.x, origin_screen_position.y};
        ImVec2 lower_right = {origin_screen_position.x + size.x, origin_screen_position.y + size.y};
        ImRect rect = {upper_left, lower_right};
        igItemSize_Vec2(size, -1.0f);
        igItemAdd(rect, 0, NULL, 0);
    }

end_child:
    igEndChild();
    return error;
}

void poulpe_textedit_set_textbuffer(struct poulpe_textedit *textedit, struct poulpe_textbuffer *textbuffer)
{
    textedit->textbuffer = textbuffer;
}

void poulpe_textedit_ensure_cursor_visiblity(struct poulpe_textedit *textedit)
{
    ImGuiWindow *window = igGetCurrentWindowRead();
    ImVec2 content;
    igGetContentRegionAvail(&content);

    uint32_t start = window->Scroll.y / igGetTextLineHeight();
    uint32_t end = (window->Scroll.y + content.y) / igGetTextLineHeight() - 1;
    end = fmin(end, poulpe_textbuffer_text_size(textedit->textbuffer));
    end = fmax(end, 0);

    if (textedit->cursor->line_index < start)
        igSetScrollY_WindowPtr(window, textedit->cursor->line_index * igGetTextLineHeight());

    if (textedit->cursor->line_index > end)
        igSetScrollY_WindowPtr(window, (textedit->cursor->line_index + 1) * igGetTextLineHeight() - content.y);
}

static enum poulpe_error _update_view(struct poulpe_textedit *textedit)
{
    ImGuiWindow *window = igGetCurrentWindowRead();
    ImVec2 content;
    igGetContentRegionAvail(&content);

    textedit->line_start = window->Scroll.y / igGetTextLineHeight();
    textedit->line_end = (window->Scroll.y + content.y) / igGetTextLineHeight();
    textedit->line_end = fmin(textedit->line_end, poulpe_textbuffer_text_size(textedit->textbuffer));
    textedit->line_end = fmax(textedit->line_end, 0);

    textedit->y = window->Scroll.y;
    
    return POULPE_ERROR_NONE;
}

void poulpe_textedit_get_coordinates(struct poulpe_textedit *textedit, ImVec2 mouse_position, ImVec2 *coordinates)
{
    ImVec2 origin_screen_position;
    igGetCursorScreenPos(&origin_screen_position);

    uint32_t line_index = 0;
    uint32_t last_line = poulpe_textbuffer_text_size(textedit->textbuffer) - 1;
    while (line_index < last_line && mouse_position.y > origin_screen_position.y + (line_index + 1) * igGetTextLineHeight())
        line_index++;
    
    uint32_t glyph_index = 0;
    while (glyph_index < poulpe_textbuffer_line_size(textedit->textbuffer, line_index))
    {
        float text_size = poulpe_textbuffer_line_subset_textsize(textedit->textbuffer, line_index, 0, glyph_index);
        float text_size_advance = poulpe_textbuffer_line_subset_textsize(textedit->textbuffer, line_index, 0, glyph_index + 1);

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

static enum poulpe_error _handle_keyboard_delete(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index);
    textedit->cursor->glyph_index = textedit->cursor->glyph_index > line_length ? line_length : textedit->cursor->glyph_index;

    if (textedit->cursor->glyph_index < poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index))
        poulpe_textbuffer_line_erase(textedit->textbuffer, textedit->cursor->line_index, textedit->cursor->glyph_index);
    else if (textedit->cursor->line_index < poulpe_textbuffer_text_size(textedit->textbuffer) - 1)
    {
        for (uint32_t i = 0; i < poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index + 1); i++)
        {
            struct poulpe_glyph *glyph = poulpe_textbuffer_line_at(textedit->textbuffer, textedit->cursor->line_index + 1, i);
            poulpe_textbuffer_line_push_back(textedit->textbuffer, textedit->cursor->line_index, glyph);
        }
        poulpe_textbuffer_text_erase(textedit->textbuffer, textedit->cursor->line_index + 1);
    }
        
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_backspace(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index);
    textedit->cursor->glyph_index = textedit->cursor->glyph_index > line_length ? line_length : textedit->cursor->glyph_index;

    if (textedit->cursor->glyph_index != 0)
    {
        poulpe_textbuffer_line_erase(textedit->textbuffer, textedit->cursor->line_index, textedit->cursor->glyph_index - 1);
        poulpe_cursor_move_left(textedit->cursor);
    }
    else if (textedit->cursor->line_index != 0)
    {
        uint32_t previous_size = poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index - 1);
        for (uint32_t i = 0; i < poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index); i++)
        {
            struct poulpe_glyph *glyph = poulpe_textbuffer_line_at(textedit->textbuffer, textedit->cursor->line_index, i);
            poulpe_textbuffer_line_push_back(textedit->textbuffer, textedit->cursor->line_index - 1, glyph);
        }

        poulpe_textbuffer_text_erase(textedit->textbuffer, textedit->cursor->line_index);
        poulpe_cursor_move_up(textedit->cursor);
        textedit->cursor->glyph_index = previous_size;
    }
    
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_left(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index);
    textedit->cursor->glyph_index = textedit->cursor->glyph_index > line_length ? line_length : textedit->cursor->glyph_index;

    poulpe_cursor_move_left(textedit->cursor);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_right(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index);
    textedit->cursor->glyph_index = textedit->cursor->glyph_index > line_length ? line_length : textedit->cursor->glyph_index;

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

    uint32_t line_length = poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index);
    textedit->cursor->glyph_index = textedit->cursor->glyph_index > line_length ? line_length : textedit->cursor->glyph_index;

    enum poulpe_error error =  poulpe_textbuffer_new_line(textedit->textbuffer, textedit->cursor->line_index, textedit->cursor->glyph_index);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    textedit->cursor->glyph_index = 0;
    poulpe_cursor_move_down(textedit->cursor);
        
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_tab(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    uint32_t line_length = poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index);
    textedit->cursor->glyph_index = textedit->cursor->glyph_index > line_length ? line_length : textedit->cursor->glyph_index;
    
    struct poulpe_glyph glyph = {0};
    enum poulpe_error error = poulpe_glyph_from_char(&glyph, '\t');
    if (error != POULPE_ERROR_NONE)
        return error;

    error = poulpe_textbuffer_line_insert(textedit->textbuffer, textedit->cursor->line_index, textedit->cursor->glyph_index, &glyph);
    if (error != POULPE_ERROR_NONE)
        return error;

    poulpe_cursor_move_right(textedit->cursor);

    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_default(struct poulpe_textedit *textedit, struct poulpe_event_keyboard *event)
{
    uint32_t line_length = poulpe_textbuffer_line_size(textedit->textbuffer, textedit->cursor->line_index);
    textedit->cursor->glyph_index = textedit->cursor->glyph_index > line_length ? line_length : textedit->cursor->glyph_index;

    for (uint32_t i = 0; i < event->count; i++)
    {
        struct poulpe_glyph glyph = {0};
        enum poulpe_error error = poulpe_glyph_from_char(&glyph, event->data[i]);
        if (error != POULPE_ERROR_NONE)
            return error;

        error = poulpe_textbuffer_line_insert(textedit->textbuffer, textedit->cursor->line_index, textedit->cursor->glyph_index, &glyph);
        if (error != POULPE_ERROR_NONE)
            return error;

        poulpe_cursor_move_right(textedit->cursor);
    }
    return POULPE_ERROR_NONE;
}