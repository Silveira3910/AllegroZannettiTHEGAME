#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include "player.h"

#define MAX_ITENS 5        // Total de itens no jogo
#define ITEM_SIZE 20.0f    // Tamanho do item
#define DISTANCIA_MIN_GERACAO 200.0f // Distancia minima para gerar o proximo item

typedef struct {
    float x, y;
    bool ativo; // Indica se o item esta ativo no mapa
} Item;

// Funcoes publicas para o modulo de itens
void itens_init();
void itens_update(Player *player);
void itens_draw(float camera_x, float camera_y);
int itens_get_coletados(); // Retorna o numero de itens coletados
void itens_reset(); // Reseta o estado dos itens para um novo jogo

// funcao para gerar o proximo item no mapa
void itens_gerar_proximo(float ultima_x, float ultima_y);

#endif
