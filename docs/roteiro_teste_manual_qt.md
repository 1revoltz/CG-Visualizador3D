# Roteiro de teste manual no Qt

Este roteiro valida as principais funcoes do Visualizador 3D pela interface do Qt.

## Preparacao geral

1. Abra `Visualizador3D.pro` no Qt Creator.
2. Compile e execute o projeto.
3. Confirme que a janela abriu com o titulo `Visualizador 3D`.
4. Confirme que os modelos de `assets/models/` aparecem na area de desenho e na lista de objetos.
5. Para testes de transformacao, selecione um objeto na lista antes de clicar nos botoes.
6. Para comparar com o estado original, reinicie o programa ou carregue novamente um OBJ.

Observacao importante: transformacoes sao acumulativas. Se voce aplicar escala `0.5` duas vezes, o objeto fica com `0.25` do tamanho original.

## Criterios gerais de aprovacao

- O programa nao deve fechar ou travar em nenhum teste.
- Apenas o objeto selecionado deve mudar nas transformacoes de objeto.
- Controles de camera/window podem alterar a cena inteira.
- A viewport deve continuar desenhando apenas dentro do retangulo definido.
- Linhas fora da window devem ser recortadas, nao desenhadas fora do limite.

## 1. Inicializacao, lista e status do mouse

| Caso | Passos | Resultado esperado |
| --- | --- | --- |
| 1.1 Modelos padrao | Execute o programa. | Os 7 modelos OBJ em `assets/models/` carregam automaticamente. A lista de objetos mostra os nomes dos modelos. |
| 1.2 Nenhum objeto selecionado | Sem selecionar item na lista, clique `Transladar`, `Escalar` e `Rotacionar`. | Nada deve mudar e o programa nao deve travar. |
| 1.3 Selecao | Selecione um item da lista. | As proximas transformacoes devem afetar somente esse objeto. |
| 1.4 Status do mouse | Mova o mouse sobre a area branca. | A barra de status mostra `Mouse: (x, y)` atualizando conforme o movimento. |

## 2. Carregar OBJ

| Caso | Passos | Resultado esperado |
| --- | --- | --- |
| 2.1 Carregar OBJ valido | Clique `Carregar OBJ` e selecione `assets/models/Rayquaza.obj`. | Um novo objeto chamado `Rayquaza` aparece na lista, fica selecionado e e desenhado centralizado na cena. |
| 2.2 Carregar outro OBJ valido | Clique `Carregar OBJ` e selecione outro arquivo de `assets/models/`. | O objeto e adicionado ao fim da lista sem remover os anteriores. |
| 2.3 Cancelar dialogo | Clique `Carregar OBJ` e cancele a janela. | Nada muda na lista e a cena permanece igual. |
| 2.4 OBJ invalido opcional | Crie um arquivo `tmp/obj_invalido.obj` contendo apenas texto qualquer, tente carregar esse arquivo. | O programa mostra mensagem de erro e nao adiciona objeto novo. |

## 3. Transladar objeto

Use a projecao `Frontal (XY)` para os testes 3.1 a 3.5.

| Caso | X | Y | Z | Resultado esperado |
| --- | --- | --- | --- | --- |
| 3.1 Sem deslocamento | `0` | `0` | `0` | O objeto permanece no mesmo lugar. |
| 3.2 Direita | `50` | `0` | `0` | O objeto selecionado aparece 50 unidades mais a direita. |
| 3.3 Esquerda | `-50` | `0` | `0` | O objeto selecionado aparece 50 unidades mais a esquerda. |
| 3.4 Cima | `0` | `50` | `0` | O objeto selecionado aparece mais acima na tela. |
| 3.5 Baixo | `0` | `-50` | `0` | O objeto selecionado aparece mais abaixo na tela. |
| 3.6 Profundidade em Frontal | `0` | `0` | `100` | Em `Frontal (XY)`, quase nao deve haver mudanca visual, porque Z nao aparece nesse plano. |
| 3.7 Profundidade em Superior | `0` | `0` | `100` | Em `Superior (XZ)`, o objeto muda no eixo vertical da vista. |
| 3.8 Profundidade em Lateral | `0` | `0` | `100` | Em `Lateral (ZY)`, o objeto muda no eixo horizontal da vista. |
| 3.9 Profundidade em Perspectiva | `0` | `0` | `100` | Em `Perspectiva`, o objeto tende a parecer menor por ficar mais distante. Com `-100`, tende a parecer maior. |

