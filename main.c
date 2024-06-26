#include "source/include.h"

/* ================================================================ */

#define BUFF 32                                         /* Load/Save file's name length */

#define EXT ".json"                                     /* The only extension that can be used now */
#define JSONL 6                                         /* `.json\0` */

static char load_file[BUFF];                            /* Load file's name */
static char save_file[BUFF];                            /* Save file's name */

static char location[5 + BUFF] = "save/";               /* Final destination of a file */

static int is_edit = 0;

static int is_clean = 0;

/* ================================================================ */

int main(int argc, char** argv) {

    World_t world = NULL;
    Window_t window = NULL;

    if (dir_exists(location) == NULL) {
        dir_create(location, 0700);
    }

    /* ================================================================ */
    /* ===================== Command line parsing ===================== */
    /* ================================================================ */

    int option;

    while (1) {

        int option_index = 0;   

        static struct option long_options[] = {

            {"save", required_argument, NULL, 0},
            {"load", required_argument, NULL, 1},
            {"edit", no_argument, NULL, 2},
            {"clean", no_argument, NULL, 3},
            {NULL, 0, NULL, 4},
        };

        option = getopt_long(argc, argv, "-:", long_options, &option_index);

        if (option == -1) {
            break ;
        }

        switch (option) {

            char* sub = NULL;

            case 0:

                if (optarg) {

                    strncpy(save_file, optarg, BUFF - JSONL);

                    if ((sub = strstr(optarg, EXT)) == NULL) {
                            
                        strcat(save_file, EXT);
                        save_file[strlen(save_file)] = '\0';
                    }

                    save_file[strlen(save_file)] = '\0';
                }

                break ;

            case 1:

                if (optarg) {

                    strncpy(load_file, optarg, BUFF - JSONL);

                    if ((sub = strstr(optarg, EXT)) == NULL) {
                            
                        strcat(load_file, EXT);
                        save_file[strlen(load_file)] = '\0';
                    }

                    load_file[strlen(load_file)] = '\0';

                    strcat(location, load_file);
                }

                break ;

            case 2:
                is_edit = !is_edit;

                break ;

            case 3:
                is_clean = !is_clean;

                break ;

            case 4:

            case ':':
                printf("Missing option for %s\n", optarg);

                break ;

            case '?':
                
                printf("Unknown option %s\n", optarg);

                break ;
        }
    }

    /* ================================================================ */

    if ((LilEn_init("settings.json")) == EXIT_FAILURE) {

        LilEn_print_error();

        exit(EXIT_FAILURE);
    }

    if ((world = World_new()) == NULL) {
        LilEn_print_error();

        LilEn_quit();

        exit(EXIT_FAILURE);
    }

    if ((window = Window_new("Game of Life", world->width, world->height, SDL_WINDOW_SHOWN, SDL_RENDERER_ACCELERATED)) == NULL) {

        LilEn_print_error();
        LilEn_quit();

        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);

    if (strlen(load_file) > 0) {
        World_load(location, world);
    }

    if (is_edit) {

        if (is_clean) {
            clear_2D_array(world->current, world->rows, world->columns, 0);
        }

        World_edit(world);
    }
    else {
        World_run(world);
    }

    if (strlen(save_file) > 0) {

        /* Remove the old name */
        memset(location, '\0', strlen(location));

        /* Build a new save target */
        strcat(location, "save/");
        strcat(location, save_file);
        
        /* Save the World! */
        World_save(location, world);
    }

    World_log(world);

    LilEn_quit();

    World_destroy(&world);

    return EXIT_SUCCESS;
}
