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
	if (glfwInit() != GLFW_TRUE)
	{
		throw std::runtime_error("failed to initialize GLFW!");
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
	if (pWindow == NULL)
	{
		throw std::runtime_error("failed to create GLFW window!");
	}
	glfwSetWindowUserPointer(pWindow, this);
	glfwSetFramebufferSizeCallback(pWindow, framebufferResizeCallback);
}
void WindowEngine::createSurface()
{	
	if (glfwCreateWindowSurface(pCore->instance().instanceHandle(), pWindow, NULL, &surface) != VK_SUCCESS) {
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
	pCore->instanceExtensions().assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
}
void WindowEngine::destroySurface()
{
	vkDestroySurfaceKHR(pCore->instance().instanceHandle(), surface, NULL);
}
void WindowEngine::destroyWindow()
{
	glfwDestroyWindow(pWindow);
	glfwTerminate();
}

GLFWwindow* WindowEngine::windowHandle() const
{
	return pWindow;
}

VkSurfaceKHR WindowEngine::surfaceHandle() const
{
	return surface;
}

bool WindowEngine::isFramebufferResized() const
{
	return framebufferResized;
}

void WindowEngine::clearFramebufferResized()
{
	framebufferResized = false;
}

bool WindowEngine::isKeyPressed(int key) const
{
    return pWindow != nullptr && glfwGetKey(pWindow, key) == GLFW_PRESS;
}

bool WindowEngine::isMouseButtonPressed(int button) const
{
    return pWindow != nullptr && glfwGetMouseButton(pWindow, button) == GLFW_PRESS;
}

void WindowEngine::cursorPosition(double& x, double& y) const
{
    if (pWindow == nullptr)
    {
        x = 0.0;
        y = 0.0;
        return;
    }

    glfwGetCursorPos(pWindow, &x, &y);
}

#pragma endregion
