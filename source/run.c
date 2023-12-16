#include "include.h"

/* ================================================================ */


void World_run(const World_t world) {

    SDL_Event e;
    int running = 1;

    char fps_b[32];
    Text_t fps_text = NULL;

    char generation_b[32];
    Text_t generation_text = NULL;

    TTF_Font* font = NULL;

    Timer_t delay = Timer_new();
    int start = 0;

    /* ================================ */

    font = Font_load("montserrat.regular.ttf", 12);

    fps_text = Text_new("fps:", font);
    generation_text = Text_new("gen:", font);

    /* ================================ */

    Timer_set(delay, 3);

    while (running) {
        Timer_tick(g_timer);
        Timer_tick(world->clock);
        Timer_tick(delay);

        while (SDL_PollEvent(&e)) {

            switch (e.type) {

                case SDL_QUIT:

                    running = !running;

                    break ;
            }
        }

        if (Timer_is_ready(g_timer)) {
            
            /* ======================= Background color ======================= */
            LilEn_set_colorRGB(world->bg_color[0], world->bg_color[1], world->bg_color[2], world->bg_color[3]);
            Window_clear(NULL);

            /* ========================== Cell color ========================== */
            World_present(world, NULL);

            /* ========================= Grid drawing ========================= */
            if (world->is_grid) {

                LilEn_set_colorRGB(world->g_color[0], world->g_color[1], world->g_color[2], world->g_color[3]);

                Window_display_grid(NULL, world->cell_size);
            }

            /* ======================== Text updating ========================= */
            sprintf(fps_b, "fps: %.1f", 1.0f / g_timer->acc);
            sprintf(generation_b, "gen: %ld", world->generation);

            LilEn_set_colorRGB(world->text_color[0], world->text_color[1], world->text_color[2], world->text_color[3]);
            Text_update(fps_text, fps_b, font);
            Text_update(generation_text, generation_b, font);

            fps_text->position.x = world->width - (fps_text->position.w + 32);
            fps_text->position.y = world->height - (fps_text->position.h + 32);

            generation_text->position.x = world->width - (generation_text->position.w + 32);
            generation_text->position.y = world->height - (generation_text->position.h + 16);
            
            Text_display(fps_text, NULL);
            Text_display(generation_text, NULL);

            /* ======================== Window update ========================= */
            Window_update(NULL);

            Timer_reset(g_timer);
        }

        if (start) {

            if (Timer_is_ready(world->clock)) {

                Timer_reset(world->clock);

                World_evolve(world);
            }
        }

        if (Timer_is_ready(delay)) {
            start = !start;
        }
    }

    Text_destroy(&fps_text);
    Text_destroy(&generation_text);

    Font_unload(font);

    return ;
}

/* ================================================================ */

