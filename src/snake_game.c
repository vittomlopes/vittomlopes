
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "menu.h"
#include <SDL2/SDL_mixer.h>
// Definindo o tamanho do labirinto
#define LINES 8
#define COLS 12
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 700
int x = (SCREEN_WIDTH - 100) / 2;
int y = (SCREEN_HEIGHT - 100) / 2;
#define MAX_RECORDS 100  // Atualize o limite para 10

#define RECORDS_FILE "scores.txt"

extern int calcMenuY(int selectedOption);  // Declaração da função Assembly
SDL_Texture* appleTexture = NULL; 
// Definição das direções
typedef struct {
    char playerName[20];
    int score;
} Record;

typedef enum {
    UP, DOWN, LEFT, RIGHT
} Direction;
// Estrutura para armazenar informações do jogo
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    char playerName[20];
    int score;
    int level;
    int snakeLength;
    SDL_Rect snake[100]; // Representação da cobra
    SDL_Rect apple;      // Maçã
    // SDL_Rect rocks[10];  // Pedras
    SDL_Rect appleRect; // Adicionando a variável appleRect na struct do jogo
    int numRocks;
    Mix_Chunk *eatSound;  // Declaração correta da variável global
    TTF_Font *titleFont;
    TTF_Font *menuFont;
    Direction direction; // Direção da cobra
} Game;
// Funções declaradas
void updateSnake(Game *game);
void checkAppleCollision(Game *game);
void showStartScreen(Game *game);
void showGameScreen(Game *game);
// Função para carregar o ícone
SDL_Surface* loadIcon(const char *path) {
    SDL_Surface *icon = IMG_Load(path);  // Use IMG_Load para PNG, JPG ou BMP
    if (!icon) {
        printf("Erro ao carregar o ícone: %s\n", IMG_GetError());
    }
    return icon;
}
bool initSDL(Game *game) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erro ao inicializar SDL_mixer: %s\n", Mix_GetError());
        return false;
    }
    game->eatSound = Mix_LoadWAV("./sounds/eat.wav");
    if (!game->eatSound) {
        printf("Erro ao carregar som de comer: %s\n", Mix_GetError());
    }
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return false;
    }
    if (TTF_Init() == -1) {
        printf("Erro ao inicializar SDL_ttf: %s\n", TTF_GetError());
        return false;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("Erro ao inicializar SDL_image: %s\n", IMG_GetError());
        return false;
    }

    game->window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!game->window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        return false;
    }

    // Carregar e definir o ícone
    SDL_Surface *icon = loadIcon("./assets/logo.jpeg");  // Substitua pelo caminho do seu ícone
    if (icon) {
        SDL_SetWindowIcon(game->window, icon);
        SDL_FreeSurface(icon);  // Libere a superfície após definir o ícone
    }

    game->titleFont = TTF_OpenFont("./fonts/SigmarOne-Regular.ttf", 120);
    game->menuFont = TTF_OpenFont("./fonts/SigmarOne-Regular.ttf", 24);
    if (!game->titleFont || !game->menuFont) {
        printf("Erro ao carregar fonte: %s\n", TTF_GetError());
        return false;
    }
 
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer) {
        printf("Erro ao criar renderizador: %s\n", SDL_GetError());
        return false;
    }
     
    return true;
}

