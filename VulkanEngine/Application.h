#pragma once
#ifndef ENGINE_APPLICATION
#define ENGINE_APPLICATION

#include "Core.h"
#include "Renderer.h"
#include "VulkanContext.h"

class Application
{
public:
	Application();
	void run();

private:
	Core core_;
	VulkanContext vulkanContext_;
	Renderer renderer_;

	void initWindow();
	void mainLoop();
	void cleanup();
};

#endif // !ENGINE_APPLICATION
