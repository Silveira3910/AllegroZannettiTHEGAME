#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#define PLAYER_SIZE 32.0f
#define PLAYER_SPEED 3.0f

// Definição da struct Player
typedef struct {
    float x, y;
    float largura, altura;
    float speed;
} Player;

// Prototipos das funcoes (declaracoes)
void player_init(Player *p);
void player_update(Player *p, const bool keys[]);
void player_draw(const Player *p, float camera_x, float camera_y);
void player_destroy();

#endif
