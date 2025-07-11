#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h> // Para ALLEGRO_SAMPLE, ALLEGRO_SAMPLE_INSTANCE
#include <allegro5/allegro_acodec.h> // Para suporte a codecs de audio como OGG, WAV

#include <stdbool.h>

// Definicoes de dimensoes do mapa e bloco
#define TAM_BLOCO 20
#define MAPA_COLUNAS 160
#define MAPA_LINHAS 60
#define LARGURA_MAPA (MAPA_COLUNAS * TAM_BLOCO)
#define ALTURA_MAPA (MAPA_LINHAS * TAM_BLOCO)

// Dimensoes da tela
#define LARGURA_TELA 800
#define ALTURA_TELA 600

#define FPS 90.0f

// Margens da camera
#define CAMERA_MARGIN_X (LARGURA_TELA / 4.0f)
#define CAMERA_MARGIN_Y (ALTURA_TELA / 4.0f)

// Limite maximo de fantasmas no mapa
#define MAX_FANTASMAS 12

// Inclui o cabecalho do modulo de itens e player
#include "item.h"
#include "player.h"

// Estados do jogo
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    // Adicione outros estados aqui se precisar
} GAME_STATE;

// Variavel global para o estado atual do jogo
extern GAME_STATE current_game_state;

// Enumeracao para as teclas de controle
enum KEYS {
    KEY_W, KEY_A, KEY_S, KEY_D,
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_R,
    KEY_F,
    NUM_KEYS
};

// Variaveis globais acessiveis externamente
extern float mouse_x;
extern float mouse_y;
extern float camera_x;
extern float camera_y;
extern float game_time;
extern bool escape_area_active;


extern ALLEGRO_SAMPLE *trilha_sonora;
extern ALLEGRO_SAMPLE_INSTANCE *inst_trilha_sonora;
extern ALLEGRO_SAMPLE *fantasma_morrendo_sfx;

extern ALLEGRO_BITMAP *sprite_jogador;




void game_init();
void game_loop();
void game_destroy();


void game_menu_init();
void game_menu_update(ALLEGRO_EVENT *event);
void game_menu_draw();
void game_menu_shutdown();

void reset_game();

#endif // GAME_H