void drawCircle(SDL_Renderer *renderer, int x, int y, int radius) {
    for (int w = 0; w < 2 * radius; w++) {
        for (int h = 0; h < 2 * radius; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}
void drawApple(Game *game) {
    // Desenho da maçã como um círculo
    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255); // Cor vermelha para a maçã
    drawCircle(game->renderer, game->appleRect.x + game->appleRect.w / 2, game->appleRect.y + game->appleRect.h / 2, game->appleRect.w / 2);
    // Desenhando uma borda mais espessa
    SDL_SetRenderDrawColor(game->renderer, 200, 0, 0, 255); // Cor da borda
    drawCircle(game->renderer, game->appleRect.x + game->appleRect.w / 2, game->appleRect.y + game->appleRect.h / 2, game->appleRect.w / 2 + 3);
    // Sombreamento para dar um efeito 3D
    SDL_SetRenderDrawColor(game->renderer, 150, 0, 0, 128); // Cor mais escura para sombreamento
    drawCircle(game->renderer, game->appleRect.x + game->appleRect.w / 2 + 5, game->appleRect.y + game->appleRect.h / 2 + 5, game->appleRect.w / 2);
    // Desenhando a folha
    int leafWidth = 25;
    int leafHeight = 12;
    SDL_Rect leaf = {game->appleRect.x + game->appleRect.w / 2 - leafWidth / 2, 
                     game->appleRect.y - leafHeight, leafWidth, leafHeight};
    SDL_SetRenderDrawColor(game->renderer, 34, 139, 34, 255); // Cor verde para a folha
    SDL_RenderFillRect(game->renderer, &leaf);
    // Nervura da folha
    SDL_SetRenderDrawColor(game->renderer, 0, 100, 0, 255); // Cor da nervura
    SDL_RenderDrawLine(game->renderer, leaf.x + leafWidth / 2, leaf.y + leafHeight / 2, leaf.x + leafWidth / 2, leaf.y + leafHeight); 
}

// Renderiza texto na tela
void renderText(Game *game, const char *text, int x, int y, SDL_Color color, TTF_Font *font) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
if (surface == NULL) {
    printf("Erro ao criar superfície de texto: %s\n", TTF_GetError());
    return;  // Ou qualquer outra ação para tratar o erro
}

    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    
    // Calcular posição centralizada com base na largura do texto
    int textWidth = surface->w;
    int textHeight = surface->h;
    SDL_Rect dest = {x - textWidth / 2, y, textWidth, textHeight}; // Centraliza o texto no eixo X
    SDL_RenderCopy(game->renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    
}
void resetGame(Game *game) {
    game->score = 0; 
    game->snakeLength = 1; 
    game->direction = RIGHT; 
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    game->snake[0] = (SDL_Rect){centerX - 15, centerY - 15, 30, 30};
    // Tamanho da maçã
    int appleWidth = 40;
    int appleHeight = 40;
    // Atualizar a posição da maçã
    game->appleRect = (SDL_Rect){rand() % (SCREEN_WIDTH - appleWidth), rand() % (SCREEN_HEIGHT - appleHeight), appleWidth, appleHeight};
    
    // Desenhar a maçã como um círculo vermelho
    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255); // Cor vermelha
    drawCircle(game->renderer, game->appleRect.x + appleWidth / 2, game->appleRect.y + appleHeight / 2, appleWidth / 2);
    game->numRocks = 0;
}
void showEnterNameScreen(Game *game) {
    bool inNameScreen = true;
    SDL_Event e;
    SDL_Color textColor = {255, 255, 255, 255};  // Cor branca para o texto
    SDL_Color borderColor = {255, 255, 255, 255}; // Cor da borda da caixa de entrada
    int maxNameLength = 19;
    char inputName[20] = "";
    bool nameError = false; // Variável para exibir mensagem de erro

    SDL_Rect inputBox = {SCREEN_WIDTH / 2 - 150, 190, 300, 50}; // Caixa de entrada

    // Carregar a spritesheet da cobra animada
    SDL_Texture *cobraTexture = IMG_LoadTexture(game->renderer, "./assets/cobra_spritesheet.png");
    if (!cobraTexture) {
        printf("Erro ao carregar spritesheet da cobra: %s\n", IMG_GetError());
        return;
    }

    // Configuração da animação
    int frameWidth = 150;   // Largura de cada quadro da animação
    int frameHeight = 170;   // Altura do quadro
    int totalFrames = 85;    // Quantidade total de frames na spritesheet
    int currentFrame = 0;   // Frame atual da animação
    int animationSpeed = 100; // Tempo entre frames em milissegundos

    SDL_Rect cobraRect = {SCREEN_WIDTH / 2 - frameWidth / 2, SCREEN_HEIGHT - 300, frameWidth, frameHeight};
    SDL_Rect spriteClip = {0, 0, frameWidth, frameHeight}; // Define a área do frame atual

    Uint32 lastFrameTime = SDL_GetTicks();

    while (inNameScreen) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                game->running = false;
                inNameScreen = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(inputName) > 0) {
                    inputName[strlen(inputName) - 1] = '\0';  
                    nameError = false; // Remove a mensagem de erro ao apagar
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    if (strlen(inputName) > 0) {  // Só aceita "Enter" se houver nome
                        strncpy(game->playerName, inputName, sizeof(game->playerName));
                        inNameScreen = false;  
                    } else {
                        nameError = true; // Ativa a mensagem de erro
                    }
                } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    inNameScreen = false;
                    showStartScreen(game);
                    return;
                } else if (e.key.keysym.sym == SDLK_F12) {  // Alternar modo tela cheia
                    if (SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN) {
                        SDL_SetWindowFullscreen(game->window, 0);  // Sai do modo tela cheia
                    } else {
                        SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN);  // Entra no modo tela cheia
                    }
                } else if (strlen(inputName) < maxNameLength && e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
                    strncat(inputName, (char[]){(char)e.key.keysym.sym, '\0'}, 1);
                    nameError = false; // Remove a mensagem de erro ao digitar
                }
            }
        }

        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
        SDL_RenderClear(game->renderer);

        renderText(game, "Digite seu nome:", SCREEN_WIDTH / 2, 150, textColor, game->menuFont);

        SDL_SetRenderDrawColor(game->renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawRect(game->renderer, &inputBox);

        if (strlen(inputName) > 0) {
            renderText(game, inputName, SCREEN_WIDTH / 2, 200, textColor, game->menuFont);
        } else {
            renderText(game, "Digite aqui...", SCREEN_WIDTH / 2, 200, (SDL_Color){150, 150, 150, 255}, game->menuFont);
        }

        renderText(game, "Pressione Enter para OK, ESC para Cancelar", SCREEN_WIDTH / 2, 260, textColor, game->menuFont);

        // Exibir mensagem de erro se tentar pressionar Enter sem nome
        if (nameError) {
            renderText(game, "Por favor, digite um nome!", SCREEN_WIDTH / 2, 300, (SDL_Color){255, 0, 0, 255}, game->menuFont);
        }

        // Atualizar a animação da cobra
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > lastFrameTime + animationSpeed) {
            currentFrame = (currentFrame + 1) % totalFrames;
            spriteClip.x = currentFrame * frameWidth; // Move para o próximo frame da spritesheet
            lastFrameTime = currentTime;
        }

        // Renderizar o frame atual da animação
        SDL_RenderCopy(game->renderer, cobraTexture, &spriteClip, &cobraRect);

        SDL_RenderPresent(game->renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(cobraTexture);
}




// Função para carregar os recordes do arquivo
void loadRecords(Record records[MAX_RECORDS]) {
    FILE *file = fopen(RECORDS_FILE, "r");
    if (!file) {
        printf("Erro ao carregar os recordes\n");
        return;
    }

    for (int i = 0; i < MAX_RECORDS; i++) {
        if (fscanf(file, "%19s %d\n", records[i].playerName, &records[i].score) != 2) {
            records[i].score = 0; // Se não houver dados suficientes, define a pontuação como 0
        }
    }

    fclose(file);
}



