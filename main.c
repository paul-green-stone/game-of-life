#include "source/include.h"

int main(int argc, char** argv) {

    World_t world = World_new();
    Window_t window = NULL;
    SDL_Event e;
    int running = 1;

    srand((unsigned) time(0));

    if ((LilEn_init("settings.json")) == EXIT_FAILURE) {

        LilEn_print_error();

        exit(EXIT_FAILURE);
    }

    if ((window = Window_new("Game of Life", world->width, world->height, SDL_WINDOW_SHOWN, SDL_RENDERER_ACCELERATED)) == NULL) {

        LilEn_print_error();
        LilEn_quit();

        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);

    if (world->start > 0) {
        World_randomize(world, world->start);
    }

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

            LilEn_log_FPS();

            LilEn_set_colorHEX(0xffffff);

            Window_clear(NULL);

            LilEn_set_colorRGB(world->c_color.r, world->c_color.g, world->c_color.b, world->c_color.a);
            World_present(world, NULL);

            if (world->is_grid) {

                LilEn_set_colorRGB(world->g_color.r, world->g_color.g, world->g_color.b, world->g_color.a);

                Window_display_grid(NULL, world->cell_size);
            }

            Window_update(window);

            Timer_reset(g_timer);
        }

        if (Timer_is_ready(world->clock)) {

            Timer_reset(world->clock);

            World_evolve(world);
        }
    }

    LilEn_quit();

    World_destroy(&world);

    return EXIT_SUCCESS;
}