## 4. Escalonar objeto

A escala e aplicada em torno do centroide do objeto selecionado.

| Caso | X | Y | Z | Resultado esperado |
| --- | --- | --- | --- | --- |
| 4.1 Identidade | `1` | `1` | `1` | O objeto permanece do mesmo tamanho e no mesmo centro. |
| 4.2 Reducao com ponto | `0.5` | `0.5` | `0.5` | O objeto fica com metade do tamanho e nao some. |
| 4.3 Reducao com virgula | `0,5` | `0,5` | `0,5` | Mesmo resultado do caso 4.2. Esse e o teste principal contra o bug de reducao. |
| 4.4 Reducao forte | `0.1` | `0.1` | `0.1` | O objeto fica bem pequeno, mas continua visivel perto do mesmo centro. |
| 4.5 Aumento | `2` | `2` | `2` | O objeto dobra de tamanho e continua centralizado no mesmo ponto. |
| 4.6 Reduzir largura | `0.5` | `1` | `1` | Em `Frontal (XY)`, a largura reduz pela metade e a altura fica igual. |
| 4.7 Reduzir altura | `1` | `0.5` | `1` | Em `Frontal (XY)`, a altura reduz pela metade e a largura fica igual. |
| 4.8 Reduzir profundidade | `1` | `1` | `0.5` | A mudanca fica mais visivel em `Superior (XZ)`, `Lateral (ZY)` ou `Perspectiva`. |
| 4.9 Nao uniforme | `0.5` | `2` | `1` | O objeto fica mais estreito e mais alto, mantendo o centroide. |
| 4.10 Fator zero | `0` | `1` | `1` | A interface deve mostrar aviso e nao alterar o objeto. |
| 4.11 Campo vazio | vazio | `1` | `1` | A interface deve mostrar aviso de valor invalido e nao alterar o objeto. |
| 4.12 Valor negativo | `-1` | `1` | `1` | O objeto espelha no eixo X em torno do centroide. Isso e valido matematicamente, mas nao e reducao. |

## 5. Rotacionar objeto

Use um objeto carregado no centro da cena para facilitar a observacao.

| Caso | Angulo | Eixo | Pivo | Resultado esperado |
| --- | --- | --- | --- | --- |
| 5.1 Sem rotacao | `0` | `Z` | Centroide | O objeto permanece igual. |
| 5.2 Rotacao Z positiva | `90` | `Z` | Centroide | Em `Frontal (XY)`, o objeto gira 90 graus em torno do proprio centro. |
| 5.3 Rotacao Z negativa | `-90` | `Z` | Centroide | O objeto gira no sentido oposto. Se aplicado apos 5.2, deve aproximadamente voltar ao estado anterior. |
| 5.4 Rotacao X | `45` | `X` | Centroide | A forma projetada muda por inclinacao 3D, mantendo o centro. |
| 5.5 Rotacao Y | `45` | `Y` | Centroide | A forma projetada muda por inclinacao 3D, mantendo o centro. |
| 5.6 Ponto fixo central | `90` | `Z` | Ponto fixo `400`, `300`, `0` | Para um OBJ carregado centralizado, o comportamento fica parecido com girar no centro. |
| 5.7 Ponto fixo externo | `45` | `Z` | Ponto fixo `0`, `0`, `0` | O objeto gira/orbita em torno da origem e muda de posicao na tela. |
| 5.8 Rotacao acumulativa | `15` | `Z` | Centroide, clique 4 vezes | O resultado visual deve se aproximar de uma rotacao de 60 graus. |