// Função para salvar os recordes no arquivo
void saveRecords(Record records[MAX_RECORDS]) {
    FILE *file = fopen(RECORDS_FILE, "w");
    if (!file) {
        printf("Erro ao salvar os recordes\n");
        return;
    }

    for (int i = 0; i < MAX_RECORDS; i++) {
        fprintf(file, "%s %d\n", records[i].playerName, records[i].score);
    }

    fclose(file);
}
// Função para atualizar o recorde, evitando nomes duplicados
void updateRecords(Record records[MAX_RECORDS], const char *playerName, int score) {
    int existingIndex = -1;
    
    // Verifica se o jogador já está na lista
    for (int i = 0; i < MAX_RECORDS; i++) {
        if (strcmp(records[i].playerName, playerName) == 0) {
            existingIndex = i;
            break;
        }
    }

    // Se o jogador já estiver na lista e a nova pontuação for maior, atualiza
    if (existingIndex != -1) {
        if (score > records[existingIndex].score) {
            printf("Atualizando a pontuação de %s de %d para %d\n", playerName, records[existingIndex].score, score);
            records[existingIndex].score = score;
        } else {
            printf("A nova pontuação de %s (%d) não é maior que a atual (%d)\n", playerName, score, records[existingIndex].score);
            return; // Sai da função sem alterações
        }
    } else {
        // Se for um novo jogador, insere corretamente
        int pos = -1;
        for (int i = 0; i < MAX_RECORDS; i++) {
            if (score > records[i].score) {
                pos = i;
                break;
            }
        }

        // Se não houver espaço para um novo recorde, sai da função
        if (pos == -1) return;

        printf("Inserindo novo recorde: %s com %d pontos\n", playerName, score);

        // Move os recordes para abrir espaço para o novo
        for (int j = MAX_RECORDS - 1; j > pos; j--) {
            records[j] = records[j - 1];
        }

        // Insere o novo recorde na posição correta
        strncpy(records[pos].playerName, playerName, sizeof(records[pos].playerName) - 1);
        records[pos].playerName[sizeof(records[pos].playerName) - 1] = '\0'; // Garante terminação segura
        records[pos].score = score;
    }

    // Reorganiza os recordes para garantir que fiquem em ordem decrescente
    for (int i = 0; i < MAX_RECORDS - 1; i++) {
        for (int j = i + 1; j < MAX_RECORDS; j++) {
            if (records[j].score > records[i].score) {
                Record temp = records[i];
                records[i] = records[j];
                records[j] = temp;
            }
        }
    }

    // Salva os recordes atualizados
    saveRecords(records);
}
void showRecordsScreen(Game *game) {

      // Carregar a fonte Arial somente para a tela de tutorial
      TTF_Font *arialFont = TTF_OpenFont("./fonts/SigmarOne-Regular.ttf", 50);
      if (!arialFont) {
          printf("Erro ao carregar a fonte: %s\n", TTF_GetError());
          return;
      }
    Record records[MAX_RECORDS];
    loadRecords(records);  // Carregar os recordes

    SDL_Color white = {255, 255, 255, 255};  // Branco para os nomes e pontuações
    SDL_Color red = {255, 0, 0, 255};  // Vermelho para "RECORDES"
    SDL_Color borderColor = {255, 255, 255, 255}; // Cor da borda (branco)
    SDL_Event e;
    bool inRecords = true;
    bool isTextVisible = true;  // Controla se o texto está visível ou não
    int blinkTimer = 0;  // Contador de tempo para piscar

    // Variáveis para controlar a rolagem dos recordes
    int scrollOffset = 0;  // Deslocamento de rolagem
    int recordsPerPage = 10;  // Quantidade de recordes por página

    // Ajustar a posição da caixa para baixo
    int boxYPosition = 200;  // A posição Y da caixa de recordes
    SDL_Rect borderRect = {SCREEN_WIDTH / 4, boxYPosition, SCREEN_WIDTH / 2, 400};  // Caixa movida para baixo

    while (inRecords) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                game->running = false;
                inRecords = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    inRecords = false; // Voltar ao menu
                } else if (e.key.keysym.sym == SDLK_UP && scrollOffset > 0) {
                    scrollOffset--;  // Rolar para cima com as setas
                } else if (e.key.keysym.sym == SDLK_DOWN && scrollOffset + recordsPerPage < MAX_RECORDS) {
                    scrollOffset++;  // Rolar para baixo com as setas
                } else if (e.key.keysym.sym == SDLK_w && scrollOffset > 0) {
                    scrollOffset--;  // Rolar para cima com W
                } else if (e.key.keysym.sym == SDLK_s && scrollOffset + recordsPerPage < MAX_RECORDS) {
                    scrollOffset++;  // Rolar para baixo com S
                } else if (e.key.keysym.sym == SDLK_F12) {  // Alternar modo tela cheia
                    if (SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN) {
                        SDL_SetWindowFullscreen(game->window, 0);  // Sai do modo tela cheia
                    } else {
                        SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN);  // Entra no modo tela cheia
                    }
                }
            } else if (e.type == SDL_MOUSEWHEEL) {
                // Verifica se a rolagem da roda do mouse aconteceu dentro da caixa de recordes
                if (e.wheel.y > 0 && scrollOffset > 0) {  // Rolagem para cima
                    scrollOffset--;
                } else if (e.wheel.y < 0 && scrollOffset + recordsPerPage < MAX_RECORDS) {  // Rolagem para baixo
                    scrollOffset++;
                }
            }
        }

        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);  // Cor de fundo preta
        SDL_RenderClear(game->renderer);

        // Renderizar "RECORDES" em vermelho
        renderText(game, "RECORDES", SCREEN_WIDTH / 2, 40, red, arialFont);
        // Desenhando a borda da caixa de recordes
        SDL_SetRenderDrawColor(game->renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawRect(game->renderer, &borderRect);  // Desenha a borda

        // Exibir os recordes com base no offset
        for (int i = 0; i < recordsPerPage; i++) {
            int index = i + scrollOffset;
            if (index < MAX_RECORDS && records[index].score > 0) {  // Exibe apenas os recordes com pontuação
                char recordText[40];
                sprintf(recordText, "%d. %s - %d", index + 1, records[index].playerName, records[index].score);
                renderText(game, recordText, SCREEN_WIDTH / 2, boxYPosition + 30 + i * 30, white, game->menuFont);  // Ajustar posição Y
            }
        }

        // Animação do texto "Voltar [Enter]" piscando
        if (blinkTimer % 4 == 0) {  // A cada 30 ciclos, alterna a visibilidade
            isTextVisible = !isTextVisible;  // Alterna entre visível e invisível
        }

        // Exibir o texto "Voltar [Enter]" piscando
        if (isTextVisible) {
            renderText(game, "Voltar [Enter]", 120, SCREEN_HEIGHT - 40, white, game->menuFont);
        }

        SDL_RenderPresent(game->renderer);
        SDL_Delay(100);

        blinkTimer++;  // Incrementa o timer de animação
    }
}

