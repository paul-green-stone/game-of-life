#include "include.h"

/* Little buffer size */
#define LBUFF 128

/* ================================================================ */

FILE* file_create(const char* name) {

    FILE* file = NULL;

    if (name == NULL) {
        return NULL;
    }

    strncpy(g_filename, name, LBUFF - 1);
    g_filename[length(g_filename)] = '\0';

    if ((file = fopen(g_filename, "w")) == NULL) {
        /* Upon NULL, call `LilEn_print_error` to get more inforamation */
        return NULL;
    }

    return file;
}

/* ================================================================ */

int dir_create(const char* pathname, mode_t mode) {

    strncpy(g_filename, pathname, LBUFF - 1);
    g_filename[length(g_filename)] = '\0';

    if (mkdir(pathname, mode) != 0) {
        /* Upon NULL, call `LilEn_print_error` to get more inforamation */
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#undef LBUFF

/* ================================================================ */