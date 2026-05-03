# Plano de Refatoracao da Engine

## Visao

Transformar a base atual da `VulkanEngine` em uma game engine moderna, incremental e orientada a:

- performance em GPU
- boa experiencia de uso para quem desenvolve jogos e ferramentas
- arquitetura limpa e escalavel
- build reproduzivel
- facilidade de depuracao e profiling

Este plano assume uma evolucao gradual. A ideia nao e parar tudo para reescrever do zero, e sim consolidar a base atual e crescer com seguranca.

## Objetivos do Motor

### Objetivos tecnicos

- usar Vulkan de forma robusta e previsivel
- reduzir overhead de CPU e sincronizacao desnecessaria
- organizar recursos graficos com ownership claro
- suportar multiplos objetos, materiais, cameras e cenas
- preparar terreno para multithreading e streaming de assets

### Objetivos de produto

- abrir projeto e rodar rapido
- build simples no `Visual Studio` e `VS Code`
- erros mais claros
- estrutura amigavel para adicionar novos recursos
- ferramentas internas com boa usabilidade

## Principios Arquiteturais

- RAII sempre que possivel para recursos Vulkan e recursos do motor
- ownership explicito com `std::unique_ptr` e tipos de valor
- evitar ponteiros publicos expostos sem necessidade
- separar `platform`, `render backend`, `renderer`, `scene`, `editor/tools`
- evitar estado global informal
- priorizar dados contiguos e estruturas cache-friendly
- criar APIs pequenas, claras e previsiveis
- medir performance antes de otimizar microdetalhes

## Estado Atual

Hoje o projeto ja:

- cria janela com GLFW
- inicializa Vulkan
- escolhe device
- cria swapchain, render pass, pipeline e command buffers
- desenha um triangulo
- compila via `CMake`

## Progresso Atual

### Refatoracoes ja concluidas

- build migrado para `CMake`
- integracao com `glfw`, `volk` e `Vulkan-Headers`
- triangulo funcionando novamente em `Debug`
- carga de shaders mais robusta
- `Core` migrado para ownership com `std::unique_ptr`
- introducao de `Application`
- introducao de `Renderer`
- introducao de `VulkanContext`
- triangulo migrado de `gl_VertexIndex` para `vertex buffer`
- compilacao automatica de shaders integrada ao build
- `UniformBuffer + Camera` inicial integrados ao renderer
- primeira `Mesh` com `index buffer` e `Transform` integrada ao renderer
- primeira abstracao reutilizavel de `VulkanBuffer` aplicada em geometria e camera
- primeira `Scene` minima integrada ao renderer
- renderer preparado para iterar sobre multiplos meshes da cena
- primeiro `SceneObject` explicito separando transform e mesh renderizado
- `EntityId` e API inicial de criacao/remocao de objetos na cena
- cena com estado `dirty` e resincronizacao de geometria no renderer
- transformacoes atualizadas sem recriar buffers de geometria
- multiplas entidades compartilhando a mesma geometria por `MeshAssetId`
- materiais basicos por `MaterialAssetId`
- base inicial de `TextureAssetId` e UV preparada para texturas futuras
- primeiro caminho real de textura Vulkan com `VkImage`, `VkImageView`, `VkSampler` e `combined image sampler`
- `depth buffer` inicial integrado ao render pass, framebuffer e pipeline
- primeira base 3D simples com vertices em `vec3`, camera em perspectiva e objetos separados em profundidade
- primeira malha 3D de validacao com cubo indexado e multiplos objetos compartilhando a mesma malha

### Fase atual em andamento

Estamos consolidando a `Fase 3 - Cena e Gameplay Foundation`, com foco em:

- fortalecer a separacao entre cena, assets e recursos Vulkan
- permitir materiais e texturas por objeto
- preparar o renderer para recursos visuais mais completos
- manter algo renderizando a cada passo

### Marco de fase

- entrada explicita na `Fase 3 - Cena e Gameplay Foundation`
- motivo: a renderizacao passou a consumir uma `Scene` e a iterar meshes em vez de depender de um unico objeto hardcoded

### Proxima frente iniciada

Ja iniciamos a entrada na `Fase 2 - Base de Renderer Real` com os seguintes objetivos imediatos:

- introduzir uma abstracao de buffer Vulkan
- sair da dependencia exclusiva de `gl_VertexIndex`
- preparar a renderizacao de geometria real com `vertex buffer`
- automatizar a compilacao de shaders no build
- introduzir `UniformBuffer + Camera`

Limitacoes atuais:

