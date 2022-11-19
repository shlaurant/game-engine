#pragma once

#include "entity_window.h"

namespace editor::gui::tab {
    const static float Align = 150.f;
    const static float ItemWidth = 80.f;

    bool information(entity_data &);
    bool transform(entity_data &);
    bool rigidbody(entity_data &);
    bool collider(entity_data &);
    bool sprite(entity_data &);
    bool animation(entity_data &);
    bool text(entity_data &);
    bool script(entity_data &);

    template<typename T>
    void delete_context_menu(entity_data &entity) {
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::Button("delete")) {
                entity.delete_comp<T>();
            }
            ImGui::EndPopup();
        }
    }
}


#define PREFIX(T, name) if(!entity.has_comp<T>()) return false; \
bool is_changed = false; if(ImGui::TreeNode(name)) {             \
delete_context_menu<T>(entity); ImGui::PushItemWidth(ItemWidth); ImGui::Indent();

#define SUFFIX(T) ImGui::Unindent(); ImGui::PopItemWidth(); ImGui::TreePop(); \
} else {delete_context_menu<T>(entity);} return is_changed;