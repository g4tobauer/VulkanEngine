#include "FileStreamEngine.h"
#include "Core.h"
#include <filesystem>
#include <array>

#ifdef _WIN32
#include <windows.h>
#endif

#pragma region Public

FileStreamEngine::FileStreamEngine(Core* core)
{    
	pCore = core;
}

FileStreamEngine::~FileStreamEngine()
{
    pCore = NULL;
}

std::vector<char> FileStreamEngine::readFile(const std::string& filename) {
    namespace fs = std::filesystem;

    std::vector<fs::path> candidates;
    candidates.push_back(fs::path(filename));
    candidates.push_back(fs::current_path() / filename);
    candidates.push_back(fs::current_path() / "VulkanEngine" / filename);

#ifdef _WIN32
    char modulePath[MAX_PATH] = {};
    DWORD pathLength = GetModuleFileNameA(nullptr, modulePath, MAX_PATH);
    if (pathLength > 0 && pathLength < MAX_PATH) {
        fs::path executableDir = fs::path(modulePath).parent_path();
        candidates.push_back(executableDir / filename);
        candidates.push_back(executableDir / ".." / ".." / ".." / "VulkanEngine" / filename);
    }
#endif

    for (const fs::path& candidate : candidates) {
        std::ifstream file(candidate, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            continue;
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
        file.close();

        return buffer;
    }

    throw std::runtime_error("failed to open shader file: " + filename);
}


#pragma endregion
