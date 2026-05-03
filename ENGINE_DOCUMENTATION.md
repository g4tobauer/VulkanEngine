# Documentacao da VulkanEngine

## Visao geral

Esta engine comecou como um `Hello Triangle` em Vulkan e esta sendo refatorada, passo a passo, para virar a base de uma game engine moderna.

Hoje ela ja tem:

- inicializacao Vulkan modular
- renderer separado da aplicacao
- cena com objetos
- geometria enviada para GPU
- compartilhamento de malha por asset
- materiais basicos por asset
- texturas placeholder reais no Vulkan por asset
- `depth buffer` inicial no renderer
- primeira base 3D simples com perspectiva
- primeira malha 3D de validacao com cubo
- pipeline grafico configurado por shaders SPIR-V

O objetivo desta documentacao e ajudar quem for estudar o projeto a entender:

- como a engine funciona
- como ela foi estruturada
- quem chama quem
- por que essa estrutura foi escolhida
- o significado dos termos tecnicos principais

## Fluxo principal da engine

O ponto de entrada e [main.cpp](C:/Users/Joao/source/repos/g4tobauer/VulkanEngine/VulkanEngine/main.cpp).

Na pratica, o fluxo e este:

1. `main.cpp` cria `Application`
2. `Application::run()` cria a janela
3. `VulkanContext::initialize()` sobe a base Vulkan
4. `Renderer::initialize()` cria swapchain, render pass, pipeline, cena e command buffers
5. `Application::mainLoop()` fica em loop enquanto a janela estiver aberta
6. a cada iteracao, `Renderer::drawFrame()` desenha um frame
7. no fechamento, `Renderer::shutdown()` e `VulkanContext::shutdown()` limpam os recursos

## Quem chama quem

Fluxo alto nivel:

`main.cpp`
-> `Application`
-> `VulkanContext`
-> `Renderer`
-> modulos Vulkan e modulos de cena

Chamadas principais:

- `Application` controla o ciclo de vida do programa
- `VulkanContext` controla a inicializacao base do Vulkan
- `Renderer` controla a parte de renderizacao por frame
- `SceneEngine` controla os objetos da cena
- `AssetManager` controla assets de malha, material e textura
- `GeometryEngine` sobe malhas para buffers Vulkan
- `CameraEngine` atualiza os dados de camera por frame
- `VulkanCommandPoolEngine` grava os comandos de draw
- `VulkanSemaphoresEngine` sincroniza acquire, submit e present

## Estrutura atual

### `Application`

Responsabilidade:

- ciclo de vida da aplicacao
- inicializacao da janela
- loop principal
- cleanup final

Por que existe:

Antes, muita coisa ficava espalhada ou acoplada ao fluxo geral. `Application` deixa claro onde o programa comeca, roda e termina.

### `Core`

Responsabilidade:

- ownership dos subsistemas
- ponto central de acesso entre modulos

Por que existe:

Hoje o `Core` funciona como um container principal da engine. Ele ainda e um ponto de acoplamento forte, mas ja ficou mais seguro com `std::unique_ptr`.

### `VulkanContext`

Responsabilidade:

- `volkInitialize`
- criacao da `VkInstance`
- criacao da `VkSurfaceKHR`
- escolha da GPU
- criacao do `VkDevice`

Por que existe:

Separa a base Vulkan do restante do renderer. Isso evita que `Application` precise conhecer detalhes de inicializacao do backend grafico.

### `Renderer`

Responsabilidade:

- inicializar recursos de render
- sincronizar geometria da cena com a GPU
- desenhar frames
- destruir os recursos de render

Por que existe:

Ele concentra a logica de renderizacao da engine. Assim, a cena e a aplicacao nao precisam conhecer a ordem detalhada de criacao de swapchain, pipeline, framebuffers e draw.

### `SceneEngine`

Responsabilidade:

- gerenciar objetos da cena
- criar/remover objetos
- atualizar transformacoes
- informar quando a cena mudou

Por que existe:

