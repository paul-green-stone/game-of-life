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

DIR* dir_create(const char pathname, mode_t mode) {

    DIR* dir = NULL;

    strncpy(g_filename, pathname, LBUFF - 1);
    g_filename[length(g_filename)] = '\0';

    if ((dir = mkdir(pathname, mode)) == NULL) {
        /* Upon NULL, call `LilEn_print_error` to get more inforamation */
        return NULL;
    }

    return dir;
}

#undef LBUFF

/* ================================================================ */