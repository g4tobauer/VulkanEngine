#pragma once
#ifndef ENGINE_WINDOW
#define ENGINE_WINDOW

#include "BaseEngine.h"

class WindowEngine : BaseEngine
{
public:
	GLFWwindow* pWindow;

	WindowEngine(Core* core);
	~WindowEngine();

	void createWindow();
	bool isOpen();
	void putRequiredInstanceExtensions(std::vector<const char*> *engineExtensions);
	void destroyWindow();
};
#endif // !ENGINE_WINDOW