Sem uma cena, o renderer ficaria preso a objetos hardcoded. `SceneEngine` e a primeira camada de runtime do motor.

### `AssetManager`

Responsabilidade:

- registrar assets de malha
- registrar assets de material
- registrar assets de textura
- manter as texturas reais do Vulkan
- montar `descriptor sets` de material para sampling
- permitir compartilhamento de geometria entre objetos
- separar recurso visual de instancia da cena

Por que existe:

Dois objetos podem usar a mesma malha. Sem isso, a engine duplicaria vertex/index buffer desnecessariamente.
Agora ele tambem evita duplicacao de textura e centraliza o binding visual dos materiais.

### `GeometryEngine`

Responsabilidade:

- transformar `Mesh` em buffers Vulkan
- manter vertex/index buffers na GPU

Por que existe:

Separa “dados de malha no mundo CPU” de “buffers de geometria no backend Vulkan”.

### `CameraEngine`

Responsabilidade:

- manter uniform buffers por frame
- enviar view/projection para o shader

Por que existe:

A camera muda com frequencia e precisa de um caminho proprio de atualizacao por frame.

### `VulkanGraphicPipelineEngine`

Responsabilidade:

- carregar shaders SPIR-V
- criar `VkPipelineLayout`
- criar `VkPipeline`

Por que existe:

O pipeline e um dos objetos mais importantes do Vulkan. Ter um modulo proprio facilita estudar e evoluir esse trecho.

### `VulkanCommandPoolEngine`

Responsabilidade:

- criar command pool
- alocar command buffers
- gravar comandos de renderizacao

Por que existe:

Em Vulkan, desenhar nao e “chamar draw direto”; primeiro voce grava comandos, depois submete para a GPU.

### `VulkanSemaphoresEngine`

Responsabilidade:

- sincronizacao por frame
- acquire da imagem da swapchain
- submit para a fila grafica
- present da imagem

Por que existe:

Sem sincronizacao correta, a GPU pode desenhar na hora errada ou apresentar imagens invalidas.

## Conceitos tecnicos importantes

### O que e `Mesh`

`Mesh` e a descricao geometrica de um objeto.

No projeto, uma `Mesh` contem:

- vertices
- indices

Hoje a engine ja consegue representar tanto uma malha muito simples, como triangulo, quanto uma malha 3D mais realista de teste, como um cubo indexado.

Exemplo mental:

- vertices = pontos com atributos
- indices = ordem em que os vertices formam triangulos

No cubo atual, usamos:

- 24 vertices
- 36 indices

Isso acontece porque cada face precisa de combinacoes proprias de cor e UV, entao nem sempre um cubo pode ser representado por apenas 8 vertices compartilhados.

### O que e `Vertex`

`Vertex` e um unico vertice da malha.

No projeto atual, ele tem:

- posicao 3D
- cor RGB
- UV

No futuro, poderia ter tambem:

- normal
- UV
- tangente
- pesos de animacao

### O que e `Index Buffer`

Um `index buffer` diz quais vertices devem ser usados para formar triangulos.

Vantagem:

- evita repetir vertices iguais
- reduz memoria
- melhora organizacao da malha

### O que e `Buffer`

Em Vulkan, `buffer` e um bloco linear de memoria usado para dados.

Exemplos:

- `vertex buffer`
- `index buffer`
- `uniform buffer`

Na engine, a classe `VulkanBuffer` encapsula:

- criacao
- alocacao de memoria
- map/unmap
- escrita de dados
- destruicao

### O que e `Uniform Buffer`

`Uniform Buffer` e um buffer pequeno usado para dados lidos pelo shader.

No projeto atual, ele carrega:

- view
- projection
- viewProjection

Esses dados costumam mudar por frame ou por camera.

### O que e `Push Constant`

`Push Constant` e uma forma leve e rapida de mandar poucos dados para o shader.

Na engine atual, ele envia:

- `model matrix` de cada objeto

Por que usar aqui:

- e pequeno
- muda por draw
- evita recriar uniform buffer para cada objeto

