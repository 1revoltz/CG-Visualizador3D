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