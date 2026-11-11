#include "Editor.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <IconsCodicons.h>

#include "TitleBar.h"
#include "Hierarchy/Hierarchy.h"
#include "Hierarchy/Inspector.h"

#include <fstream>

static void LoadImGui_StyleFromFile(const char* pFilename)
{
	std::ifstream inFile(pFilename, std::ios::binary);

	size_t FileStyleSize = 0;
	inFile.read(reinterpret_cast<char*>(&FileStyleSize), sizeof(size_t));

	// # Check the size
	if (FileStyleSize != sizeof(ImGuiStyle))
	{
		std::cout << "ImGui: Load style file error in size: sizeof(ImGuiStyle) != size of ImGuiStyle saved data on the file\n";
		exit(1);
	}

	ImGuiStyle& Style = ImGui::GetStyle();

	inFile.read(reinterpret_cast<char*>(&Style), sizeof(ImGuiStyle));
	inFile.close();
}

static void UploadImGui_StyleToFile(const char* pFilename)
{
	std::ofstream outFile(pFilename, std::ios::binary);

	if (!outFile.is_open())
	{
		std::cout << "ImGui: Upload style file error in open\n";
		exit(1);
	}

	ImGuiStyle& Style = ImGui::GetStyle();

	size_t StyleSize = sizeof(ImGuiStyle);

	// # Allocate the ImGuiStyle struct size for check & protection
	outFile.write(reinterpret_cast<const char*>(&StyleSize), sizeof(size_t));

	// # Write the ImGuiStyle struct
	outFile.write(reinterpret_cast<const char*>(&Style), StyleSize);
	outFile.close();
}

Editor::Editor(AppContext* pAppContext, EditorContext* pEditorContext)
{
	m_pAppContext = pAppContext;

	SetupStyle();

	m_EditorLabels.push_back(new TitleBar{ pAppContext });
	m_EditorLabels.push_back(new Hierarchy{ pEditorContext });
	m_EditorLabels.push_back(new Inspector{ pEditorContext });
}

void Editor::Update()
{
	ImGui::ShowStyleEditor();

	if (ImGui::Button(ICON_CI_SAVE))
	{
		UploadImGui_StyleToFile("res/editor_settings/editor_style.txt");
	}

	for (auto* Label : m_EditorLabels)
	{
		Label->OnUpdate();
	}
}

void Editor::SetupStyle()
{
	ImGuiIO& io = ImGui::GetIO();

	// # Add the editor font
	io.Fonts->AddFontFromFileTTF("res/fonts/JetBrainsMonoNL-ExtraBold.ttf", 22.0f);

	// # Add the editor icons
	ImFontConfig Config;
	Config.MergeMode = true;
	Config.PixelSnapH = true;
	Config.GlyphMinAdvanceX = 16.0f;
	Config.GlyphOffset = ImVec2(-3.5f, 8.0f);

	static const ImWchar IconsRanges[] = { ICON_MIN_CI, ICON_MAX_CI, 0 };

	io.Fonts->AddFontFromFileTTF("res/fonts/codicon.ttf", 28.0f, &Config, IconsRanges);

	// # Editor color style setting
	ImGui::StyleColorsDark();
	
	// # Load Style for file
	LoadImGui_StyleFromFile("res/editor_settings/editor_style.txt");
}
