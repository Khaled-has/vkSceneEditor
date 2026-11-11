#ifndef IMGUIASSET_H
#define IMGUIASSET_H

#include <imgui.h>
#include <IconsCodicons.h>

class ImGuiAsset
{
public:
	ImGuiAsset() {}

	void Setup();
	void Destroy();

public:
	static void AddFontFromFileTTF(
		const char* pFilename, float mSizePixel = 0.0f,
		const ImFontConfig* pImFontConfig = (const ImFontConfig*)0,
		const ImWchar* mGlyphRanges = (const ImWchar*)0
	);

private:
	static ImGuiAsset* m_pInstance;
};

#endif