## 6. Projecoes

| Caso | Selecao no combo | Resultado esperado |
| --- | --- | --- |
| 6.1 Frontal | `Frontal (XY)` | Exibe X na horizontal e Y na vertical. E a vista padrao. |
| 6.2 Superior | `Superior (XZ)` | Exibe X na horizontal e Z na vertical. Mudancas em Y ficam pouco perceptiveis nessa vista. |
| 6.3 Lateral | `Lateral (ZY)` | Exibe Z na horizontal e Y na vertical. Mudancas em X ficam pouco perceptiveis nessa vista. |
| 6.4 Perspectiva | `Perspectiva` | A profundidade afeta o tamanho aparente. Objetos mais proximos parecem maiores. |
| 6.5 Troca repetida | Alterne entre todas as opcoes. | O programa redesenha a cena sem travar e sem perder objetos da lista. |

## 7. Window e zoom

Estes botoes alteram a window do mundo. Por isso, a cena costuma parecer andar no sentido oposto ao botao.

| Caso | Passos | Resultado esperado |
| --- | --- | --- |
| 7.1 Window Cima | Clique `Cima`. | A window sobe; visualmente, os objetos tendem a aparecer mais abaixo. |
| 7.2 Window Baixo | Clique `Baixo`. | A window desce; visualmente, os objetos tendem a aparecer mais acima. |
| 7.3 Window Direita | Clique `Direita`. | A window anda para a direita; visualmente, os objetos tendem a aparecer mais a esquerda. |
| 7.4 Window Esquerda | Clique `Esquerda`. | A window anda para a esquerda; visualmente, os objetos tendem a aparecer mais a direita. |
| 7.5 Zoom IN | Clique `Zoom IN`. | A window diminui e os objetos parecem maiores. |
| 7.6 Zoom Out | Clique `Zoom Out`. | A window aumenta e os objetos parecem menores. |
| 7.7 Zoom reversivel | Clique `Zoom IN` uma vez e `Zoom Out` uma vez. | A cena deve voltar aproximadamente ao enquadramento anterior. |

## 8. Viewport

Valores iniciais esperados: `X min = 50`, `Y min = 50`, `X max = 750`, `Y max = 550`.

| Caso | X min | Y min | X max | Y max | Resultado esperado |
| --- | --- | --- | --- | --- | --- |
| 8.1 Padrao | `50` | `50` | `750` | `550` | O retangulo da viewport ocupa a area interna padrao. |
| 8.2 Viewport menor | `200` | `100` | `600` | `500` | O retangulo fica menor e a cena e remapeada para dentro dele. |
| 8.3 Viewport deslocada | `100` | `200` | `700` | `550` | O retangulo muda de posicao; a cena continua desenhada dentro dele. |
| 8.4 Viewport larga | `0` | `50` | `800` | `550` | O retangulo ocupa quase toda a largura da area de desenho. |
| 8.5 Restaurar padrao | `50` | `50` | `750` | `550` | A viewport volta ao enquadramento inicial. |

## 9. Camera

Os controles de camera transformam a cena inteira, nao apenas o objeto selecionado.