### O que e `Pipeline`

No Vulkan, `pipeline` descreve como a GPU deve processar os dados.

Ele inclui, entre outras coisas:

- shaders
- formato do vertice
- montagem de primitivas
- rasterizacao
- blending
- layout de recursos

Sem pipeline, a GPU nao sabe como interpretar os vertices e gerar pixels.

### O que e `Render Pass`

`Render Pass` descreve a estrutura basica de uma etapa de renderizacao.

Exemplo:

- quais attachments existem
- como a imagem sera limpa
- como ela sera usada antes/depois do draw

Hoje o `render pass` da engine ja tem:

- um attachment de cor para a imagem da swapchain
- um attachment de profundidade para testes de depth

### O que e `Swapchain`

`Swapchain` e a fila de imagens que vao para a tela.

Fluxo mental:

1. pegar uma imagem disponivel
2. desenhar nela
3. apresentar na janela

### O que e `Depth Buffer`

`Depth Buffer` e uma imagem usada para guardar a profundidade de cada pixel desenhado.

Ele serve para a GPU decidir qual fragmento esta na frente e qual esta atras.

Sem isso, em cenas 3D, a ordem de desenho pode ficar errada. Um objeto mais distante pode aparecer por cima de um mais proximo.

Na engine atual, o `depth buffer` ja existe como:

- `VkImage`
- `VkDeviceMemory`
- `VkImageView`

Ele e criado junto com a swapchain e conectado:

- ao `render pass`
- aos `framebuffers`
- ao estado de `depth test` do pipeline

Depois da introducao da camera em perspectiva, ele comeca a ter efeito real no conteudo da cena, porque os objetos agora podem ficar em profundidades diferentes no eixo `Z`.

### O que e `Command Buffer`

`Command Buffer` e uma lista de comandos gravados para a GPU executar.

Exemplos:

- comecar render pass
- bindar pipeline
- bindar vertex/index buffer
- bindar descriptor set
- fazer `draw indexed`

### O que e `Descriptor Set`

`Descriptor Set` e o conjunto de recursos que o shader pode acessar.

No projeto atual, existem dois usos principais:

- uniform buffer da camera
- textura base do material

### O que e `SceneObject`

`SceneObject` representa uma instancia no mundo.

Hoje ele tem:

- `EntityId`
- nome
- `Transform`
- `MeshRendererComponent`

Ele nao guarda a malha inteira; ele aponta para um `MeshAssetId`.

### O que e `Transform`

`Transform` representa a pose de um objeto.

No projeto atual:

- posicao
- rotacao
- escala

Disso sai a `model matrix`.

### O que e `Asset`

`Asset` e um recurso reutilizavel da engine.

Exemplos futuros:

- malha
- textura
- material
- shader
- cena serializada

Hoje o `AssetManager` cuida de:

- malhas
- materiais basicos
- metadados iniciais de textura

No futuro, ele deve crescer para:

- texturas
- shaders
- materiais mais completos

### O que e `UV`

`UV` e a coordenada usada para mapear textura em uma malha.

Normalmente:

- `U` representa o eixo horizontal da textura
- `V` representa o eixo vertical da textura

Mesmo sem sampling de textura ativo ainda, adicionar UV agora e importante porque:

- prepara a malha para materiais texturizados
- evita retrabalho estrutural depois
- deixa o layout de vertice mais proximo de um renderer real

### O que e `TextureAsset`

`TextureAsset` representa um recurso de textura no nivel da engine.

Nesta etapa, ele ja chegou a um primeiro estagio real de Vulkan. Hoje ele envolve:

- `TextureAssetId`
- nome
- caminho de origem
- `VkImage`
- `VkImageView`
- `VkSampler`

Por enquanto, a engine gera texturas placeholder em memoria, sem depender de loader de PNG. Isso foi escolhido para:

- manter a arquitetura andando sem adicionar uma dependencia de imagem cedo demais
- validar o fluxo completo de textura no Vulkan
- preparar o terreno para um futuro carregador real de arquivos

