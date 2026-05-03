#pragma once
#ifndef ENGINE_RENDERER
#define ENGINE_RENDERER

#include "Core.h"

class Renderer
{
public:
	explicit Renderer(Core& core);

	void initialize();
	void drawFrame();
	void waitIdle();
	void shutdown();

private:
	Core& core_;
};

#endif // !ENGINE_RENDERER
