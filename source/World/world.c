#include "../include.h"

#define LOAD 0
#define SAVE 1

/* ================================================================ */
/* ============================ STATIC ============================ */
/* ================================================================ */

static const cJSON* Data_load(const char* name, const cJSON* root, cJSON_bool (check)(const cJSON* const)) {

    cJSON* data = NULL;

    return
        (root == NULL) ? 
            NULL : (name == NULL) ? 
                root : (check((data = cJSON_GetObjectItemCaseSensitive(root, name)))) ? 
                    data : NULL;
}

/**
 * Deallocate a 2-dimensional array
*/
static void deallocate_2D_array(unsigned char*** array, int rows) {

    size_t i = 0;

    if (rows < 0) {
        return ;
    }

    for (i = 0; i < (size_t) rows; i++) {
        free((*array)[i]);
    }

    free(*array);

    *array = NULL;

    return ;
}

/* ================================================================ */

/**
 * Dynamically allocate a 2-dimensional array of size row*columns
*/
static unsigned char** allocate_2D_array(int rows, int columns) {

    unsigned char** array = NULL;
    size_t i = 0;

    /* Preventing an overflow */
    if ((rows < 0) || (columns < 0)) {
        return array;
    }

    /* ======================= Array allocation ======================= */
    if ((array = (unsigned char**) calloc(rows, sizeof(unsigned char*))) == NULL) {

        LilEn_print_error();
        deallocate_2D_array(&array, rows);

        return NULL;
    }

    for (i = 0; i < (size_t) rows; i++) {

        if ((array[i] = (unsigned char*) calloc(columns, sizeof(unsigned char))) == NULL) {

            LilEn_print_error();
            deallocate_2D_array(&array, rows);

            return NULL;
        }
    }

    return array;
}

/* ================================================================ */

static int swap_2D_array(const World_t w) {

    size_t row = 0;
    size_t column = 0;

    unsigned char temp = 0;

    if (w == NULL) {
        return EXIT_FAILURE;
    }

    for (row = 0; row < w->rows; row++) {

        for (column = 0; column < w->columns; column++) {

            temp = w->previous[row][column];
            w->previous[row][column] = w->current[row][column];
            w->current[row][column] = temp;
        }
    }

    return EXIT_SUCCESS;
}

/* ================================================================ */

static int read_file(const char* filename, const World_t world) {

    /* Buffer containing file content */
    char* input = NULL;
    /* Parsed JSON object */
    cJSON* root = NULL;

    cJSON* data = NULL;

    if (world == NULL) {
        return EXIT_FAILURE;
    }

    /* ============== Open a file with default settings =============== */
    if ((input = LilEn_read_data_file(filename)) == NULL) {
        goto CLEANUP;
    }

    /* ======================= Parse the data ========================= */
    if ((root = cJSON_Parse(input)) == NULL) {
        goto CLEANUP;
    }

    /* ==================== Retrieving a cell size ==================== */
    data = (cJSON*) Data_load("cell_size", root, cJSON_IsNumber);
    world->cell_size = (data) ? data->valueint : 4;

    /* ================= Retrieving width and height ================== */
    data = (cJSON*) Data_load("width", root, cJSON_IsNumber);
    world->width = (data) ? data->valueint : 400;

    data = (cJSON*) Data_load("height", root, cJSON_IsNumber);
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
    data = (cJSON*) Data_load("start", root, cJSON_IsNumber);

    /* Out of the whole number of cells, make approximately 10% of them to be alive */
    world->start = (data) ? data->valueint : (int) (world->rows * world->columns * 0.1);

    /* ===================== Retrieving grid info ===================== */
    data = (cJSON*) Data_load("grid", root, cJSON_IsNumber);
    world->is_grid = (data) ? data->valueint : 1;

    /* ====================== Retrieving colors ======================= */
    data = (cJSON*) Data_load("cell_color", root, cJSON_IsArray);

    if (cJSON_GetArraySize(data) >= 4) {
        world->c_color = (SDL_Color) {cJSON_GetArrayItem(data, 0)->valueint, cJSON_GetArrayItem(data, 1)->valueint, cJSON_GetArrayItem(data, 2)->valueint, cJSON_GetArrayItem(data, 3)->valueint};
    }
    else {
        world->c_color = (SDL_Color) {255, 0, 0, 255};
    }

    /* ================================ */

    data = (cJSON*) Data_load("grid_color", root, cJSON_IsArray);

    if (cJSON_GetArraySize(data) >= 4) {
        world->g_color = (SDL_Color) {cJSON_GetArrayItem(data, 0)->valueint, cJSON_GetArrayItem(data, 1)->valueint, cJSON_GetArrayItem(data, 2)->valueint, cJSON_GetArrayItem(data, 3)->valueint};
    }
    else {
        world->g_color = (SDL_Color) {0, 0, 0, 255};
    }

    /* ==================== Retrieving world type ===================== */
    data = (cJSON*) Data_load("type", root, cJSON_IsNumber);
    world->type = (data) ? data->valueint : 1;

    /* ==================== Retrieving world rate ===================== */
    data = (cJSON*) Data_load("rate", root, cJSON_IsNumber);
    world->rate = (data) ? data->valueint : 5;

    /* ================================ */

    free(input);
    cJSON_Delete(root);

    return EXIT_SUCCESS;

    /* ================================ */

    { CLEANUP:

        LilEn_print_error();

        if (input != NULL) {
            free(input);
        }

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

    }

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
    printf("%-16s: %.0f (%.2f)\n", "rate", 1.0f / w->clock->time, w->clock->time);

    printf("%-16s: %s (%d)\n", "type", (w->type == 1) ? "wrap around" : (w->type == 2) ? "dead" : "alive", w->type);

    printf("%-16s: [%d, %d, %d, %d]\n", "cell color", w->c_color.r, w->c_color.g, w->c_color.b, w->c_color.a);
    printf("%-16s: [%d, %d, %d, %d]\n", "grid color", w->g_color.g, w->g_color.g, w->g_color.b, w->g_color.a);

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

    swap_2D_array(w);

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

    return ;
}

/* ================================================================ */
