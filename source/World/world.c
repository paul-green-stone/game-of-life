#include "../include.h"

#define LOAD 0
#define SAVE 1

/* Maximum filename size */
#define MAX_FILENAME 32

/* ================================================================ */
/* ============================ STATIC ============================ */
/* ================================================================ */

#define CELL 4          /* Default cell size */
#define WIDTH 400       /* Default width */
#define HEIGHT 400      /* Defaul height */

/* Default world */
static const struct world WORLD  = {
    .cell_size = CELL,
    .width = WIDTH,
    .height = HEIGHT,
    .is_grid = 1,
    .g_color = {255, 0, 0, 255},
    .c_color = {0, 0, 0, 25},
    .type = 1,
    .rate = 10,
    .start = (int) ((WIDTH / CELL) * (HEIGHT / CELL) * .4),
};

#undef CELL
#undef WIDTH
#undef HEIGH

/* ================================================================ */

/**
 * Read a .json file containing information about the world you are going to examine.
 * Initialize the world with with data just read.
*/
static int read_file(const char* filename, const World_t world) {

    /* Parsed JSON object */
    cJSON* root = NULL;

    /* A single piece of information extracted from a root */
    cJSON* data = NULL;

    if (world == NULL) {
        return EXIT_FAILURE;
    }

    /* ======================= Parse the data ========================= */
    if ((root = LilEn_read_json(filename)) == NULL) {
        goto CLEANUP;
    }

    /* ==================== Retrieving a cell size ==================== */
    data = (cJSON*) Data_read("cell_size", root, cJSON_IsNumber);
    world->cell_size = (data) ? data->valueint : 4;

    /* ================= Retrieving width and height ================== */
    data = (cJSON*) Data_read("width", root, cJSON_IsNumber);
    world->width = (data) ? data->valueint : 400;

    data = (cJSON*) Data_read("height", root, cJSON_IsNumber);
    world->height = (data) ? data->valueint : 400;

    /* ================= Adjusting sizes of the world ================= */
    world->rows = world->height / world->cell_size;
    world->columns = world->width / world->cell_size;

    int r = world->width % world->cell_size;

    if (r) {
        world->width -= r;
    }

    r = world->height % world->cell_size;

    if (r) {
        world->height -= r;
    }

    /* ==================== Retrieving start info ===================== */
    data = (cJSON*) Data_read("start", root, cJSON_IsNumber);

    /* Out of the whole number of cells, make approximately 10% of them to be alive */
    world->start = (data) ? data->valueint : (int) (world->rows * world->columns * 0.1);

    /* ===================== Retrieving grid info ===================== */
    data = (cJSON*) Data_read("grid", root, cJSON_IsNumber);
    world->is_grid = (data) ? data->valueint : 1;

    /* ====================== Retrieving colors ======================= */
    data = (cJSON*) Data_read("cell_color", root, cJSON_IsArray);

    if (cJSON_GetArraySize(data) >= 4) {

        world->c_color[0] = cJSON_GetArrayItem(data, 0)->valueint;
        world->c_color[1] = cJSON_GetArrayItem(data, 1)->valueint;
        world->c_color[2] = cJSON_GetArrayItem(data, 2)->valueint;
        world->c_color[3] = cJSON_GetArrayItem(data, 3)->valueint;
    }
    else {

        world->c_color[0] = 255;
        world->c_color[1] = 0;
        world->c_color[3] = 0;
        world->c_color[2] = 255;
    }

    /* ================================ */

    data = (cJSON*) Data_read("grid_color", root, cJSON_IsArray);

    if (cJSON_GetArraySize(data) >= 4) {

        world->g_color[0] = cJSON_GetArrayItem(data, 0)->valueint;
        world->g_color[1] = cJSON_GetArrayItem(data, 1)->valueint;
        world->g_color[2] = cJSON_GetArrayItem(data, 2)->valueint;
        world->g_color[3] = cJSON_GetArrayItem(data, 3)->valueint;
    }
    else {
        world->g_color[0] = 255;
        world->g_color[1] = 255;
        world->g_color[3] = 255;
        world->g_color[2] = 255;
    }

    /* ==================== Retrieving world type ===================== */
    data = (cJSON*) Data_read("type", root, cJSON_IsNumber);
    world->type = (data) ? data->valueint : 1;

    /* ==================== Retrieving world rate ===================== */
    data = (cJSON*) Data_read("rate", root, cJSON_IsNumber);
    world->rate = (data) ? data->valueint : 5;

    load_2D_array(root, world->current, world->rows, world->columns);

    /* ================================ */

    cJSON_Delete(root);

    return EXIT_SUCCESS;

    /* ================================ */

    { CLEANUP:

        LilEn_print_error();

        if (root != NULL) {
            cJSON_Delete(root);
        }

        return EXIT_FAILURE;
    }
}



/* ================================================================ */
/* ============================ EXTERN ============================ */
/* ================================================================ */

