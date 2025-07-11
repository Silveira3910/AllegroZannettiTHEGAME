#include "item.h"
#include "game.h"
#include "paredes.h"
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

static Item current_item;
static int itens_coletados_count;
static int itens_gerados_count;


static void gerar_posicao_item_distante(Item *item, float ultima_x, float ultima_y) {
    float x, y;
    int tentativas = 0;
    float dist_sq;


    bool check_distance = (itens_gerados_count > 0);

    do {
        x = (float)(rand() % (LARGURA_MAPA - (int)ITEM_SIZE));
        y = (float)(rand() % (ALTURA_MAPA - (int)ITEM_SIZE));
        tentativas++;

        if (check_distance) {
            float dx = x - ultima_x;
            float dy = y - ultima_y;
            dist_sq = (dx * dx) + (dy * dy);
        } else {

            dist_sq = DISTANCIA_MIN_GERACAO * DISTANCIA_MIN_GERACAO + 1;
        }

    } while ((paredes_colisao(x, y, ITEM_SIZE, ITEM_SIZE) ||
              (check_distance && dist_sq < (DISTANCIA_MIN_GERACAO * DISTANCIA_MIN_GERACAO))) &&
             tentativas < 500);

    if (tentativas >= 500) {
        fprintf(stderr, "Aviso: Nao foi possivel gerar item distante o suficiente. Gerando em qualquer lugar valido.\n");
        do {
             x = (float)(rand() % (LARGURA_MAPA - (int)ITEM_SIZE));
             y = (float)(rand() % (ALTURA_MAPA - (int)ITEM_SIZE));
        } while (paredes_colisao(x, y, ITEM_SIZE, ITEM_SIZE));
    }


    item->x = x;
    item->y = y;
}

void itens_init() {
    itens_coletados_count = 0;
    itens_gerados_count = 0;
    current_item.ativo = false; // Nenhum item ativo no inicio
}


void itens_gerar_proximo(float ultima_x, float ultima_y) {
    if (itens_gerados_count < MAX_ITENS) {
        current_item.ativo = true;
        gerar_posicao_item_distante(&current_item, ultima_x, ultima_y);
        itens_gerados_count++;
    } else {
        current_item.ativo = false;
    }
}

void itens_update(Player *player) {

    if (current_item.ativo) {
        //  colisao entre o player e o item (colisao de retangulos)
        if (player->x < current_item.x + ITEM_SIZE &&
            player->x + player->largura > current_item.x &&
            player->y < current_item.y + ITEM_SIZE &&
            player->y + player->altura > current_item.y)
        {
            current_item.ativo = false;
            itens_coletados_count++;



            if (itens_coletados_count < MAX_ITENS) {
                itens_gerar_proximo(current_item.x, current_item.y);
            }
        }
    }
}

void itens_draw(float camera_x, float camera_y) {
    if (current_item.ativo) {
        al_draw_filled_rectangle(current_item.x - camera_x,
                                 current_item.y - camera_y,
                                 current_item.x + ITEM_SIZE - camera_x,
                                 current_item.y + ITEM_SIZE - camera_y,
                                 al_map_rgb(255, 255, 0)); // Amarelo
    }
}

int itens_get_coletados() {
    return itens_coletados_count;
}

void itens_reset() {
    itens_coletados_count = 0;
    itens_gerados_count = 0;
    current_item.ativo = false; // Garante que nao haja item ativo antes de gerar o primeiro


    itens_gerar_proximo(0.0f, 0.0f);
}
