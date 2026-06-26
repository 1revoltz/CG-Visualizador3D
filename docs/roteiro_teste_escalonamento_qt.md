# Roteiro de teste manual: escalonamento no Qt

Este roteiro valida o comportamento do botao **Escalar** na interface do Visualizador 3D.

## Preparacao

1. Abra `Visualizador3D.pro` no Qt Creator.
2. Compile e execute o projeto.
3. Confirme que os modelos em `assets/models/` carregaram automaticamente.
4. Na lista de objetos, selecione um modelo facil de observar.
5. Use a projecao **Frontal XY** para testes de largura/altura.
6. Para cada teste, se quiser comparar com o tamanho original, reinicie o programa antes do proximo caso.

## Comportamento esperado da escala

A escala e aplicada em torno do centroide do objeto selecionado.

- `1` mantem o tamanho.
- Valores entre `0` e `1` reduzem.
- Valores maiores que `1` aumentam.
- Valores diferentes por eixo deformam o objeto por eixo.
- `0` nao e uma reducao normal: ele colapsa o eixo e deve ser recusado pela interface.
- Numeros com ponto decimal (`0.5`) e virgula decimal (`0,5`) devem funcionar.

## Casos de teste

| Caso | Valores em Escala X/Y/Z | Passos | Resultado esperado |
| --- | --- | --- | --- |
| 1. Identidade | `1`, `1`, `1` | Selecione um objeto e clique **Escalar**. | O objeto permanece visualmente igual e no mesmo lugar. |
| 2. Reducao uniforme com ponto | `0.5`, `0.5`, `0.5` | Selecione um objeto e clique **Escalar**. | O objeto fica com metade do tamanho, mantendo o centro no mesmo lugar. Ele nao deve sumir. |
| 3. Reducao uniforme com virgula | `0,5`, `0,5`, `0,5` | Reinicie o app ou selecione outro objeto no tamanho original. Clique **Escalar**. | Mesmo resultado do caso anterior: metade do tamanho e centro preservado. Esse e o principal teste contra o bug. |
| 4. Reducao forte | `0.1`, `0.1`, `0.1` | Clique **Escalar** uma vez. | O objeto fica bem pequeno, mas ainda deve aparecer proximo ao mesmo centro. |
| 5. Aumento uniforme | `2`, `2`, `2` | Clique **Escalar** uma vez. | O objeto dobra de tamanho em todos os eixos e continua centralizado. |
| 6. Reducao somente em X | `0.5`, `1`, `1` | Use projecao **Frontal XY** e clique **Escalar**. | A largura aparente reduz pela metade; a altura permanece igual. |
| 7. Reducao somente em Y | `1`, `0.5`, `1` | Use projecao **Frontal XY** e clique **Escalar**. | A altura aparente reduz pela metade; a largura permanece igual. |
| 8. Reducao somente em Z | `1`, `1`, `0.5` | Use projecao **Perspectiva** ou uma vista que mostre profundidade, depois clique **Escalar**. | A profundidade reduz; na vista Frontal XY a mudanca pode ser pouco perceptivel. |
| 9. Escala nao uniforme | `0.5`, `2`, `1` | Use projecao **Frontal XY** e clique **Escalar**. | O objeto fica mais estreito e mais alto, mantendo o centro. |
| 10. Fator zero | `0`, `1`, `1` | Clique **Escalar**. | A interface deve mostrar aviso de escalonamento invalido. O objeto nao deve ser alterado. |
| 11. Campo invalido | `abc`, `1`, `1` | Digite texto invalido em Escala X e clique **Escalar**. | A interface deve mostrar aviso de valor invalido, selecionar o campo e nao alterar o objeto. |
| 12. Campo vazio | vazio, `1`, `1` | Apague Escala X e clique **Escalar**. | A interface deve mostrar aviso de valor invalido e nao alterar o objeto. |
| 13. Aplicacao repetida | `0.5`, `0.5`, `0.5` | Clique **Escalar** duas vezes no mesmo objeto. | O objeto fica com 1/4 do tamanho original, pois a escala e acumulativa. |
| 14. Espelhamento por valor negativo | `-1`, `1`, `1` | Clique **Escalar**. | O objeto espelha no eixo X em torno do centroide. Isso e matematicamente valido, mas nao e reducao de tamanho. |

## Checklist de observacao

Durante cada teste, observe:

- O objeto selecionado e o unico alterado.
- O centro visual do objeto nao desloca ao escalar.
- A reducao com `0.5` e `0,5` produz o mesmo resultado.
- O objeto nao desaparece quando o fator e positivo e menor que `1`.
- A interface recusa `0`, texto invalido e campo vazio.
- A escala e acumulativa: aplicar duas vezes multiplica novamente o tamanho atual.

## Resultado esperado geral

Se todos os casos passarem, a funcao de escalonamento esta correta para uso normal. O erro original provavelmente estava ligado a entrada numerica: valores como `0,5` podiam ser convertidos para `0`, causando colapso do objeto ao tentar reduzir.