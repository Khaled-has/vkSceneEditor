#ifndef IMGUIIMAGE_H
#define IMGUIIMAGE_H

#include <imgui.h>
#include <iostream>

struct ImGuiImageInfo
{
	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mChannels;
};

class ImGuiImage
{
	friend void ImGui_CreateImage(const char* pFilename, ImGuiImage* pImage);
public:
	ImGuiImage() {}

	inline const ImGuiImageInfo& GetInfo()	const { return m_ImageInfo; }

	inline operator ImTextureID&() { return m_TexID; }
private:
	void* m_pImage;

	ImTextureID m_TexID;
	ImGuiImageInfo m_ImageInfo;
};

void ImGui_CreateImage(const char* pFilename, ImGuiImage* pImage);

#endif