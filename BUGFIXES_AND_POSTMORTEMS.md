# Bugs, Correcões e Ajustes

Este arquivo registra bugs importantes encontrados durante a evolucao da `VulkanEngine`, as hipoteses levantadas, o diagnostico final, a correcao aplicada e o que poderia ter ajudado a descobrir cada problema antes.

O objetivo e duplo:

- servir como historico tecnico da engine
- servir como material de estudo para futuras depuracoes

## Resumo rapido das correcoes importantes

### Build e ambiente

- migracao do projeto para `CMake`
- integracao de dependencias de forma mais portavel
- criacao de launchers para `VS Code`
- ajuste de `working directory` para o executavel encontrar shaders e assets

### Vulkan e runtime

- selecao de GPU exigindo suporte real a `swapchain`
- recriacao correta de `swapchain`, `render pass` e `graphics pipeline`
- simplificacao do uso de validation layers para o caminho moderno
- correcao de carga de shaders para funcionar fora da pasta do `.exe`

### Arquitetura

- introducao de `Application`, `Renderer` e `VulkanContext`
- migracao gradual de ownership para `std::unique_ptr`
- criacao de `Scene`, `SceneObject`, `AssetManager` e `VulkanBuffer`
- separacao entre assets compartilhados e instancias da cena

### Renderizacao

- transicao de `gl_VertexIndex` para `vertex buffer`
- integracao de `index buffer`
- `UniformBuffer` para camera
- `push constants` para `model matrix` e dados de material
- `depth buffer`
- base de materiais e texturas placeholder

## Post-mortem: tela preta ao reintroduzir perspectiva

### Sintoma

Quando saimos do modo 2D/ortografico e tentamos voltar para a camera em perspectiva, a tela ficava preta. Em alguns momentos apareciam deformacoes estranhas, em outros momentos nada era desenhado.

Isso era especialmente confuso porque:

- o app nao crashava
- o loop principal continuava vivo
- o renderer ja tinha funcionado antes
- a versao ortografica/desdiagnostico voltava a desenhar normalmente

Ou seja: o problema nao estava no renderer inteiro. Ele estava em uma camada mais especifica da cadeia de transformacao.

### Hipoteses levantadas

Estas foram as hipoteses principais investigadas ao longo da depuracao:

1. `shader` antigo ou binario desatualizado
2. `working directory` incorreto
3. textura, material ou iluminacao zerando a cor final
4. `back-face culling`
5. `depth buffer` ocultando tudo
6. camera posicionada atras ou dentro dos objetos
7. erro na matriz de perspectiva
8. erro na ordem de multiplicacao das matrizes
9. convencao inconsistente entre `Mat4` no C++ e `mat4` no GLSL
10. erro no mapeamento de profundidade do Vulkan

### O que foi importante perceber

O ponto decisivo foi perceber que o problema nao era "um bug generico de Vulkan". O pipeline basico ainda funcionava.

Quando colocamos uma cena de diagnostico extremamente simples, com:

- duas `quads`
- camera bem controlada
- shader simples
- fundo preto
- transformacoes previsiveis

a imagem voltou.

Isso provou que:

- `swapchain` estava ok
- `render pass` estava ok
- `framebuffer` estava ok
- `command buffer` estava gravando draw corretamente
- vertex/index buffers estavam funcionando
- o problema estava mesmo na cadeia matematica da camera/projecao/clip-space

### Causa raiz

A causa raiz foi uma combinacao de inconsistencias de convencao matematica.

Em termos praticos, havia mistura entre:

- convencao de matriz montada no C++ como se estivéssemos pensando em um estilo
- uso do `mat4` no GLSL como se estivesse em outro estilo
- tentativas de compor `model`, `view` e `projection` com ordens diferentes
- transposicao aplicada em alguns pontos, mas nao de forma completamente consistente
- transicao para perspectiva sem um caminho de depuracao visual isolado

O resultado disso e que os vertices acabavam indo para fora do volume de visao ou para uma transformacao de clip-space invalida, gerando a tela preta.

Em outras palavras:

- o draw acontecia
- mas os vertices eram transformados para coordenadas erradas
- entao nada util chegava a rasterizacao

### Solucao aplicada

A correcao nao veio de uma unica linha milagrosa. Ela veio de um processo em etapas:

1. reduzir a complexidade da cena
2. recuperar uma base visual confiavel com `quads`
3. validar que a ortografica funcionava
4. isolar que o problema estava na perspectiva
5. testar diferentes convencoes de matriz
6. alinhar o upload das matrizes e o consumo no shader
7. simplificar temporariamente a projecao no shader quando necessario para diagnostico

O aprendizado mais importante foi este:

- antes de "consertar a perspectiva", foi necessario recuperar um estado minimo que desenhasse algo de forma previsivel