Hoje o sampling ja acontece no fragment shader.

## Como o render acontece hoje

Resumo por frame:

1. `Application` atualiza um `Transform` da cena
2. `Renderer::drawFrame()` verifica se a geometria precisa ser resincronizada
3. `VulkanSemaphoresEngine::drawFrame()` adquire imagem da swapchain
4. `CameraEngine` atualiza o uniform buffer do frame
5. `VulkanCommandPoolEngine` grava o command buffer
6. para cada objeto da cena:
   - pega o `MeshAssetId`
   - pega o `MaterialAssetId`
   - busca os buffers da geometria correspondente
   - binda o `descriptor set` do material com a textura correspondente
   - envia `model matrix` e `baseColor` via push constant
   - executa `vkCmdDrawIndexed`
7. a fila grafica submete os comandos
8. a imagem e apresentada

Antes do draw, o frame atual tambem limpa:

- a cor da tela
- o `depth buffer`

## Por que a arquitetura foi ficando assim

A arquitetura evoluiu em camadas:

1. primeiro, fizemos o triangulo funcionar
2. depois, separamos aplicacao, contexto Vulkan e renderer
3. depois, criamos buffers reais
4. depois, introduzimos camera, cena e objetos
5. agora, estamos separando instancia de cena de asset compartilhado
6. e comecamos a separar geometria de aparencia com materiais
7. preparamos UVs e `TextureAsset` para texturas reais
8. ligamos o primeiro `combined image sampler` no material
9. ligamos o primeiro `depth buffer` real no renderer

O motivo principal e evitar crescimento em cima de uma base fraca.

Algumas decisoes importantes:

- `std::unique_ptr`
  - para ownership mais seguro
- `VulkanBuffer`
  - para reduzir duplicacao de codigo de buffer
- `SceneObject + MeshAssetId`
  - para separar objeto do mundo e geometria compartilhada
- `MaterialAssetId`
  - para separar aparencia do objeto da sua geometria
- `TextureAssetId`
  - para separar a referencia logica da textura do handle Vulkan concreto
- `Push Constants` para model
  - porque muda por objeto e e pequeno
- `Push Constants` para `baseColor`
  - porque a cor base do material ainda e pequena e barata de trocar por draw
- `UniformBuffer` para view/projection
  - porque muda por frame/camera
- `combined image sampler` por material
  - porque texturas mudam por objeto/material e se encaixam melhor em `descriptor sets`

## Limitacoes atuais

Ainda nao temos:

- depth buffer
- camera livre real
- serializacao de cena
- ECS
- lighting
- carregamento de modelo externo
- carregador de imagem de arquivo real
- camera em perspectiva e uso real de profundidade para objetos 3D

Ou seja: a base ja e bem melhor do que um tutorial puro, mas ainda esta numa fase inicial de engine.

## Direcao recomendada daqui para frente

Os proximos passos mais naturais sao:

- compartilhamento de pipeline/material
- carregamento real de imagem de arquivo
- depth buffer
- camera controlavel
- perspectiva e meshes com profundidade real
- carregamento de malha externa
- cena com mais componentes

## Resumo final

Hoje a engine funciona assim:

- `Application` controla o programa
- `VulkanContext` sobe o backend Vulkan
- `Renderer` desenha
- `SceneEngine` controla objetos
- `AssetManager` controla malhas, materiais e texturas compartilhadas
- `GeometryEngine` sobe a geometria para GPU
- `CameraEngine` manda dados de camera
- `Vulkan*Engine` cuida dos objetos Vulkan de baixo nivel

Ela foi estruturada desse jeito para crescer com mais seguranca, mais clareza e melhor performance do que um `Hello Triangle` monolitico.

Na cena padrao atual, o renderer desenha dois cubos que compartilham a mesma malha de asset, mas usam materiais diferentes. Isso ajuda a validar ao mesmo tempo:

- compartilhamento de geometria
- materiais por objeto
- textura placeholder
- profundidade
- perspectiva
