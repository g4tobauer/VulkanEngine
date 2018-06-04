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
}

void WindowEngine::createWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}
bool WindowEngine::isOpen()
{
	if (glfwWindowShouldClose(pWindow))
		return false;
	glfwPollEvents();
	return true;
}
void WindowEngine::putRequiredInstanceExtensions(std::vector<const char*> *engineExtensions)
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	engineExtensions->assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
}
void WindowEngine::destroyWindow()
{
	glfwDestroyWindow(pWindow);
	glfwTerminate();
}

#pragma endregion