#include "game.h"
#include "player.h"
#include "fantasma.h"
#include "paredes.h"
#include "item.h"

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


float mouse_x = LARGURA_TELA / 2.0f;
float mouse_y = ALTURA_TELA / 2.0f;
float camera_x = 0;
float camera_y = 0;
float game_time = 0.0f;
bool escape_area_active = false;

GAME_STATE current_game_state = GAME_STATE_MENU;


ALLEGRO_SAMPLE *trilha_sonora = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_trilha_sonora = NULL;
ALLEGRO_SAMPLE *fantasma_morrendo_sfx = NULL;

ALLEGRO_BITMAP *sprite_jogador = NULL;


static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_EVENT_QUEUE *event_queue = NULL;
static ALLEGRO_TIMER *timer = NULL;

static ALLEGRO_FONT *font_big_game_over = NULL;
static ALLEGRO_FONT *font_small_hud = NULL;

static ALLEGRO_FONT *font_title_menu = NULL;
static ALLEGRO_FONT *font_button_menu = NULL;
static ALLEGRO_FONT *font_tutorial_menu = NULL;

static bool keys[NUM_KEYS] = {false};
static bool done_main_loop = false;
static bool redraw = true;
static bool game_over = false;
static bool game_won = false;
static float opacidade_game_over = 0.0f;

static Player player;
static bool lanterna_ativa = false;
static float tempo_lanterna = 0.0f;
static const float DURACAO_LANTERNA = 12.0f;
static float cooldown_lanterna = 0.0f;
const float COOLDOWN_LANTERNA = 8.0f;

static float escape_x, escape_y;
static const float ESCAPE_AREA_SIZE = 32.0f;

static void update_camera() {

    camera_x = player.x + player.largura / 2.0f - LARGURA_TELA / 2.0f;
    camera_y = player.y + player.altura / 2.0f - ALTURA_TELA / 2.0f;


    if (camera_x < 0)
        camera_x = 0;
    if (camera_y < 0)
        camera_y = 0;
    if (camera_x > LARGURA_MAPA - LARGURA_TELA)
        camera_x = LARGURA_MAPA - LARGURA_TELA;
    if (camera_y > ALTURA_MAPA - ALTURA_TELA)
        camera_y = ALTURA_MAPA - ALTURA_TELA;
}


static void desenhar_feixes_visuais() {
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


        al_draw_line(light_origin_x - camera_x, light_origin_y - camera_y,
                     end_x - camera_x, end_y - camera_y,
                     al_map_rgba(255, 255, 180, 30), 1);
    }
}

static void draw_lanterna() {
    ALLEGRO_BITMAP *shadow = al_create_bitmap(LARGURA_TELA, ALTURA_TELA);
    al_set_target_bitmap(shadow);
    al_clear_to_color(al_map_rgba(0, 0, 0, 238)); // sombra escura

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


        al_draw_line(light_origin_x - camera_x, light_origin_y - camera_y,
                     end_x - camera_x, end_y - camera_y,
                     al_map_rgba(255, 255, 255, 238), 2); // Cor branca para "furar"
    }


    al_set_target_backbuffer(al_get_current_display());
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    al_draw_bitmap(shadow, 0, 0, 0);
    al_destroy_bitmap(shadow);
}


static void draw_game_end_screen() {
    if (opacidade_game_over < 1.0f)
        opacidade_game_over += 0.01f;
    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, (int)(opacidade_game_over * 255)));

    if (game_won) {
        al_draw_text(font_big_game_over, al_map_rgb(0, 255, 0), LARGURA_TELA / 2, ALTURA_TELA / 2 - 40, ALLEGRO_ALIGN_CENTRE, "VOCE VENCEU!");
    } else {
        al_draw_text(font_big_game_over, al_map_rgb(255, 0, 0), LARGURA_TELA / 2, ALTURA_TELA / 2 - 40, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
    }
    al_draw_text(font_small_hud, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 20, ALLEGRO_ALIGN_CENTRE, "Pressione R para reiniciar");


    char final_time_text[50];
    int minutes = (int)game_time / 60;
    int seconds = (int)game_time % 60;
    sprintf(final_time_text, "Tempo Final: %02d:%02d", minutes, seconds); // Formato MM:SS
    al_draw_text(font_small_hud, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 60, ALLEGRO_ALIGN_CENTRE, final_time_text);
}

