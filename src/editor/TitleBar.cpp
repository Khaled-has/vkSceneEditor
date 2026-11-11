#include "TitleBar.h"
#include <imgui.h>
#include <IconsCodicons.h>

void TitleBar::OnUpdate()
{
	if (!IsOpen) return;

	Bar();

	BarButtons();

	// # 3 : The application title
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x / 2.0f) - 50.0f, 5.0f));
	ImGui::Text("vkEngine Hello world from vulkan");

	WindowButtons();


	ImGui::End();
}

void TitleBar::Bar()
{
	// Custum window bar
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 55.0f));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin("###CustumTitleBar", nullptr, flags);
	ImGui::PopStyleColor();

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImVec2 min = ImGui::GetWindowPos();
	ImVec2 max = ImVec2(min.x + 280.0f, min.y + 55.0f);

	// 240, 80, 80
	ImU32 colorLeft = ImColor(79, 97, 125, 255);
	ImU32 colorRight = ImColor(0, 0, 0, 255);

	drawList->AddRectFilledMultiColor(min, max, colorLeft, colorRight, colorRight, colorLeft);
	drawList->AddRectFilledMultiColor(ImVec2(max.x, 0.0f), ImVec2(ImGui::GetWindowSize().x + min.x, ImGui::GetWindowSize().y + min.y), colorRight, colorRight, colorRight, colorRight);

	// # 1 : App icon
	//ImGui::SetCursorPos(ImVec2(10.0f, 6.0f));
	//ImGui::Image(, ImVec2(52.f, 40.f));

}

void TitleBar::BarButtons()
{
	// > File
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(90.0f, 0.0f));
	if (ImGui::Button("File"))
	{

	}

	// > Edite
	ImGui::SameLine();
	ImGui::SetCursorPosY(0.0f);
	if (ImGui::Button("Edite"))
	{

	}

	// > View
	ImGui::SameLine();
	ImGui::SetCursorPosY(0.0f);
	if (ImGui::Button("View"))
	{

	}

	// > Tools
	ImGui::SameLine();
	ImGui::SetCursorPosY(0.0f);
	if (ImGui::Button("Tools"))
	{

	}
}

void TitleBar::WindowButtons()
{
	// # Close button
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 38.0f, 5.0f));
	if (ImGui::Button(ICON_CI_CLOSE, ImVec2(31.0f, 31.0f)))
	{
		m_pAppContext->mCloseApp = true;
	}
	// # Maxmize button
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 78.0f, 5.0f));
	if (ImGui::Button(ICON_CI_CHROME_MAXIMIZE, ImVec2(31.0f, 31.0f)))
	{
		m_pAppContext->mMaxmizeApp = m_pAppContext->mMaxmizeApp ? false : true;
	}
	// # Minmize button
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 118.0f, 5.0f));
	if (ImGui::Button(ICON_CI_CHROME_MINIMIZE, ImVec2(31.0f, 31.0f)))
	{
		m_pAppContext->mMinmizeApp = true;
	}
}