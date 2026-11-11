#include "ImGuiAsset.h"
#include "ImGuiRenderer.h"

#include <imgui_impl_vulkan.h>

void ImGuiAsset::AddFontFromFileTTF(const char* pFilename, float mSizePixel, const ImFontConfig* pImFontConfig, const ImWchar* mGlyphRanges)
{
	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF(pFilename, mSizePixel, pImFontConfig, mGlyphRanges);
}
