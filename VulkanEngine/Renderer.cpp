#include "Renderer.h"

Renderer::Renderer(Core& core)
	: core_(core)
{
}

void Renderer::initialize()
{
	core_.swapChain().createSwapChain();
	core_.swapChain().createImageViews();
	core_.swapChain().createRenderPass();
	core_.scene().createDefaultScene();
	core_.commandPool().createCommandPool();
	core_.assets().initializeGpuResources();
	core_.camera().initialize();
	core_.graphicPipeline().createGraphicsPipeline();
	core_.swapChain().createFramebuffers();
	core_.commandPool().createCommandBuffers();
	core_.semaphores().createSyncObjects();
	syncSceneGeometry();
}

void Renderer::drawFrame()
{
	syncSceneGeometry();
	core_.semaphores().drawFrame();
	core_.scene().clearTransformDirty();
}

void Renderer::waitIdle()
{
	vkDeviceWaitIdle(core_.device().deviceHandle());
}

void Renderer::shutdown()
{
	core_.swapChain().cleanupSwapChain();
	core_.graphicPipeline().destroyGraphicsPipeline();
	core_.camera().shutdown();
	core_.assets().shutdownGpuResources();
	core_.swapChain().destroyRenderPass();
	core_.semaphores().destroySyncObjects();
	core_.commandPool().destroyCommandPool();
	core_.geometry().destroyGeometry();
	core_.scene().clear();
	core_.assets().clearMaterialAssets();
	core_.assets().clearTextureAssets();
	core_.assets().clearMeshAssets();
}

void Renderer::syncSceneGeometry()
{
	if (!core_.scene().isGeometryDirty())
	{
		return;
	}

	vkDeviceWaitIdle(core_.device().deviceHandle());
	core_.geometry().createSceneGeometry(core_.scene());
	core_.scene().clearGeometryDirty();
}
