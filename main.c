#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#define MAX_SPEED 10

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    W,
    S,
    A,
    D,
    SPEED_UP,
    SPEED_DOWN,
    RESET,
    LIB1,
    LIB2,
    LIB3,
    QUIT
} Event;

typedef struct {
    int x, y;
} Vec2;

typedef struct {
    int speed;
    Vec2 directions[2]; // Two directions for each player, example: [0] for player 1, [1] for player 2
} GameData;

typedef struct {
    int libType;
} Lib;

// Simulating a function to get events (in place of SFML or other event systems in C++)
Event getEvent() {
    // This would be replaced by actual event fetching code in C
    return UP;  // Example: Always returns UP
}

void changeDirection(GameData* data, int player, Vec2 direction) {
    if (player >= 0 && player < 2) {
        data->directions[player] = direction;
    }
}

void reset(GameData* data) {
    data->speed = 1;
    memset(data->directions, 0, sizeof(data->directions));
}

void update(GameData* data) {
    // Simulate updating the game state
    printf("Game Updated. Speed: %d\n", data->speed);
}

void draw(GameData* data) {
    // Simulate drawing the game state
    printf("Drawing game with speed: %d\n", data->speed);
}

void switchLib(Lib* lib, int newLibType, GameData* data) {
    (void)data;  // Evita warning de parâmetro não usado
    lib->libType = newLibType;
    printf("Switched to Lib %d\n", lib->libType);
}

uint64_t getMicroseconds() {
    // Simulate getting current time in microseconds
    return (uint64_t)rand();  // Apenas um exemplo
}

int main(int ac, char **av) {
    (void)ac;  // Evita warning de parâmetro não usado
    (void)av;

    GameData data = {1, {{0, 0}, {0, 0}}};  // Inicializa data com valores padrão
    Lib lib = {1};  // Inicializa lib com LIB1 como padrão
    uint64_t nextUpdate = 0;

    while (1) {
        Event event = getEvent();
        switch (event) {
            case UP:
                changeDirection(&data, 0, (Vec2){0, -1});
                break;
            case DOWN:
                changeDirection(&data, 0, (Vec2){0, 1});
                break;
            case LEFT:
                changeDirection(&data, 0, (Vec2){-1, 0});
                break;
            case RIGHT:
                changeDirection(&data, 0, (Vec2){1, 0});
                break;
            case W:
                changeDirection(&data, 1, (Vec2){0, -1});
                break;
            case S:
                changeDirection(&data, 1, (Vec2){0, 1});
                break;
            case A:
                changeDirection(&data, 1, (Vec2){-1, 0});
                break;
            case D:
                changeDirection(&data, 1, (Vec2){1, 0});
                break;
            case SPEED_UP:
                if (data.speed < MAX_SPEED) {
                    data.speed++;
                }
                break;
            case SPEED_DOWN:
                if (data.speed > 1) {
                    data.speed--;
                }
                break;
            case RESET:
                reset(&data);
                break;
            case LIB1:
                switchLib(&lib, 1, &data);
                break;
            case LIB2:
                switchLib(&lib, 2, &data);
                break;
            case LIB3:
                switchLib(&lib, 3, &data);
                break;
            case QUIT:
                return 0;
        }

        uint64_t now = getMicroseconds();
        if (now > nextUpdate) {
            update(&data);
            nextUpdate = now + (1000000 / data.speed) - (now - nextUpdate);
            nextUpdate = fmax(nextUpdate, now);
        }
        draw(&data);
    }

    return 0;
}