| Caso | Projecao | Passos | Resultado esperado |
| --- | --- | --- | --- |
| 9.1 Camera esquerda | Frontal | Clique `Camera para Esquerda`. | A cena tende a aparecer mais a direita, simulando camera indo para a esquerda. |
| 9.2 Camera direita | Frontal | Clique `Camera para Direita`. | A cena tende a aparecer mais a esquerda. |
| 9.3 Camera cima | Frontal | Clique `Camera para Cima`. | A cena tende a aparecer mais abaixo. |
| 9.4 Camera baixo | Frontal | Clique `Camera para Baixo`. | A cena tende a aparecer mais acima. |
| 9.5 Camera frente ortogonal | Frontal | Clique `Camera para Frente`. | Como a vista e ortogonal, o efeito esperado e semelhante a `Zoom IN`: cena maior. |
| 9.6 Camera tras ortogonal | Frontal | Clique `Camera para Tras`. | Como a vista e ortogonal, o efeito esperado e semelhante a `Zoom Out`: cena menor. |
| 9.7 Camera frente perspectiva | Perspectiva | Clique `Camera para Frente`. | Os objetos devem parecer mais proximos/maiores. |
| 9.8 Camera tras perspectiva | Perspectiva | Clique `Camera para Tras`. | Os objetos devem parecer mais distantes/menores. |
| 9.9 Movimento reversivel | Frontal | Clique `Camera para Esquerda` e depois `Camera para Direita`. | A cena deve voltar aproximadamente ao lugar anterior. |

## 10. Olhar com a camera

Use `Perspectiva` para observar melhor estes casos.

| Caso | Passos | Resultado esperado |
| --- | --- | --- |
| 10.1 Olhar esquerda | Clique `Olhar para a esquerda`. | A cena rotaciona horizontalmente como se a camera olhasse para a esquerda. |
| 10.2 Olhar direita | Clique `Olhar para a direita`. | A cena rotaciona horizontalmente no sentido oposto. |
| 10.3 Olhar cima | Clique `Olhar para cima`. | A cena inclina verticalmente como se a camera olhasse para cima. |
| 10.4 Olhar baixo | Clique `Olhar para baixo`. | A cena inclina verticalmente no sentido oposto. |
| 10.5 Reversibilidade | Clique `Olhar para a esquerda` e depois `Olhar para a direita`. | A cena deve voltar aproximadamente ao enquadramento anterior. |

## 11. Clipping e limites visuais

| Caso | Passos | Resultado esperado |
| --- | --- | --- |
| 11.1 Clipping por translacao | Em `Frontal (XY)`, selecione um objeto e translade com X `900`, Y `0`, Z `0`. | O objeto sai da window; as linhas devem ser recortadas ou desaparecer gradualmente, sem serem desenhadas fora da viewport. |
| 11.2 Clipping parcial | Reinicie, selecione um objeto e translade com X `500`, Y `0`, Z `0`. | Parte do objeto pode ficar visivel e parte recortada na borda. |
| 11.3 Clipping com Zoom IN | Clique `Zoom IN` varias vezes. | Objetos maiores que a window devem ser recortados na borda, mantendo o desenho dentro da viewport. |
| 11.4 Clipping em perspectiva | Use `Perspectiva` e clique varias vezes em `Camera para Frente`. | Objetos proximos demais podem ser recortados pelo volume de visualizacao, sem travar o programa. |

## 12. Fluxo completo sugerido para apresentacao

1. Abrir o programa e mostrar os modelos padrao carregados.
2. Selecionar um objeto na lista.
3. Aplicar translacao: X `50`, Y `0`, Z `0`.
4. Aplicar escala: X `0,5`, Y `0,5`, Z `0,5`.
5. Aplicar rotacao: angulo `45`, eixo `Z`, centroide.
6. Alternar entre `Frontal (XY)`, `Superior (XZ)`, `Lateral (ZY)` e `Perspectiva`.
7. Mostrar `Zoom IN` e `Zoom Out`.
8. Alterar viewport para `200`, `100`, `600`, `500` e depois restaurar para `50`, `50`, `750`, `550`.
9. Em perspectiva, usar `Camera para Frente`, `Camera para Tras`, `Olhar para a esquerda` e `Olhar para a direita`.
10. Carregar manualmente outro OBJ e confirmar que ele entra na lista.

## Resultado esperado geral

Se todos os testes passarem, as funcoes principais do programa estao coerentes: carregamento de OBJ, display file/lista, transformacoes geometricas, projecoes, window, viewport, clipping e controles de camera.