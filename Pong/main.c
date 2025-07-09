#include <stdio.h>
#include "game.h"

int main(void) {
    game_init();

    // Verifica se a inicializa��o ocorreu corretamente
    if (!al_is_system_installed()) {
        fprintf(stderr, "Erro: Allegro n�o foi inicializado corretamente.\n");
        return 1;
    }

    game_loop();
    game_destroy();

    return 0;
}