void World_edit(const World_t world) {

    SDL_Event e;
    int running = 1;

    SDL_Rect rect = {.w = world->cell_size, .h = world->cell_size};             /* Cursor position */

    SDL_Color current_cell_color = {
        .r = world->c_color[0][0],
        .g = world->c_color[0][1],
        .b = world->c_color[0][2],
        .a = world->c_color[0][3],
    };

    unsigned char modifier = 0;

    while (running) {
        Timer_tick(g_timer);

        while (SDL_PollEvent(&e)) {

            switch (e.type) {

                case SDL_QUIT:

                    running = !running;

                    break ;

                case SDL_MOUSEMOTION:

                    SDL_GetMouseState(&rect.x, &rect.y);

                    rect.x -= rect.x % rect.w;
                    rect.y -= rect.y % rect.h;

                    break ;

                case SDL_MOUSEBUTTONDOWN:

                    if (e.button.button == SDL_BUTTON_LEFT) {
                        world->current[rect.y / rect.h][rect.x / rect.w] = (world->current[rect.y / rect.h][rect.x / rect.w] == 0) ? 1 + modifier : 0;
                    }

                    break ;

                case SDL_KEYDOWN:

                    if (e.key.repeat == 0) {

                        if (e.key.keysym.scancode == SDL_SCANCODE_1) {
                            
                            if (world->colors >= 1) {
                                current_cell_color = (SDL_Color) {world->c_color[0][0], world->c_color[0][1], world->c_color[0][2], world->c_color[0][3]};

                                modifier = 0;
                            }
                        }

                        if (e.key.keysym.scancode == SDL_SCANCODE_2) {
                            
                            if (world->colors >= 2) {
                                current_cell_color = (SDL_Color) {world->c_color[1][0], world->c_color[1][1], world->c_color[1][2], world->c_color[1][3]};

                                modifier = 1;
                            }
                        }

                        if (e.key.keysym.scancode == SDL_SCANCODE_3) {
                            
                            if (world->colors >= 2) {
                                current_cell_color = (SDL_Color) {world->c_color[2][0], world->c_color[2][1], world->c_color[2][2], world->c_color[2][3]};

                                modifier = 2;
                            }
                        }

                        if (e.key.keysym.scancode == SDL_SCANCODE_4) {
                            
                            if (world->colors >= 3) {
                                current_cell_color = (SDL_Color) {world->c_color[3][0], world->c_color[3][1], world->c_color[3][2], world->c_color[3][3]};

                                modifier = 3;
                            }
                        }

                        if (e.key.keysym.scancode == SDL_SCANCODE_5) {
                            
                            if (world->colors >= 4) {
                                current_cell_color = (SDL_Color) {world->c_color[4][0], world->c_color[4][1], world->c_color[4][2], world->c_color[4][3]};

                                modifier = 4;
                            }
                        }

                        if (e.key.keysym.scancode == SDL_SCANCODE_6) {
                            
                            if (world->colors >= 5) {
                                current_cell_color = (SDL_Color) {world->c_color[5][0], world->c_color[5][1], world->c_color[5][2], world->c_color[5][3]};

                                modifier = 5;
                            }
                        }

                        if (e.key.keysym.scancode == SDL_SCANCODE_7) {
                            
                            if (world->colors >= 6) {
                                current_cell_color = (SDL_Color) {world->c_color[6][0], world->c_color[6][1], world->c_color[6][2], world->c_color[6][3]};

                                modifier = 6;
                            }
                        }

                        if (e.key.keysym.scancode == SDL_SCANCODE_8) {
                            
                            if (world->colors >= 7) {
                                current_cell_color = (SDL_Color) {world->c_color[7][0], world->c_color[7][1], world->c_color[7][2], world->c_color[7][3]};

                                modifier = 7;
                            }
                        }

                        if (e.key.keysym.scancode == SDL_SCANCODE_9) {
                            
                            if (world->colors >= 8) {
                                current_cell_color = (SDL_Color) {world->c_color[8][0], world->c_color[8][1], world->c_color[8][2], world->c_color[8][3]};

                                modifier = 8;
                            }
                        }
                    }

                    break ;
            }
        }

        if (Timer_is_ready(g_timer)) {
            
            /* ======================= Background color ======================= */
            LilEn_set_colorRGB(world->bg_color[0], world->bg_color[1], world->bg_color[2], world->bg_color[3]);
            Window_clear(NULL);

            /* =========================== On hover =========================== */
            LilEn_set_colorRGB(current_cell_color.r, current_cell_color.g, current_cell_color.b, 127);
            LilEn_draw_rect(NULL, &rect);

            /* ========================== Cell color ========================== */
            LilEn_set_colorRGB(current_cell_color.r, current_cell_color.g, current_cell_color.b, current_cell_color.a);
            World_present(world, NULL);

            /* ========================= Grid drawing ========================= */
            if (world->is_grid) {

                LilEn_set_colorRGB(world->g_color[0], world->g_color[1], world->g_color[2], world->g_color[3]);

                Window_display_grid(NULL, world->cell_size);
            }

            /* ======================== Window update ========================= */
            Window_update(NULL);

            Timer_reset(g_timer);
        }
    }

    return ;
}
