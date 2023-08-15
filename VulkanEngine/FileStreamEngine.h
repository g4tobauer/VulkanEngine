#pragma once
#ifndef ENGINE_VULKAN_FILE_STREAM
#define ENGINE_VULKAN_FILE_STREAM

#include "BaseEngine.h"

class FileStreamEngine : BaseEngine
{
public:
	FileStreamEngine(Core* core);
	~FileStreamEngine();

	std::vector<char> readFile(const std::string& filename);
private:
};
#endif // !ENGINE_VULKAN_FILE_STREAM