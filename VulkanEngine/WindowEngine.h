#pragma once
#ifndef ENGINE_WINDOW
#define ENGINE_WINDOW

#include "BaseEngine.h"

class WindowEngine : BaseEngine
{
public:
	bool framebufferResized = false;
	GLFWwindow* pWindow;
	VkSurfaceKHR* pSurface;

	WindowEngine(Core* core);
	~WindowEngine();

	void createWindow();
	void createSurface();
	bool isOpen();
	void putRequiredInstanceExtensions();
	void destroySurface();
	void destroyWindow();

private:
	VkSurfaceKHR surface;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		WindowEngine* app = reinterpret_cast<WindowEngine*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}
};
#endif // !ENGINE_WINDOW