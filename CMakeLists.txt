cmake_minimum_required(VERSION 3.24)

project(VulkanEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(VOLK_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)

FetchContent_Declare(
  volk
  GIT_REPOSITORY https://github.com/zeux/volk.git
  GIT_TAG 1.4.304
)

FetchContent_Declare(
  vulkan_headers
  GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
  GIT_TAG vulkan-sdk-1.4.304.1
)

FetchContent_MakeAvailable(vulkan_headers)
set(VULKAN_HEADERS_INSTALL_DIR "${vulkan_headers_SOURCE_DIR}" CACHE PATH "" FORCE)

FetchContent_MakeAvailable(glfw volk)

add_executable(VulkanEngineApp
  VulkanEngine/Core.cpp
  VulkanEngine/FileStreamEngine.cpp
  VulkanEngine/main.cpp
  VulkanEngine/VulkanCommandPoolEngine.cpp
  VulkanEngine/VulkanDebugCallbackEngine.cpp
  VulkanEngine/VulkanDeviceEngine.cpp
  VulkanEngine/VulkanGraphicPipelineEngine.cpp
  VulkanEngine/VulkanInstanceEngine.cpp
  VulkanEngine/VulkanSemaphoresEngine.cpp
  VulkanEngine/VulkanSwapChainEngine.cpp
  VulkanEngine/WindowEngine.cpp
)

target_include_directories(VulkanEngineApp PRIVATE VulkanEngine)
target_link_libraries(VulkanEngineApp PRIVATE glfw volk::volk)

if(MSVC)
  target_compile_options(VulkanEngineApp PRIVATE /W4 /permissive-)
else()
  target_compile_options(VulkanEngineApp PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(
  TARGET VulkanEngineApp POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/VulkanEngine/shaders
          $<TARGET_FILE_DIR:VulkanEngineApp>/shaders
)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT VulkanEngineApp)
