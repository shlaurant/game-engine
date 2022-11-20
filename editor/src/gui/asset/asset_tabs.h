#pragma once

namespace editor::gui::asset_tab {
    void animation();
    void audio();
    void font();
    void texture();
}

#define PREFIX_AS(type) if(ImGui::TreeNode(#type "s")) { \
ImGui::Indent(); auto type ## s = scene_data::instance()->assets<editor::type>(); \
for(auto &tmp : type ## s) { if(auto p = tmp.lock()){ImGui::Separator(); ImGui::Text("uuid"); ImGui::SameLine();\
ImGui::Text("%s", std::to_string(p->asset_id).c_str());ImGui::SameLine();\
if(ImGui::Button("delete")){ scene_data::instance()->remove_asset<editor::type>(p->asset_id);}\
INPUT_STR(name, type ## name, p->name)

#define SUFFIX_AS }} ImGui::Unindent(); ImGui::TreePop();}