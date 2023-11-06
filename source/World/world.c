#include "../include.h"

/* ================================================================ */

World_t World_new(void) {

    World_t world = NULL;

    /* Buffer containing file content */
    char* input = NULL;
    /* Parsed JSON object */
    cJSON* root = NULL;

    size_t i = 0;

    /* ================================================================ */
    /* ============= Allocating a new instance of a world ============= */
    /* ================================================================ */

    if ((world = (World_t) calloc(1, sizeof(struct world))) == NULL) {
        goto CLEANUP;
    }

    /* ================================================================ */
    /* ============== Open a file with default settings =============== */
    /* ================================================================ */

    if ((input = LilEn_read_data_file("source/World/default.json")) == NULL) {
        goto CLEANUP;
    }

    /* ================================================================ */
    /* ======================= Parse the data ========================= */
    /* ================================================================ */

    if ((root = cJSON_Parse(input)) == NULL) {
        goto CLEANUP;
    }

    cJSON* data = NULL;

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

    for (; i < array_size; i++) {

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

    /* ================================================================ */

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

    if ((world->world = (char**) malloc(sizeof(char*) * world->columns)) == NULL) {
        goto CLEANUP;
    }

    for (i = 0; i < world->rows; i++) {

        if ((world->world[i] = (char*) calloc(1, sizeof(char) * world->rows)) == NULL) {
            //memset(world->world[i], 0, sizeof(char) * world->rows);
            goto CLEANUP;
        }
    }

    /* ================================================================ */

    free(input);
    cJSON_Delete(root);

    return world;

    /* ================================================================ */
    /* ========================= Cleaning up ========================== */
    /* ================================================================ */

    { CLEANUP:

        LilEn_print_error();

        if (input != NULL) {
            free(input);
        }

        if (root != NULL) {
            cJSON_Delete(root);
        }

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

    printf("%-16s: %s (%d)\n", "type", (w->type == 1) ? "wrap around" : (w->type == 2) ? "dead" : "alive", w->type);

    printf("%-16s: [%d, %d, %d, %d]\n", "cell color", w->c_color.r, w->c_color.g, w->c_color.b, w->c_color.a);
    printf("%-16s: [%d, %d, %d, %d]\n", "grid color", w->g_color.g, w->g_color.g, w->g_color.b, w->g_color.a);

    return ;
}

/* ================================================================ */

int World_destroy(World_t* w) {

    size_t i = 0;

    if ((w == NULL) || (*w == NULL)) {
        return EXIT_FAILURE;
    }

    for (; i < (*w)->columns; i++) {
        free((*w)->world[i]);
    }

    free((*w)->world);

    free(*w);

    *w = NULL;

    return EXIT_SUCCESS;
}

/* ================================================================ */

void World_log_world(const World_t w) {

    size_t row = 0;
    size_t column = 0;

    if (w == NULL) {
        return ;
    }

    for (; row < w->rows; row++) {
        for (; column < w->columns; column++) {
            printf("%d", w->world[row][column]);

            if (column + 1 < w->columns) {
                printf(", ");
            }
        }

        column = 0;
        printf("\n");
    }
}

/* ================================================================ */
