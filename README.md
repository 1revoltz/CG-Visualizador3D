# Visualizador 3D

Projeto Qt/C++ para visualizacao de modelos OBJ em wireframe, com transformacoes geometricas, projecoes ortogonais/perspectiva, viewport, clipping 2D e clipping volumetrico.

## Estrutura

```text
src/
  main.cpp
  core/        # Matriz, Objeto e leitor OBJ
  ui/          # MainWindow e frame de visualizacao
forms/         # Arquivos .ui do Qt Designer
assets/models/ # Modelos OBJ carregados na cena inicial
docs/          # Enunciado e materiais do trabalho
```

## Build com qmake

```powershell
qmake Visualizador3D.pro -spec win32-g++ CONFIG+=debug
mingw32-make -j4
```

No Qt Creator, abra `Visualizador3D.pro`.

## Observacoes

- Arquivos locais do Qt Creator (`*.user`) e artefatos de build ficam fora do Git.
- Os modelos padrao ficam em `assets/models/` e sao carregados automaticamente na inicializacao.

## Roteiros de teste

- Teste manual completo no Qt: `docs/roteiro_teste_manual_qt.md`.
- Bancada automatizada de escalonamento: `tests/escalonamento_bancada.pro`.
## Bancada de escalonamento

A reducao de tamanho deve ser feita com fatores positivos entre 0 e 1. Por exemplo, `0.5` ou `0,5` reduz pela metade mantendo o centroide do objeto no mesmo lugar. Fator `1` mantem o tamanho, fatores maiores que `1` aumentam, e fator `0` colapsa o eixo, por isso a interface recusa esse valor.

Para testar a matematica sem depender da interface:

```powershell
$env:Path = 'C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.11.1\mingw_64\bin;' + $env:Path
qmake tests\escalonamento_bancada.pro -o build\escalonamento_bancada\Makefile
mingw32-make -C build\escalonamento_bancada
.\build\escalonamento_bancada\release\escalonamento_bancada.exe
```

A bancada valida conversao de `0,5` e `0.5`, rejeicao de texto invalido, preservacao do centroide e multiplicacao correta das dimensoes em escala uniforme e nao uniforme.
