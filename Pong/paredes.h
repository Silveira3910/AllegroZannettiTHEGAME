#ifndef PAREDES_H
#define PAREDES_H

#include <stdbool.h>
#include "player.h"

typedef struct {
    float x, y;
    float caixalargura, caixaaltura;
} OBSTACLE;

extern OBSTACLE obstaculos[];
extern const int NUM_OBSTACULOS;

bool colisao(float x1, float y1, float w1, float h1,
             float x2, float y2, float w2, float h2);

void paredes_init();
void paredes_draw(float camera_x, float camera_y);
void paredes_destroy();


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
