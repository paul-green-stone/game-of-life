#include "../include.h"

#define LOAD 0
#define SAVE 1

/* ================================================================ */
/* ============================ STATIC ============================ */
/* ================================================================ */

static int check(int m, const World_t w, const cJSON* r) {

    if ((m < 0) || (m > 1)) {
        return EXIT_FAILURE;
    }
    
    if (w == NULL) {
        return EXIT_FAILURE;
    }

    if (r == NULL) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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

static int Do_cell_size(int mode, const World_t world, const cJSON* root) {

    cJSON* data = NULL;
    
    /* ======================= Check input data ======================= */
    if (check(mode, world, root) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    switch (mode) {

        case LOAD:
            
            /* =================== Loading data from a file =================== */
            if ((data = cJSON_GetObjectItemCaseSensitive(root, "cell_size")) == NULL) {
                return EXIT_FAILURE;
            }

            if (!cJSON_IsNumber(data)) {
                return EXIT_FAILURE;
            }

            world->cell_size = data->valueint;

            break ;

        case SAVE:

            /* =================== Saving data into a file ==================== */
            break ;
    }

    return EXIT_SUCCESS;
}

/* ================================================================ */

static int Do_dimensions(int mode, const World_t world, const cJSON* root) {

    cJSON* data = NULL;
    
    /* ======================= Check input data ======================= */
    if (check(mode, world, root) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    switch (mode) {

        case LOAD:  
            
            /* =================== Loading data from a file =================== */
            if ((data = cJSON_GetObjectItemCaseSensitive(root, "width")) == NULL) {
                return EXIT_FAILURE;
            }

            if (!cJSON_IsNumber(data)) {
                return EXIT_FAILURE;
            }

            world->width = data->valueint;

            if ((data = cJSON_GetObjectItemCaseSensitive(root, "height")) == NULL) {
                return EXIT_FAILURE;
            }

            if (!cJSON_IsNumber(data)) {
                return EXIT_FAILURE;
            }

            world->height = data->valueint;

            break ;

        case SAVE:

            /* =================== Saving data into a file ==================== */
            break ;
    }

    return EXIT_SUCCESS;
}

/* ================================================================ */

static int Do_grid(int mode, const World_t world, const cJSON* root) {

    cJSON* data = NULL;
    
    /* ======================= Check input data ======================= */
    if (check(mode, world, root) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    switch (mode) {

        case LOAD:
            
            /* =================== Loading data from a file =================== */
            if ((data = cJSON_GetObjectItemCaseSensitive(root, "is_grid")) == NULL) {
                return EXIT_FAILURE;
            }

            if (!cJSON_IsNumber(data)) {
                return EXIT_FAILURE;
            }

            world->is_grid = data->valueint;

            break ;

        case SAVE:

            /* =================== Saving data into a file ==================== */
            break ;
    }

    return EXIT_SUCCESS;
}

/* ================================================================ */

static int Do_colors(int mode, const World_t world, const cJSON* root) {

    cJSON* data = NULL;
    size_t array_size = 0;
    size_t i = 0;
    
    /* ======================= Check input data ======================= */
    if (check(mode, world, root) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    switch (mode) {

        case LOAD:
            
            /* =================== Loading data from a file =================== */
            if ((data = cJSON_GetObjectItemCaseSensitive(root, "cell_color")) == NULL) {
                return EXIT_FAILURE;
            }

            if (!cJSON_IsArray(data)) {
                return EXIT_FAILURE;
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
                return EXIT_FAILURE;
            }

            if (!cJSON_IsArray(data)) {
                return EXIT_FAILURE;
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

            break ;

        case SAVE:

            /* =================== Saving data into a file ==================== */
            break ;
    }

    return EXIT_SUCCESS;
}

/* ================================================================ */

static int Do_type(int mode, const World_t world, const cJSON* root) {

    cJSON* data = NULL;

    /* ======================= Check input data ======================= */
    if (check(mode, world, root) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    switch (mode) {

        case LOAD:
            
            /* =================== Loading data from a file =================== */
            if ((data = cJSON_GetObjectItemCaseSensitive(root, "type")) == NULL) {
                return EXIT_FAILURE;
            }

            if (!cJSON_IsNumber(data)) {
                return EXIT_FAILURE;
            }

            world->type = data->valueint;

            break ;

        case SAVE:

            /* =================== Saving data into a file ==================== */
            break ;
    }

    return EXIT_SUCCESS;
}

/* ================================================================ */

static int Do_rate(int mode, const World_t world, const cJSON* root) {

    cJSON* data = NULL;
    
    /* ======================= Check input data ======================= */
    if (check(mode, world, root) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    switch (mode) {

        case LOAD:
            
            /* =================== Loading data from a file =================== */
            if ((data = cJSON_GetObjectItemCaseSensitive(root, "rate")) == NULL) {
                return EXIT_FAILURE;
            }

            if (!cJSON_IsNumber(data)) {
                return EXIT_FAILURE;
            }

            world->rate = data->valueint;

            break ;

        case SAVE:

            /* =================== Saving data into a file ==================== */
            break ;
    }

    return EXIT_SUCCESS;
}
/* ================================================================ */

static int read_file(const char* filename, const World_t world) {

    /* Buffer containing file content */
    char* input = NULL;
    /* Parsed JSON object */
    cJSON* root = NULL;

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
    if (Do_cell_size(LOAD, world, root) == EXIT_FAILURE) {
        goto CLEANUP;
    }

    /* ================= Retrieving width and height ================== */
    if (Do_dimensions(LOAD, world, root) == EXIT_FAILURE) {
        goto CLEANUP;
    }

    /* ===================== Retrieving grid info ===================== */
    if (Do_grid(LOAD, world, root) == EXIT_FAILURE) {
        goto CLEANUP;
    }

    /* ====================== Retrieving colors ======================= */
    if (Do_colors(LOAD, world, root) == EXIT_FAILURE) {
        goto CLEANUP;
    }

    /* ==================== Retrieving world type ===================== */
    if (Do_type(LOAD, world, root) == EXIT_FAILURE) {
        goto CLEANUP;
    }

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
    if (Do_rate(LOAD, world, root) == EXIT_FAILURE) {
        goto CLEANUP;
    }

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
                w->previous[row][column] = 0;
            }
            /* Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction. */
            if (w->previous[row][column] == 0 && acc == 3) {
                w->previous[row][column] = 1;
            }
        }
    }

    return ;
}

/* ================================================================ */
