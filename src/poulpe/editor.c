#include <stdlib.h>
#include <math.h>

#include <cimgui.h>

#include "poulpe/editor.h"
#include "poulpe/log.h"
#include "poulpe/text.h"
#include "poulpe/textbuffer.h"
#include "poulpe/style.h"
#include "poulpe/io.h"
#include "poulpe/event.h"

#include "poulpe/component.h"
#include "poulpe/components/textview.h"
#include "poulpe/components/textedit.h"
#include "poulpe/components/cursor.h"
#include "poulpe/components/statusbar.h"

struct poulpe_editor* poulpe_editor_new(const char *path)
{
    struct poulpe_editor *editor;

    editor = calloc(1, sizeof(struct poulpe_textview));
    if (!editor)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate textview");
        return NULL;
    }

    editor->textview = (struct poulpe_textview *) poulpe_component_new(POULPE_COMPONENT_TYPE_TEXTVIEW);
    if (!editor->textview)
        return NULL;

    if (poulpe_textview_open_file(editor->textview, path) != POULPE_ERROR_NONE)
        return NULL;

    editor->statusbar = (struct poulpe_statusbar *) poulpe_component_new(POULPE_COMPONENT_TYPE_STATUSBAR);
    if (!editor->statusbar)
        return NULL;

    poulpe_textview_set_editor(editor->textview, editor);
    poulpe_statusbar_set_editor(editor->statusbar, editor);

    return editor;
}

void poulpe_editor_free(struct poulpe_editor *editor)
{
    poulpe_component_free((struct poulpe_component *) editor->statusbar);
    poulpe_component_free((struct poulpe_component *) editor->textview);
    free(editor);
}

enum poulpe_error poulpe_editor_draw(struct poulpe_editor *editor)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    igPushFont(poulpe_style.large_font);

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowMinSize, (ImVec2) {0.f, 0.f});
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2) {0.f, 0.f});
    igPushStyleVar_Vec2(ImGuiStyleVar_ItemSpacing, (ImVec2) {0.f, 0.f});
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.f);

    igPushStyleColor_U32(ImGuiCol_Text, igColorConvertFloat4ToU32(poulpe_style.theme->primary_text));
    igPushStyleColor_U32(ImGuiCol_ChildBg, igColorConvertFloat4ToU32(poulpe_style.theme->border));
    igPushStyleColor_U32(ImGuiCol_Border, igColorConvertFloat4ToU32(poulpe_style.theme->border));
    igPushStyleColor_U32(ImGuiCol_FrameBg, igColorConvertFloat4ToU32(poulpe_style.theme->main_background));
    igPushStyleColor_U32(ImGuiCol_Button, igColorConvertFloat4ToU32(poulpe_style.theme->main_background));
    igPushStyleColor_U32(ImGuiCol_ButtonHovered, igColorConvertFloat4ToU32(poulpe_style.theme->hovered_border));
    igPushStyleColor_U32(ImGuiCol_ButtonActive, igColorConvertFloat4ToU32(poulpe_style.theme->active_border));
    igPushStyleColor_U32(ImGuiCol_ScrollbarBg, igColorConvertFloat4ToU32(poulpe_style.theme->secondary_background));
    igPushStyleColor_U32(ImGuiCol_ScrollbarGrab, igColorConvertFloat4ToU32(poulpe_style.theme->widget));
    igPushStyleColor_U32(ImGuiCol_ScrollbarGrabHovered, igColorConvertFloat4ToU32(poulpe_style.theme->hovered_widget));
    igPushStyleColor_U32(ImGuiCol_ScrollbarGrabActive, igColorConvertFloat4ToU32(poulpe_style.theme->active_widget));

    if (!igBeginChild_Str("Poulpe##editor", (ImVec2){0}, false, ImGuiWindowFlags_NoScrollbar))
        goto end_child;
    
    error = poulpe_component_draw((struct poulpe_component *) editor->textview);
    if (error != POULPE_ERROR_NONE)
        goto end_child;

    error = poulpe_component_draw((struct poulpe_component *) editor->statusbar);
    if (error != POULPE_ERROR_NONE)
        goto end_child;
    
end_child:
    igEndChild();
    igPopStyleVar(4);
    igPopStyleColor(11);
    igPopFont();

    return error;
}

enum poulpe_error poulpe_editor_save(struct poulpe_editor *editor)
{
    return poulpe_textbuffer_save_file(editor->textview->textbuffer);
}

const char * poulpe_editor_filename(struct poulpe_editor *editor)
{
    return editor->textview->textbuffer->filename;
}

const char * poulpe_editor_path(struct poulpe_editor *editor)
{
    return editor->textview->textbuffer->path;
}

enum poulpe_error poulpe_editor_set_language(struct poulpe_editor *editor, enum poulpe_language_type language)
{
    return poulpe_textbuffer_set_language(editor->textview->textbuffer, language);
}