void reset_game() {
    player_init(&player);
    player_posicionar_no_ponto_p(&player);


    escape_x = player.x;
    escape_y = player.y;

    fantasmas_init();
    paredes_init();
    itens_reset();
    game_time = 0.0f;

    lanterna_ativa = false;
    tempo_lanterna = 0.0f;
    cooldown_lanterna = 0.0f;

    escape_area_active = false;

    for (int i = 0; i < NUM_KEYS; i++)
        keys[i] = false;

    game_over = false;
    game_won = false;
    opacidade_game_over = 0.0f;


    update_camera(); // Chama para garantir que a camera


    if (inst_trilha_sonora && !al_get_sample_instance_playing(inst_trilha_sonora)) {
        al_play_sample_instance(inst_trilha_sonora);
    }
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
    sprite_jogador = al_load_bitmap("imagens/player.png");


    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(10); // 10 canais

    display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        exit(EXIT_FAILURE);
    }
    al_set_window_title(display, "Morto não fala"); // Titulo da janela

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / FPS);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    //  HUD
    font_big_game_over = al_load_ttf_font("fonts/OpenSans-Regular.ttf", 48, 0);
    if (!font_big_game_over) {
        fprintf(stderr, "Erro ao carregar font_big_game_over. Verifique 'fonts/OpenSans-Regular.ttf'.\n");

        font_big_game_over = al_create_builtin_font();
    }

    font_small_hud = al_load_ttf_font("fonts/OpenSans-Regular.ttf", 20, 0);
    if (!font_small_hud) {
        fprintf(stderr, "Erro ao carregar font_small_hud. Verifique 'fonts/OpenSans-Regular.ttf'.\n");
        font_small_hud = al_create_builtin_font();
    }


    font_title_menu = al_load_ttf_font("fonts/OpenSans-Regular.ttf", 72, 0);
    if (!font_title_menu) {
        fprintf(stderr, "Falha ao carregar font_title_menu. Verifique 'fonts/OpenSans-Regular.ttf'.\n");
        font_title_menu = al_create_builtin_font();
    }

    font_button_menu = al_load_ttf_font("fonts/OpenSans-Regular.ttf", 36, 0);
    if (!font_button_menu) {
        fprintf(stderr, "Falha ao carregar font_button_menu. Verifique 'fonts/OpenSans-Regular.ttf'.\n");
        font_button_menu = al_create_builtin_font();
    }

    font_tutorial_menu = al_load_ttf_font("fonts/OpenSans-Regular.ttf", 20, 0);
    if (!font_tutorial_menu) {
        fprintf(stderr, "Falha ao carregar font_tutorial_menu. Verifique 'fonts/OpenSans-Regular.ttf'.\n");
        font_tutorial_menu = al_create_builtin_font();
    }


    trilha_sonora = al_load_sample("audio/piano-horror.ogg");
    if (!trilha_sonora) {
        fprintf(stderr, "Erro ao carregar musica de fundo. Verifique 'audio/piano-terror.ogg'.\n");
    } else {
        inst_trilha_sonora = al_create_sample_instance(trilha_sonora);
        if (!inst_trilha_sonora) {
            fprintf(stderr, "Erro ao criar instancia da musica.\n");
            al_destroy_sample(trilha_sonora);
            trilha_sonora = NULL;
        } else {
            al_attach_sample_instance_to_mixer(inst_trilha_sonora, al_get_default_mixer());
            al_set_sample_instance_playmode(inst_trilha_sonora, ALLEGRO_PLAYMODE_LOOP);
            al_set_sample_instance_gain(inst_trilha_sonora, 0.8f);
        }
    }

    //  fantasma morrendo
    fantasma_morrendo_sfx = al_load_sample("audio/ghost-scream.ogg");
    if (!fantasma_morrendo_sfx) {
        fprintf(stderr, "Erro ao carregar som de fantasma morrendo. Verifique 'audio/ghost-scream.ogg'.\n");
    }


    al_start_timer(timer);
}

