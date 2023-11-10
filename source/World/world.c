#include "../include.h"

/* ================================================================ */
/* ============================ STATIC ============================ */
/* ================================================================ */

/**
 * Deallocate a 2-dimensional array
*/
static void deallocate_2D_array(unsigned char*** array, int rows) {

    size_t i = 0;

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
    if ((rows < 0) && (columns < 0)) {
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

    // for (i = 0; i < rows; i++) {

    //     for (j = 0; j < columns; j++) {
    //         array[i][j] = 0;
    //     }
    // }

    return array;
}

/* ================================================================ */

static int read_file(const char* filename, const World_t world) {

    /* Buffer containing file content */
    char* input = NULL;
    /* Parsed JSON object */
    cJSON* root = NULL;

    /* A piece of data extracted from a document */
    cJSON* data = NULL;

    size_t i = 0;

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
    if ((data = cJSON_GetObjectItemCaseSensitive(root, "cell_size")) == NULL) {
        goto CLEANUP;
    }

    if (!cJSON_IsNumber(data)) {
        goto CLEANUP;
    }

    world->cell_size = data->valueint;

    /* ================= Retrieving width and height ================== */
    if ((data = cJSON_GetObjectItemCaseSensitive(root, "width")) == NULL) {
        goto CLEANUP;
    }

    if (!cJSON_IsNumber(data)) {
        goto CLEANUP;
    }

    world->width = data->valueint;

    /* ================================ */

    if ((data = cJSON_GetObjectItemCaseSensitive(root, "height")) == NULL) {
        goto CLEANUP;
    }

    if (!cJSON_IsNumber(data)) {
        goto CLEANUP;
    }

    world->height = data->valueint;

    /* ===================== Retrieving grid info ===================== */
    if ((data = cJSON_GetObjectItemCaseSensitive(root, "is_grid")) == NULL) {
        goto CLEANUP;
    }

    if (!cJSON_IsNumber(data)) {
        goto CLEANUP;
    }

    world->is_grid = data->valueint;

    /* ====================== Retrieving colors ======================= */
    size_t array_size = 0;

    if ((data = cJSON_GetObjectItemCaseSensitive(root, "cell_color")) == NULL) {
        goto CLEANUP;
    }

    if (!cJSON_IsArray(data)) {
        goto CLEANUP;
    }

    /* Get the size of the retrieved array */
    array_size = (size_t) cJSON_GetArraySize(data);

    for (i = 0; i < array_size; i++) {

        cJSON* item = cJSON_GetArrayItem(data, i);

        if (item == NULL) {
            continue ;
        }

        switch (i) {

            case 0:
                world->c_color.r = item->valueint;
                break ;

            case 1:
                world->c_color.g = item->valueint;
                break ;

            case 2:
                world->c_color.b = item->valueint;
                break ;

            case 3:
                world->c_color.a = item->valueint;
                break ;
        }
    }

    /* ================================ */

    if ((data = cJSON_GetObjectItemCaseSensitive(root, "grid_color")) == NULL) {
        goto CLEANUP;
    }

    if (!cJSON_IsArray(data)) {
        goto CLEANUP;
    }

    /* Get the size of the retrieved array */
    array_size = (size_t) cJSON_GetArraySize(data);

    for (i = 0; i < array_size; i++) {

        cJSON* item = cJSON_GetArrayItem(data, i);

        if (item == NULL) {
            continue ;
        }

        switch (i) {

            case 0:
                world->g_color.r = item->valueint;
                break ;

            case 1:
                world->g_color.g = item->valueint;
                break ;

            case 2:
                world->g_color.b = item->valueint;
                break ;

            case 3:
                world->g_color.a = item->valueint;
                break ;
        }
    }

    /* ==================== Retrieving world type ===================== */
    if ((data = cJSON_GetObjectItemCaseSensitive(root, "type")) == NULL) {
        goto CLEANUP;
    }

    if (!cJSON_IsNumber(data)) {
        goto CLEANUP;
    }

    world->type = data->valueint;

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

    /* ==================== Retrieving world rate ===================== */
    if ((data = cJSON_GetObjectItemCaseSensitive(root, "rate")) == NULL) {
        goto CLEANUP;
    }

    if (!cJSON_IsNumber(data)) {
        goto CLEANUP;
    }

    world->rate = data->valueint;

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
