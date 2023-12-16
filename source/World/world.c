#include "../include.h"

#define LOAD 0
#define SAVE 1

/* Maximum filename size */
#define MAX_FILENAME 32

#define LOCAL_BUF_SIZE 16

/* ================================================================ */
/* ============================ STATIC ============================ */
/* ================================================================ */

static int array_max_index(unsigned char* array, size_t size) {

    struct {
        unsigned char buffer[LOCAL_BUF_SIZE];
        unsigned short size;
        unsigned short current_position;
    } buffer = {
        .size = 0,
        .current_position = 0,
    };

    /* Loop variable */
    size_t i = 0;

    /* Index that contains the maximum array value */
    int max = 0;
    
    for (i = 0; i < size; i++) {
        max = (array[i] > array[max]) ? (int) i : max;
    }

    for (i = 0; i < size; i++) {

        if (array[i] == array[max]) {
            buffer.buffer[buffer.current_position++] = i;
            buffer.size++;
        }
    }

    if (buffer.size > 1) {
        max = buffer.buffer[RAND_RANGE(0, buffer.size - 1)];
    }

    return max;
}

/**
 * `value` is the cell value. If the cell is greater than 0, it means it's alive.
 * If we subtract 1 from a live cell, we get its color modifier. The size of the array
 * is determined by the number of colors available in the current application run; a user shouldn't
 * set it manually. The function stores a color modifier in the array
*/
static int World_get_color_modifers(unsigned char* array, int value) {

    if (value) {
        array[value - 1]++;
    }

    return 1;
}

static World_t _World_alloc(void) {

    World_t world = NULL;

    if ((world = (World_t) calloc(1, sizeof(struct world))) == NULL) {
        goto ERROR;
    }

    if ((world->clock = Timer_new()) == NULL) {
        goto ERROR;
    }

    return world;

    { ERROR:
        free(world);

        return NULL;
    }
}

/* ================================ */

#define CELL 4          /* Default cell size */
#define WIDTH 400       /* Default width */
#define HEIGHT 400      /* Defaul height */
#define PERCENT .4      /* By default,40% of cells are being tried to initialized */

/* Default world */
static const struct world WORLD  = {
    .cell_size = CELL,
    .width = WIDTH,
    .height = HEIGHT,
    .is_grid = 1,
    .g_color = {0, 0, 0, 25},
    .c_color = {
        {255, 0, 0, 255},   /* Red */
        {0, 255, 0, 255},   /* Green */
        {0, 0, 255, 255},   /* Blue */
    },
    .bg_color = {255, 255, 255, 255},
    .text_color = {0, 0, 0, 127},
    .type = 1,
    .rate = 10,
    .percent = PERCENT,
    .generation = 0,
    .colors = 3,
};

#undef CELL
#undef WIDTH
#undef HEIGH
#undef PERCENT

/* ================================================================ */
/* ============================ EXTERN ============================ */
/* ================================================================ */