void game_loop() {
    ALLEGRO_EVENT ev;
    bool done_game_loop = false;

    while (!done_game_loop) {
        al_wait_for_event(event_queue, &ev);

        switch (ev.type) {
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                if (current_game_state == GAME_STATE_PLAYING && !game_over) {


                    if (inst_trilha_sonora && !al_get_sample_instance_playing(inst_trilha_sonora)) {
                         al_play_sample_instance(inst_trilha_sonora);
                    }

                    game_time += 1.0f / FPS;
                    player_update(&player, keys);
                    fantasmas_update(&player);
                    itens_update(&player);
                    update_camera();


                    if (fantasmas_check_collision(&player)) {
                        game_over = true;
                        game_won = false;
                        if (inst_trilha_sonora) al_stop_sample_instance(inst_trilha_sonora);
                    }


                    if (itens_get_coletados() >= MAX_ITENS) {
                        escape_area_active = true;

                        if (player.x < escape_x + player.largura &&
                            player.x + player.largura > escape_x &&
                            player.y < escape_y + player.altura &&
                            player.y + player.altura > escape_y)
                        {
                            game_over = true;
                            game_won = true; // Ganhou!
                            if (inst_trilha_sonora) al_stop_sample_instance(inst_trilha_sonora);
                        }
                    }


                    if (lanterna_ativa) {
                        tempo_lanterna -= 1.0f / FPS;
                        if(tempo_lanterna <= 0.0f) {
                            lanterna_ativa = false;
                            tempo_lanterna = 0.0f;
                            cooldown_lanterna = COOLDOWN_LANTERNA;
                        }
                    } else {
                        if (cooldown_lanterna > 0.0f) {
                            cooldown_lanterna -= 1.0f / FPS;
                            if(cooldown_lanterna < 0.0f) {
                                cooldown_lanterna = 0.0f;
                            }
                        }
                    }
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done_game_loop = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                if (current_game_state == GAME_STATE_PLAYING) {
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
                            case ALLEGRO_KEY_F:
                                if(!lanterna_ativa && cooldown_lanterna == 0.0f) {
                                    lanterna_ativa = true;
                                    tempo_lanterna = DURACAO_LANTERNA;
                                }
                                break;
                        }
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_R && game_over) {
                        reset_game();
                        current_game_state = GAME_STATE_PLAYING;
                    }
                }
                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    done_game_loop = true;
                }
                break;

            case ALLEGRO_EVENT_KEY_UP:
                if (current_game_state == GAME_STATE_PLAYING) {
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
                }
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
                mouse_x = ev.mouse.x;
                mouse_y = ev.mouse.y;
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (current_game_state == GAME_STATE_MENU) {
                    game_menu_update(&ev);
                    if (current_game_state == GAME_STATE_PLAYING) {
                        reset_game();
                    }
                }
                break;
        }

        //  desenho
        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            al_clear_to_color(al_map_rgb(0, 0, 0));

            if (current_game_state == GAME_STATE_MENU) {
                game_menu_draw();
            } else if (current_game_state == GAME_STATE_PLAYING) {
                paredes_draw(camera_x, camera_y);
                player_draw(&player, camera_x, camera_y);
                fantasmas_draw(camera_x, camera_y);
                itens_draw(camera_x, camera_y);

                if (escape_area_active) {
                    al_draw_filled_rectangle(escape_x - camera_x,
                                             escape_y - camera_y,
                                             escape_x + ESCAPE_AREA_SIZE - camera_x,
                                             escape_y + ESCAPE_AREA_SIZE - camera_y,
                                             al_map_rgba(0, 255, 0, 100));
                    al_draw_rectangle(escape_x - camera_x,
                                      escape_y - camera_y,
                                      escape_x + ESCAPE_AREA_SIZE - camera_x,
                                      escape_y + ESCAPE_AREA_SIZE - camera_y,
                                      al_map_rgb(0, 255, 0), 2);
                }

                if(lanterna_ativa) {
                    desenhar_feixes_visuais();
                    draw_lanterna();
                } else {
                    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 238));
                }

                char hud_text[100];

                int minutes = (int)game_time / 60;
                int seconds = (int)game_time % 60;
                sprintf(hud_text, "Tempo: %02d:%02d", minutes, seconds);
                al_draw_text(font_small_hud, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, hud_text);

                sprintf(hud_text, "Itens: %d/%d", itens_get_coletados(), MAX_ITENS);
                al_draw_text(font_small_hud, al_map_rgb(255, 255, 255), 10, 30, ALLEGRO_ALIGN_LEFT, hud_text);

                if (lanterna_ativa) {
                    sprintf(hud_text, "Lanterna: %.1fs", tempo_lanterna);
                    al_draw_text(font_small_hud, al_map_rgb(0, 255, 0), LARGURA_TELA - 10, 10, ALLEGRO_ALIGN_RIGHT, hud_text);
                } else if (cooldown_lanterna > 0.0f) {
                    sprintf(hud_text, "Recarga: %.1fs", cooldown_lanterna);
                    al_draw_text(font_small_hud, al_map_rgb(255, 0, 0), LARGURA_TELA - 10, 10, ALLEGRO_ALIGN_RIGHT, hud_text);
                } else {
                     al_draw_text(font_small_hud, al_map_rgb(0, 255, 0), LARGURA_TELA - 10, 10, ALLEGRO_ALIGN_RIGHT, "Lanterna: PRONTA");
                }

                if (game_over) {
                    draw_game_end_screen();
                }
            }

            al_flip_display();
        }
    }
}

