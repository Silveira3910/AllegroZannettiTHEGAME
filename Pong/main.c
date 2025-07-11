#include <stdio.h>
#include <allegro5/allegro.h>
#include "game.h"

int main(void) {
    game_init(); // Inicializa tudo: Allegro, display, timers, fontes, audio, etc.

    game_loop(); // Contém o loop principal do jogo

    game_destroy(); // Libera todos os recursos do jogo

    return 0;
}
