#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <stdbool.h>

// --- Configura��es Globais ---
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 90.0f         			// O 'f' � um float literal

// --- Dimens�es do Mapa ---
#define LARGURA_MAPA (LARGURA_TELA * 3)
#define ALTURA_MAPA (ALTURA_TELA * 3)

// --- Margens da C�mera (Dead Zone) ---
#define CAMERA_MARGIN_X (LARGURA_TELA / 4.0f)
#define CAMERA_MARGIN_Y (ALTURA_TELA / 4.0f)

#define NUM_OBSTACULOS 3
typedef struct
{
 float x,y;
 float caixalargura,caixaaltura;
}OBSTACLE;

OBSTACLE obstaculos[NUM_OBSTACULOS] =
{
    {400, 400, 100, 100},
    {200, 200, 50, 150},
    {700, 100, 120, 80}
};
                                            // --- ENUM PARA RASTREAR TECLAS PRESSIONADAS ---
enum KEYS {
    KEY_W, KEY_A, KEY_S, KEY_D, 		    // WASD
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, 	// SETAS
    NUM_KEYS
};
bool keys[NUM_KEYS] = {false};  		// ARRYA PARA RASTREAR O ESTADO DAS TECLAS
                                        // --- FUN��O PRINCIPAL DO JOGO ---

bool colisao(float x1, float y1, float w1, float h1,float x2, float y2, float w2, float h2)
{
    return !(x1 + w1 < x2 || x1 > x2 + w2 || y1 + h1 < y2 || y1 > y2 + h2);
}

