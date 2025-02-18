# Snake Game 🐍

Este é um jogo Snake clássico desenvolvido em **C**. O jogador pode escolher entre diferentes níveis de dificuldade, registrar recordes e competir pela maior pontuação. O jogo utiliza controles simples e é projetado para rodar em terminais.

---

## 🎰 Funcionalidades
- Escolha de dificuldade: Fácil, Médio e Difícil, com velocidades ajustadas.
- Sistema de recordes com o nome do jogador.
- Movimentação fluida da cobra com detecção de colisões.
- Objetivo: Comer frutas (`X`) e evitar bater nas paredes ou na própria cobra.

---

## ⚙️ Requisitos
- **Compilador GCC** ou compatível (Linux, Windows, ou macOS).
- O código utiliza:
  - `conio.h`: Para funções como `kbhit()` e `getch()` no Windows.
  - `unistd.h`: Para `usleep()` em sistemas baseados em Unix (Linux/MacOS).
- **Terminal** para rodar o jogo.

---


## 🚀 Como Executar
### Windows:
1. Instale o compilador GCC (via MinGW, por exemplo).
ou
1. Clone este repositório:
   ```bash
   git clone https://github.com/vittomlopes/snake-game.git
   cd snake-game
   
2. Compile o código:
   ```bash
   gcc cobrinha.c -o cobrinha
   ```
3. Execute o jogo:
   ```bash
   cobrinha.exe
   ```

### Linux/MacOS:
1. Certifique-se de que o GCC está instalado:
   ```bash
   sudo apt install gcc    # Para Debian/Ubuntu
   ```
2. Compile o código:
   ```bash
   gcc cobrinha.c -o cobrinha
   ```
3. Execute o jogo:
   ```bash
   ./cobrinha
   ```

---

## 🕹️ Controles e Menus
- **Menu Principal**:
  - `1`: Escolher dificuldade e começar o jogo.
  - `2`: Ver recordes.
  - `0`: Sair do jogo.
- **Controles Durante o Jogo**:
  - `W`: Mover para cima.
  - `S`: Mover para baixo.
  - `A`: Mover para a esquerda.
  - `D`: Mover para a direita.

---

## 🏆 Sistema de Recordes
- Quando o jogo termina, a pontuação é comparada ao recorde atual.
- Se a pontuação for maior, o nome do jogador e a pontuação são registrados como novo recorde.

---

## 📁 Estrutura do Código
### Principais Funções:
1. `exibirMenuPrincipal()`
   - Exibe o menu principal com as opções do jogo.

2. `exibirMenuDificuldade(int *dificuldade)`
   - Permite ao jogador selecionar a dificuldade do jogo.

3. `comecarJogo(int dificuldade)`
   - Inicializa o jogo, configura a cobra, objetivo e controla a lógica principal.

4. `moverCobra(...)`
   - Controla a movimentação da cobra e verifica colisões.

5. `gerarObjetivo(...)`
   - Posiciona aleatoriamente o objetivo (fruta) no tabuleiro.

6. `exibirRecord()`
   - Mostra o recorde atual.

7. `limparTela()`
   - Limpa a tela dependendo do sistema operacional.



## 👥 Equipe
- **João Vitor Lopes Chaves**


---


## 📝 Licença
Este projeto é licenciado sob a [Licença MIT](./LICENSE).