void game_destroy() {
    fantasmas_destroy();
    paredes_destroy();
    player_destroy();



    if (inst_trilha_sonora) {
        al_destroy_sample_instance(inst_trilha_sonora);
    }
    if (trilha_sonora) {
        al_destroy_sample(trilha_sonora);
    }

    if (fantasma_morrendo_sfx) {
        al_destroy_sample(fantasma_morrendo_sfx);
    }

    if (font_big_game_over) al_destroy_font(font_big_game_over);
    if (font_small_hud) al_destroy_font(font_small_hud);
    if (font_title_menu) al_destroy_font(font_title_menu);
    if (font_button_menu) al_destroy_font(font_button_menu);
    if (font_tutorial_menu) al_destroy_font(font_tutorial_menu);

    if (sprite_jogador) {
    al_destroy_bitmap(sprite_jogador);
    sprite_jogador = NULL;
        }

    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    al_shutdown_primitives_addon();
    al_shutdown_image_addon();
    al_shutdown_font_addon();
    al_shutdown_ttf_addon();
    al_uninstall_audio();
    al_uninstall_keyboard();
    al_uninstall_mouse();
    al_uninstall_system();
}

void game_menu_init() {}
void game_menu_update(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        if (event->mouse.button == 1) {
            float mouse_x_event = event->mouse.x;
            float mouse_y_event = event->mouse.y;

            float button_x = LARGURA_TELA / 2 - 100;
            float button_y = ALTURA_TELA / 2 + 50;
            float button_width = 200;
            float button_height = 60;

            if (mouse_x_event >= button_x && mouse_x_event <= button_x + button_width &&
                mouse_y_event >= button_y && mouse_y_event <= button_y + button_height) {
                current_game_state = GAME_STATE_PLAYING;
            }
        }
    }
}

void game_menu_draw() {
    ALLEGRO_COLOR text_color = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR button_color = al_map_rgb(50, 150, 50);
    ALLEGRO_COLOR button_border_color = al_map_rgb(0, 200, 0);
    ALLEGRO_COLOR tutorial_color = al_map_rgb(200, 200, 200);

    if (font_title_menu) {
        al_draw_text(font_title_menu, text_color, LARGURA_TELA / 2, ALTURA_TELA / 4,
                     ALLEGRO_ALIGN_CENTER, "Morto Nao Fala");
    } else {
        al_draw_text(al_create_builtin_font(), text_color, LARGURA_TELA / 2, ALTURA_TELA / 4,
                     ALLEGRO_ALIGN_CENTER, "Morto Nao fala");
    }

    float button_x = LARGURA_TELA / 2 - 100;
    float button_y = ALTURA_TELA / 2 + 50;
    float button_width = 200;
    float button_height = 60;

    al_draw_filled_rectangle(button_x, button_y, button_x + button_width, button_y + button_height, button_color);
    al_draw_rectangle(button_x, button_y, button_x + button_width, button_y + button_height, button_border_color, 3);

    if (font_button_menu) {
        al_draw_text(font_button_menu, text_color, LARGURA_TELA / 2, button_y + button_height / 4,
                     ALLEGRO_ALIGN_CENTER, "Iniciar Jogo");
    } else {
        al_draw_text(al_create_builtin_font(), text_color, LARGURA_TELA / 2, button_y + button_height / 4,
                     ALLEGRO_ALIGN_CENTER, "Iniciar Jogo");
    }

    if (font_tutorial_menu) {
        float tutorial_x = LARGURA_TELA - 20;
        float tutorial_y = ALTURA_TELA - 150;

        al_draw_text(font_tutorial_menu, tutorial_color, tutorial_x, tutorial_y, ALLEGRO_ALIGN_RIGHT, "Tutorial:");
        al_draw_text(font_tutorial_menu, tutorial_color, tutorial_x, tutorial_y + 25, ALLEGRO_ALIGN_RIGHT, "Mover: Teclas W, A, S, D");
        al_draw_text(font_tutorial_menu, tutorial_color, tutorial_x, tutorial_y + 50, ALLEGRO_ALIGN_RIGHT, "Lanterna: Tecla F");
        al_draw_text(font_tutorial_menu, tutorial_color, tutorial_x, tutorial_y + 75, ALLEGRO_ALIGN_RIGHT, "Reiniciar (apos Game Over): Tecla R");
        al_draw_text(font_tutorial_menu, tutorial_color, tutorial_x, tutorial_y + 100, ALLEGRO_ALIGN_RIGHT, "Sair do Jogo: Tecla ESC");
    } else {
        al_draw_text(al_create_builtin_font(), tutorial_color, LARGURA_TELA - 20, ALTURA_TELA - 150, ALLEGRO_ALIGN_RIGHT, "Tutorial: Mover (WASD), Lanterna (F), Sair (ESC)");
    }
}

void game_menu_shutdown() {}