void showTutorialScreen(Game *game) {
    bool inTutorial = true;
    SDL_Event e;

    // Carregar a fonte Arial somente para a tela de tutorial
    TTF_Font *arialFont = TTF_OpenFont("./fonts/SigmarOne-Regular.ttf", 24);
    if (!arialFont) {
        printf("Erro ao carregar a fonte: %s\n", TTF_GetError());
        return;
    }

    // Texto do tutorial atualizado com F12
    const char *tutorialText[] = {
        "Controles do jogo:",
        "\n",
        "Seta para cima ou W: Move para cima",
        "Seta para baixo ou S: Move para baixo",
        "Seta para esquerda ou A: Move para a esquerda",
        "Seta para direita ou D: Move para a direita",
        "Tecla Enter: Seleciona no menu",
        "Tecla Esc: Sai do jogo",
        "Rolagem do mouse: Move a lista de recordes",
        "Tecla F12: Alterna entre modo tela cheia e modo janela"
    };

    SDL_Color white = {255, 255, 255, 255}; // Branco
    SDL_Color red = {255, 0, 0, 255}; // Vermelho para "TUTORIAL"
    Uint32 lastBlinkTime = SDL_GetTicks();
    bool showBlinkText = true; // Alterna entre verdadeiro e falso para piscar

    while (inTutorial) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                game->running = false;
                inTutorial = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) { // Apenas "Enter" para sair do tutorial
                    inTutorial = false;
                }
            } else if (e.key.keysym.sym == SDLK_F12) {  // Tecla F12 para alternar entre tela cheia e modo janela
                if (SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN) {
                    SDL_SetWindowFullscreen(game->window, 0);  // Sai do modo tela cheia
                } else {
                    SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN);  // Entra no modo tela cheia
                }
            }
        }

        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
        SDL_RenderClear(game->renderer);

        // Renderizar "TUTORIAL" em vermelho
        renderText(game, "TUTORIAL", SCREEN_WIDTH / 2, 50, red, arialFont);

        // Renderizar o restante do texto em branco
        for (int i = 0; i < sizeof(tutorialText) / sizeof(tutorialText[0]); i++) {
            renderText(game, tutorialText[i], SCREEN_WIDTH / 2, 150 + i * 40, white, arialFont);
        }

        // Lógica para piscar o texto
        if (SDL_GetTicks() - lastBlinkTime > 500) { // Alterna a cada 500ms
            showBlinkText = !showBlinkText;
            lastBlinkTime = SDL_GetTicks();
        }

        if (showBlinkText) {
            renderText(game, "Pressione Enter para voltar ao menu", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80, white, arialFont);
        }

        SDL_RenderPresent(game->renderer);
        SDL_Delay(100);
    }

    TTF_CloseFont(arialFont);
}

