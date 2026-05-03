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
	auto& scene = core_.scene();
	EntityId animatedObjectId = 0;

	if (scene.renderObjects().size() > 1)
	{
		animatedObjectId = scene.renderObjects()[1].id;
	}

	while (core_.window().isOpen())
	{
		if (animatedObjectId != 0)
		{
			SceneObject* object = scene.findObject(animatedObjectId);
			if (object != nullptr)
			{
				Transform updatedTransform = object->transform;
				updatedTransform.rotationRadians += 0.01f;
				scene.updateTransform(animatedObjectId, updatedTransform);
			}
		}

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
