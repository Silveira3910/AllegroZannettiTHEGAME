#ifndef FANTASMA_H
#define FANTASMA_H

#include <stdbool.h>
#include "player.h"

typedef struct {
    float x, y;
    float dx, dy;
    float tempo_direcao;
    float tempo_exposto;
    bool ativo;
} Fantasma;

void fantasmas_init();
void fantasmas_update(const Player *player);
void fantasmas_draw(float camera_x, float camera_y);
bool fantasmas_check_collision(const Player *player);
void fantasmas_destroy();

#endif
