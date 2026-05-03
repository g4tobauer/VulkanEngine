#include "Application.h"

Application::Application()
	: core_(),
	  vulkanContext_(core_),
	  renderer_(core_)
{
}

void Application::run()
{
	initWindow();
	vulkanContext_.initialize();
	mainLoop();
	cleanup();
}

void Application::initWindow()
{
	core_.window().createWindow();
}

void Application::mainLoop()
{
	renderer_.initialize();

	while (core_.window().isOpen())
	{
		renderer_.drawFrame();
	}

	renderer_.waitIdle();
}

void Application::cleanup()
{
	renderer_.shutdown();
	vulkanContext_.shutdown();
	core_.window().destroyWindow();
}
