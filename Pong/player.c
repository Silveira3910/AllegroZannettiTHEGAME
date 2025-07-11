#include "game.h"
#include "player.h"
#include "paredes.h"
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

// Definicoes de player
#define PLAYER_SIZE 32.0f
#define PLAYER_SPEED 3.0f

void player_init(Player *p) {
    if (!p) return;

    p->largura = PLAYER_SIZE;
    p->altura = PLAYER_SIZE;
    p->speed = PLAYER_SPEED;
    p->x = (LARGURA_MAPA - PLAYER_SIZE) / 2.0f;
    p->y = (ALTURA_MAPA - PLAYER_SIZE) / 2.0f;
}

void player_update(Player *p, const bool keys[]) {
    if (!p || !keys) return;

    float novo_x = p->x;
    float novo_y = p->y;

    // Movimentacao com base nas teclas pressionadas
    if (keys[KEY_W]) novo_y -= p->speed;
    if (keys[KEY_S]) novo_y += p->speed;
    if (keys[KEY_A]) novo_x -= p->speed;
    if (keys[KEY_D]) novo_x += p->speed;

    // Checa colisao nas direcoes separadas
    if (!paredes_colisao(novo_x, p->y, p->largura, p->altura))
        p->x = novo_x;

    if (!paredes_colisao(p->x, novo_y, p->largura, p->altura))
        p->y = novo_y;
}

void player_draw(const Player *p, float camera_x, float camera_y) {
    if (!p) return;

    float centro_x = p->x + p->largura / 2.0f - camera_x;
    float centro_y = p->y + p->altura / 2.0f - camera_y;
    float raio = p->largura / 2.0f; // Usando largura como base para o raio

    al_draw_filled_circle(centro_x, centro_y, raio, al_map_rgb(0, 255, 0)); // Jogador verde
}

void player_destroy() {
}