void showInitialScreen(Game *game) {
    SDL_Texture *background = NULL;
    Mix_Music *music = NULL;
    Mix_Chunk *startSound = NULL;
    bool inStartScreen = true;
    SDL_Event e;
    
    // Carregar a imagem de fundo
    SDL_Surface *imageSurface = IMG_Load("./assets/tela_inicial.png");
    if (!imageSurface) {
        printf("Erro ao carregar imagem inicial: %s\n", IMG_GetError());
        return;
    }
    background = SDL_CreateTextureFromSurface(game->renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    // Carregar e tocar a música de fundo
    music = Mix_LoadMUS("./sounds/inicial.mp3");
    if (!music) {
        printf("Erro ao carregar música inicial: %s\n", Mix_GetError());
    } else {
        Mix_PlayMusic(music, -1);
        Mix_VolumeMusic(50); // Define um volume inicial médio
    }

    // Carregar som do botão de início
    startSound = Mix_LoadWAV("./sounds/start_button.mp3");
    if (!startSound) {
        printf("Erro ao carregar som do botão de início: %s\n", Mix_GetError());
    }

    SDL_Color textColor = {255, 255, 255}; // Cor branca
    bool showText = true; // Controla a animação piscando
    Uint32 lastToggle = SDL_GetTicks();
    
    int alpha = 0; // Opacidade inicial
    const int fadeSpeed = 2; // Velocidade do fade-in e fade-out
    bool fadeOut = false; // Indica quando iniciar o fade-out

    while (inStartScreen) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                game->running = false;
                inStartScreen = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    if (startSound) {
                        Mix_PlayChannel(-1, startSound, 0);
                    }
                    fadeOut = true; // Iniciar o fade-out

                    // Aumenta o volume ao pressionar Enter
                    int currentVolume = Mix_VolumeMusic(-1); // Obtém o volume atual
                    int newVolume = currentVolume + 100; // Aumenta em 20 unidades
                    if (newVolume > 128) newVolume = 128; // Limita ao máximo
                    Mix_VolumeMusic(newVolume);
                } else if (e.key.keysym.sym == SDLK_F12) {  // Tecla F12 para alternar entre tela cheia e modo janela
                    if (SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN) {
                        SDL_SetWindowFullscreen(game->window, 0);  // Sai do modo tela cheia
                    } else {
                        SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN);  // Entra no modo tela cheia
                    }
                }
            }
        }

        // Aplicar efeito de fade-in (se não estiver no fade-out)
        if (!fadeOut && alpha < 255) {
            alpha += fadeSpeed;
            if (alpha > 255) alpha = 255;
        }

        // Aplicar efeito de fade-out
        if (fadeOut) {
            alpha -= fadeSpeed;
            if (alpha <= 0) {
                alpha = 0;
                inStartScreen = false; // Sai do loop quando a tela estiver escura
            }
        }

        SDL_SetTextureAlphaMod(background, alpha);

        // Renderizar a tela inicial
        SDL_RenderClear(game->renderer);
        SDL_RenderCopy(game->renderer, background, NULL, NULL);

        // Alternar a exibição do texto "Começar" piscando
        if (!fadeOut && SDL_GetTicks() - lastToggle > 700) { 
            showText = !showText;
            lastToggle = SDL_GetTicks();
        }

        if (showText && !fadeOut) {
            renderText(game, "Press Start", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100, textColor, game->menuFont);
        }

        SDL_RenderPresent(game->renderer);
        SDL_Delay(20);
    }

    // Parar a música e liberar os recursos
    Mix_HaltMusic();
    Mix_FreeMusic(music);
    Mix_FreeChunk(startSound);
    SDL_DestroyTexture(background);
}
// Função para renderizar um texto com efeito 3D
void renderText3D(Game *game, const char *text, int x, int y, SDL_Color frontColor, SDL_Color shadowColor, int depth) {
    for (int i = depth; i > 0; i--) {
        renderText(game, text, x + i, y + i, shadowColor, game->titleFont); // Sombra deslocada
    }
    renderText(game, text, x, y, frontColor, game->titleFont); // Texto principal
}

void showStartScreen(Game *game) {
    resetGame(game); // Reseta o jogo ao iniciar o menu
    bool inMenu = true;
    SDL_Event e;
    int selectedOption = 0;
    const char *menuOptions[] = {"Iniciar", "Recordes", "Tutorial", "Sair"};
    int totalOptions = 4;

    // Calcular espaço do menu
    int menuHeight = totalOptions * 50;
    int startY = (SCREEN_HEIGHT - menuHeight) / 2 + 50;

    // Carregar música de fundo
    Mix_Music *music = Mix_LoadMUS("./sounds/music.wav");
    if (!music) {
        printf("Erro ao carregar música: %s\n", Mix_GetError());
    } else {
        Mix_PlayMusic(music, -1);
    }

    // Carregar som de clique
    Mix_Chunk *clickSound = Mix_LoadWAV("./sounds/clicar_button.mp3");
    if (!clickSound) {
        printf("Erro ao carregar som de clique: %s\n", Mix_GetError());
    }

    // Definir as cores alternantes (Vermelho, Amarelo e Verde)
    SDL_Color titleColors[] = {{255, 0, 0}, {255, 255, 0}, {0, 255, 0}};
    int colorIndex = 0;
    SDL_Color shadowColor = {50, 50, 50}; // Cor da sombra para efeito 3D

    while (inMenu) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                game->running = false;
                inMenu = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_s) {
                    selectedOption = (selectedOption + 1) % totalOptions;
                } else if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_w) {
                    selectedOption = (selectedOption - 1 + totalOptions) % totalOptions;
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    Mix_PlayChannel(-1, clickSound, 0);
                    if (selectedOption == 0) {
                        showEnterNameScreen(game);
                        inMenu = false;
                    } else if (selectedOption == 1) {
                        showRecordsScreen(game);
                    } else if (selectedOption == 2) {
                        showTutorialScreen(game);
                    } else if (selectedOption == 3) {
                        game->running = false;
                        inMenu = false;
                    }
                } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    game->running = false;
                    inMenu = false;
                } else if (e.key.keysym.sym == SDLK_F12) {
                    if (SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN) {
                        SDL_SetWindowFullscreen(game->window, 0);
                    } else {
                        SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN);
                    }
                }
            }
        }

        // Fundo preto
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
        SDL_RenderClear(game->renderer);

        // Renderizar o título "SNAKE" com efeito 3D
        renderText3D(game, "SNAKE", SCREEN_WIDTH / 2, 50, titleColors[colorIndex], shadowColor, 5);
        colorIndex = (colorIndex + 1) % 3; // Alterna entre Vermelho, Amarelo e Verde

        // Renderizar opções do menu
        for (int i = 0; i < totalOptions; i++) {
            int menuY = startY + i * 70;
            SDL_Color menuColor = (i == selectedOption) ? (SDL_Color){255, 255, 0} : (SDL_Color){200, 200, 200};
            renderText(game, menuOptions[i], SCREEN_WIDTH / 2, menuY, menuColor, game->menuFont);
        }

        SDL_RenderPresent(game->renderer);
        SDL_Delay(200);
    }

    // Liberar os recursos ao sair do menu
    Mix_FreeChunk(clickSound);
    Mix_FreeMusic(music);
}



