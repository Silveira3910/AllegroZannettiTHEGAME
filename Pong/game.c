#include "game.h"
#include "player.h"
#include "fantasma.h"
#include "paredes.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Variáveis globais acessíveis externamente (não static)
float mouse_x = LARGURA_TELA / 2.0f;
float mouse_y = ALTURA_TELA / 2.0f;

float camera_x = 0;
float camera_y = 0;

static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_EVENT_QUEUE *event_queue = NULL;
static ALLEGRO_TIMER *timer = NULL;

static ALLEGRO_FONT *font_big = NULL;
static ALLEGRO_FONT *font_small = NULL;

static bool keys[NUM_KEYS] = { false };
static bool done = false;
static bool redraw = true;
static bool game_over = false;
static float opacidade_game_over = 0.0f;

static Player player;
static bool lanterna_ativa = false;
static float tempo_lanterna = 0.0f;
static const float DURACAO_LANTERNA = 12.0f;
static float cooldown_lanterna = 0.0f;
const float COOLDOWN_LANTERNA = 8.0f;

void update_camera() {
    // Centraliza a câmera no jogador
    camera_x = player.x + player.largura / 2.0f - LARGURA_TELA / 2.0f;
    camera_y = player.y + player.altura / 2.0f - ALTURA_TELA / 2.0f;

    // Limita para não ultrapassar bordas do mapa
    if (camera_x < 0) camera_x = 0;
    if (camera_y < 0) camera_y = 0;
    if (camera_x > LARGURA_MAPA - LARGURA_TELA)
        camera_x = LARGURA_MAPA - LARGURA_TELA;
    if (camera_y > ALTURA_MAPA - ALTURA_TELA)
        camera_y = ALTURA_MAPA - ALTURA_TELA;
}

void desenhar_feixes_visuais() {
    float light_origin_x = player.x + player.largura / 2.0f;
    float light_origin_y = player.y + player.altura / 2.0f;

    float delta_x_screen = mouse_x - (light_origin_x - camera_x);
    float delta_y_screen = mouse_y - (light_origin_y - camera_y);
    float base_angle = atan2f(delta_y_screen, delta_x_screen);

    float light_angle_spread = ALLEGRO_PI / 4.0f;
    float light_length = 150.0f;
    int num_feixes = 200;

    for (int i = 0; i <= num_feixes; i++) {
        float angle = base_angle - light_angle_spread / 2.0f + (light_angle_spread / num_feixes) * i;

        float end_x = light_origin_x;
        float end_y = light_origin_y;

        for (float dist = 0; dist < light_length; dist += 2.0f) {
            float px = light_origin_x + dist * cosf(angle);
            float py = light_origin_y + dist * sinf(angle);

            if (paredes_colisao(px, py, 1, 1)) {
                end_x = px;
                end_y = py;
                break;
            }

            end_x = px;
            end_y = py;
        }

        // Aqui você desenha o feixe visual visível
        al_draw_line(light_origin_x - camera_x, light_origin_y - camera_y,
                     end_x - camera_x, end_y - camera_y,
                     al_map_rgba(255, 255, 180, 30), 1);
    }
}

void draw_lanterna() {
    ALLEGRO_BITMAP *shadow = al_create_bitmap(LARGURA_TELA, ALTURA_TELA);
    al_set_target_bitmap(shadow);
    al_clear_to_color(al_map_rgba(0, 0, 0, 220));  // sombra escura

    al_set_blender(ALLEGRO_DEST_MINUS_SRC, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    float light_origin_x = player.x + player.largura / 2.0f;
    float light_origin_y = player.y + player.altura / 2.0f;

    float delta_x_screen = mouse_x - (light_origin_x - camera_x);
    float delta_y_screen = mouse_y - (light_origin_y - camera_y);
    float base_angle = atan2f(delta_y_screen, delta_x_screen);

    float light_angle_spread = ALLEGRO_PI / 4.0f;
    float light_length = 150.0f;
    int num_feixes = 200;

    for (int i = 0; i <= num_feixes; i++) {
        float angle = base_angle - light_angle_spread / 2.0f + (light_angle_spread / num_feixes) * i;
        float end_x = light_origin_x;
        float end_y = light_origin_y;

        for (float dist = 0; dist < light_length; dist += 2.0f) {
            float px = light_origin_x + dist * cosf(angle);
            float py = light_origin_y + dist * sinf(angle);

            if (paredes_colisao(px, py, 1, 1)) {
                end_x = px;
                end_y = py;
                break;
            }

            end_x = px;
            end_y = py;
        }

        // Fura a sombra nesses pontos
        al_draw_line(light_origin_x - camera_x, light_origin_y - camera_y,
                     end_x - camera_x, end_y - camera_y,
                     al_map_rgba(255, 255, 255, 255), 2);
    }

    // Restaura para desenhar na tela principal
    al_set_target_backbuffer(al_get_current_display());
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    al_draw_bitmap(shadow, 0, 0, 0);
    al_destroy_bitmap(shadow);
}

void draw_game_over() {
    if (opacidade_game_over < 1.0f) opacidade_game_over += 0.01f;
    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, (int)(opacidade_game_over * 255)));
    al_draw_text(font_big, al_map_rgb(255, 0, 0), LARGURA_TELA / 2, ALTURA_TELA / 2 - 40, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
    al_draw_text(font_small, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 20, ALLEGRO_ALIGN_CENTRE, "Pressione R para reiniciar");
}

