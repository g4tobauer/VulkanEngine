#pragma once
#ifndef ENGINE_WINDOW
#define ENGINE_WINDOW

#include "BaseEngine.h"

class WindowEngine : BaseEngine
{
public:
	GLFWwindow* pWindow;
	VkSurfaceKHR* pSurface;

	WindowEngine(Core* core);
	~WindowEngine();

	void createWindow();
	void createSurface();
	bool isOpen();
	void putRequiredInstanceExtensions(std::vector<const char*> *engineExtensions);
	void destroySurface();
	void destroyWindow();

private:
	VkSurfaceKHR surface;
};
#endif // !ENGINE_WINDOW