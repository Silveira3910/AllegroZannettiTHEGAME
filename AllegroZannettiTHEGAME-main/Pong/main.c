#include <stdio.h>
#include <allegro5/allegro.h>
#include "game.h"

int main(void) {
    game_init();

    game_loop();

    game_destroy();

    al_reserve_samples(1);

    ALLEGRO_SAMPLE *sample = al_load_sample("audio/piano-terror.ogg");
    if (!sample) {
        printf("Erro ao carregar áudio!\n");
        return -1;
    }

    al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    al_rest(5.0);
    al_destroy_sample(sample);

    return 0;

}

