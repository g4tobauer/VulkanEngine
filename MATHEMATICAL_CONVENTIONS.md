# Convencoes Matematicas da VulkanEngine

Este documento define as convencoes matematicas usadas pela engine hoje.

Ele existe para evitar exatamente o tipo de bug que ja aconteceu durante a reintroducao da perspectiva: a mistura de convencoes diferentes entre C++, shader e espacos de coordenadas.

## Objetivo

Quando alguem mexer em:

- `Transform`
- `Camera`
- `projection`
- `shader.vert`
- `push constants`
- `UniformBufferObject`

deve consultar este arquivo antes.

## Regra mais importante

A engine precisa operar com uma convencao coerente do inicio ao fim.

Os pontos que sempre precisam combinar sao:

1. como a `Mat4` e montada no C++
2. como ela e multiplicada no C++
3. como ela e enviada para GPU
4. como o GLSL interpreta `mat4`
5. qual ordem `model`, `view` e `projection` seguem

Se um desses pontos mudar isoladamente, o risco de tela preta ou deformacao sobe muito.

## Estruturas de matriz no projeto

Hoje a struct [Mat4](C:/Users/Joao/source/repos/g4tobauer/VulkanEngine/VulkanEngine/CommonStructs.h) e uma matriz `4x4` armazenada em um vetor linear de `16` `floats`.

O helper `multiply(lhs, rhs)` faz multiplicacao matricial classica.

Tambem existe `transpose(...)`, que e usado quando precisamos alinhar o layout do C++ com o que o shader espera.

## Convencao pratica atual da engine

Na pratica, a engine esta usando esta estrategia:

- o C++ monta suas matrizes usando a representacao da `Mat4` local
- antes de enviar para a GPU, algumas matrizes sao transpostas
- o shader trabalha com `mat4` do GLSL no fluxo tradicional `mat * vec`

Ou seja:

- no CPU, a engine mantem a propria representacao
- no shader, consumimos como matriz GLSL padrao
- a ponte entre as duas coisas e controlada por `transpose(...)`

## Ordem de transformacao

Para objetos da cena:

- `scale`
- depois `rotation`
- depois `translation`

Isso e montado no [Transform::toMatrix()](C:/Users/Joao/source/repos/g4tobauer/VulkanEngine/VulkanEngine/CommonStructs.h).

Em termos conceituais, queremos:

- escalar o objeto no espaco local
- rotacionar esse objeto ainda no espaco local
- mover o resultado para a posicao final no mundo

## Espacos usados pela engine

### Espaco local

E o espaco original da malha.

Exemplo:

- o cubo e definido em coordenadas locais
- o centro do cubo normalmente fica em volta da origem local

### Espaco de mundo

E o resultado de aplicar o `model`.

Aqui o objeto ja foi:

- escalado
- rotacionado
- posicionado na cena

### Espaco de camera ou view

E o mundo visto do ponto de vista da camera.

Aqui tudo e convertido para um sistema relativo a camera.

### Espaco de clip

E o resultado da projecao.

Depois disso, o pipeline do Vulkan decide o que esta dentro ou fora do volume visivel.

## Sobre a camera

A camera atual e controlada em [CameraEngine.cpp](C:/Users/Joao/source/repos/g4tobauer/VulkanEngine/VulkanEngine/CameraEngine.cpp).

Ela preenche o [UniformBufferObject](C:/Users/Joao/source/repos/g4tobauer/VulkanEngine/VulkanEngine/CommonStructs.h) por frame.

Hoje o `UBO` carrega:

- `view`
- `projection`
- `viewProjection`
- `cameraPosition`
- parametros de projecao
- luz
- opcoes de debug

## Sobre profundidade no Vulkan

O Vulkan trabalha com profundidade em intervalo `0..1` depois da projecao.

Isso e importante porque formulas copiadas de exemplos de OpenGL podem falhar se forem usadas sem adaptacao.

Esse foi um dos pontos mais perigosos durante a depuracao da perspectiva.

## Sobre `transpose`

`transpose` nao deve ser usado como "tentativa aleatoria".

Ele deve existir apenas quando houver uma razao clara:

- a representacao da matriz no C++ nao esta no mesmo formato que o shader esta consumindo

Se um dia a engine migrar completamente para uma unica convencao matematicamente consistente de ponta a ponta, o ideal e reduzir ao maximo o uso de transposicao manual espalhada pelo codigo.

## Regra para futuras alteracoes

Sempre que mexer em camera ou projecao:

1. validar uma cena minima primeiro
2. validar modo ortografico simples
3. validar modo de debug visual
4. so depois testar perspectiva completa

## Debug render mode

A engine agora possui modos de depuracao visual no fragment shader.

Controles atuais:

- `F1`: iluminacao normal
- `F2`: albedo sem iluminacao
- `F3`: visualizacao de normais
- `F4`: visualizacao de UV
- `F5`: visualizacao de profundidade
- `F6`: alterna entre projecao ortografica e perspectiva
- `F7`: reseta a camera para a pose padrao do modo atual

Esses modos ajudam a responder perguntas como:

- a geometria esta sendo desenhada?
- a textura esta chegando?
- as normais parecem corretas?
- o depth buffer esta coerente?

No estado atual do projeto:

- a perspectiva e o modo padrao
- a ortografica continua disponivel como modo de diagnostico e inspecao

## Camera 3D atual

Em perspectiva, a camera agora tem orientacao propria.

Controles atuais:

- segurar `botao direito do mouse`: ativa `mouse look`
- mover o mouse com o botao direito pressionado: gira a camera em `yaw/pitch`
- `W/S`: anda para frente e para tras na direcao da camera
- `A/D`: anda para os lados em relacao a camera
- `Q/E`: sobe e desce no eixo vertical
- `F7`: reseta posicao e orientacao

Internamente, o shader nao usa apenas a posicao da camera. Ele tambem recebe:

- `cameraForward`
- `cameraRight`
- `cameraUp`

Esses vetores permitem converter o mundo para o espaco da camera de forma mais coerente com a orientacao atual.

## Caminho temporario de estabilizacao da perspectiva

Neste momento da evolucao da engine, a transformacao por objeto no vertex shader foi simplificada.

Em vez de depender imediatamente de uma `model matrix` completa por `push constant`, o shader usa um transform explicito por objeto:

- `objectPosition`
- `objectScale`
- `objectRotation` com `cos` e `sin`

Isso foi feito para reduzir ambiguidades de convencao entre CPU e GPU enquanto a camada matematica ainda esta sendo consolidada.

Nao e necessariamente a forma final da engine, mas e uma forma muito melhor de depurar porque:

- deixa o fluxo local -> mundo mais visivel
- reduz o numero de lugares onde um `transpose` pode confundir
- isola melhor bugs de camera e projecao

Quando a convencao matematica estiver completamente fechada, essa parte pode ser migrada de novo para uma representacao matricial mais formal, se fizer sentido.

## Recomendacao futura

Se quisermos reduzir risco estrutural, o melhor caminho e:

- consolidar uma unica convencao matematica formal
- minimizar `transpose` manual
- considerar uma camada matematica mais padronizada
- manter shaders de debug sempre disponiveis
