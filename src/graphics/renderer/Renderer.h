#ifndef RENDERER_H
#define RENDERER_H

#include "ImGui/ImGuiImage.h"
#include "Camera.h"

namespace gfx
{
	typedef enum PipelineType
	{
		GFX_PIPELINE_TYPE_RASTERZATION = 0,
		GFX_PIPELINE_TYPE_PATH_TRACHING = 1
	} PipelineType;

	struct RenderResolution
	{
		uint32_t mWidth;
		uint32_t mHeight;
	};

	class RenderTarget
	{
	public:
		RenderTarget(Camera* pCamera, PipelineType mPipeType) 
			: m_pCamera(pCamera), m_PipeType(mPipeType) 
		{}

		virtual void SetupPipeline(RenderResolution mRenderRes) = 0;
		virtual void ResetRenderResolution(RenderResolution) = 0;

		inline const ImTextureID& GetRenderImage() const { return m_RenderImage; }

	protected:
		Camera* m_pCamera;
		PipelineType m_PipeType;

		ImTextureID m_RenderImage;
	};

	class Renderer
	{
	public:
		Renderer() {}

		virtual void Setup() = 0;
		virtual void Destroy() = 0;

		virtual void Render() = 0;

		virtual void SubmitRenderTarget(RenderTarget* pRenderTarget) = 0;
	};

	Renderer* CreateRenderer(std::function<void(void* pInstance, void* pSurface)> func_CreateVkSurface);

	RenderTarget* CreateRenderTarget(Camera* pCamera, PipelineType mPipeType);
}

#endif