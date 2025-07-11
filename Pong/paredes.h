#ifndef PAREDES_H
#define PAREDES_H

#include <stdbool.h>
#include "player.h"

typedef struct {
    float x, y, largura, altura;
} OBSTACLE;

void paredes_init();
void paredes_draw(float camera_x, float camera_y);
bool paredes_colisao(float x, float y, float largura, float altura);
void paredes_destroy();
void player_posicionar_no_ponto_p(Player *player);

#endif
