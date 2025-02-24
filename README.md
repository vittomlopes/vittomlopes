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
 

## 🚀 Como Compilar e Executar  

### 🔹 1. Instalando a SDL2  

Antes de compilar, certifique-se de que a **SDL2** e suas bibliotecas estão instaladas:  

#### 🖥️ **Linux (Ubuntu/Debian)**  
```sh
sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
```

#### 🪟 **Windows (MinGW)**  
1. Baixe os arquivos da SDL2: [SDL Releases](https://github.com/libsdl-org/SDL/releases)  
2. Extraia e adicione os arquivos `.lib` e `.dll` ao seu projeto.  
3. Para instalar o MinGW e o Make, execute no **PowerShell** como administrador:  
   ```sh
   winget install -e --id MSYS2.MSYS2
   ```
4. Depois, abra o **MSYS2** e instale os pacotes necessários:  
   ```sh
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make
   ```

#### 🍏 **macOS (Homebrew)**  
```sh
brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf
```

---

### 🔹 2. Compilando o Jogo  

O jogo utiliza um **Makefile** para facilitar a compilação.  

#### 🖥️ **Linux/macOS**  
No terminal, dentro da pasta do projeto, execute:  
```sh
make
```

#### 🪟 **Windows (MSYS2 MinGW)**  
Abra o **MSYS2 MinGW** e navegue até a pasta do projeto, depois execute:  
```sh
mingw32-make
```

Isso irá gerar um executável chamado **`game`**.  

⚠️ **Se houver erros de compilação**, verifique se todas as bibliotecas da **SDL2** estão instaladas corretamente (veja a seção acima).  

---

### 🔹 3. Limpando Arquivos Compilados  

Se quiser remover os arquivos gerados e recompilar do zero:  

#### 🖥️ **Linux/macOS**  
```sh
make clean
```

#### 🪟 **Windows (MSYS2 MinGW)**  
```sh
mingw32-make clean
```

---

### 🔹 4. Executando o Jogo  

Após a compilação, execute o jogo:  

#### 🖥️ **Linux/macOS**  
```sh
./game
```

#### 🪟 **Windows (PowerShell ou CMD na pasta do projeto)**  
```sh
game.exe
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