int main()
{
                                         // INICIA O ALLEGRO
    if (!al_init()) {
        return -1;
    }

    al_init_primitives_addon();
    al_install_keyboard();
    al_init_image_addon();
    al_install_mouse(); 			    // SUPORTE PARA O MOUSE
    al_init_font_addon();
    al_init_ttf_addon();

                                        // JANELA DO JOGO
    ALLEGRO_DISPLAY *display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!display) {
        return -1;
    }

                                        // CRIA A FILA DE EVENTOS
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    if (!event_queue) {
        al_destroy_display(display);
        return -1;
    }

                                        // CRIA UM TIMER PARA CONTROLAR O FPS
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        return -1;
    }

                                        // REGISTRA A FONTE DE EVENTOS
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());

                                        // --- VARIAVEIS DO JOGO ---
	// LOCAL ONDE PERSONAGEM NASCE
    float player_x = LARGURA_MAPA / 2.0f - 16;
    float player_y = ALTURA_MAPA / 2.0f - 16;
    float player_speed = 3.0f;

    obstaculos[0].x = player_x + 30;   // 30 pixels à direita
    obstaculos[0].y = player_y + 30;   // 30 pixels abaixo
    obstaculos[0].caixalargura = 100;
    obstaculos[0].caixaaltura = 100;

    obstaculos[1].x = player_x - 150;  // 150 pixels à esquerda
    obstaculos[1].y = player_y;        // mesma altura do player
    obstaculos[1].caixalargura = 50;
    obstaculos[1].caixaaltura = 150;

    obstaculos[2].x = player_x + 120;  // 120 pixels à direita
    obstaculos[2].y = player_y - 100;  // 100 pixels acima
    obstaculos[2].caixalargura = 120;
    obstaculos[2].caixaaltura = 80;

    float light_length = 150.0f;
    float light_angle_spread = ALLEGRO_PI / 4.0f;

    float mouse_x = LARGURA_TELA / 2.0f; 	// MOUSE NA TELA(POSI��O)
    float mouse_y = ALTURA_TELA / 2.0f;

                                        // --- VARIAVEIS DA CAMERA ---
    // COME�AR COM O JOGADOR MAIS CENTRADO
    float camera_x = player_x - LARGURA_TELA / 2.0f + 16;
    float camera_y = player_y - ALTURA_TELA / 2.0f + 16;

    // CAMERA FIQUE NO LIMITE DO MAPA E N�O FUJA
    if (camera_x < 0) camera_x = 0;
    if (camera_x > LARGURA_MAPA - LARGURA_TELA) camera_x = (float)(LARGURA_MAPA - LARGURA_TELA);
    if (camera_y < 0) camera_y = 0;
    if (camera_y > ALTURA_MAPA - ALTURA_TELA) camera_y = (float)(ALTURA_MAPA - ALTURA_TELA);


    bool done = false;
    ALLEGRO_EVENT ev;
    bool redraw = true;

    al_start_timer(timer);

                                            // --- LOOP PRINCIPAL DO JOGO ---
    while (!done) {
        al_wait_for_event(event_queue, &ev);

                                            // --- PROCESSAMENTO DOS EVENTOS ---
        if (ev.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            done = true;
        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_W: case ALLEGRO_KEY_UP: keys[KEY_W] = true; break;
                case ALLEGRO_KEY_S: case ALLEGRO_KEY_DOWN: keys[KEY_S] = true; break;
                case ALLEGRO_KEY_A: case ALLEGRO_KEY_LEFT: keys[KEY_A] = true; break;
                case ALLEGRO_KEY_D: case ALLEGRO_KEY_RIGHT: keys[KEY_D] = true; break;
                case ALLEGRO_KEY_ESCAPE: done = true; break;
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_W: case ALLEGRO_KEY_UP: keys[KEY_W] = false; break;
                case ALLEGRO_KEY_S: case ALLEGRO_KEY_DOWN: keys[KEY_S] = false; break;
                case ALLEGRO_KEY_A: case ALLEGRO_KEY_LEFT: keys[KEY_A] = false; break;
                case ALLEGRO_KEY_D: case ALLEGRO_KEY_RIGHT: keys[KEY_D] = false; break;
            }
        } else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
            mouse_x = ev.mouse.x;
            mouse_y = ev.mouse.y;
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            // FUNCIONAMENTO DA MOVIMENTA��O (PELAS COORDENADAS)
            if (keys[KEY_W] || keys[KEY_UP]) { player_y -= player_speed; }
            if (keys[KEY_S] || keys[KEY_DOWN]) { player_y += player_speed; }
            if (keys[KEY_A] || keys[KEY_LEFT]) { player_x -= player_speed; }
            if (keys[KEY_D] || keys[KEY_RIGHT]) { player_x += player_speed; }

            // PERSONAGEM N�O CAIR DA TERRA PLANA
            if (player_x < 0) player_x = 0;
            if (player_x > LARGURA_MAPA - 32) player_x = (float)(LARGURA_MAPA - 32);
            if (player_y < 0) player_y = 0;
            if (player_y > ALTURA_MAPA - 32) player_y = (float)(ALTURA_MAPA - 32);

            // --- ATUALIZA��O DA DEAD ZONE(CAMERA) ---
            float player_screen_x = player_x - camera_x + 16; // Posi��o X do centro da bola na tela
            float player_screen_y = player_y - camera_y + 16; // Posi��o Y do centro da bola na tela

            // AJUSTA CAMERA_X
            if (player_screen_x < CAMERA_MARGIN_X) {
                camera_x = player_x - CAMERA_MARGIN_X - 16; // CAMERA PARA A ESQUERDA
            } else if (player_screen_x > LARGURA_TELA - CAMERA_MARGIN_X) {
                camera_x = player_x - (LARGURA_TELA - CAMERA_MARGIN_X) - 16; // CAMERA PARA DIREITA
            }

            // AJUSTA CAMERA_Y
            if (player_screen_y < CAMERA_MARGIN_Y) {
                camera_y = player_y - CAMERA_MARGIN_Y - 16; // CAMERA PARA CIMA
            } else if (player_screen_y > ALTURA_TELA - CAMERA_MARGIN_Y) {
                camera_y = player_y - (ALTURA_TELA - CAMERA_MARGIN_Y) - 16; // MOVE PARA BAIXO
            }

            // LIMITADOR PARA N�O SAIR A CAMERA DA TERRA PLANA
            if (camera_x < 0) camera_x = 0;
            if (camera_x > LARGURA_MAPA - LARGURA_TELA) camera_x = (float)(LARGURA_MAPA - LARGURA_TELA);
            if (camera_y < 0) camera_y = 0;
            if (camera_y > ALTURA_MAPA - ALTURA_TELA) camera_y = (float)(ALTURA_MAPA - ALTURA_TELA);


	           // --- MAPA EM PRETO ---
            al_clear_to_color(al_map_rgb(0, 0, 0)); // LIMPA A TELA EM PRETO

	           // CH�O DO MAPA
            al_draw_filled_rectangle(0 - camera_x, 0 - camera_y,
                                     LARGURA_MAPA - camera_x, ALTURA_MAPA - camera_y,
                                     al_map_rgb(50, 50, 50)); // CINZA PARA O CH�O
	     int i;
	     for (i = 0; i < NUM_OBSTACULOS; i++) {
    al_draw_filled_rectangle(
        obstaculos[i].x - camera_x,
        obstaculos[i].y - camera_y,
        obstaculos[i].x + obstaculos[i].caixalargura - camera_x,
        obstaculos[i].y + obstaculos[i].caixaaltura - camera_y,
        al_map_rgb(150, 0, 0)
    );}
	           // COR DO PERSONAGEM
            al_draw_filled_circle(player_x + 16 - camera_x, player_y + 16 - camera_y, 16, al_map_rgb(0, 255, 0)); // COR VERDE

	           // COORDENADAS PARA O CENTRO DO PERSONAGEM
            float light_origin_x = player_x + 16;
            float light_origin_y = player_y + 16;

        	   // C�LCULO DA DIRE��O DA LANTERNA PARA O MOUSE
	           // AJUSTAR ANTES LIGHT_ORIGIN NA POSI��O E DEPOIS O CALCULO DELTA
            float delta_x_screen = mouse_x - (light_origin_x - camera_x);
            float delta_y_screen = mouse_y - (light_origin_y - camera_y);
            float base_angle = atan2f(delta_y_screen, delta_x_screen);

	          // CALCULO PARA OS PONTOS EXTREMOS DO CONE DE LUZ
            float angle1 = base_angle - light_angle_spread / 2.0f;
            float angle2 = base_angle + light_angle_spread / 2.0f;

            float p2_x = light_origin_x + light_length * cos(angle1);
            float p2_y = light_origin_y + light_length * sin(angle1);

            float p3_x = light_origin_x + light_length * cos(angle2);
            float p3_y = light_origin_y + light_length * sin(angle2);

           	  // DESENHA A PARTE ESCURO DO AMBIENTE
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
            al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 200));

            	 // FAZ O CONE DE LUZ EM MODO BLEND ADITIVO
	    	 // FUNCIONA COMO UMA SOMATORIA DAS CORES, SERVE PARA A LUZ MUDAR DO PRETO PARA A COR DO CH�O OU OUTRO OBJETO
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_ONE);
            	 // MOSTRA O TRIANGULO DA LUZ AJUSTADO PELO BLEND ADITIVO
            al_draw_filled_triangle(light_origin_x - camera_x, light_origin_y - camera_y,
                                    p2_x - camera_x, p2_y - camera_y,
                                    p3_x - camera_x, p3_y - camera_y,
                                    al_map_rgba(255, 255, 200, 100));

            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA); // VOLTA COM O BLEND PADR�O

            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}

// as colisões tão sendo desenhadas mas não tem um teste ainda ou um impedimento de movimentação tá 