- acoplamento alto entre modulos via `Core`
- ponteiros crus e ownership manual
- pouca encapsulacao
- renderer ainda muito preso ao fluxo do triangulo
- ausencia de sistema de cena, camera, mesh, material e assets
- ausencia de infraestrutura de ferramentas

## Roadmap de Refatoracao

## Fase 1 - Fundacao da Arquitetura

### Objetivo

Parar de crescer em cima de uma base fragil.

### Entregas

- [x] substituir `new/delete` por `std::unique_ptr`
- [x] reduzir ponteiros publicos em `Core`
- [x] introduzir classes centrais:
  - `Application`
  - `Window`
  - `VulkanContext`
  - `Renderer`
- [~] mover responsabilidade de inicializacao/destruicao para RAII
- [~] reduzir exposicao de handles Vulkan entre modulos
- definir convencoes de logs, asserts e tratamento de erro
- organizar pasta do projeto por dominios

### Estrutura alvo inicial

```text
Engine/
  Core/
  Platform/
  Render/
    Vulkan/
  Scene/
  Assets/
  Math/
  Tools/
Game/
Editor/
```

### Criterio de pronto

- triangulo ainda renderizando
- fluxo de criacao e destruicao mais seguro
- menos dependencia direta entre modulos

## Fase 2 - Base de Renderer Real

### Objetivo

Sair do renderer de demo e entrar em um renderer reutilizavel.

### Entregas

- classes para `Buffer`, `Image`, `ImageView`, `Sampler`, `ShaderModule`
- `Mesh` com vertex/index buffer
- `UploadContext` para envio de dados a GPU
- `FrameContext` para recursos por frame
- `DescriptorAllocator`
- `PipelineLayout` e `Material` basicos
- `Camera` com view/projection
- `Transform`

### Criterio de pronto

- renderizar mais de um objeto
- renderizar mesh a partir de vertex buffer real
- camera movendo na cena

## Fase 3 - Cena e Gameplay Foundation

### Objetivo

Criar a primeira base de motor para jogos, nao apenas renderer.

### Entregas

- `Entity` e `Scene`
- componentes basicos:
  - `TransformComponent`
  - `MeshRendererComponent`
  - `CameraComponent`
  - `LightComponent`
- sistema simples de update
- formatacao minima de projeto/jogo
- carregamento de cena

### Decisao importante

Nesta fase vamos decidir entre:

- modelo OO simples de cena
- ECS leve

### Recomendacao inicial

Comecar com uma estrutura simples e migrar para ECS quando a complexidade justificar.

## Fase 4 - Assets e Pipeline de Conteudo

### Objetivo

Parar de depender de arquivos soltos e criar um fluxo de conteudo real.

### Entregas

- `AssetManager`
- resolucao de paths centralizada
- cache de shaders
- carregamento de texturas
- carregamento de modelos
- serializacao inicial de cenas
- hot reload opcional para shaders

### Criterio de pronto

- abrir uma cena com mesh e textura sem hardcode espalhado

## Fase 5 - Renderer Moderno

### Objetivo

Levar a engine para um nivel moderno e eficiente em GPU.

### Entregas

- depth buffer
- frustum culling
- instancing
- uniform/storage buffers melhor organizados
- materiais mais completos
- luz direcional e pontual
- skybox
- pass de sombra inicial
- sistema de render passes ou frame graph simplificado

### Foco de performance

- minimizar recriacao de pipelines
- reduzir alocacoes por frame
- usar persistencia de recursos quando possivel
- preparar desenho em lotes

## Fase 6 - Ferramentas e Usabilidade

### Objetivo

Melhorar a produtividade de quem usa a engine.

### Entregas

- console/log interno
- janela de estatisticas
- integracao com UI de debug
- visualizacao de recursos
- configuracao de projeto
- perfis de build
- documentacao de onboarding

### Ferramentas candidatas

- ImGui para debug UI
- Tracy para profiling
- RenderDoc para captura grafica

## Fase 7 - Base de Game Engine Inicial

### Objetivo

Fechar o primeiro pacote "engine usavel".

### Recursos basicos esperados

- janela e input
- cena
- entidades
- camera
- luz
- meshes
- materiais
- texturas
- renderer 3D basico
- debug UI
- asset manager
- serializacao de cenas

## Fase 8 - Crescimento Futuro

Depois da base inicial pronta, podemos crescer em:

- animacao
- audio
- scripting
- editor visual
- fisica
- navmesh
- particulas
- compute shaders
- streaming de mundo
- renderizacao avancada
- multiplataforma

## Backlog Tecnico Prioritario

### Prioridade alta

