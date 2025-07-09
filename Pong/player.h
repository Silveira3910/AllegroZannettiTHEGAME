#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

// Estrutura que representa o jogador no jogo
typedef struct {
    float x, y;            // Posi��o (coordenadas no mapa)
    float largura, altura; // Dimens�es do jogador
    float speed;           // Velocidade de movimento
} Player;

// Inicializa o jogador com valores padr�o
void player_init(Player *p);

// Atualiza a posi��o do jogador com base nas teclas pressionadas e colis�es
void player_update(Player *p, const bool keys[]);

// Desenha o jogador na tela considerando a posi��o da c�mera
void player_draw(const Player *p, float camera_x, float camera_y);

// Libera recursos associados ao jogador
void player_destroy();

#endif