World_t World_new(void) {

    World_t world = NULL;
    FILE* file = NULL;

    if ((world = _World_alloc()) == NULL) {
        return NULL;
    }

    /* File does not exist */
    if (file_exists("world.json") != 0) {

        /* Create a file */
        if ((file = file_create("world.json")) == NULL) {
            LilEn_print_error();
        }

        fclose(file);

        /* Save a default world into a file */
        World_save("world.json", (World_t) &WORLD);
    }

    World_load("world.json", world);

    /* ================================ */

    if ((world->current = allocate_2D_array(world->rows, world->columns)) == NULL) {
        goto CLEANUP;
    }

    if ((world->previous = allocate_2D_array(world->rows, world->columns)) == NULL) {
        goto CLEANUP;
    }

    if (world->percent > 0) {
        World_randomize(world, ((int) (world->width / world->cell_size) * (world->height / world->cell_size) * world->percent));
    }

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

    /* Parsed JSON object */
    cJSON* root = NULL;

    /* A single piece of information extracted from a root */
    cJSON* data = NULL;

    size_t i;

    if (filename == NULL) {
        return EXIT_FAILURE;
    }

    if (w == NULL) {
        return EXIT_FAILURE;
    }

    /* ======================= Parse the data ========================= */
    if ((root = LilEn_read_json(filename)) == NULL) {
        goto CLEANUP;
    }

    /* ==================== Retrieving a cell size ==================== */
    data = (cJSON*) Data_read("cell_size", root, cJSON_IsNumber);
    w->cell_size = (data) ? (size_t) data->valueint : WORLD.cell_size;

    /* ================= Retrieving width and height ================== */
    data = (cJSON*) Data_read("width", root, cJSON_IsNumber);
    w->width = (data) ? data->valueint : WORLD.width;

    data = (cJSON*) Data_read("height", root, cJSON_IsNumber);
    w->height = (data) ? data->valueint : WORLD.height;

    /* ================= Adjusting sizes of the world ================= */
    w->rows = w->height / w->cell_size;
    w->columns = w->width / w->cell_size;

    int r = w->width % w->cell_size;

    if (r) {
        w->width -= r;
    }

    r = w->height % w->cell_size;

    if (r) {
        w->height -= r;
    }

    /* ==================== Retrieving start info ===================== */
    data = (cJSON*) Data_read("percent", root, cJSON_IsNumber);
    w->percent = (data) ? data->valuedouble : WORLD.percent;

    /* ===================== Retrieving grid info ===================== */
    data = (cJSON*) Data_read("is_grid", root, cJSON_IsNumber);
    w->is_grid = (data) ? data->valueint : WORLD.is_grid;

    /* ===================== Retrieving grid rate ===================== */
    data = (cJSON*) Data_read("rate", root, cJSON_IsNumber);
    w->rate = (data) ? data->valueint : WORLD.rate;

    /* ==================== Retrieving world type ===================== */
    data = (cJSON*) Data_read("type", root, cJSON_IsNumber);
    w->type = (data) ? data->valueint : WORLD.type;

    /* ================= Retrieving world generation ================== */
    data = (cJSON*) Data_read("generation", root, cJSON_IsNumber);
    w->generation = (data) ? (size_t) data->valueint : WORLD.generation;

    /* ================================ */
    /* ====== READING CELL COLOR ====== */
    /* ================================ */

    data = (cJSON*) Data_read("cell_color", root, cJSON_IsArray);

    if (data != NULL) {

        /* Number of colors in the array of colors */
        size_t number_of_colors = cJSON_GetArraySize(data);

        w->colors = number_of_colors;

        unsigned char base = 255;

        for (i = 0; i < number_of_colors; i++) {

            /* Get the current color */
            cJSON* color = cJSON_GetArrayItem(data, i);

            if (cJSON_GetArraySize(color) >= 4) {
                w->c_color[i][0] = cJSON_GetArrayItem(color, 0)->valueint;
                w->c_color[i][1] = cJSON_GetArrayItem(color, 1)->valueint;
                w->c_color[i][2] = cJSON_GetArrayItem(color, 2)->valueint;
                w->c_color[i][3] = cJSON_GetArrayItem(color, 3)->valueint;
            }
            else {
                w->c_color[i][0] = base - 25;
                w->c_color[i][1] = base - 25;
                w->c_color[i][2] = base - 25;
                w->c_color[i][3] = (base -= 25);
            }

            /* Up to 9 colors */
            if (i == 8) {
                break ;
            }
        }
    }
    else {

        for (i = 0; i < WORLD.colors; i++) {
            w->c_color[i][0] = WORLD.c_color[i][0];
            w->c_color[i][1] = WORLD.c_color[i][1];
            w->c_color[i][2] = WORLD.c_color[i][2];
            w->c_color[i][3] = WORLD.c_color[i][3];
        }
    }

    /* ================================ */
    /* ====== READING GRID COLOR ====== */
    /* ================================ */

    data = (cJSON*) Data_read("grid_color", root, cJSON_IsArray);

    if (cJSON_GetArraySize(data) >= 4) {

        w->g_color[0] = cJSON_GetArrayItem(data, 0)->valueint;
        w->g_color[1] = cJSON_GetArrayItem(data, 1)->valueint;
        w->g_color[2] = cJSON_GetArrayItem(data, 2)->valueint;
        w->g_color[3] = cJSON_GetArrayItem(data, 3)->valueint;
    }
    else {
        w->g_color[0] = WORLD.g_color[0];
        w->g_color[1] = WORLD.g_color[1];
        w->g_color[3] = WORLD.g_color[2];
        w->g_color[2] = WORLD.g_color[3];
    }

    /* ================================ */
    /* === READING BACKGROUND COLOR === */
    /* ================================ */

    data = (cJSON*) Data_read("bg_color", root, cJSON_IsArray);

    if (cJSON_GetArraySize(data) >= 4) {

        w->bg_color[0] = cJSON_GetArrayItem(data, 0)->valueint;
        w->bg_color[1] = cJSON_GetArrayItem(data, 1)->valueint;
        w->bg_color[2] = cJSON_GetArrayItem(data, 2)->valueint;
        w->bg_color[3] = cJSON_GetArrayItem(data, 3)->valueint;
    }
    else {
        w->bg_color[0] = WORLD.bg_color[0];
        w->bg_color[1] = WORLD.bg_color[1];
        w->bg_color[3] = WORLD.bg_color[2];
        w->bg_color[2] = WORLD.bg_color[3];
    }

    /* ================================ */
    /* ====== READING TEXT COLOR ====== */
    /* ================================ */

    data = (cJSON*) Data_read("text_color", root, cJSON_IsArray);

    if (cJSON_GetArraySize(data) >= 4) {

        w->text_color[0] = cJSON_GetArrayItem(data, 0)->valueint;
        w->text_color[1] = cJSON_GetArrayItem(data, 1)->valueint;
        w->text_color[2] = cJSON_GetArrayItem(data, 2)->valueint;
        w->text_color[3] = cJSON_GetArrayItem(data, 3)->valueint;
    }
    else {
        w->text_color[0] = WORLD.text_color[0];
        w->text_color[1] = WORLD.text_color[1];
        w->text_color[3] = WORLD.text_color[2];
        w->text_color[2] = WORLD.text_color[3];
    }

    /* Loading the current generation */
    load_2D_array(root, "current", w->current, w->rows, w->columns);

    /* ================================ */

    Timer_set(w->clock, 1.0 / w->rate);

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

    printf("%-16s: [%d, %d, %d, %d]\n", "grid color", w->g_color[0], w->g_color[1], w->g_color[2], w->g_color[3]);
    printf("%-16s: [%d, %d, %d, %d]\n", "text color", w->text_color[0], w->text_color[1], w->text_color[2], w->text_color[3]);
    printf("%-16s: [%d, %d, %d, %d]\n", "BG color", w->bg_color[0], w->bg_color[1], w->bg_color[2], w->bg_color[3]);

    return ;
}

