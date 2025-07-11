#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include "player.h"

#define MAX_ITENS 5        // Total de itens no jogo
#define ITEM_SIZE 20.0f
#define DISTANCIA_MIN_GERACAO 200.0f // Distancia minima para gerar o proximo item

typedef struct {
    float x, y;
    bool ativo;
} Item;


void itens_init();
void itens_update(Player *player);
void itens_draw(float camera_x, float camera_y);
int itens_get_coletados();
void itens_reset();


void itens_gerar_proximo(float ultima_x, float ultima_y);

#endif
