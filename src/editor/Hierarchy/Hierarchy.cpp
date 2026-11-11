#include "Hierarchy.h"
#include <imgui.h>

void Hierarchy::OnUpdate()
{
	ImGui::Begin("Hierarchy");

	for (auto& EnTT : m_pEditorContext->pScene->GetAllEntities())
	{
		Entity entity{ EnTT, m_pEditorContext->pScene };
		DrawEntityHierarchy(entity);
	}

	ImGui::End();
}

void Hierarchy::DrawEntityHierarchy(Entity& mEntity)
{
	auto& tag = mEntity.GetComponent<TagComponent>();

	ImGuiTreeNodeFlags flags =
		((m_pEditorContext->mSelectedEntity == (uint32_t)mEntity) ? ImGuiTreeNodeFlags_Selected : 0)
		| ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	void* ptrID = (void*)(uint64_t)(uint32_t)mEntity;

	bool opened = ImGui::TreeNodeEx(ptrID, flags, "%s", tag.mTag.c_str());

	if (ImGui::IsItemClicked())
	{
		m_pEditorContext->mSelectedEntity = (uint32_t)mEntity;
	}
	

	if (opened)
		ImGui::TreePop();
}