- refatorar `Core` para ownership seguro
- encapsular `VkInstance`, `VkDevice`, `VkSwapchainKHR`, `VkPipeline`
- criar tipos de recurso Vulkan com RAII
- adicionar vertex/index buffer real
- adicionar depth buffer
- estruturar `Renderer`
- estruturar `Scene`

### Prioridade media

- sistema de materiais
- carregamento de textura
- camera orbit/free fly
- culling
- profiling
- logging melhor

### Prioridade baixa

- editor
- hot reload sofisticado
- serializacao mais completa
- multiplataforma

## Proposta de Estrutura Arquitetural

### Camadas

- `Application`
  - ciclo de vida do programa
- `Platform`
  - janela, input, tempo, arquivos
- `Render::Vulkan`
  - contexto Vulkan e wrappers
- `Renderer`
  - logica de renderizacao da engine
- `Scene`
  - entidades, componentes, cameras, luzes
- `Assets`
  - shaders, texturas, modelos, materiais, cenas
- `Tools`
  - debug UI, editor, profiling

### Relacoes recomendadas

- `Application` possui `Window`, `Renderer`, `SceneManager`
- `Renderer` usa `VulkanContext`
- `Scene` nao conhece Vulkan diretamente
- `Material` nao fala com `Window`
- `AssetManager` nao depende do loop principal

## Padrões de Qualidade

- warnings tratados cedo
- sem alocacao desnecessaria por frame
- sem paths hardcoded
- logs claros para falhas de inicializacao
- asserts em invariantes importantes
- docs curtas para cada modulo relevante
- testes unitarios quando fizer sentido em codigo nao grafico

## Performance e GPU

### Diretrizes

- evitar sincronizacao global sem necessidade
- usar buffers por frame
- evitar recriar recursos pesados
- consolidar uploads
- pensar em `bindless` e `descriptor indexing` no futuro
- agrupar draw calls quando fizer sentido
- medir com profiler antes de otimizar

### Ferramentas recomendadas

- RenderDoc
- Tracy
- Nsight Graphics
- Radeon GPU Profiler, se aplicavel

## Usabilidade e DX

DX aqui significa experiencia de desenvolvimento.

Coisas que devemos perseguir desde cedo:

- mensagens de erro claras
- configuracao simples
- estruturas e nomes previsiveis
- exemplos pequenos funcionando
- docs por modulo
- templates basicos de cena/material

## Proximos Passos Recomendados

### Sprint 1

- [x] refatorar `Core` para `Application`
- [x] introduzir `VulkanContext`
- [x] remover ownership manual mais perigoso
- [~] reduzir acoplamento entre modulos Vulkan

### Sprint 2

- [~] criar `Buffer` e `Mesh`
- [x] sair do triangulo hardcoded para vertex buffer real
- [~] introduzir `UniformBuffer + Camera`
- [~] introduzir `index buffer` e `Transform`
- [~] consolidar abstracao de `VulkanBuffer`

### Sprint 3

- [~] criar `Camera`, `Transform`, `Scene`
- [~] mover objeto renderizado para uma `Scene` minima
- [~] iterar multiplos meshes da cena no renderer
- [~] introduzir `SceneObject` e `MeshRendererComponent`
- [~] introduzir `EntityId` e API de gerenciamento da cena
- [~] adicionar `dirty state` e atualizacao de transform por `EntityId`
- [~] separar mudancas de geometria de mudancas de transform
- [~] compartilhar geometria entre entidades por asset de malha
- [~] introduzir material basico por asset
- [~] preparar assets de textura e UVs
- renderizar uma mesh com camera controlavel

### Sprint 4

- [~] texturas, materiais e asset manager inicial
- [~] primeiro sampler de textura real no shader
- [~] primeiro `depth buffer` funcional para preparar a base 3D
- [~] primeira camera em perspectiva e uso real de profundidade
- [~] primeira malha 3D mais representativa para validar a arquitetura

## Como Vamos Trabalhar

Estrategia recomendada:

1. refatorar em pequenos passos
2. manter algo renderizando o tempo todo
3. validar build e runtime a cada etapa
4. introduzir abstrações apenas quando elas realmente melhorarem o design
5. documentar decisoes importantes

## Conclusao

A base atual ja provou que consegue inicializar Vulkan e desenhar o primeiro triangulo. O proximo passo nao e adicionar recursos aleatoriamente, e sim consolidar a arquitetura para que cada novo recurso entre de forma previsivel, segura e performatica.

Este documento e o plano inicial de longo prazo da engine. A ideia e evoluir em camadas, sem perder o foco em performance, clareza e usabilidade.
