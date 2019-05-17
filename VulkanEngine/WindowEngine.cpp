#include "WindowEngine.h"
#include "Core.h"

#pragma region Public

WindowEngine::WindowEngine(Core* core)
{
	pCore = core;
}
WindowEngine::~WindowEngine()
{
	pCore = NULL;
	pWindow = NULL;
	pSurface = NULL;
}

void WindowEngine::createWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
}
void WindowEngine::createSurface()
{	
	if (glfwCreateWindowSurface(*pCore->pVulkanInstanceEngine->pInstance, pWindow, NULL, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
	pSurface = &surface;
}
bool WindowEngine::isOpen()
{
	if (glfwWindowShouldClose(pWindow))
		return false;
	glfwPollEvents();
	return true;
}
void WindowEngine::putRequiredInstanceExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	pCore->engineExtentions.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
}
void WindowEngine::destroySurface()
{
	vkDestroySurfaceKHR(*pCore->pVulkanInstanceEngine->pInstance, surface, NULL);
}
void WindowEngine::destroyWindow()
{
	glfwDestroyWindow(pWindow);
	glfwTerminate();
}

#pragma endregion