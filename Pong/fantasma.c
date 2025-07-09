#include "fantasma.h"
#include "game.h"
#include "paredes.h"
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_FANTASMAS 12
#define FANTASMA_SIZE 32.0f
#define TEMPO_EXPOSICAO_LUZ 3.0f
#define RAIO_LUZ 150.0f
#define ANGULO_LUZ (ALLEGRO_PI / 4.0f)

static Fantasma fantasmas[MAX_FANTASMAS];
static float tempo_gerar_fantasma = 0.0f;

void fantasmas_init() {
    srand((unsigned)time(NULL));
    for (int i = 0; i < MAX_FANTASMAS; i++) {
        fantasmas[i].ativo = false;
    }
}

static void gerar_fantasma() {
    for (int i = 0; i < MAX_FANTASMAS; i++) {
        if (!fantasmas[i].ativo) {
            float x, y;
            int tentativas = 0;
            do {
                x = (float)(rand() % (LARGURA_MAPA - (int)FANTASMA_SIZE));
                y = (float)(rand() % (ALTURA_MAPA - (int)FANTASMA_SIZE));
                tentativas++;
            } while (paredes_colisao(x, y, FANTASMA_SIZE, FANTASMA_SIZE) && tentativas < 10);

            fantasmas[i].x = x;
            fantasmas[i].y = y;
            fantasmas[i].dx = 0;
            fantasmas[i].dy = 0;
            fantasmas[i].tempo_direcao = 0;
            fantasmas[i].tempo_exposto = 0;
            fantasmas[i].ativo = true;
            break;
        }
    }
}

void fantasmas_update(const Player *player) {
    tempo_gerar_fantasma += 1.0f / FPS;
    if (tempo_gerar_fantasma >= 2.0f) {
        tempo_gerar_fantasma = 0.0f;
        gerar_fantasma();
    }

    float luz_cx = player->x + player->largura / 2.0f;
    float luz_cy = player->y + player->altura / 2.0f;

    float delta_x = mouse_x - (luz_cx - camera_x);
    float delta_y = mouse_y - (luz_cy - camera_y);
    float angulo_lanterna = atan2f(delta_y, delta_x);

    for (int i = 0; i < MAX_FANTASMAS; i++) {
        if (!fantasmas[i].ativo) continue;

        fantasmas[i].tempo_direcao -= 1.0f / FPS;
        if (fantasmas[i].tempo_direcao <= 0) {
            float ang = (float)(rand() % 628) / 100.0f;
            float speed = 1.0f + (rand() % 100) / 100.0f;
            fantasmas[i].dx = cosf(ang) * speed;
            fantasmas[i].dy = sinf(ang) * speed;
            fantasmas[i].tempo_direcao = 2.0f + (rand() % 300) / 100.0f;
        }

        float novo_x = fantasmas[i].x + fantasmas[i].dx;
        float novo_y = fantasmas[i].y + fantasmas[i].dy;

        if (!paredes_colisao(novo_x, fantasmas[i].y, FANTASMA_SIZE, FANTASMA_SIZE)) {
            fantasmas[i].x = novo_x;
        }
        if (!paredes_colisao(fantasmas[i].x, novo_y, FANTASMA_SIZE, FANTASMA_SIZE)) {
            fantasmas[i].y = novo_y;
        }

        float dx = player->x - fantasmas[i].x;
        float dy = player->y - fantasmas[i].y;
        float distancia = sqrtf(dx * dx + dy * dy);

        if (distancia < 200) {
            float ang = atan2f(dy, dx);
            fantasmas[i].dx = cosf(ang) * 1.5f;
            fantasmas[i].dy = sinf(ang) * 1.5f;
        }

        float fx = fantasmas[i].x + FANTASMA_SIZE / 2.0f;
        float fy = fantasmas[i].y + FANTASMA_SIZE / 2.0f;
        float dx_luz = fx - luz_cx;
        float dy_luz = fy - luz_cy;
        float dist_luz = sqrtf(dx_luz * dx_luz + dy_luz * dy_luz);
        float ang_fantasma = atan2f(dy_luz, dx_luz);
        float diff_ang = fabsf(ang_fantasma - angulo_lanterna);

        if (diff_ang > ALLEGRO_PI) {
            diff_ang = 2 * ALLEGRO_PI - diff_ang;
        }

        if (dist_luz < RAIO_LUZ && diff_ang < ANGULO_LUZ / 2.0f) {
            fantasmas[i].tempo_exposto += 1.0f / FPS;
        } else {
            fantasmas[i].tempo_exposto = 0.0f;
        }

        if (fantasmas[i].tempo_exposto >= TEMPO_EXPOSICAO_LUZ) {
            fantasmas[i].ativo = false;
        }
    }
}

bool fantasmas_check_collision(const Player *player) {
    for (int i = 0; i < MAX_FANTASMAS; i++) {
        if (!fantasmas[i].ativo) continue;

        float dx = (fantasmas[i].x + FANTASMA_SIZE / 2) - (player->x + player->largura / 2);
        float dy = (fantasmas[i].y + FANTASMA_SIZE / 2) - (player->y + player->altura / 2);
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < FANTASMA_SIZE) {
            return true;
        }
    }
    return false;
}

void fantasmas_draw(float camera_x, float camera_y) {
    for (int i = 0; i < MAX_FANTASMAS; i++) {
        if (!fantasmas[i].ativo) continue;

        al_draw_filled_circle(
            fantasmas[i].x + FANTASMA_SIZE / 2 - camera_x,
            fantasmas[i].y + FANTASMA_SIZE / 2 - camera_y,
            FANTASMA_SIZE / 2,
            al_map_rgb(255, 0, 0));
    }
}

void fantasmas_destroy() {
    // Recursos de som/imagem podem ser liberados aqui futuramente
}
