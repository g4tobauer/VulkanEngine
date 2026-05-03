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
	core_.graphicPipeline().createGraphicsPipeline();
	core_.swapChain().createFramebuffers();
	core_.commandPool().createCommandPool();
	core_.commandPool().createCommandBuffers();
	core_.semaphores().createSyncObjects();
}

void Renderer::drawFrame()
{
	core_.semaphores().drawFrame();
}

void Renderer::waitIdle()
{
	vkDeviceWaitIdle(*core_.device().pDevice);
}

void Renderer::shutdown()
{
	core_.swapChain().cleanupSwapChain();
	core_.graphicPipeline().destroyGraphicsPipeline();
	core_.swapChain().destroyRenderPass();
	core_.semaphores().destroySyncObjects();
	core_.commandPool().destroyCommandPool();
}
