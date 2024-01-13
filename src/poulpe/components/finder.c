#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>

#include <cimgui.h>

#include <sake/macro.h>
#include <sake/vector.h>

#include "poulpe/components/finder.h"
#include "poulpe/components/statusbar.h"
#include "poulpe/components/textview.h"
#include "poulpe/components/textedit.h"

#include "poulpe/editor.h"
#include "poulpe/log.h"
#include "poulpe/text.h"
#include "poulpe/style.h"
#include "poulpe/io.h"
#include "poulpe/textbuffer.h"

static enum poulpe_error _handle_keyboard(struct poulpe_finder *finder, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_enter(struct poulpe_finder *finder, struct poulpe_event_keyboard *event);
static enum poulpe_error _handle_keyboard_shift_enter(struct poulpe_finder *finder, struct poulpe_event_keyboard *event);

struct poulpe_finder * poulpe_finder_new(void)
{
    struct poulpe_finder *finder;

    finder = calloc(1, sizeof(struct poulpe_finder));
    if (!finder)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate finder");
        return NULL;
    }

    finder->results = sake_vector_new(sizeof(ImVec2), NULL);

    finder->case_sensitive = true;

    return finder;
}

void poulpe_finder_free(struct poulpe_finder *finder)
{
    sake_vector_free(finder->results);
    free(finder);
}

enum poulpe_error poulpe_finder_notify(struct poulpe_finder *finder, struct poulpe_event *event)
{
    if (!igIsWindowFocused(ImGuiFocusedFlags_ChildWindows))
        return POULPE_ERROR_NONE;
    
    switch (event->type)
    { 
    case POULPE_EVENT_TYPE_MOUSE:
        return POULPE_ERROR_NONE;

    case POULPE_EVENT_TYPE_KEYBOARD:
        return _handle_keyboard(finder, (struct poulpe_event_keyboard *)event);

    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown event type");
        return POULPE_ERROR_UNKNOWN;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_finder_draw(struct poulpe_finder *finder)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    ImVec2 content;
    igGetContentRegionAvail(&content);

    ImGuiIO *io = igGetIO();
    io->ConfigInputTextEnterKeepActive = true;

    error = poulpe_io_handle_keyboard((struct poulpe_component *) finder);
    if (error != POULPE_ERROR_NONE)
        return error;

    ImDrawList *draw_list = igGetWindowDrawList();
    ImGuiWindow *window = igGetCurrentWindowRead();

    ImDrawList_AddRect(draw_list, 
                       (ImVec2) {window->DC.CursorPos.x - 1, window->DC.CursorPos.y - 1},
                       (ImVec2) {window->InnerRect.Max.x, window->InnerRect.Max.y}, igColorConvertFloat4ToU32(poulpe_style.theme->border), 0.0f, 0, 1.0f);

    igSetCursorPosY(1);

    char prev_find[64] = "";
    strcpy(prev_find, finder->find);
    igInputTextEx("##Find", "Find", finder->find, sizeof(finder->find), (ImVec2) {200.f, content.y - 1}, 0, NULL, NULL);

    bool tmp_case_sensitive = finder->case_sensitive;
    if (tmp_case_sensitive)
    {
        igPushStyleColor_U32(ImGuiCol_Button, igColorConvertFloat4ToU32(poulpe_style.theme->hovered_border));
        igPushStyleColor_U32(ImGuiCol_ButtonHovered, igColorConvertFloat4ToU32(poulpe_style.theme->active_border));
        igPushStyleColor_U32(ImGuiCol_ButtonActive, igColorConvertFloat4ToU32(poulpe_style.theme->widget));
    }

    igSameLine(0.0f, -1.0f);
    igSetCursorPosY(1);
    if (igSmallButton("Aa"))
        finder->case_sensitive = !finder->case_sensitive;

    if (tmp_case_sensitive)
        igPopStyleColor(3);

    igSameLine(0.0f, -1.0f);
    ImDrawList_AddRect(draw_list, 
                       (ImVec2) {window->DC.CursorPos.x - 1, window->DC.CursorPos.y - 1},
                       (ImVec2) {window->InnerRect.Max.x, window->InnerRect.Max.y}, igColorConvertFloat4ToU32(poulpe_style.theme->border), 0.0f, 0, 1.0f);

    igSetCursorPosY(1);
    igInputTextEx("##Replace", "Replace", finder->replace, sizeof(finder->replace), (ImVec2) {200.f, content.y - 1}, 0, NULL, NULL);

    igSameLine(0.0f, -1.0f);
    igSetCursorPosY(1);
    igSmallButton("All");

    igSameLine(0.0f, -1.0f);

    ImDrawList_AddRect(draw_list, 
                       (ImVec2) {window->DC.CursorPos.x - 1, window->DC.CursorPos.y - 1},
                       (ImVec2) {window->InnerRect.Max.x, window->InnerRect.Max.y}, igColorConvertFloat4ToU32(poulpe_style.theme->border), 0.0f, 0, 1.0f);

    igSameLine(0.0f, content.y);
    igSetCursorPosY(1);

    if (sake_vector_size(finder->results))
        igText("%u of %u", finder->result_index + 1, sake_vector_size(finder->results));
    else
        igText("No results");

    return POULPE_ERROR_NONE;
}

void poulpe_finder_set_statusbar(struct poulpe_finder *finder, struct poulpe_statusbar *statusbar)
{
    finder->statusbar = statusbar;
}

static enum poulpe_error _handle_keyboard(struct poulpe_finder *finder, struct poulpe_event_keyboard *event)
{
    if (event->enter && !event->shift)
        return _handle_keyboard_enter(finder, event);
    if (event->enter && event->shift)
        return _handle_keyboard_shift_enter(finder, event);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_shift_enter(struct poulpe_finder *finder, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    if (strlen(finder->find))
    {
        struct poulpe_textbuffer *textbuffer = finder->statusbar->editor->textview->textbuffer;
        ImVec2 pos = {0, 0};
        sake_vector_clear(finder->results);
        while (poulpe_textbuffer_find(textbuffer, finder->find, false, &pos))
        {
            finder->results = sake_vector_push_back(finder->results, &pos);
            pos.y += strlen(finder->find);
        }
    }

    if (sake_vector_size(finder->results))
    {
        finder->result_index = (finder->result_index - 1 + sake_vector_size(finder->results)) % sake_vector_size(finder->results);
        finder->focus = true;
    }

    return POULPE_ERROR_NONE;
}

static enum poulpe_error _handle_keyboard_enter(struct poulpe_finder *finder, struct poulpe_event_keyboard *event)
{
    SAKE_MACRO_UNUSED(event);

    if (strlen(finder->find))
    {
        struct poulpe_textbuffer *textbuffer = finder->statusbar->editor->textview->textbuffer;
        ImVec2 pos = {0, 0};
        sake_vector_clear(finder->results);
        while (poulpe_textbuffer_find(textbuffer, finder->find, false, &pos))
        {
            finder->results = sake_vector_push_back(finder->results, &pos);
            pos.y += strlen(finder->find);
        }
    }

    if (sake_vector_size(finder->results))
    {
        finder->result_index = (finder->result_index + 1) % sake_vector_size(finder->results);
        finder->focus = true;
    }

    return POULPE_ERROR_NONE;
}