/* ================================================================ */

int World_destroy(World_t* w) {

    if ((w == NULL) || (*w == NULL)) {
        return EXIT_FAILURE;
    }

    deallocate_2D_array(&(*w)->current, (*w)->rows);

    deallocate_2D_array(&(*w)->previous, (*w)->rows);

    Timer_destroy(&(*w)->clock);

    free(*w);

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

    SDL_Color old_color = *g_color;

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

                LilEn_set_colorRGB(world->c_color[world->current[row][column] - 1][0], world->c_color[world->current[row][column] - 1][1], world->c_color[world->current[row][column] - 1][2], world->c_color[world->current[row][column] - 1][3]);

                LilEn_draw_rect(w, &cell);
            }
        }
    }

    LilEn_set_colorRGB(old_color.r, old_color.g, old_color.b, old_color.a);

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

        w->current[row][column] = 1 + RAND_RANGE(0, w->colors);
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

    unsigned char modifiers[w->colors];

    /* After some time I found that my system put garbage into the array */
    memset(modifiers, 0, w->colors);

    if (w == NULL) { 
        return ;
    }

    rows = w->rows;
    columns = w->columns;

    swap_2D_array(w->current, w->previous, w->rows, w->columns);

    for (row = 0; row < rows; row++) {

        for (column = 0; column < columns; column++) {
            
            /* We are comparing here against the presence of a live cell */
            acc 
                = (w->previous[(row - 1 < 0) ? rows - 1 : row - 1][column] > 0 && World_get_color_modifers(modifiers, w->previous[(row - 1 < 0) ? rows - 1 : row - 1][column]))
                + (w->previous[(row - 1 < 0) ? rows - 1 : row - 1][(column + 1 >= columns) ? 0 : column + 1] > 0 && World_get_color_modifers(modifiers, w->previous[(row - 1 < 0) ? rows - 1 : row - 1][(column + 1 >= columns) ? 0 : column + 1]))
                + (w->previous[row][(column + 1 >= columns) ? 0 : column + 1] > 0 && World_get_color_modifers(modifiers, w->previous[row][(column + 1 >= columns) ? 0 : column + 1]))
                + (w->previous[(row + 1 >= rows) ? 0 : row + 1][(column + 1 >= columns) ? 0 : column + 1] > 0 && World_get_color_modifers(modifiers, w->previous[(row + 1 >= rows) ? 0 : row + 1][(column + 1 >= columns) ? 0 : column + 1]))
                + (w->previous[(row + 1 >= rows) ? 0 : row + 1][column] > 0 && World_get_color_modifers(modifiers, w->previous[(row + 1 >= rows) ? 0 : row + 1][column]))
                + (w->previous[(row + 1 >= rows) ? 0 : row + 1][(column - 1 < 0) ? columns - 1 : column - 1] > 0 && World_get_color_modifers(modifiers, w->previous[(row + 1 >= rows) ? 0 : row + 1][(column - 1 < 0) ? columns - 1 : column - 1]))
                + (w->previous[row][((column - 1 <= 0) ? columns - 1 : column - 1)] > 0 && World_get_color_modifers(modifiers, w->previous[row][((column - 1 <= 0) ? columns - 1 : column - 1)]))
                + (w->previous[(row - 1 < 0) ? rows - 1 : row - 1][(column - 1 < 0) ? columns - 1 : column - 1] > 0 && World_get_color_modifers(modifiers, w->previous[(row - 1 < 0) ? rows - 1 : row - 1][(column - 1 < 0) ? columns - 1 : column - 1]));
            
            /* Any live cell with fewer than two live neighbours dies, as if by underpopulation. */
            if (w->previous[row][column] > 0 && acc < 2) {
                w->current[row][column] = 0;
            }
            /* Any live cell with two or three live neighbours lives on to the next generation. */
            else if ((w->previous[row][column] > 0) && (acc == 2 || acc == 3)) {
                w->current[row][column] = 1 + array_max_index(modifiers, w->colors);
            }
            /* Any live cell with more than three live neighbours dies, as if by overpopulation. */
            else if (w->previous[row][column] > 0 && acc > 3) {
                w->current[row][column] = 0;
            }
            /* Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction. */
            else if (w->previous[row][column] < 1 && acc == 3) {
                w->current[row][column] = 1 + array_max_index(modifiers, w->colors);
            }
            else {
                w->current[row][column] = w->previous[row][column];
            }

            memset(modifiers, 0, w->colors);
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

    if ((file = file_create(filename)) == NULL) {
        
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

    data = (data = cJSON_CreateNumber(w->generation)) ? data : NULL;
    cJSON_AddItemToObject(root, "generation", data);

    data = (data = cJSON_CreateNumber(w->percent)) ? data : NULL;
    cJSON_AddItemToObject(root, "percent", data);

    /* ================================ */
    /* ====== SAVING CELL COLOR ======= */
    /* ================================ */

    /* Create the main array */
    if ((array = cJSON_CreateArray()) == NULL) {

        fprintf(file, "%s", cJSON_Print(root));

        cJSON_Delete(root);
        fclose(file);

        return EXIT_FAILURE;
    }

    /* For every color in the main array */
    for (i = 0; i < w->colors; i++) {

        /* Create an array to store a single color */
        cJSON* color = cJSON_CreateArray();
        
        /* Rewd component */
        data = (data = cJSON_CreateNumber(w->c_color[i][0])) ? data : NULL;
        cJSON_AddItemToArray(color, data);
        
        /* Green component */
        data = (data = cJSON_CreateNumber(w->c_color[i][1])) ? data : NULL;
        cJSON_AddItemToArray(color, data);

        /* Blue component */
        data = (data = cJSON_CreateNumber(w->c_color[i][2])) ? data : NULL;
        cJSON_AddItemToArray(color, data);

        /* Alpha component */
        data = (data = cJSON_CreateNumber(w->c_color[i][3])) ? data : NULL;
        cJSON_AddItemToArray(color, data);

        cJSON_AddItemToArray(array, color);
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

    if ((array = cJSON_CreateArray()) == NULL) {

        fprintf(file, "%s", cJSON_Print(root));

        cJSON_Delete(root);
        fclose(file);

        return EXIT_FAILURE;
    }

    for (i = 0; i < sizeof(w->bg_color) / (sizeof(w->bg_color[0])); i++) {

        data = (data = cJSON_CreateNumber(w->bg_color[i])) ? data : NULL;
        cJSON_AddItemToArray(array, data);
    }

    cJSON_AddItemToObject(root, "bg_color", array);

    /* ================================ */
    /* ====== SAVING TEXT COLOR ======= */
    /* ================================ */

    if ((array = cJSON_CreateArray()) == NULL) {

        fprintf(file, "%s", cJSON_Print(root)); 

        cJSON_Delete(root);
        fclose(file);

        return EXIT_FAILURE;
    }

    for (i = 0; i < sizeof(w->text_color) / (sizeof(w->text_color[0])); i++) {

        data = (data = cJSON_CreateNumber(w->text_color[i])) ? data : NULL;
        cJSON_AddItemToArray(array, data);
    }

    cJSON_AddItemToObject(root, "text_color", array);

    save_2D_array(root, "current", w->current, w->rows, w->columns);

    /* ================================ */

    fprintf(file, "%s", cJSON_Print(root));

    cJSON_Delete(root);
    fclose(file);

    return EXIT_SUCCESS;
}

/* ================================================================ */