bool checkCollisions(Game *game) {
    // Verificar colisão com as bordas da tela
    if (game->snake[0].x < 0 || game->snake[0].x + game->snake[0].w > SCREEN_WIDTH || 
        game->snake[0].y < 0 || game->snake[0].y + game->snake[0].h > SCREEN_HEIGHT) {
        return true;
    }
    
    // Verificar colisão com o próprio corpo
    for (int i = 1; i < game->snakeLength; i++) {
        if (SDL_HasIntersection(&game->snake[0], &game->snake[i])) {
            return true;
        }
    }
    
    return false;
}
void drawSnake(Game *game) {
    for (int i = 0; i < game->snakeLength; i++) {
        SDL_Rect snakePart = game->snake[i];
        // int radius = snakePart.w / 2; // Define o raio para criar uma forma mais arredondada
        // Colorir o corpo da cobra com um efeito de gradiente (de verde claro para verde escuro)
        SDL_Color color;
        if (i == 0) { // Cabeça da cobra
            color = (SDL_Color){0, 255, 0, 255};  // Cor verde brilhante para a cabeça
        } else {
            // Gradiente simples de verde claro para verde escuro
            color = (SDL_Color){0, 200 + (i * 10), 0, 255};
        }
        SDL_SetRenderDrawColor(game->renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(game->renderer, &snakePart);  // Desenha o corpo como quadrados arredondados
        
        // Para adicionar um efeito de borda
        SDL_SetRenderDrawColor(game->renderer, 0, 100, 0, 255);  // Cor mais escura para borda
        SDL_RenderDrawRect(game->renderer, &snakePart);  // Adiciona borda ao redor da parte do corpo
    }
}
// Função para desenhar as bordas do jogo
// Função para desenhar as bordas do jogo com uma moldura de pixel azul
void drawBorders(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 255, 255);  // Cor azul para a borda
    
    // Desenhar a borda externa
    for (int i = 0; i < 10; i++) {
        SDL_Rect outerBorder = {i, i, SCREEN_WIDTH - 2 * i, SCREEN_HEIGHT - 2 * i};
        SDL_RenderDrawRect(game->renderer, &outerBorder);
    }
    
    // Desenhar uma borda interna mais fina, com o mesmo tom de azul
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 255, 255);  // Azul claro para borda interna
    SDL_RenderDrawRect(game->renderer, &(SDL_Rect){10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20});
}
void showGameOverScreen(Game *game) {
    bool inGameOver = true;
    SDL_Event e;
    int selectedOption = 0;  // Começa na primeira opção
    const char *menuOptions[] = {"Novamente", "Casa", "Sair"};
    int totalOptions = 3;
    int menuHeight = totalOptions * 90;
    int startY = (SCREEN_HEIGHT - menuHeight) / 2;

    Mix_Chunk *gameOverSound = Mix_LoadWAV("./sounds/som_perdedor.mp3");
    if (!gameOverSound) {
        printf("Erro ao carregar som de game over: %s\n", Mix_GetError());
    } else {
        Mix_PlayChannel(-1, gameOverSound, 0);
    }

    // Carregar som de clique
    Mix_Chunk *clickSound = Mix_LoadWAV("./sounds/clicar_button.mp3");
    if (!clickSound) {
        printf("Erro ao carregar som de clique: %s\n", Mix_GetError());
    }

    Record records[MAX_RECORDS];
    loadRecords(records);
    updateRecords(records, game->playerName, game->score);

    int scrollOffset = 0;
    const int maxVisibleRecords = 5;

    while (inGameOver) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                game->running = false;
                inGameOver = false;
            } else if (e.type == SDL_KEYDOWN) {
                // Verificar as teclas de navegação (setas e W/S)
                if (e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_s) {  // Descer
                    selectedOption = (selectedOption + 1) % totalOptions;
                } else if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_w) {  // Subir
                    selectedOption = (selectedOption - 1 + totalOptions) % totalOptions;
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    // Tocar o som de clique ao selecionar uma opção
                    Mix_PlayChannel(-1, clickSound, 0);

                    if (selectedOption == 0) {
                        inGameOver = false;
                        showGameScreen(game);
                    } else if (selectedOption == 1) {
                        inGameOver = false;
                        showStartScreen(game);
                    } else if (selectedOption == 2) {
                        game->running = false;
                        inGameOver = false;
                    }
                } else if (e.key.keysym.sym == SDLK_F12) {  // Tecla F12 para alternar entre tela cheia e modo janela
                    if (SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN) {
                        SDL_SetWindowFullscreen(game->window, 0);  // Sai do modo tela cheia
                    } else {
                        SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN);  // Entra no modo tela cheia
                    }
                }
            } else if (e.type == SDL_MOUSEWHEEL) {
                if (e.wheel.y > 0 && scrollOffset > 0) {
                    scrollOffset--;
                } else if (e.wheel.y < 0 && scrollOffset + maxVisibleRecords < MAX_RECORDS) {
                    scrollOffset++;
                }
            }
        }

        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
        SDL_RenderClear(game->renderer);
        SDL_Color red = {255, 0, 0};
        renderText(game, "GAME OVER", SCREEN_WIDTH / 2, 30, red, game->titleFont);

        for (int i = 0; i < totalOptions; i++) {
            SDL_Color color = (i == selectedOption) ? (SDL_Color){255, 255, 0} : (SDL_Color){200, 200, 200};
            renderText(game, menuOptions[i], SCREEN_WIDTH / 2, startY + (i * 50), color, game->menuFont);
        }

        int recordBoxWidth = 400;
        int recordBoxHeight = 250;
        int recordBoxX = SCREEN_WIDTH / 2 - recordBoxWidth / 2;
        int recordBoxY = 380;
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
        SDL_Rect recordBox = { recordBoxX, recordBoxY, recordBoxWidth, recordBoxHeight };
        SDL_RenderDrawRect(game->renderer, &recordBox);
        renderText(game, "Recordes:", SCREEN_WIDTH / 2, recordBoxY + 5, (SDL_Color){255, 0, 0}, game->menuFont);

        for (int i = 0; i < maxVisibleRecords && i + scrollOffset < MAX_RECORDS; i++) {
            char recordText[100];
            snprintf(recordText, sizeof(recordText), "%d. %s - %d", i + 1 + scrollOffset, records[i + scrollOffset].playerName, records[i + scrollOffset].score);
            renderText(game, recordText, SCREEN_WIDTH / 2, recordBoxY + 43 + i * 40, (SDL_Color){255, 255, 255}, game->menuFont);
        }

        SDL_RenderPresent(game->renderer);
        SDL_Delay(150);
    }

    // Liberar os recursos
    Mix_FreeChunk(gameOverSound);
    Mix_FreeChunk(clickSound);
}


