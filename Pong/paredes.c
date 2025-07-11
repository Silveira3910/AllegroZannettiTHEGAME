#include "paredes.h"
#include "game.h"
#include "player.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

const int NUM_OBSTACULOS = 10;
OBSTACLE obstaculos[10];

const char mapa[MAPA_LINHAS][MAPA_COLUNAS + 1] = {
    "################################################################################################################################################################",
    "#                                        #       #                   #       #                            #           #                                        #",
    "#                                        #       #                           #                            #           #                                        #",
    "#                                        #       #                           #                            #           #                                        #",
    "#                                        #                                   #                            #           #                                        #",
    "#                                        #                           #       #                            #           #                                        #",
    "#                                        #                           #       #                            #           #                                        #",
    "#                                                #                   #       #                            #           #                                        #",
    "#                                                #                   #       #                            #           #                                        #",
    "#                                                ########      #######       #                            #           #                                        #",
    "#                                        #       #                   #       #                            #           #                                        #",
    "#                                        #       #                   #       #                            #                                                    #",
    "#                                        #       #                   #       #                            #                                                    #",
    "#                                        #       #                           #                            #                                                    #",
    "#                                        #       #                           #################       ###########################################       #########",
    "#                                        #       #                                                                                                             #",
    "####################       ###############       #           #########                                                                                         #",
    "#                                                                    #                                                                                         #",
    "#                                                                    #                                                                                         #",
    "# P                                                                  #       ####################################       #                  #####################",
    "#                                                                    #                                          #       #                                      #",
    "#                                                                    #                                          #       #                                      #",
    "######       #####################################       #############                                          #       #                                      #",
    "#                                 #                                  #       #                                  #       #                  #                   #",
    "#                                 #                                  #       #                                  #       #                  #                   #",
    "#                                 #                                  #       #                                  #       #                  #                   #",
    "#                                 #                                  #       #                                  #       #                  #                   #",
    "#                                 #                                  #       #                                  #       #                  #                   #",
    "#                                 #                                  #       #                                  #       #                  #                   #",
    "#                                 #                                  #       #                                  #       #                  #                   #",
    "#                                                                    #       #                                  #       #                  #                   #",
    "#                                                                    #       #                                  #       #                  #                   #",
    "#                                                                    #       ####################################       #                  #                   #",
    "#       ###########      ##########                                                                                                        #######       #######",
    "#                                 #                                                                                                        #                   #",
    "#                                 #                                                                                                                            #",
    "#                                 #                                  #       #                                          #                                      #",
    "#                                 #                                  #       #                                          #                                      #",
    "#                                 #                                  #       #                                          #                  #                   #",
    "######################################################################       ###############################################################                   #",
    "#                                                                    #                                                  ####################                   #",
    "#                                                                    #                                                  ####################                   #",
    "#                                                                    #                                                  ####################                   #",
    "#                                                                    #       #                                          ########################################",
    "#                                                                    #       #                                          ########################################",
    "#                                                                    #       #                                          ########################################",
    "#                                                                            #                                          ########################################",
    "#                                                                            #                                          ########################################",
    "#                                                                            #                                          ########################################",
    "#                                                                    #       #                                          ########################################",
    "#                                                                    #       #                                          ########################################",
    "#                                                                    #       #                                          ########################################",
    "#                                                                    #                                                  ########################################",
    "#                                                                    #                                                  ########################################",
    "#                                                                    #                                                  ########################################",
    "#            ############################################            ###########################################################################################",
    "#            ############################################            ###########################################################################################",
    "#            ############################################            ###########################################################################################",
    "#            ############################################            ###########################################################################################",
    "################################################################################################################################################################"
};

void paredes_init() {
    // Pode carregar imagens aqui
}

void paredes_draw(float camera_x, float camera_y) {
    ALLEGRO_COLOR cor_parede = al_map_rgb(100, 100, 100); // Cor cinza para as paredes

    for (int i = 0; i < MAPA_LINHAS; i++) {
        for (int j = 0; j < MAPA_COLUNAS; j++) {
            if (mapa[i][j] == '#') {
                float x = j * TAM_BLOCO - camera_x;
                float y = i * TAM_BLOCO - camera_y;
                al_draw_filled_rectangle(x, y, x + TAM_BLOCO, y + TAM_BLOCO, cor_parede);
            }
        }
    }
}

// Verifica colisao de um retangulo (x,y,largura,altura) com qualquer parede no mapa
bool paredes_colisao(float x, float y, float largura, float altura) {
    // Converte as coordenadas do objeto para indices de bloco no mapa
    int inicio_x = (int)(x / TAM_BLOCO);
    int inicio_y = (int)(y / TAM_BLOCO);
    int fim_x = (int)((x + largura - 1) / TAM_BLOCO);
    int fim_y = (int)((y + altura - 1) / TAM_BLOCO);

    // Garante que os indices estao dentro dos limites do mapa
    if (inicio_x < 0) inicio_x = 0;
    if (inicio_y < 0) inicio_y = 0;
    if (fim_x >= MAPA_COLUNAS) fim_x = MAPA_COLUNAS - 1;
    if (fim_y >= MAPA_LINHAS) fim_y = MAPA_LINHAS - 1;

    for (int i = inicio_y; i <= fim_y; i++) {
        for (int j = inicio_x; j <= fim_x; j++) {
            // Se encontrar um bloco de parede ('#'), ha colisao
            if (mapa[i][j] == '#') {
                return true;
            }
        }
    }
    return false; // Nenhuma colisao encontrada
}

void paredes_destroy() {
}

// Funcao para posicionar o player (ou qualquer objeto) no ponto 'P' do mapa
void player_posicionar_no_ponto_p(Player *player) {
    for (int linha = 0; linha < MAPA_LINHAS; linha++) {
        for (int col = 0; col < MAPA_COLUNAS; col++) {
            if (mapa[linha][col] == 'P') {
                player->x = col * TAM_BLOCO;
                player->y = linha * TAM_BLOCO;
                return; // Encontrou o 'P' e posicionou, pode sair
            }
        }
    }
    fprintf(stderr, "Aviso: Ponto 'P' nao encontrado no mapa. Posicionando jogador em (0,0).\n");
    player->x = 0;
    player->y = 0;
}
