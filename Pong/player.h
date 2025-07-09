#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

// Estrutura que representa o jogador no jogo
typedef struct {
    float x, y;            // Posição (coordenadas no mapa)
    float largura, altura; // Dimensões do jogador
    float speed;           // Velocidade de movimento
} Player;

// Inicializa o jogador com valores padrão
void player_init(Player *p);

// Atualiza a posição do jogador com base nas teclas pressionadas e colisões
void player_update(Player *p, const bool keys[]);

// Desenha o jogador na tela considerando a posição da câmera
void player_draw(const Player *p, float camera_x, float camera_y);

// Libera recursos associados ao jogador
void player_destroy();

#endif