// Função para mudar a cor do fundo com base no nível
void setBackgroundColor(Game *game) {
    if (game->level >= 1 && game->level <= 5) {
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);  // Preto
    } else if (game->level >= 6 && game->level <= 10) {
        SDL_SetRenderDrawColor(game->renderer, 0, 255, 255, 255);  // Ciano
    } else if (game->level >= 11 && game->level <= 15) {
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 0, 255);  // Amarelo
    } else if (game->level >= 16 && game->level <= 20) {
        SDL_SetRenderDrawColor(game->renderer, 164, 234, 79, 255);  // #A4EA4F
    } else if (game->level >= 21 && game->level <= 25) {
        SDL_SetRenderDrawColor(game->renderer, 255, 4, 0, 255);  // Vermelho
    } else if (game->level >= 26 && game->level <= 30) {
        SDL_SetRenderDrawColor(game->renderer, 180, 113, 80, 255);  // #b47150
    } else if (game->level >= 31 && game->level <= 35) {
        SDL_SetRenderDrawColor(game->renderer, 128, 0, 32, 255);  // Bordô
    } else {
        // Para níveis mais altos, alterna entre preto e bordô
        if ((game->level / 5) % 2 == 0) {
            SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);  // Preto
        } else {
            SDL_SetRenderDrawColor(game->renderer, 128, 0, 32, 255);  // Bordô
        }
    }
}