void reset_game() {
    player_init(&player);

    // Posicionar no ponto 'P' do mapa
    player_posicionar_no_ponto_p(&player);

    fantasmas_init();
    paredes_init();

    for (int i = 0; i < NUM_KEYS; i++)
        keys[i] = false;

    game_over = false;
    opacidade_game_over = 0.0f;

    // Ajuste inicial da câmera para centralizar o jogador, respeitando limites do mapa
    camera_x = player.x - LARGURA_TELA / 2.0f;
    camera_y = player.y - ALTURA_TELA / 2.0f;

    if (camera_x < 0) camera_x = 0;
    if (camera_y < 0) camera_y = 0;
    if (camera_x > LARGURA_MAPA - LARGURA_TELA)
        camera_x = LARGURA_MAPA - LARGURA_TELA;
    if (camera_y > ALTURA_MAPA - ALTURA_TELA)
        camera_y = ALTURA_MAPA - ALTURA_TELA;
}

void game_init() {
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        exit(EXIT_FAILURE);
    }
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(1);

    display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        exit(EXIT_FAILURE);
    }

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / FPS);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    font_big = al_load_ttf_font("fonts/OpenSans-Regular.ttf", 48, 0);
    if (!font_big) {
        fprintf(stderr, "Erro ao carregar fonte grande.\n");
        exit(EXIT_FAILURE);
    }

    font_small = al_load_ttf_font("fonts/OpenSans-Regular.ttf", 20, 0);
    if (!font_small) {
        fprintf(stderr, "Erro ao carregar fonte pequena.\n");
        exit(EXIT_FAILURE);
    }

    reset_game();
    al_start_timer(timer);
}

void game_loop() {
    ALLEGRO_EVENT ev;

    while (!done) {
        al_wait_for_event(event_queue, &ev);

        switch (ev.type) {
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                if (!game_over) {
                    player_update(&player, keys);
                    fantasmas_update(&player);
                    update_camera();

                    if (fantasmas_check_collision(&player)) {
                        game_over = true;
                    }

                    if (lanterna_ativa)
                    {
                        desenhar_feixes_visuais();
                        tempo_lanterna -= 1.0f / FPS;
                        if(tempo_lanterna <= 0.0f)
                        {
                            lanterna_ativa = false;
                            tempo_lanterna = 0.0f;
                            cooldown_lanterna = COOLDOWN_LANTERNA;
                        }
                    }
                   else{
                            if (cooldown_lanterna > 0.0f)
                            {
                                cooldown_lanterna -= 1.0f/FPS;
                                if(cooldown_lanterna < 0.0f)
                                {
                                    cooldown_lanterna = 0.0f;
                                }
                            }
                        }

                }
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                if (!game_over) {
                    switch (ev.keyboard.keycode) {
                        case ALLEGRO_KEY_W: keys[KEY_W] = true; break;
                        case ALLEGRO_KEY_A: keys[KEY_A] = true; break;
                        case ALLEGRO_KEY_S: keys[KEY_S] = true; break;
                        case ALLEGRO_KEY_D: keys[KEY_D] = true; break;
                        case ALLEGRO_KEY_UP: keys[KEY_UP] = true; break;
                        case ALLEGRO_KEY_DOWN: keys[KEY_DOWN] = true; break;
                        case ALLEGRO_KEY_LEFT: keys[KEY_LEFT] = true; break;
                        case ALLEGRO_KEY_RIGHT: keys[KEY_RIGHT] = true; break;
                        case ALLEGRO_KEY_ESCAPE: done = true; break;
                        case ALLEGRO_KEY_F:
                            if(!lanterna_ativa && cooldown_lanterna == 0.0f)
                            {
                                lanterna_ativa = true;
                                tempo_lanterna = DURACAO_LANTERNA;
                            }break;
                    }
                } else {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_R) {
                        reset_game();
                    }
                }
                break;

            case ALLEGRO_EVENT_KEY_UP:
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_W: keys[KEY_W] = false; break;
                    case ALLEGRO_KEY_A: keys[KEY_A] = false; break;
                    case ALLEGRO_KEY_S: keys[KEY_S] = false; break;
                    case ALLEGRO_KEY_D: keys[KEY_D] = false; break;
                    case ALLEGRO_KEY_UP: keys[KEY_UP] = false; break;
                    case ALLEGRO_KEY_DOWN: keys[KEY_DOWN] = false; break;
                    case ALLEGRO_KEY_LEFT: keys[KEY_LEFT] = false; break;
                    case ALLEGRO_KEY_RIGHT: keys[KEY_RIGHT] = false; break;
                }
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
            case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
                mouse_x = ev.mouse.x;
                mouse_y = ev.mouse.y;
                break;
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            al_clear_to_color(al_map_rgb(0, 0, 0));

            paredes_draw(camera_x, camera_y);
            player_draw(&player, camera_x, camera_y);
            fantasmas_draw(camera_x, camera_y);
           if(lanterna_ativa)
           {
               desenhar_feixes_visuais();
               draw_lanterna();
           }else
           {
                 al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 220));
        }



            if (game_over) {
                draw_game_over();
            }

            al_flip_display();
        }
    }
}

void game_destroy() {
    fantasmas_destroy();
    paredes_destroy();
    player_destroy();

    if (font_big) al_destroy_font(font_big);
    if (font_small) al_destroy_font(font_small);

    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
}
