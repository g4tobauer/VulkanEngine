# Vulkan Engine

Projeto em C++ com Vulkan que desenha um triângulo usando uma arquitetura modular simples:

- `WindowEngine`: cria a janela GLFW e a `VkSurfaceKHR`
- `VulkanInstanceEngine`: cria a instância Vulkan
- `VulkanDebugCallbackEngine`: ativa validação e mensagens de debug
- `VulkanDeviceEngine`: escolhe a GPU e cria o device lógico
- `VulkanSwapChainEngine`: gerencia swapchain, image views, render pass e framebuffers
- `VulkanGraphicPipelineEngine`: cria o pipeline gráfico a partir dos shaders SPIR-V
- `VulkanCommandPoolEngine`: aloca e grava command buffers
- `VulkanSemaphoresEngine`: faz sincronização e apresenta os frames

## Build moderno com CMake

O projeto foi preparado para compilar com `CMake` usando:

- `glfw` via `FetchContent`
- `volk` via `FetchContent`

Isso remove os caminhos absolutos antigos do Visual Studio e evita dependência de `vulkan-1.lib` no build.

## Requisitos

- Windows 10/11
- Visual Studio 2022 com workload `Desenvolvimento para desktop com C++`
  ou Build Tools equivalentes
- CMake 3.24+
- Git
- Driver de vídeo com suporte a Vulkan
- Internet na primeira configuração do CMake, para baixar `glfw` e `volk`

## Como compilar

### Opção 1: usando presets

```powershell
cmake --preset vs2022-x64
cmake --build --preset build-debug
```

O executável ficará em:

```text
build\vs2022-x64\Debug\VulkanEngineApp.exe
```

### Opção 2: linha de comando simples

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

## Execução

Ao compilar, a pasta `shaders` é copiada automaticamente para a pasta do executável.

Se tudo estiver certo, o programa deve abrir uma janela e desenhar um triângulo.

## Observações

- Os arquivos `.sln` e `.vcxproj` antigos continuam no repositório, mas o caminho recomendado agora é o `CMake`.
- Os shaders pré-compilados `.spv` já fazem parte do projeto, então não é necessário compilar GLSL para testar o triângulo.