// Função para carregar música de fundo com base no nível (já fornecida)
void setBackgroundMusic(Game *game) {
    Mix_Music *backgroundMusic = NULL;

    if (game->level >= 1 && game->level <= 5) {
        backgroundMusic = Mix_LoadMUS("./sounds/level1_5.mp3");
    } else if (game->level >= 6 && game->level <= 10) {
        backgroundMusic = Mix_LoadMUS("./sounds/level6_10.mp3");
    } else if (game->level >= 11 && game->level <= 15) {
        backgroundMusic = Mix_LoadMUS("./sounds/level11_15.mp3");
    } else if (game->level >= 16 && game->level <= 20) {
        backgroundMusic = Mix_LoadMUS("./sounds/level16_20.mp3");
    } else if (game->level >= 21 && game->level <= 25) {
        backgroundMusic = Mix_LoadMUS("./sounds/level21_25.mp3");
    } else if (game->level >= 26 && game->level <= 30) {
        backgroundMusic = Mix_LoadMUS("./sounds/level26_30.mp3");
    } else if (game->level >= 31 && game->level <= 100) {
        backgroundMusic = Mix_LoadMUS("./sounds/level31_100.mp3");
    }

    // Certifique-se de que a música carregada é tocada corretamente
    if (backgroundMusic) {
        // Verifique se a música já está tocando, caso contrário, inicie a música
        if (Mix_PlayingMusic() == 0) {
            Mix_PlayMusic(backgroundMusic, -1);  // -1 para tocar em loop
        }
    } else {
        printf("Erro ao carregar música: %s\n", Mix_GetError());
    }
}
void showGameScreen(Game *game) {
    // Certifique-se de que o nível é redefinido corretamente ao reiniciar o jogo
    game->level = (game->score / 200) + 1;  // Cada 200 pontos aumenta um nível
    
    // Carregar música e cor de fundo de acordo com o nível
    setBackgroundMusic(game);
    setBackgroundColor(game);  // Chama a função para mudar a cor do fundo

    resetGame(game);  // Limpa os dados do jogo sempre que começar um novo
    Uint32 lastTime = SDL_GetTicks();
    const float BASE_SPEED = 0.5;  // Velocidade inicial
    float speedMultiplier = BASE_SPEED;

    // Configura a cobra e maçã
    game->snakeLength = 1;
    game->level = 1;  // Começa no nível 1
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    game->snake[0] = (SDL_Rect){centerX - 15, centerY - 15, 30, 30};
    game->apple = (SDL_Rect){rand() % (SCREEN_WIDTH - 20), rand() % (SCREEN_HEIGHT - 20), 20, 20};

    game->direction = RIGHT;
    SDL_Event e;

    while (game->running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                game->running = false;
            } else if (e.type == SDL_KEYDOWN) {
                // Usando W, S, A, D ou UP, DOWN, LEFT, RIGHT para mover
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                    case SDLK_w:  // Usando W
                        if (game->direction != DOWN) game->direction = UP;
                        break;
                    case SDLK_DOWN:
                    case SDLK_s:  // Usando S
                        if (game->direction != UP) game->direction = DOWN;
                        break;
                    case SDLK_LEFT:
                    case SDLK_a:  // Usando A
                        if (game->direction != RIGHT) game->direction = LEFT;
                        break;
                    case SDLK_RIGHT:
                    case SDLK_d:  // Usando D
                        if (game->direction != LEFT) game->direction = RIGHT;
                        break;
                    case SDLK_ESCAPE:
                        game->running = false;
                        break;
                    case SDLK_F12:  // Tecla F12 para alternar entre tela cheia e modo janela
                        if (SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN) {
                            SDL_SetWindowFullscreen(game->window, 0);  // Sai do modo tela cheia
                        } else {
                            SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN);  // Entra no modo tela cheia
                        }
                        break;
                }
            }
        }

        // Atualizar nível e velocidade
        game->level = (game->score / 200) + 1;  // Cada 200 pontos aumenta um nível
        speedMultiplier = BASE_SPEED + (game->level - 1) * 00.1;  // Aumenta 0.1x por nível

        // Atualiza a cor de fundo com base no novo nível
        setBackgroundColor(game); 

        Uint32 frameDelay = (Uint32)(1000 / (15 * speedMultiplier));

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastTime >= frameDelay) {
            lastTime = currentTime;
            updateSnake(game);

            if (checkCollisions(game)) {
                Mix_HaltMusic();
                printf("Fim de jogo! Colisão detectada.\n");
                SDL_Delay(500);
                showGameOverScreen(game);
                break;
            }

            checkAppleCollision(game);
            SDL_RenderClear(game->renderer);  // Limpa a tela com a nova cor de fundo
            drawBorders(game);
            drawSnake(game);

            // Desenhar maçã
            SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);
            drawCircle(game->renderer, game->apple.x + game->apple.w / 2, game->apple.y + game->apple.h / 2, game->apple.w / 2);
            SDL_SetRenderDrawColor(game->renderer, 200, 0, 0, 255);
            drawCircle(game->renderer, game->apple.x + game->apple.w / 2, game->apple.y + game->apple.h / 2, game->apple.w / 2 + 3);

            // Folha da maçã
            SDL_Rect leaf = {game->apple.x + game->apple.w / 2 - 12, game->apple.y - 12, 25, 12};
            SDL_SetRenderDrawColor(game->renderer, 34, 139, 34, 255);
            SDL_RenderFillRect(game->renderer, &leaf);
            SDL_SetRenderDrawColor(game->renderer, 0, 100, 0, 255);
            SDL_RenderDrawLine(game->renderer, leaf.x + 12, leaf.y + 6, leaf.x + 12, leaf.y + 12);

            // Exibir score, nível e velocidade
            char infoText[100];
            sprintf(infoText, "Scores: %d       |       Level: %d       |       Speed: %.1fx", game->score, game->level, speedMultiplier);

            // Calcular a largura do texto
            int textWidth, textHeight;
            TTF_SizeText(game->menuFont, infoText, &textWidth, &textHeight);

            // Ajustar a posição para mover o texto para a direita
            int textX = (SCREEN_WIDTH - textWidth) / 2 + 280;  // Adicionar 150 para mover para a direita
            int textY = 10;  // Posição vertical fixa (no topo)

            renderText(game, infoText, textX, textY, (SDL_Color){255, 255, 255}, game->menuFont);

            SDL_RenderPresent(game->renderer);
        }
    }

    Mix_HaltMusic();
}




    void cleanUp(Game *game) {
        if (game->eatSound) {
            Mix_FreeChunk(game->eatSound);
        }
        Mix_CloseAudio();
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
    
    void updateSnake(Game *game) {
        // Desloca cada parte da cobra para a posição do segmento anterior
        for (int i = game->snakeLength - 1; i > 0; i--) {
            game->snake[i] = game->snake[i - 1];
        }
        
        // Atualiza a cabeça da cobra com base na direção
        if (game->direction == UP) {
            game->snake[0].y -= 30; // Movimentação ajustada para 30 pixels
        } else if (game->direction == DOWN) {
            game->snake[0].y += 30;
        } else if (game->direction == LEFT) {
            game->snake[0].x -= 30;
        } else if (game->direction == RIGHT) {
            game->snake[0].x += 30;
        }
        
        // A cobra só tem 1 bloco ou 15 blocos, e nunca mais do que isso
        if (game->snakeLength > 20) {
            game->snakeLength = 1;  // Redefine a cobra para 1 bloco, após atingir 15
        }
    }
    
void checkAppleCollision(Game *game) {
    if (SDL_HasIntersection(&game->snake[0], &game->apple)) {
        game->score += 50;
        
        // Se a cobra não tem 15 blocos, ela vai crescer
        if (game->snakeLength < 15) {
            game->snakeLength++;  // Aumenta o tamanho da cobra
        } else {
            // A cobra já tem 15 blocos, ela "volta" para um bloco
            game->snakeLength = 1;  // A cobra tem apenas um bloco após atingir 15
        }
        
        // Gera uma nova maçã
        game->apple = (SDL_Rect){rand() % (SCREEN_WIDTH - 20), rand() % (SCREEN_HEIGHT - 20), 20, 20};
        
        // Reproduz o som de comer
        Mix_PlayChannel(-1, game->eatSound, 0);
    }
}

// Loop principal do jogo
void gameLoop(Game *game) {
    while (game->running) {
        showGameScreen(game);
    }
}
// Libera os recursos alocados
void closeSDL(Game *game, SDL_Texture *appleTexture) {
    if (appleTexture) {
        SDL_DestroyTexture(appleTexture);
    }
    TTF_CloseFont(game->titleFont);
    TTF_CloseFont(game->menuFont);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
int main() {
    
    Game game = {0};
    game.running = true;
    game.score = 0;
    game.level = 1;
    
    if (!initSDL(&game)) {
        return 1;
    }
    showInitialScreen(&game);  // <- Chamada para a tela inicial
    showStartScreen(&game);  // Exibe a tela inicial
    gameLoop(&game);         // Inicia o loop de jogo
    closeSDL(&game, appleTexture);
    
    return 0;
} 