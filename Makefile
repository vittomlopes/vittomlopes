# Variáveis de compilação
CC=gcc
AS=nasm
CFLAGS=-Wall -g
ASFLAGS=-f elf64

# Variáveis específicas para Windows (MinGW)
WIN_CC=x86_64-w64-mingw32-gcc
WIN_LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

# Variáveis específicas para Linux
LINUX_LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

# Diretórios
SRC_DIR=src
OBJ_DIR=obj
OUT=game

# Arquivos de objetos
OBJ=$(OBJ_DIR)/menu_selection.o $(OBJ_DIR)/snake_game.o

# Verificação do sistema operacional
ifeq ($(OS),Windows_NT)
    # Para Windows, usa o MinGW
    CC=$(WIN_CC)
    LDFLAGS=$(WIN_LDFLAGS)
else
    # Para Linux, usa o GCC
    LDFLAGS=$(LINUX_LDFLAGS)
endif

# Alvo principal
$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $(OUT) $(LDFLAGS)

# Compilação dos arquivos Assembly
$(OBJ_DIR)/menu_selection.o: $(SRC_DIR)/menu_selection.asm
	$(AS) $(ASFLAGS) $(SRC_DIR)/menu_selection.asm -o $(OBJ_DIR)/menu_selection.o

# Compilação do arquivo C
$(OBJ_DIR)/snake_game.o: $(SRC_DIR)/snake_game.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/snake_game.c -o $(OBJ_DIR)/snake_game.o

# Limpeza dos arquivos de objetos e executáveis
clean:
	rm -f $(OBJ_DIR)/*.o $(OUT)

# Compilação automática de dependências
depend:
	$(CC) -M $(SRC_DIR)/*.c > .depend

# Incluir dependências automáticas
-include .depend