World_t World_new(void) {

    World_t world = NULL;
    cJSON* root = NULL;

    if (file_exists("world.json") == 0) {
        
        /* Read from the file */
        if ((root = LilEn_read_json("world.json")) == NULL) {

            /* Something went wrong. Using a fallback */
            LilEn_print_error();
        }


    }
    else {

        /* Allocating a new instance of a world */
        if ((world = (World_t) calloc(1, sizeof(struct world))) == NULL) {
            goto CLEANUP;
        }
    }

    /* ================================================================ */
    /* ============= Allocating a new instance of a world ============= */
    /* ================================================================ */

    if ((world = (World_t) calloc(1, sizeof(struct world))) == NULL) {
        goto CLEANUP;
    }

    /* ================================ */

    if (read_file("source/World/default.json", world) == EXIT_FAILURE) {
        goto CLEANUP;
    }

    /* ================================ */

    if ((world->current = allocate_2D_array(world->rows, world->columns)) == NULL) {
        goto CLEANUP;
    }

    if ((world->previous = allocate_2D_array(world->rows, world->columns)) == NULL) {
        goto CLEANUP;
    }

    /* ====================== Creating a clock  ======================= */
    if ((world->clock = Timer_new()) == NULL) {
        goto CLEANUP;
    }

    Timer_set(world->clock, 1.0f / world->rate);

    /* ================================================================ */

    return world;

    /* ================================================================ */
    /* ========================= Cleaning up ========================== */
    /* ================================================================ */

    { CLEANUP:

        LilEn_print_error();

        if (world != NULL) {
            World_destroy(&world);
        }

        return NULL;
    }
}

/* ================================================================ */

int World_load(const char* filename, const World_t w) {

    if (filename == NULL) {
        return EXIT_FAILURE;
    }

    read_file(filename, w);

    w->start = 0;

    return EXIT_SUCCESS;
}

/* ================================================================ */

void World_log(const World_t w) {

    if (w == NULL) {
        return ;
    }

    printf("%-16s: %ld\n", "cell size", w->cell_size);
    printf("%-16s: %d\n", "width", w->width);
    printf("%-16s: %d\n", "height", w->height);
    printf("%-16s: %s\n", "grid", (w->is_grid) ? "yes" : "no");
    printf("%-16s: %ld\n", "rows", w->rows);
    printf("%-16s: %ld\n", "columns", w->columns);
    printf("%-16s: %ld\n", "number of cells", w->columns * w->rows);
    printf("%-16s: %.0f (%.2f)\n", "rate", 1.0f / w->clock->time, w->clock->time);
    printf("%-16s: %ld\n", "generation", w->generation);

    printf("%-16s: %s (%d)\n", "type", (w->type == 1) ? "wrap around" : (w->type == 2) ? "dead" : "alive", w->type);

    printf("%-16s: [%d, %d, %d, %d]\n", "cell color", w->c_color[0], w->c_color[1], w->c_color[2], w->c_color[3]);
    printf("%-16s: [%d, %d, %d, %d]\n", "grid color", w->g_color[0], w->g_color[1], w->g_color[2], w->g_color[3]);

    return ;
}

/* ================================================================ */

int World_destroy(World_t* w) {

    if ((w == NULL) || (*w == NULL)) {
        return EXIT_FAILURE;
    }

    deallocate_2D_array(&(*w)->current, (*w)->rows);

    deallocate_2D_array(&(*w)->previous, (*w)->rows);

    free(*w);

    Timer_destroy(&(*w)->clock);

    *w = NULL;

    return EXIT_SUCCESS;
}

/* ================================================================ */

void World_log_generation(const World_t w) {

    size_t row = 0;
    size_t column = 0;

    if (w == NULL) {
        return ;
    }

    for (row = 0; row < w->rows; row++) {

        for (column = 0; column < w->columns; column++) {

            printf("%d", w->current[row][column]);

            if (column + 1 < w->columns) {
                printf(", ");
            }
        }

        printf("\n");
    }

    return ;
}

/* ================================================================ */  

void World_present(const World_t world, const Window_t w) {

    size_t row, column;
    SDL_Rect cell = {.w = world->cell_size, .h = world->cell_size};

    if ((w == NULL) && (g_window == NULL)) {
        return ;
    }

    if (world == NULL) {
        return ;
    }

    for (column = 0; column < world->columns; column++) {

        cell.x = column * world->cell_size;

        for (row = 0; row < world->rows; row++) {

            cell.y = row * world->cell_size;

            if (world->current[row][column]) {
                LilEn_draw_rect(w, &cell);
            }
        }
    }

    return ;
}

/* ================================================================ */

void World_randomize(const World_t w, int c) {

    size_t i = 0;
    size_t num = 0;
    size_t row;
    size_t column;

    if (w == NULL) {
        return ;
    }

    if (c < 0) {
        return ;
    }

    num = c;

    for (i = 0; i < num; i++) {

        row = RAND_RANGE(0, w->rows - 1);
        column = RAND_RANGE(0, w->columns - 1);

        w->current[row][column] = 1;
    }

    return ;
}

