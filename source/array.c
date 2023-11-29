#include "include.h"

/* ================================================================ */

unsigned char** allocate_2D_array(int rows, int columns) {

    unsigned char** array = NULL;

    /* Array variable */
    size_t i = 0;

    /* size_t version or `rows` */
    size_t r = 0;

    /* Prevent overflow */
    if ((rows < 0) || (columns < 0)) {
        return array;
    }

    /* One-time cast */
    r = (size_t) rows;

    /* ======================= Array allocation ======================= */

    /* Allocate `rows` rows for an array. Clear it */
    if ((array = (unsigned char**) calloc(rows, sizeof(unsigned char*))) == NULL) {

        LilEn_print_error();
        deallocate_2D_array(&array, rows);

        return NULL;
    }

    /* For each row allocate `columns` columns and clear them, set their initial values to 0 */
    for (i = 0; i < r; i++) {

        if ((array[i] = (unsigned char*) calloc(columns, sizeof(unsigned char))) == NULL) {

            LilEn_print_error();
            deallocate_2D_array(&array, rows);

            return NULL;
        }
    }

    return array;
}

/* ================================================================ */

int save_2D_array(const cJSON* root, const char* name, unsigned char** array, int rows, int columns) {

    /* Array object to add into a root tree */
    cJSON* a = NULL;
    /* JSON object equivalent to a single array row */
    cJSON* r = NULL;
    /* JSON object equivalent to a value of a row cell */
    cJSON* data = NULL;

    size_t row;
    size_t rs;      /* Total number of rows */

    size_t column;
    size_t cs;      /* Total number of columns */

    /* Prevent overflow */
    if ((rows < 0) || (columns < 0)) {
        return EXIT_FAILURE;
    }

    /* One-time cast */
    rs = (size_t) rows;
    cs = (size_t) columns;

    if ((a = cJSON_CreateArray()) == NULL) {
        return EXIT_FAILURE;
    }

    for (row = 0; row < rs; row++) {

        if ((r = cJSON_CreateArray()) == NULL) {
            return EXIT_FAILURE;
        }

        for (column = 0; column < cs; column++) {

            data = (data = cJSON_CreateNumber(array[row][column])) ? data : NULL;
            cJSON_AddItemToArray(r, data);
        }

        cJSON_AddItemToArray(a, r);
    }

    cJSON_AddItemToObject((cJSON*) root, name == NULL ? "array" : name, a);

    return EXIT_SUCCESS;
}

/* ================================================================ */

void deallocate_2D_array(unsigned char*** array, int rows) {

    /* Array variable */
    size_t i = 0;

    /* size_t version or `rows` */
    size_t r = 0;

    /* Prevent overflow. INT_MAX is the maximum possible value here */
    if (rows < 0) {
        return ;
    }

    /* One-time cast to size_t in order to not waste time on casting every iteration. Yes, I'm aware of compiler optimiation -_-*/
    r = (size_t) rows;

    for (i = 0; i < r; i++) {
        free((*array)[i]);
    }

    free(*array);

    *array = NULL;

    return ;
}

/* ================================================================ */

int swap_2D_array(unsigned char** array_1, unsigned char** array_2, int rows, int columns) {

    size_t row = 0;
    size_t r = 0;

    size_t column = 0;
    size_t c = 0;

    /* Temporary value container */
    unsigned char temp = 0;

    /* None of the arrays can be NULL */
    if ((array_1 == NULL) || (array_2 == NULL)) {
        return EXIT_FAILURE;
    }

    /* Prevent overflow */
    if ((rows < 0) || (columns < 0)) {
        return EXIT_FAILURE;
    }

    /* One-time cast */
    r = (size_t) rows;
    c = (size_t) columns;

    for (row = 0; row < r; row++) {

        for (column = 0; column < c; column++) {

            temp = array_1[row][column];
            array_1[row][column] = array_2[row][column];
            array_2[row][column] = temp;
        }
    }

    return EXIT_SUCCESS;
}

/* ================================================================ */

int load_2D_array(const cJSON* root, const char* name, unsigned char** dst, int rows, int columns) {

    size_t row = 0;
    size_t rs = 0;

    size_t column = 0;
    size_t cs = 0;

    /* Array extracted from the root */
    cJSON* array = NULL;

    cJSON* element = NULL;

    cJSON* r = NULL;

    if (dst == NULL) {
        return EXIT_FAILURE;
    }

    /* Prevent overflow */
    if ((rows < 0) || (columns < 0)) {
        return EXIT_FAILURE;
    }

    /* One-time cast */
    rs = (size_t) rows;
    cs = (size_t) columns;

    /* Retreiving an array from a .json file */
    if ((array = (cJSON*) Data_read(name == NULL ? "array" : name, root, cJSON_IsArray)) == NULL) {
        return EXIT_FAILURE;
    }

    /* Get the number of rows in the extracted array */
    if (cJSON_GetArraySize(array) == 0) {
        return EXIT_SUCCESS;
    }

    /* For every row ... */
    for (row = 0; row < rs; row++) {

        /* get the size of it */
        r = cJSON_GetArrayItem(array, row);

        /* and going through the row .. */
        for (column = 0; column < cs; column++) {

            /* take the value of each cell in it ... */
            element = cJSON_GetArrayItem(r, column);

            /* and place it into the destination array */
            dst[row][column] = element->valueint;
        }
    }

    return EXIT_SUCCESS;
}