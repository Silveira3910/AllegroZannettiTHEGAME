#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 90.0f

#define TAM_BLOCO 20
#define MAPA_COLUNAS 160
#define MAPA_LINHAS 60
#define LARGURA_MAPA (MAPA_COLUNAS * TAM_BLOCO)
#define ALTURA_MAPA (MAPA_LINHAS * TAM_BLOCO)

#define CAMERA_MARGIN_X (LARGURA_TELA / 4.0f)
#define CAMERA_MARGIN_Y (ALTURA_TELA / 4.0f)

#define MAX_FANTASMAS 12

enum KEYS {
    KEY_W, KEY_A, KEY_S, KEY_D,
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_R,
    NUM_KEYS
};

extern float mouse_x;
extern float mouse_y;
extern float camera_x;
extern float camera_y;

#endif
