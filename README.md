# Snake Game 🐍

Este é um jogo Snake clássico desenvolvido em **C**. O jogador pode escolher entre diferentes níveis de dificuldade, registrar recordes e competir pela maior pontuação. O jogo utiliza controles simples e é projetado para rodar em terminais.

---

## 🎰 Tela Inicial e Menu Principal
O jogo possui uma tela inicial chamativa e um menu principal interativo:

### Tela Inicial:
- Fundo preto com uma cobra verde desenhada de forma amigável.
- O título "Snake" em verde.
- Mensagem "Press Start" para iniciar o jogo.
- Assinatura do autor: "Snake Game PT BR | João Vitor Lopes Chaves © 2025".

### Menu Principal:
- O título "SNAKE" em vermelho destacado.
- Opções do menu:
  - **Iniciar** (em amarelo): Começa o jogo.
  - **Recordes**: Exibe os recordes salvos.
  - **Tutorial**: Explica as regras e controles.
  - **Sair**: Fecha o jogo.

---

## 🎮 Funcionalidades
- Vários níveis de dificuldade.
- Registro de recordes.
- Controles intuitivos.
- Interface amigável para terminal.

---

## ⚙️ Requisitos
- Compilador GCC ou compatível (Linux, Windows ou macOS).
- Bibliotecas necessárias:
  - SDL2 → Para gráficos e entrada do usuário.
  - conio.h → Para kbhit() e getch() (apenas no Windows).
  - unistd.h → Para usleep() (apenas no Linux/macOS).
- Terminal para rodar o jogo.

---

## 🚀 Como Executar

### Instalando a SDL2

No Linux (Ubuntu/Debian):
```sh
sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
```

No Windows (MinGW):
1. Baixe os arquivos da SDL2 em: https://github.com/libsdl-org/SDL/releases
2. Extraia e adicione os arquivos .lib e .dll no seu projeto.

No macOS (Homebrew):
```sh
brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf
```

---

## 🎩 Controles e Menus

- **Seta para cima ou W**: Move para cima
- **Seta para baixo ou S**: Move para baixo
- **Seta para esquerda ou A**: Move para a esquerda
- **Seta para direita ou D**: Move para a direita
- **Tecla Enter**: Seleciona no menu
- **Tecla Esc**: Sai do jogo
- **Rolagem do mouse**: Move a lista de recordes
- **Tecla F12**: Alterna entre modo tela cheia e modo janela

---

## 🎉 Créditos
Desenvolvido por João Vitor Lopes Chaves

---

## 🔗 Links
- [Repositório no GitHub](#) *(Adicionar o link real aqui)*
- [Documentação Oficial da SDL2](https://github.com/libsdl-org/SDL)




