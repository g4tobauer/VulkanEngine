#pragma once
#ifndef ENGINE_APPLICATION
#define ENGINE_APPLICATION

#include "Core.h"
#include "Renderer.h"

class Application
{
public:
	Application();
	void run();

private:
	Core core_;
	Renderer renderer_;

	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();
};

#endif // !ENGINE_APPLICATION