Sem esse checkpoint, qualquer mudanca parecia aleatoria.

### Passo a passo do diagnostico

#### Etapa 1: verificar se o problema era execucao antiga

Primeiro foi preciso garantir que o executavel rodado no debugger era realmente o mais recente.

Foi por isso que entraram:

- launcher com `build`
- launcher com `rebuild`
- ajuste de `cwd`

Sem isso, a gente podia depurar o codigo certo e executar o binario errado.

#### Etapa 2: testar camadas visuais simples

Depois reduzimos as variaveis:

- desligamos suspeitas sobre `culling`
- aumentamos ambiente e simplificamos luz
- usamos geometria muito simples

Isso ajudou a evitar que um bug de iluminacao parecesse bug de camera.

#### Etapa 3: criar cena de diagnostico

Essa foi a virada.

Trocar a cena para `quads` grandes com camera controlada mostrou que:

- a infraestrutura de render estava viva
- a logica de cena basica tambem
- a falha estava na transformacao 3D/perspectiva

#### Etapa 4: separar ortografica de perspectiva

Quando a ortografica funcionou e a perspectiva falhou, ficou claro que:

- nao era problema de `mesh`
- nao era problema de buffer
- nao era problema geral de shader
- era problema especifico de projecao e/ou espaco de camera

#### Etapa 5: revisar convencoes matematicas

A partir dai, o foco foi revisar:

- onde a translacao estava sendo armazenada na `Mat4`
- ordem de `scale`, `rotation` e `translation`
- ordem de `view` e `projection`
- se o shader estava tratando a matriz no mesmo padrao do C++
- se a profundidade estava no intervalo esperado pelo Vulkan

### O que poderia ter ajudado a descobrir antes

Se este projeto ja tivesse algumas ferramentas de diagnostico, esse bug teria sido descoberto mais cedo.

As coisas que mais teriam ajudado seriam:

#### 1. Um modo de debug visual desde o inicio

Exemplos:

- shader que pinta por `normal`
- shader que pinta por `depth`
- shader que pinta por posicao em camera-space
- modo sem luz e sem textura

Isso evita perder tempo confundindo bug de geometria com bug de iluminacao.

#### 2. Uma biblioteca matematica consolidada

Se o projeto estivesse usando uma base como `glm` de forma consistente para todas as transformacoes, o risco de misturar convencoes seria menor.

O problema nao foi "usar Mat4 propria" por si so. O problema foi usar uma `Mat4` propria enquanto a engine ainda estava evoluindo varias convencoes ao mesmo tempo.

#### 3. Testes pequenos de transformacao

Mesmo sem framework formal de testes, ja ajudaria ter validacoes locais como:

- um ponto conhecido no mundo indo para um ponto esperado em clip-space
- comparacao entre matriz esperada e matriz gerada
- teste simples de camera olhando para um objeto em `z`

#### 4. Logging de camera e bounds

Ter logs opcionais de:

- posicao da camera
- `model matrix`
- `view matrix`
- `projection matrix`
- `AABB` aproximada dos objetos

teria ajudado a ver mais cedo quando os objetos estavam indo para fora do volume de visao.

#### 5. Um checkpoint visual oficial

Ter um "modo seguro" da engine teria acelerado muito a depuracao.

Esse modo seguro seria algo como:

- geometria simples
- shader simples
- ortografica
- sem textura
- sem iluminacao complexa

Na pratica, foi isso que criamos manualmente durante o incidente.

## Estado atual e recomendacao

Depois desse incidente, a recomendacao de trabalho na engine e:

1. manter um checkpoint visual simples sempre disponivel
2. reintroduzir complexidade em camadas
3. nao subir camera, perspectiva, cubo, material e iluminacao tudo ao mesmo tempo
4. documentar convencoes matematicas de forma explicita

Tambem foi adicionado um caminho de depuracao melhor:

- `debug render mode` com `F1` a `F5`
- alternancia de projecao com `F6`
- transform por objeto mais explicito no vertex shader para reduzir ambiguidade entre CPU e GPU

## Convencoes que precisam ficar claras no projeto

O projeto deve deixar documentado, de forma objetiva:

- como a `Mat4` esta organizada em memoria
- em que lado os vetores sao multiplicados conceitualmente
- em que ordem `model`, `view` e `projection` sao compostas
- quando e por que existe `transpose`
- qual intervalo de profundidade estamos assumindo para Vulkan

Enquanto isso nao estiver absolutamente claro, bugs de camera e projecao tendem a reaparecer.

## Proximos ajustes recomendados

- criar um `RenderDebugMode`
- criar shaders de diagnostico opcionais
- documentar formalmente a convencao matematica da engine
- considerar migrar a camada de matematica para uma convencao unica e mais previsivel
- adicionar asserts/logs de camera e matrizes em `Debug`
