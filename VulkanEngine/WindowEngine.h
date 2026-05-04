#pragma once
#ifndef ENGINE_WINDOW
#define ENGINE_WINDOW

#include "BaseEngine.h"

class WindowEngine : BaseEngine
{
public:
	WindowEngine(Core* core);
	~WindowEngine();

	void createWindow();
	void createSurface();
	bool isOpen();
	void putRequiredInstanceExtensions();
	void destroySurface();
	void destroyWindow();
	GLFWwindow* windowHandle() const;
	VkSurfaceKHR surfaceHandle() const;
	bool isFramebufferResized() const;
	void clearFramebufferResized();
	bool isKeyPressed(int key) const;
	bool isMouseButtonPressed(int button) const;
	void cursorPosition(double& x, double& y) const;

private:
	bool framebufferResized = false;
	GLFWwindow* pWindow = nullptr;
	VkSurfaceKHR* pSurface = nullptr;
	VkSurfaceKHR surface;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		(void)width;
		(void)height;
		WindowEngine* app = reinterpret_cast<WindowEngine*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}
};
#endif // !ENGINE_WINDOW
