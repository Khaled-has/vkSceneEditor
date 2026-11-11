#include "Renderer.h"

#include "vkRenderer.h"
#include "vkRenderTarget.h"

namespace gfx
{
	Renderer* CreateRenderer(std::function<void (void* pInstance, void* pSurface)> func_CreateVkSurface)
	{
		return new vkRenderer(func_CreateVkSurface);
	}

	RenderTarget* CreateRenderTarget(Camera* pCamera, PipelineType mPipeType)
	{
		return new vkRenderTarget(pCamera, mPipeType);
	}
}