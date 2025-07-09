#ifndef PAREDES_H
#define PAREDES_H

#include <stdbool.h>
#include "player.h"

#define MAPA_COLUNAS 160
#define MAPA_LINHAS 60
#define TAM_BLOCO 20

extern const char mapa[MAPA_LINHAS][MAPA_COLUNAS + 1];

void paredes_init();
void paredes_draw(float camera_x, float camera_y);
bool paredes_colisao(float x, float y, float largura, float altura);
void paredes_destroy();

// Nova função para posicionar jogador no ponto 'P'
void player_posicionar_no_ponto_p(Player *player);

#endif
