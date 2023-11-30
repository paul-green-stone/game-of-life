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

    /* ================================ */

    font = Font_load("montserrat.regular.ttf", 16);

    fps_text = Text_new("fps:", font);
    generation_text = Text_new("gen:", font);

    /* ================================ */

    while (running) {
        Timer_tick(g_timer);
        Timer_tick(world->clock);

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
            LilEn_set_colorRGB(world->c_color[0], world->c_color[1], world->c_color[2], world->c_color[3]);
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

        if (Timer_is_ready(world->clock)) {

            Timer_reset(world->clock);

            World_evolve(world);
        }
    }

    Font_unload(font);

    return ;
}