/* ================================================================ */

void World_evolve(const World_t w) {

    int row = 0;
    int rows = 0;

    int column = 0;
    int columns = 0;

    /* Accumulator */
    int acc = 0;

    if (w == NULL) { 
        return ;
    }

    rows = w->rows;
    columns = w->columns;

    swap_2D_array(w->current, w->previous, w->rows, w->columns);

    for (row = 0; row < rows; row++) {

        for (column = 0; column < columns; column++) {

            acc 
                = w->previous[(row - 1 < 0) ? rows - 1 : row - 1][column]
                + w->previous[(row - 1 < 0) ? rows - 1 : row - 1][(column + 1 >= columns) ? 0 : column + 1]
                + w->previous[row][(column + 1 >= columns) ? 0 : column + 1]
                + w->previous[(row + 1 >= rows) ? 0 : row + 1][(column + 1 >= columns) ? 0 : column + 1]
                + w->previous[(row + 1 >= rows) ? 0 : row + 1][column]
                + w->previous[(row + 1 >= rows) ? 0 : row + 1][(column - 1 < 0) ? columns - 1 : column - 1]
                + w->previous[row][((column - 1 <= 0) ? columns - 1 : column - 1)]
                + w->previous[(row - 1 < 0) ? rows - 1 : row - 1][(column - 1 < 0) ? columns - 1 : column - 1];
            
            /* Any live cell with fewer than two live neighbours dies, as if by underpopulation. */
            if (w->previous[row][column] == 1 && acc < 2) {
                w->current[row][column] = 0;
            }
            /* Any live cell with two or three live neighbours lives on to the next generation. */
            else if ((w->previous[row][column] == 1) && (acc == 2 || 2 == 3)) {
                w->current[row][column] = 1;
            }
            /* Any live cell with more than three live neighbours dies, as if by overpopulation. */
            else if (w->previous[row][column] == 1 && acc > 3) {
                w->current[row][column] = 0;
            }
            /* Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction. */
            else if (w->previous[row][column] == 0 && acc == 3) {
                w->current[row][column] = 1;
            }
            else {
                w->current[row][column]= w->previous[row][column];
            }
        }
    }

    w->generation++;

    return ;
}

/* ================================================================ */

int World_save(const char* filename, const World_t w) {

    FILE* file = NULL;
    cJSON* root = NULL;
    cJSON* data = NULL;
    cJSON* array = NULL;

    size_t i = 0;

    if (w == NULL) {
        return EXIT_FAILURE;
    }

    if ((root = cJSON_CreateObject()) == NULL) {
        return EXIT_FAILURE;
    }

    if ((file = create_file(filename)) == NULL) {
        
        cJSON_Delete(root);

        return EXIT_FAILURE;
    }

    /* ================================ */

    data = (data = cJSON_CreateNumber(w->cell_size)) ? data : NULL;
    cJSON_AddItemToObject(root, "cell_size", data);

    data = (data = cJSON_CreateNumber(w->width)) ? data : NULL;
    cJSON_AddItemToObject(root, "width", data);

    data = (data = cJSON_CreateNumber(w->height)) ? data : NULL;
    cJSON_AddItemToObject(root, "height", data);

    data = (data = cJSON_CreateNumber(w->is_grid)) ? data : NULL;
    cJSON_AddItemToObject(root, "is_grid", data);

    data = (data = cJSON_CreateNumber(w->type)) ? data : NULL;
    cJSON_AddItemToObject(root, "type", data);

    data = (data = cJSON_CreateNumber(w->rate)) ? data : NULL;
    cJSON_AddItemToObject(root, "rate", data);



    if ((array = cJSON_CreateArray()) == NULL) {

        fprintf(file, "%s", cJSON_Print(root));

        cJSON_Delete(root);
        fclose(file);

        return EXIT_FAILURE;
    }

    for (i = 0; i < sizeof(w->c_color) / (sizeof(w->c_color[0])); i++) {

        data = (data = cJSON_CreateNumber(w->c_color[i])) ? data : NULL;
        cJSON_AddItemToArray(array, data);
    }

    cJSON_AddItemToObject(root, "cell_color", array);



    if ((array = cJSON_CreateArray()) == NULL) {

        fprintf(file, "%s", cJSON_Print(root));

        cJSON_Delete(root);
        fclose(file);

        return EXIT_FAILURE;
    }

    for (i = 0; i < sizeof(w->g_color) / (sizeof(w->g_color[0])); i++) {

        data = (data = cJSON_CreateNumber(w->g_color[i])) ? data : NULL;
        cJSON_AddItemToArray(array, data);
    }

    cJSON_AddItemToObject(root, "grid_color", array);

    save_2D_array(root, "current", w->current, w->rows, w->columns);

    /* ================================ */

    fprintf(file, "%s", cJSON_Print(root));

    cJSON_Delete(root);
    fclose(file);

    return EXIT_SUCCESS;
}

/* ================================================================ */
