#include <string.h>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_VULKAN
#include <cimgui_impl.h>

#include "poulpe/components/tabitem.h"
#include "poulpe/components/tabmenu.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"

#include "sake/macro.h"

enum poulpe_error poulpe_tabitem_init(struct poulpe_tabitem **tabitem)
{
    struct poulpe_tabitem *tabitem_;

    tabitem_ = calloc(1, sizeof(struct poulpe_tabitem));
    if (!tabitem_)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate tabitem");
        return POULPE_ERROR_MEMORY;
    }

    tabitem_->base.parent = NULL;
    tabitem_->base.type = POULPE_COMPONENT_TYPE_TABITEM;

    *tabitem = tabitem_;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_tabitem_update(struct poulpe_tabitem *tabitem, vec2 upper_left, vec2 lower_right)
{
    SAKE_MACRO_UNUSED(tabitem);
    SAKE_MACRO_UNUSED(upper_left);
    SAKE_MACRO_UNUSED(lower_right);
    return POULPE_ERROR_NONE;
}

//     void AddTab(size_t Index, const char *Text)
//     {
//         static const size_t TabWidth = 85;
//         static const size_t TabHeight = 18;

//         ImGui::PushID(Index);
//         ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

//         if (Index == 1)
//             ImGui::SameLine(Index * (TabWidth + 5));
//         else if (Index > 1)
//             ImGui::SameLine(Index * (TabWidth + 4 - Index));

//         if (CurTabOpen == Index)
//             ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(15, 15, 15)); // Color on tab open
//         else
//             ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(30, 30, 30)); // Color on tab closed

//         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(40, 40, 40)); // Color on mouse hover in tab
//         ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(35, 35, 35));  // Color on click tab

//         if (ImGui::Button(Text, ImVec2(TabWidth, TabHeight))) // If tab clicked
//             CurTabOpen = Index;

//         ImGui::PopStyleVar();
//         ImGui::PopStyleColor(3);
//         ImGui::PopID();
//     }

enum poulpe_error poulpe_tabitem_draw(struct poulpe_tabitem *tabitem)
{
    ImVec4 fg_color;
    struct poulpe_tabmenu *tabmenu = (struct poulpe_tabmenu *) tabitem->base.parent;
    
    if (tabmenu->current_index == tabitem->index)
        memcpy(&fg_color, poulpe_theme_light.tab_active, sizeof(ImVec4));
    else
        memcpy(&fg_color, poulpe_theme_light.tab_inactive, sizeof(ImVec4));
    
    igPushStyleColor_Vec4(ImGuiCol_Button, fg_color);

    igSameLine(0.0f, 0.0f);
    if (igButton(tabitem->text, (ImVec2) {0.0f, 0.0f}))
        tabmenu->current_index = tabitem->index;

    igPopStyleColor(1);
    
    return POULPE_ERROR_NONE;
}

void poulpe_tabitem_destroy(struct poulpe_tabitem *tabitem)
{
    free(tabitem);
}
