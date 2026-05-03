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
	auto& window = core_.window();
	auto& camera = core_.camera();
	EntityId animatedObjectId = 0;
	auto previousFrameTime = std::chrono::steady_clock::now();
	bool debugKeyLatch[5] = { false, false, false, false, false };

	if (scene.renderObjects().size() > 1)
	{
		animatedObjectId = scene.renderObjects()[1].id;
	}

	while (core_.window().isOpen())
	{
		const auto currentFrameTime = std::chrono::steady_clock::now();
		const float deltaTimeSeconds = std::chrono::duration<float>(currentFrameTime - previousFrameTime).count();
		previousFrameTime = currentFrameTime;

		camera.updateFromInput(deltaTimeSeconds);

		const int debugKeys[5] = {
			GLFW_KEY_F1,
			GLFW_KEY_F2,
			GLFW_KEY_F3,
			GLFW_KEY_F4,
			GLFW_KEY_F5
		};

		for (int i = 0; i < 5; ++i)
		{
			const bool pressed = window.isKeyPressed(debugKeys[i]);
			if (pressed && !debugKeyLatch[i])
			{
				camera.setDebugRenderMode(static_cast<CameraEngine::DebugRenderMode>(i));
			}

			debugKeyLatch[i] = pressed;
		}

		if (animatedObjectId != 0)
		{
			SceneObject* object = scene.findObject(animatedObjectId);
			if (object != nullptr)
			{
				Transform updatedTransform = object->transform;
				updatedTransform.rotationRadians += 1.0f * deltaTimeSeconds;
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
